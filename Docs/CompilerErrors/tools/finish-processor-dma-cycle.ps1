$path='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'
$text=[IO.File]::ReadAllText($path)
$old='#include "../../../Memory/DMA/DMAController.h"'
$new='namespace HybridCPU_ISE::CloseToHSL::Memory::DMA { class DMAController; }'
if(-not $text.Contains($old)){throw 'Expected include not found'}
[IO.File]::WriteAllText($path,$text.Replace($old,$new),[Text.UTF8Encoding]::new($false))
