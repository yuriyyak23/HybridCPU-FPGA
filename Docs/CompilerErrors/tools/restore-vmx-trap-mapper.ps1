$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Frontend'
$p=Join-Path $base 'Retire\VmxRetireModel.h'; $s=[IO.File]::ReadAllText($p); $m=[regex]::Match($s,'(?s)enum class VmxOperationKind : unsigned char\s*\{.*?\};'); if(-not $m.Success){throw 'enum missing'}
$enum="#pragma once`r`nnamespace YAKSys_Hybrid_CPU::Core {`r`n"+$m.Value+"`r`n}`r`n"; [IO.File]::WriteAllText((Join-Path $base 'Retire\VmxOperationKind.h'),$enum)
$s=$s.Replace($m.Value,'').Replace('#pragma once',"#pragma once`r`n#include `"VmxOperationKind.h`""); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'Projection\Events\TrapDecision.h'; $s=@'
#pragma once
#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include "../../../FrozenAbi/VmcsFieldAliases/VmxExitQualification.h"
#include "../../../../../Runtime/Events/Traps/YAKSys_Hybrid_CPU.Core.TrapRequest.h"
#include <memory>
namespace YAKSys_Hybrid_CPU::Core {
class TrapDecision {
public:
    const bool ShouldExit; const VmExitReason ExitReason; const std::shared_ptr<VmxExitQualification> ExitQualification; const std::shared_ptr<TrapRequest> Request;
    TrapDecision(bool exit, VmExitReason reason, std::shared_ptr<VmxExitQualification> qualification, std::shared_ptr<TrapRequest> request) : ShouldExit(exit), ExitReason(reason), ExitQualification(std::move(qualification)), Request(std::move(request)) {}
    static std::shared_ptr<TrapDecision> NoExit(std::shared_ptr<TrapRequest> request) { return std::make_shared<TrapDecision>(false,VmExitReason::None,VmxExitQualification::None,std::move(request)); }
    static std::shared_ptr<TrapDecision> Exit(std::shared_ptr<TrapRequest> request, VmExitReason reason) {
        auto leaf=static_cast<unsigned short>((static_cast<unsigned char>(request->TargetKind) << 8) | static_cast<unsigned char>(request->AccessType));
        auto descriptor=request->TargetKind == TrapTargetKind::CompatibilityOperation ? request->Target : request->Target | ((request->Auxiliary & 0xffffULL) << 16);
        return std::make_shared<TrapDecision>(true,reason,std::make_shared<VmxExitQualification>(leaf,VmxInvalidationScope::None,descriptor),std::move(request));
    }
};
}
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'Projection\Events\VmxTrapProjectionMapper.h'; $s=[IO.File]::ReadAllText($p).Replace('#include <memory>',"#include `"TrapDecision.h`"`r`n#include <memory>"); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'Projection\Events\VmxTrapProjectionMapper.cpp'; $s=[IO.File]::ReadAllText($p).Replace('#include "../../Retire/VmxRetireModel.h"','#include "../../Retire/VmxOperationKind.h"')
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: The following 'switch expression'"); $end=$s.IndexOf("`n`t}",$i); if($i -lt 0 -or $end -lt 0){throw 'switch anchor missing'}
$impl=@'
switch(kind) {
        case NeutralTrapResultKind::None: return VmExitReason::None;
        case NeutralTrapResultKind::InstructionIntercept: return VmExitReason::InstructionIntercept;
        case NeutralTrapResultKind::CsrIntercept: return VmExitReason::CsrIntercept;
        case NeutralTrapResultKind::CompatibilityOperationIntercept: return VmExitReason::VmxOperationIntercept;
        case NeutralTrapResultKind::MemoryIntercept: return VmExitReason::MemoryIntercept;
        case NeutralTrapResultKind::LaneOperationIntercept: return VmExitReason::LaneOperationIntercept;
        case NeutralTrapResultKind::PreemptionTimerExpired: return VmExitReason::VmxPreemptionTimerExpired;
        case NeutralTrapResultKind::SecurityPolicyViolation: return VmExitReason::SecurityPolicyViolation;
        default: return VmExitReason::SecurityPolicyViolation;
        }
'@
$s=$s.Substring(0,$i)+$impl+$s.Substring($end); [IO.File]::WriteAllText($p,$s)