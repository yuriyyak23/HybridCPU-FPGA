$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Decoder'
$p=Join-Path $r 'DecodedBundleDescriptor.h'; $s=[IO.File]::ReadAllText($p); $m='#include "../Pipeline/MicroOps/Types/MicroOp.IO.h"'
if(-not $s.Contains($m)){throw 'Expected MicroOp.IO include missing'}
$s=$s.Replace($m,'namespace YAKSys_Hybrid_CPU::Core { class TrapMicroOp; }')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'DecodedBundleDescriptor.cpp'; $s=[IO.File]::ReadAllText($p); $m='#include "DecodedBundleDescriptor.h"'
if(-not $s.Contains($m)){throw 'Expected source include missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "../Pipeline/MicroOps/Types/MicroOp.IO.h"')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
