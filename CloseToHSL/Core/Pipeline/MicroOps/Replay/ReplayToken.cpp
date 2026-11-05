#include "ReplayToken.h"
#include "../../../Diagnostics/TraceSink.h"
#include "../../../Decoder/Sha256Digest.h"
#include <bit>
#include <cstdint>
#include <ctime>
#include <limits>
#include <stdexcept>
#include <utility>
#include <sstream>
#include <algorithm>
#include <cwctype>

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace HybridCPU_ISE::Core
{
	namespace
	{
		std::wstring JsonEscape(const std::wstring& value)
		{
			std::wstring out=L"\"";
			for (wchar_t c : value) { switch(c) { case L'"': out+=L"\\\""; break; case L'\\': out+=L"\\\\"; break; case L'\n': out+=L"\\n"; break; case L'\r': out+=L"\\r"; break; case L'\t': out+=L"\\t"; break; default: if(c<0x20) { static constexpr wchar_t hex[]=L"0123456789abcdef"; out+=L"\\u"; out+=hex[(c>>12)&15]; out+=hex[(c>>8)&15]; out+=hex[(c>>4)&15]; out+=hex[c&15]; } else out+=c; } }
			out+=L'"'; return out;
		}
		std::wstring JsonValue(const std::wstring& json, const std::wstring& key)
		{
			const auto marker=JsonEscape(key); auto p=json.find(marker); if(p==std::wstring::npos) throw std::invalid_argument("Missing ReplayToken JSON field.");
			p=json.find(L':',p+marker.size()); if(p==std::wstring::npos) throw std::invalid_argument("Malformed ReplayToken JSON field."); ++p;
			while(p<json.size() && std::iswspace(json[p])) ++p; if(p==json.size()) throw std::invalid_argument("Truncated ReplayToken JSON value.");
			const auto begin=p; bool quoted=false, escaped=false; int depth=0;
			for(;p<json.size();++p) { const wchar_t c=json[p]; if(quoted) { if(escaped) escaped=false; else if(c==L'\\') escaped=true; else if(c==L'"') quoted=false; continue; } if(c==L'"') { quoted=true; continue; } if(c==L'{'||c==L'[') { ++depth; continue; } if(c==L'}'||c==L']') { if(depth==0) break; --depth; if(depth==0) { ++p; break; } continue; } if(depth==0&&(c==L','||std::iswspace(c))) break; }
			return json.substr(begin,p-begin);
		}
		unsigned long long JsonUnsigned(const std::wstring& j,const std::wstring& k) { const auto v=JsonValue(j,k); std::size_t n=0; auto x=std::stoull(v,&n); if(n!=v.size()) throw std::invalid_argument("Invalid ReplayToken integer."); return x; }
		std::wstring JsonStringValue(const std::wstring& json,const std::wstring& key)
		{
			auto v=JsonValue(json,key); if(v.size()<2||v.front()!=L'"'||v.back()!=L'"') throw std::invalid_argument("Invalid ReplayToken string field."); std::wstring out;
			for(std::size_t i=1;i+1<v.size();++i){wchar_t c=v[i]; if(c!=L'\\'){out+=c;continue;} if(++i+1>=v.size()) throw std::invalid_argument("Invalid JSON escape."); c=v[i]; switch(c){case L'"':case L'\\':case L'/':out+=c;break;case L'b':out+=L'\b';break;case L'f':out+=L'\f';break;case L'n':out+=L'\n';break;case L'r':out+=L'\r';break;case L't':out+=L'\t';break;case L'u':{if(i+4>=v.size())throw std::invalid_argument("Invalid Unicode escape."); unsigned x=0; for(int d=0;d<4;++d){wchar_t h=v[++i]; x<<=4; if(h>=L'0'&&h<=L'9')x+=h-L'0';else if(h>=L'a'&&h<=L'f')x+=h-L'a'+10;else if(h>=L'A'&&h<=L'F')x+=h-L'A'+10;else throw std::invalid_argument("Invalid Unicode escape.");} out+=static_cast<wchar_t>(x);break;} default:throw std::invalid_argument("Invalid JSON escape.");}}
			return out;
		}
	}

	std::wstring ReplayToken::ToJson() const
	{
		std::wostringstream out; out << L"{\n  \"RandomSeed\": " << RandomSeed << L",\n  \"VL\": " << VL << L",\n  \"VLMAX\": " << VLMAX << L",\n  \"VSEW\": " << static_cast<unsigned>(VSEW) << L",\n  \"LMUL\": " << static_cast<unsigned>(LMUL) << L",\n  \"TailAgnostic\": " << (TailAgnostic?L"true":L"false") << L",\n  \"MaskAgnostic\": " << (MaskAgnostic?L"true":L"false") << L",\n  \"RoundingMode\": " << static_cast<unsigned>(RoundingMode) << L",\n  \"ExceptionMode\": " << static_cast<unsigned>(ExceptionMode) << L",\n  \"MemorySize\": " << MemorySize << L",\n  \"TraceHash\": " << JsonEscape(TraceHash) << L",\n  \"Timestamp\": " << Timestamp << L",\n  \"PreExecutionRegisterState\": {";
		std::vector<std::pair<int,unsigned long long>> registers(PreExecutionRegisterState.begin(),PreExecutionRegisterState.end()); std::sort(registers.begin(),registers.end()); bool first=true; for(const auto& [id,value]:registers){out<<(first?L"":L",")<<L"\n    \""<<id<<L"\": "<<value; first=false;} if(!registers.empty())out<<L'\n';
		out<<L"  },\n  \"PreExecutionMemoryState\": ["; first=true; for(const auto& [address,data]:PreExecutionMemoryState){out<<(first?L"":L",")<<L"\n    {\"Address\": "<<address<<L", \"Data\": ["; for(std::size_t i=0;i<data.size();++i){if(i)out<<L", ";out<<static_cast<unsigned>(data[i]);} out<<L"]}"; first=false;} if(!PreExecutionMemoryState.empty())out<<L'\n';
		out<<L"  ],\n  \"OwnerThreadId\": "<<OwnerThreadId<<L",\n  \"HasSideEffects\": "<<(HasSideEffects?L"true":L"false")<<L"\n}"; return out.str();
	}
	std::shared_ptr<ReplayToken> ReplayToken::FromJson(const std::wstring& json,std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> memory)
	{
		if(json.empty()) throw std::invalid_argument("json"); auto token=std::make_shared<ReplayToken>();
		token->RandomSeed=JsonUnsigned(json,L"RandomSeed"); token->VL=static_cast<unsigned int>(JsonUnsigned(json,L"VL")); token->VLMAX=static_cast<unsigned int>(JsonUnsigned(json,L"VLMAX")); token->VSEW=static_cast<unsigned char>(JsonUnsigned(json,L"VSEW")); token->LMUL=static_cast<unsigned char>(JsonUnsigned(json,L"LMUL")); token->TailAgnostic=JsonValue(json,L"TailAgnostic")==L"true"; token->MaskAgnostic=JsonValue(json,L"MaskAgnostic")==L"true"; token->RoundingMode=static_cast<unsigned char>(JsonUnsigned(json,L"RoundingMode")); token->ExceptionMode=static_cast<unsigned char>(JsonUnsigned(json,L"ExceptionMode")); token->MemorySize=JsonUnsigned(json,L"MemorySize"); token->TraceHash=JsonStringValue(json,L"TraceHash"); token->Timestamp=static_cast<long long>(std::stoll(JsonValue(json,L"Timestamp"))); token->OwnerThreadId=static_cast<int>(std::stol(JsonValue(json,L"OwnerThreadId"))); const auto side=JsonValue(json,L"HasSideEffects"); if(side!=L"true"&&side!=L"false")throw std::invalid_argument("Invalid ReplayToken boolean."); token->HasSideEffects=side==L"true";
		const auto regs=JsonValue(json,L"PreExecutionRegisterState"); std::size_t p=1; while(p+1<regs.size()){while(p<regs.size()&&(std::iswspace(regs[p])||regs[p]==L','))++p;if(p+1>=regs.size())break;if(regs[p++]!=L'"')throw std::invalid_argument("Invalid ReplayToken register map.");auto e=regs.find(L'"',p);if(e==std::wstring::npos)throw std::invalid_argument("Invalid ReplayToken register key.");int id=std::stoi(regs.substr(p,e-p));p=regs.find(L':',e);if(p==std::wstring::npos)throw std::invalid_argument("Invalid ReplayToken register value.");++p;while(p<regs.size()&&std::iswspace(regs[p]))++p;auto end=p;while(end<regs.size()&&std::iswdigit(regs[end]))++end;token->PreExecutionRegisterState[id]=std::stoull(regs.substr(p,end-p));p=end;}
		const auto snapshots=JsonValue(json,L"PreExecutionMemoryState"); p=1; while((p=snapshots.find(L"\"Address\"",p))!=std::wstring::npos){auto colon=snapshots.find(L':',p)+1;while(colon<snapshots.size()&&std::iswspace(snapshots[colon]))++colon;auto end=colon;while(end<snapshots.size()&&std::iswdigit(snapshots[end]))++end;auto address=std::stoull(snapshots.substr(colon,end-colon));auto dataKey=snapshots.find(L"\"Data\"",end);auto open=snapshots.find(L'[',dataKey);auto close=snapshots.find(L']',open);if(dataKey==std::wstring::npos||open==std::wstring::npos||close==std::wstring::npos)throw std::invalid_argument("Invalid ReplayToken memory snapshot.");std::vector<unsigned char> bytes;std::wistringstream values(snapshots.substr(open+1,close-open-1));std::wstring item;while(std::getline(values,item,L',')){std::size_t first=0;while(first<item.size()&&std::iswspace(item[first]))++first;if(first==item.size())continue;auto value=std::stoul(item.substr(first));if(value>255)throw std::invalid_argument("ReplayToken byte outside range.");bytes.push_back(static_cast<unsigned char>(value));}token->PreExecutionMemoryState.emplace_back(address,std::move(bytes));p=close+1;}
		if(memory)token->BindMainMemory(std::move(memory)); return token;
	}

	unsigned long long ReplayToken::getRandomSeed() const
	{
		return RandomSeed;
	}

	void ReplayToken::setRandomSeed(unsigned long long value)
	{
		RandomSeed = value;
	}

	unsigned int ReplayToken::getVL() const
	{
		return VL;
	}

	void ReplayToken::setVL(unsigned int value)
	{
		VL = value;
	}

	unsigned int ReplayToken::getVLMAX() const
	{
		return VLMAX;
	}

	void ReplayToken::setVLMAX(unsigned int value)
	{
		VLMAX = value;
	}

	unsigned char ReplayToken::getVSEW() const
	{
		return VSEW;
	}

	void ReplayToken::setVSEW(unsigned char value)
	{
		VSEW = value;
	}

	unsigned char ReplayToken::getLMUL() const
	{
		return LMUL;
	}

	void ReplayToken::setLMUL(unsigned char value)
	{
		LMUL = value;
	}

	bool ReplayToken::getTailAgnostic() const
	{
		return TailAgnostic;
	}

	void ReplayToken::setTailAgnostic(bool value)
	{
		TailAgnostic = value;
	}

	bool ReplayToken::getMaskAgnostic() const
	{
		return MaskAgnostic;
	}

	void ReplayToken::setMaskAgnostic(bool value)
	{
		MaskAgnostic = value;
	}

	unsigned char ReplayToken::getRoundingMode() const
	{
		return RoundingMode;
	}

	void ReplayToken::setRoundingMode(unsigned char value)
	{
		RoundingMode = value;
	}

	unsigned char ReplayToken::getExceptionMode() const
	{
		return ExceptionMode;
	}

	void ReplayToken::setExceptionMode(unsigned char value)
	{
		ExceptionMode = value;
	}

	unsigned long long ReplayToken::getMemorySize() const
	{
		return MemorySize;
	}

	void ReplayToken::setMemorySize(unsigned long long value)
	{
		MemorySize = value;
	}

	std::wstring ReplayToken::getTraceHash() const
	{
		return TraceHash;
	}

	void ReplayToken::setTraceHash(const std::wstring &value)
	{
		TraceHash = value;
	}

	long long ReplayToken::getTimestamp() const
	{
		return Timestamp;
	}

	void ReplayToken::setTimestamp(long long value)
	{
		Timestamp = value;
	}

	std::unordered_map<int, unsigned long long> ReplayToken::getPreExecutionRegisterState() const
	{
		return PreExecutionRegisterState;
	}

	void ReplayToken::setPreExecutionRegisterState(const std::unordered_map<int, unsigned long long> &value)
	{
		PreExecutionRegisterState = value;
	}

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
    std::vector<ReplayToken::MemorySnapshot> ReplayToken::getPreExecutionMemoryState() const { return PreExecutionMemoryState; }
    void ReplayToken::setPreExecutionMemoryState(const std::vector<MemorySnapshot>& value) { PreExecutionMemoryState = value; }
    int ReplayToken::getOwnerThreadId() const { return OwnerThreadId; }
    void ReplayToken::setOwnerThreadId(int value) { OwnerThreadId = value; }
    bool ReplayToken::getHasSideEffects() const { return HasSideEffects; }
    void ReplayToken::setHasSideEffects(bool value) { HasSideEffects = value; }
    ReplayToken::ReplayToken() : VLMAX(32), VSEW(32), LMUL(1), Timestamp(static_cast<long long>(std::time(nullptr))) {}
    ReplayToken::ReplayToken(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> memory) : ReplayToken() { if (memory) _boundMainMemory = std::move(memory); }
    std::shared_ptr<ReplayToken> ReplayToken::BindMainMemory(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::MainMemoryArea> memory) { if (!memory) throw std::invalid_argument("mainMemory"); _boundMainMemory = std::move(memory); return shared_from_this(); }
    std::shared_ptr<ReplayToken> ReplayToken::CreateFromConfig(unsigned long long seed, unsigned int vl, unsigned int vlmax, unsigned char vsew, unsigned char lmul, bool tail, bool mask, unsigned char rounding, unsigned char exceptionMode, unsigned long long memorySize) {
        auto token = std::make_shared<ReplayToken>(); token->RandomSeed=seed; token->VL=vl; token->VLMAX=vlmax; token->VSEW=vsew; token->LMUL=lmul; token->TailAgnostic=tail; token->MaskAgnostic=mask; token->RoundingMode=rounding; token->ExceptionMode=exceptionMode; token->MemorySize=memorySize; return token;
    }
    std::wstring ReplayToken::ComputeTraceHash(const std::vector<TraceEvent>& events) {
        if (events.empty()) return L"";
        std::string text;
        for (const auto& event : events) {
            text += std::to_string(event.PC); text += '|';
            text += std::to_string(event.BundleId); text += '|';
            text += std::to_string(event.OpIndex); text += '|';
            text += std::to_string(std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(event.Opcode))); text += '|';
            text += std::to_string(event.ExceptionCount); text += '\n';
        }
        return YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(std::span<const unsigned char>(reinterpret_cast<const unsigned char*>(text.data()), text.size()));
    }
    bool ReplayToken::MatchesConfig(const std::shared_ptr<ReplayToken>& o) const { return o && RandomSeed==o->RandomSeed && VL==o->VL && VLMAX==o->VLMAX && VSEW==o->VSEW && LMUL==o->LMUL && TailAgnostic==o->TailAgnostic && MaskAgnostic==o->MaskAgnostic && RoundingMode==o->RoundingMode && ExceptionMode==o->ExceptionMode && MemorySize==o->MemorySize; }
}
