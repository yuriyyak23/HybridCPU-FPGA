$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Safety'
$p=Join-Path $r 'SecureComputePlacedIntent.h'; $s=[IO.File]::ReadAllText($p)
$m1='#include "../../Decoder/Rf06ExecutionContracts.h"'; $m2='#include "../Scheduling/PostStageBIssuedAttempt.h"'
if(-not $s.Contains($m1) -or -not $s.Contains($m2)){throw 'Expected includes missing'}
$s=$s.Replace($m1,'namespace HybridCPU::ExternalRuntime::Contracts { class ExternalSecureRegisterLocalRequest; }').Replace($m2,"namespace YAKSys_Hybrid_CPU::Core::Decoder { class ScheduledOperation; }`r`nnamespace YAKSys_Hybrid_CPU::Core { class PostStageBIssuedAttempt; }")
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'SecureComputePlacedIntent.cpp'; $s=[IO.File]::ReadAllText($p)
$pattern='(?s)(SecureComputePlacedIntent::SecureComputePlacedIntent\([^\r\n]+\))\s*\{\s*Request = request;\s*ScheduledOperation = scheduledOperation;\s*IssuedAttempt = issuedAttempt;\s*\}'
if(([regex]::Matches($s,$pattern)).Count -ne 1){throw 'Expected constructor missing'}
$s=[regex]::Replace($s,$pattern,'$1 : Request(request), ScheduledOperation(scheduledOperation), IssuedAttempt(issuedAttempt) {}')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
