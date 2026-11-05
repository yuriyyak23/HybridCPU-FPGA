$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$p=Join-Path $r 'Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'; $s=[IO.File]::ReadAllText($p); $m='#include "../../Pipeline/MicroOps/Types/MicroOp.IO.h"'
if(-not $s.Contains($m)){throw 'Expected MicroOp.IO include not found'}
$s=$s.Replace($m,''); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'; $s=[IO.File]::ReadAllText($p); $m='#include "YAKSys_Hybrid_CPU.Processor.h"'
if(-not $s.Contains($m)){throw 'Expected processor source include missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "../../Pipeline/MicroOps/Types/MicroOp.IO.h"'); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Pipeline\Safety\SafetyVerifier.Types.h'; $s=[IO.File]::ReadAllText($p); $m='namespace YAKSys_Hybrid_CPU::Core { class LegalityDecision; }'
if(-not $s.Contains($m)){throw 'Expected declaration missing'}
$s=$s.Replace($m,'namespace YAKSys_Hybrid_CPU::Core { class LegalityDecision; class PhaseCertificateTemplate4Way; }'); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
