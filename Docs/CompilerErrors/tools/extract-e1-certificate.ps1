$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$p=Join-Path $root 'Pipeline\Safety\YAKSys_Hybrid_CPU.Core.SafetyVerifier.h'
$s=[IO.File]::ReadAllText($p)
$pattern='(?s)\t\tclass VirtualizationAdmissionCertificate final : public std::enable_shared_from_this<VirtualizationAdmissionCertificate>\s*\{.*?\r?\n\t\t\};'
$m=[regex]::Match($s,$pattern)
if(-not $m.Success) { throw 'Certificate declaration not found' }
$block=$m.Value
$out=Join-Path $root 'Pipeline\Safety\VirtualizationAdmissionCertificate.h'
$header="#pragma once`r`n#include <any>`r`n#include <memory>`r`nnamespace YAKSys_Hybrid_CPU::Core {`r`nenum class VmxOperationKind : unsigned char;`r`n"+$block+"`r`n}`r`n"
[IO.File]::WriteAllText($out,$header)
$s=$s.Replace($block,'        using VirtualizationAdmissionCertificate = YAKSys_Hybrid_CPU::Core::VirtualizationAdmissionCertificate;')
$s=$s.Replace('#include "SafetyVerifier.Types.h"',"#include `"VirtualizationAdmissionCertificate.h`"`r`n#include `"SafetyVerifier.Types.h`"")
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'Runtime\Events\Hypercalls\VirtualizationOperandSnapshot.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include "../../../Pipeline/Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"','#include "../../../Pipeline/Safety/VirtualizationAdmissionCertificate.h"')
$s=$s.Replace('SafetyVerifier::VirtualizationAdmissionCertificate','VirtualizationAdmissionCertificate')
[IO.File]::WriteAllText($p,$s)
