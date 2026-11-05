#include <memory>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <stdexcept>
#include <cassert>
#include <algorithm>
namespace YAKSys_Hybrid_CPU::Core::Registers { struct RenameMap { static constexpr int ArchRegs=32; }; namespace Retire { struct RetireRecord { int vt, reg; unsigned long long value; static RetireRecord RegisterWrite(int v,int r,unsigned long long x){return {v,r,x};} }; struct RetireCoordinator { std::vector<RetireRecord> writes; void Retire(RetireRecord r){writes.push_back(r);} }; } }
namespace YAKSys_Hybrid_CPU { struct Processor { struct MainMemoryArea { std::vector<unsigned char> data = std::vector<unsigned char>(16); bool available=true; long long getLength(){return data.size();} bool TryReadPhysicalRange(unsigned long long a,std::vector<unsigned char>& out){if(!available)return false;std::copy_n(data.begin()+a,out.size(),out.begin());return true;} bool TryWritePhysicalRange(unsigned long long a,const std::vector<unsigned char>& in){if(!available)return false;std::copy(in.begin(),in.end(),data.begin()+a);return true;} }; struct CPU_Core { static constexpr int SmtWays=4; std::shared_ptr<Core::Registers::Retire::RetireCoordinator> retire=std::make_shared<Core::Registers::Retire::RetireCoordinator>(); int ReadActiveVirtualThreadId(){return 2;} unsigned long long ReadArch(int vt,int r){return vt*100+r;} auto getRetireCoordinator(){return retire;} }; }; }
namespace HybridCPU_ISE::Core { class ReplayToken { public: using MemorySnapshot=std::pair<unsigned long long,std::vector<unsigned char>>; std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> _boundMainMemory; std::map<int,unsigned long long> PreExecutionRegisterState; std::vector<MemorySnapshot> PreExecutionMemoryState; int OwnerThreadId=0; bool HasSideEffects=false;
static bool HasExactMainMemoryRange(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea>,unsigned long long,int); std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> GetRequiredMainMemory(const std::wstring&) const; static void ReadMainMemoryExact(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea>,unsigned long long,std::vector<unsigned char>&,const std::wstring&); static void WriteMainMemoryExact(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea>,unsigned long long,const std::vector<unsigned char>&,const std::wstring&); bool HasFullyBoundRollbackMemoryState() const; void CaptureMemoryState(unsigned long long,int); int ResolveOwnerThreadId(const std::shared_ptr<YAKSys_Hybrid_CPU::Processor::CPU_Core>&) const; void CaptureRegisterState(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::CPU_Core>&,const std::vector<int>&); void Rollback(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::CPU_Core>&); bool CanSafelyRollback() const; };
    bool ReplayToken::HasExactMainMemoryRange(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> memory, unsigned long long address, int size) {
        if (!memory || size <= 0 || memory->getLength() < 0) return false;
        auto length = static_cast<unsigned long long>(memory->getLength());
        auto requested = static_cast<unsigned long long>(size);
        return requested <= length && address <= length - requested;
    }
    std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> ReplayToken::GetRequiredMainMemory(const std::wstring&) const {
        if (!_boundMainMemory) throw std::logic_error("Replay token has no bound main memory.");
        return _boundMainMemory;
    }
    void ReplayToken::ReadMainMemoryExact(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> memory, unsigned long long address, std::vector<unsigned char>& buffer, const std::wstring&) {
        if (buffer.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) || !HasExactMainMemoryRange(memory, address, static_cast<int>(buffer.size())) || !memory->TryReadPhysicalRange(address, buffer)) throw std::logic_error("Replay memory capture could not materialize the complete range.");
    }
    void ReplayToken::WriteMainMemoryExact(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> memory, unsigned long long address, const std::vector<unsigned char>& buffer, const std::wstring&) {
        if (buffer.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) || !HasExactMainMemoryRange(memory, address, static_cast<int>(buffer.size())) || !memory->TryWritePhysicalRange(address, buffer)) throw std::logic_error("Replay memory restore could not materialize the complete range.");
    }
    bool ReplayToken::HasFullyBoundRollbackMemoryState() const {
        if (PreExecutionMemoryState.empty() || !_boundMainMemory) return false;
        for (const auto& [address, bytes] : PreExecutionMemoryState)
            if (bytes.empty() || bytes.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) || !HasExactMainMemoryRange(_boundMainMemory, address, static_cast<int>(bytes.size()))) return false;
        return true;
    }
    void ReplayToken::CaptureMemoryState(unsigned long long address, int size) {
        if (size <= 0) return;
        auto memory = GetRequiredMainMemory(L"ReplayToken.CaptureMemoryState");
        std::vector<unsigned char> bytes(static_cast<std::size_t>(size));
        ReadMainMemoryExact(memory, address, bytes, L"ReplayToken.CaptureMemoryState");
        PreExecutionMemoryState.emplace_back(address, std::move(bytes));
    }
    int ReplayToken::ResolveOwnerThreadId(const std::shared_ptr<YAKSys_Hybrid_CPU::Processor::CPU_Core>& core) const {
        return OwnerThreadId >= 0 && OwnerThreadId < YAKSys_Hybrid_CPU::Processor::CPU_Core::SmtWays ? OwnerThreadId : core->ReadActiveVirtualThreadId();
    }
    void ReplayToken::CaptureRegisterState(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::CPU_Core>& core, const std::vector<int>& registerIds) {
        if (registerIds.empty()) return;
        PreExecutionRegisterState.clear(); auto vt = ResolveOwnerThreadId(core);
        for (int reg : registerIds) if (reg >= 0 && reg < YAKSys_Hybrid_CPU::Core::Registers::RenameMap::ArchRegs) PreExecutionRegisterState[reg] = core->ReadArch(vt, reg);
    }
    void ReplayToken::Rollback(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::CPU_Core>& core) {
        auto vt = ResolveOwnerThreadId(core);
        auto memory = PreExecutionMemoryState.empty() ? nullptr : GetRequiredMainMemory(L"ReplayToken.Rollback");
        for (const auto& [reg, value] : PreExecutionRegisterState)
            if (reg >= 0 && reg < YAKSys_Hybrid_CPU::Core::Registers::RenameMap::ArchRegs) core->getRetireCoordinator()->Retire(YAKSys_Hybrid_CPU::Core::Registers::Retire::RetireRecord::RegisterWrite(vt, reg, value));
        for (const auto& [address, bytes] : PreExecutionMemoryState) WriteMainMemoryExact(memory, address, bytes, L"ReplayToken.Rollback");
    }
    bool ReplayToken::CanSafelyRollback() const {
        if (!HasSideEffects) return true;
        bool hasRegisters = !PreExecutionRegisterState.empty(), hasMemory = !PreExecutionMemoryState.empty();
        bool validMemory = HasFullyBoundRollbackMemoryState();
        return !(hasMemory && !validMemory) && (hasRegisters || validMemory);
    }
}
int main(){ using namespace HybridCPU_ISE::Core; ReplayToken t; t.HasSideEffects=true; assert(!t.CanSafelyRollback()); bool fail=false; try{t.CaptureMemoryState(0,2);}catch(const std::logic_error&){fail=true;} assert(fail); auto m=std::make_shared<YAKSys_Hybrid_CPU::Processor::MainMemoryArea>();t._boundMainMemory=m;m->data[2]=9;t.CaptureMemoryState(2,1);assert(t.CanSafelyRollback());m->data[2]=4;auto c=std::make_shared<YAKSys_Hybrid_CPU::Processor::CPU_Core>();t.OwnerThreadId=8;t.CaptureRegisterState(c,{1,-1,33});t.Rollback(c);assert(m->data[2]==9 && c->retire->writes.size()==1 && c->retire->writes[0].vt==2 && c->retire->writes[0].value==201);fail=false;try{t.CaptureMemoryState(15,2);}catch(const std::logic_error&){fail=true;}assert(fail);m->available=false;fail=false;try{t.CaptureMemoryState(2,1);}catch(const std::logic_error&){fail=true;}assert(fail && t.PreExecutionMemoryState.size()==1);}
