$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\DmaStreamCompute'
$p=Join-Path $r 'DmaStreamComputeToken.h'; $s=[IO.File]::ReadAllText($p); $m='#include "../../Memory/MemoryCoherencyObserver.h"'
if(-not $s.Contains($m)){throw 'Expected memory observer include missing'}
$s=$s.Replace($m,'namespace YAKSys_Hybrid_CPU::Core::Memory { class MemoryCoherencyObserver; }')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'DmaStreamComputeToken.cpp'; $s=[IO.File]::ReadAllText($p); $m='#include "DmaStreamComputeToken.h"'
if(-not $s.Contains($m)){throw 'Expected token include missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "../../Memory/MemoryCoherencyObserver.h"')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
