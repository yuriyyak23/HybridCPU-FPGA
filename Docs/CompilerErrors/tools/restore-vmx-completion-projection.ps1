$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Frontend\Projection\Completion'
$p=Join-Path $base 'CompletionProjectionService.h'; $s=@'
#pragma once
#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include <memory>
namespace YAKSys_Hybrid_CPU::Core {
struct VmxCompletionProjection {
    const VmExitReason ExitReason; const unsigned long long ExitQualification, GuestPhysicalAddress, EptViolationQualification;
    VmxCompletionProjection(VmExitReason reason, unsigned long long qualification, unsigned long long address, unsigned long long aux)
        : ExitReason(reason), ExitQualification(qualification), GuestPhysicalAddress(address), EptViolationQualification(aux) {}
    static const std::shared_ptr<VmxCompletionProjection> None;
};
inline const std::shared_ptr<VmxCompletionProjection> VmxCompletionProjection::None = std::make_shared<VmxCompletionProjection>(VmExitReason::None, 0, 0, 0);
}
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.CompletionProjectionService.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('#include <memory>',"#include `"CompletionProjectionService.h`"`r`n#include <memory>"); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.CompletionProjectionService.cpp'; $s=[IO.File]::ReadAllText($p)
$enum=[IO.File]::ReadAllText((Join-Path $base '../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h')); $names=[regex]::Matches($enum,'(?m)^\s*([A-Za-z][A-Za-z0-9_]*)\s*=\s*(?:0x[0-9A-Fa-f]+|[0-9]+)\s*,?\s*$') | ForEach-Object {$_.Groups[1].Value}; if($names.Count -lt 10){throw 'enum names missing'}
$cases=($names | ForEach-Object {"        case VmExitReason::$($_): return true;"}) -join "`r`n"
$func="namespace { bool IsDefinedVmExitReason(unsigned int value) { switch (static_cast<VmExitReason>(value)) {`r`n$cases`r`n        default: return false;`r`n    } } }`r`n"
$s=$s.Replace('std::shared_ptr<VmxCompletionProjection> CompletionProjectionService::ProjectToVmx',"$func`r`n`tstd::shared_ptr<VmxCompletionProjection> CompletionProjectionService::ProjectToVmx").Replace('Enum::IsDefined(typeof(VmExitReason), record->getReasonCode())','IsDefinedVmExitReason(record->getReasonCode())'); [IO.File]::WriteAllText($p,$s)