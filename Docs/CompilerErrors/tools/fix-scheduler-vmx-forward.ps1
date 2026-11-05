$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Assist\InterCore'
$h=Join-Path $dir 'YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h'
$c=Join-Path $dir 'YAKSys_Hybrid_CPU.Core.MicroOpScheduler.cpp'
$include='#include "../../MicroOps/Types/MicroOp.IO.h"'
$hs=[IO.File]::ReadAllText($h)
$cs=[IO.File]::ReadAllText($c)
$marker='#include "YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h"'
if (-not $hs.Contains($include) -or -not $cs.Contains($marker)) { throw 'Expected includes not found' }
$hs=$hs.Replace($include,'namespace YAKSys_Hybrid_CPU::Core { class VmxMicroOp; }')
$cs=$cs.Replace($marker,$marker+"`r`n"+$include)
[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false))
