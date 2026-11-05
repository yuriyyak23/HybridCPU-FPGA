$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Records'
$h=Join-Path $dir 'SecureComputeNoEffectCompletionOwner.h'; $c=Join-Path $dir 'SecureComputeNoEffectCompletionOwner.cpp'
$hs=[IO.File]::ReadAllText($h); $cs=[IO.File]::ReadAllText($c)
$inc='#include "../../../Architecture/Registers/Architectural/ArchContextState.h"'; $own='#include "SecureComputeNoEffectCompletionOwner.h"'
if(-not $hs.Contains($inc) -or -not $cs.Contains($own)){throw 'Expected include missing'}
$hs=$hs.Replace($inc,'namespace YAKSys_Hybrid_CPU::Core::Registers { class ArchContextState; }')
$cs=$cs.Replace($own,$own+"`r`n"+$inc)
[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false))
