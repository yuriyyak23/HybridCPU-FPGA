$p=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Decoder\Rf06ExecutionContracts.h'
$s=[IO.File]::ReadAllText($p)
$decl='namespace YAKSys_Hybrid_CPU::Core::Decoder { class CanonicalScalarLoadAddressPlan; }'
if (!$s.Contains($decl)) {$s=$s.Replace('#include "CanonicalDecodedContracts.h"',$decl+"`r`n"+'#include "CanonicalDecodedContracts.h"')}
foreach ($line in @('using namespace HybridCPU_ISE::Arch;', 'using namespace YAKSys_Hybrid_CPU::Arch::Generated;', 'using namespace YAKSys_Hybrid_CPU::Core::Pipeline::Scheduling;')) { $s=$s.Replace($line,'') }
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false));Write-Output 'Updated forward declaration and unused namespace imports'
