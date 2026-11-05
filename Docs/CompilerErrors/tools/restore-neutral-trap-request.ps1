$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Events\Traps'
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.TrapRequest.h'; $s=@'
#pragma once
#include "TrapRequest.h"
#include <memory>
namespace YAKSys_Hybrid_CPU::Core {
enum class VmxOperationKind : unsigned char;
struct TrapRequest {
    const TrapTargetKind TargetKind; const unsigned long long Target, Auxiliary; const TrapAccessType AccessType;
    const unsigned char VtId; const unsigned short ExecutionDomainTag, AddressSpaceTag;
    TrapRequest(TrapTargetKind kind = TrapTargetKind::None, unsigned long long target = 0, unsigned long long auxiliary = 0, TrapAccessType access = TrapAccessType::None, unsigned char vt = 0, unsigned short domain = 0, unsigned short space = 0) : TargetKind(kind), Target(target), Auxiliary(auxiliary), AccessType(access), VtId(vt), ExecutionDomainTag(domain), AddressSpaceTag(space) {}
    static std::shared_ptr<TrapRequest> ForInstruction(unsigned short opcode, unsigned char vt, unsigned short domain = 0, unsigned short space = 0) { return std::make_shared<TrapRequest>(TrapTargetKind::InstructionOpcode, opcode, 0, TrapAccessType::Execute, vt, domain, space); }
    static std::shared_ptr<TrapRequest> ForCsr(unsigned short csr, TrapAccessType access, unsigned char vt, unsigned short domain = 0, unsigned short space = 0) { return std::make_shared<TrapRequest>(TrapTargetKind::CsrAddress, csr, 0, access, vt, domain, space); }
    static std::shared_ptr<TrapRequest> ForCompatibilityOperation(unsigned char operation, unsigned short opcode, unsigned char vt, unsigned short domain = 0, unsigned short space = 0) { return std::make_shared<TrapRequest>(TrapTargetKind::CompatibilityOperation, operation, opcode, TrapAccessType::CompatibilityOperation, vt, domain, space); }
    static std::shared_ptr<TrapRequest> ForVmxOperation(VmxOperationKind operation, unsigned short opcode, unsigned char vt, unsigned short domain = 0, unsigned short space = 0) { return ForCompatibilityOperation(static_cast<unsigned char>(operation), opcode, vt, domain, space); }
    static std::shared_ptr<TrapRequest> ForMemory(unsigned long long address, unsigned long long length, TrapAccessType access, unsigned char vt, unsigned short domain = 0, unsigned short space = 0) { return std::make_shared<TrapRequest>(TrapTargetKind::MemoryRange, address, length, access, vt, domain, space); }
    static std::shared_ptr<TrapRequest> ForLaneOperation(unsigned char lane, unsigned short opcode, unsigned char vt, unsigned short domain = 0, unsigned short space = 0) { return std::make_shared<TrapRequest>(TrapTargetKind::LaneOperation, lane, opcode, TrapAccessType::LaneOperation, vt, domain, space); }
    static std::shared_ptr<TrapRequest> ForPreemptionTimer(unsigned char vt, unsigned short domain, unsigned short space, unsigned long long deadline) { return std::make_shared<TrapRequest>(TrapTargetKind::PreemptionTimer, deadline, 0, TrapAccessType::TimerExpiry, vt, domain, space); }
};
}
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'NeutralTrapResult.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('#pragma once',"#pragma once`r`n#include `"YAKSys_Hybrid_CPU.Core.TrapRequest.h`"`r`n#include <memory>`r`n#include <utility>")
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); if($i -lt 0){throw 'anchor missing'}
$s=$s.Substring(0,$i)+@'
class NeutralTrapResult final {
public:
    const bool ShouldTrap; const NeutralTrapResultKind Kind; const std::shared_ptr<TrapRequest> Request;
    NeutralTrapResult(bool shouldTrap, NeutralTrapResultKind kind, std::shared_ptr<TrapRequest> request) : ShouldTrap(shouldTrap), Kind(kind), Request(std::move(request)) {}
    static std::shared_ptr<NeutralTrapResult> Continue(std::shared_ptr<TrapRequest> request) { return std::make_shared<NeutralTrapResult>(false, NeutralTrapResultKind::None, std::move(request)); }
    static std::shared_ptr<NeutralTrapResult> Trap(std::shared_ptr<TrapRequest> request, NeutralTrapResultKind kind) { return std::make_shared<NeutralTrapResult>(kind != NeutralTrapResultKind::None, kind, std::move(request)); }
    static std::shared_ptr<NeutralTrapResult> Denied(std::shared_ptr<TrapRequest> request) { return Trap(std::move(request), NeutralTrapResultKind::SecurityPolicyViolation); }
};
}
'@
[IO.File]::WriteAllText($p,$s)