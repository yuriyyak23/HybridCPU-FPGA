$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Events\Hypercalls'
$h=Join-Path $dir 'DomainHypercallExactRuntimeProfile.h'; $c=Join-Path $dir 'DomainHypercallExactRuntimeProfile.cpp'
$hs=[IO.File]::ReadAllText($h); $cs=[IO.File]::ReadAllText($c)
$inc='#include "../../Migration/Checkpoint/DomainHypercallDrainLifecycleOwner.h"'; $own='#include "DomainHypercallExactRuntimeProfile.h"'
if(-not $hs.Contains($inc) -or -not $cs.Contains($own)){throw 'Expected include missing'}
$hs=$hs.Replace($inc,'namespace YAKSys_Hybrid_CPU::Core { class DomainHypercallDrainLifecycleOwner; }')
$cs=$cs.Replace($own,$own+"`r`n"+$inc)
[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false))
