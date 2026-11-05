$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common'
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Processor.h'; $s=[IO.File]::ReadAllText($p); $m='#include "../../Execution/ExternalAccelerators/ExternalAcceleratorRuntime.h"'
if(-not $s.Contains($m)){throw 'Expected accelerator runtime include missing'}
$s=$s.Replace($m,'namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators { class ExternalAcceleratorRuntime; }')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Processor.cpp'; $s=[IO.File]::ReadAllText($p); $m='#include "YAKSys_Hybrid_CPU.Processor.h"'
if(-not $s.Contains($m)){throw 'Expected source include missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "../../Execution/ExternalAccelerators/ExternalAcceleratorRuntime.h"')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
