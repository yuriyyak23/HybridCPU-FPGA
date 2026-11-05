$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\MicroOps\Replay\ReplayToken.cpp';$s=Get-Content $p -Raw
$marker='    std::vector<ReplayToken::MemorySnapshot> ReplayToken::getPreExecutionMemoryState() const'
if(!$s.Contains($marker)){throw 'insertion marker missing'}
$body=@"
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

"@
$s=$s.Replace($marker,$body+$marker).Replace('#include <ctime>',"#include <ctime>`r`n#include <limits>")
Set-Content $p $s -Encoding UTF8 -NoNewline
