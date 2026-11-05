$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\Certificates\ReplayPhaseSubstrate.Interfaces.h'; $s=Get-Content $p -Raw
$includes=@('#include "../MicroOps/Types/MicroOp.h"','#include "../Safety/SafetyVerifier.Types.h"','#include "ReplayPhaseSubstrate.Implementations.h"','#include "YAKSys_Hybrid_CPU.Core.BundleResourceCertificate4Way.h"','#include "BundleResourceCertificate4Way.h"','#include "../Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"')
foreach($line in $includes){$s=$s.Replace($line,'')}
$s=$s.Replace('namespace YAKSys_Hybrid_CPU::Core { class LegalityDecision;', 'namespace YAKSys_Hybrid_CPU::Core { class MicroOp; class BundleResourceCertificate4Way; enum class CertificateRejectDetail : unsigned char; class LegalityDecision;')
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\Certificates\ReplayPhaseSubstrate.Interfaces.cpp'; $s=Get-Content $p -Raw
if (!$s.Contains('#include "ReplayPhaseSubstrate.Implementations.h"')) { $s=$s.Replace('#include "ReplayPhaseSubstrate.Interfaces.h"', "#include `"ReplayPhaseSubstrate.Interfaces.h`"`r`n#include `"ReplayPhaseSubstrate.Implementations.h`"") }
Set-Content $p $s -Encoding UTF8 -NoNewline
