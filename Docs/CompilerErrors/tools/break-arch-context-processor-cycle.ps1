$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common'
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Processor.h'; $s=[IO.File]::ReadAllText($p)
$m='#include "../Registers/Architectural/ArchContextState.h"'
if(-not $s.Contains($m)){throw 'Expected ArchContextState include missing'}
$s=$s.Replace($m,'namespace YAKSys_Hybrid_CPU::Core::Registers { class ArchContextState; class ArchContextCollection; enum class ArchContextWriteKind; }')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Processor.cpp'; $s=[IO.File]::ReadAllText($p); $m='#include "YAKSys_Hybrid_CPU.Processor.h"'
if(-not $s.Contains($m)){throw 'Expected processor include missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "../Registers/Architectural/ArchContextState.h"')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
