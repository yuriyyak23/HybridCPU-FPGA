$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common';$h=Join-Path $dir 'YAKSys_Hybrid_CPU.Processor.h';$c=Join-Path $dir 'YAKSys_Hybrid_CPU.Processor.cpp';$hs=[IO.File]::ReadAllText($h);$cs=[IO.File]::ReadAllText($c)
$inc='#include "../../../Memory/DMA/DMAController.h"';$own='#include "YAKSys_Hybrid_CPU.Processor.h"';if(-not $hs.Contains($inc) -or -not $cs.Contains($own)){throw 'Expected include missing'}
$hs=$hs.Replace($inc,'namespace HybridCPU_ISE::CloseToHSL::Memory::DMA { class DMAController; }');$cs=$cs.Replace($own,$own+"`r`n"+$inc)
[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false))
