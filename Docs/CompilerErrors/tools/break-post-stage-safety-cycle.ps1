$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Scheduling'
$p=Join-Path $r 'PostStageBIssuedAttempt.h'; $s=[IO.File]::ReadAllText($p)
$m1='#include "../Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"'; $m2='#include "../Safety/SecureComputeIssuePermitOwner.h"'
if(-not $s.Contains($m1) -or -not $s.Contains($m2)){throw 'Expected safety includes missing'}
$s=$s.Replace($m1,'namespace YAKSys_Hybrid_CPU::Core { class SafetyVerifier; class SecureComputePreparedMachineAdmission; class LaneId; }').Replace($m2,'')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'PostStageBIssuedAttempt.cpp'; $s=[IO.File]::ReadAllText($p); $m='#include "PostStageBIssuedAttempt.h"'
if(-not $s.Contains($m)){throw 'Expected source header missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "../Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"'+"`r`n"+'#include "../Safety/SecureComputeIssuePermitOwner.h"')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
