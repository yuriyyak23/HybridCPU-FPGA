$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$h=Join-Path $root 'Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'; $c=Join-Path $root 'Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'
$hs=[IO.File]::ReadAllText($h); $cs=[IO.File]::ReadAllText($c)
$includes=@('#include "../../Pipeline/MicroOps/Types/MicroOp.Misc.h"','#include "../../Pipeline/MicroOps/Memory/MicroOp.LoadStore.h"')
foreach($inc in $includes){if(-not $hs.Contains($inc)){throw "Missing include $inc"}; $hs=$hs.Replace($inc,'')}
$marker='#include "YAKSys_Hybrid_CPU.Processor.h"'; if(-not $cs.Contains($marker)){throw 'Missing own include'}
$cs=$cs.Replace($marker,$marker+"`r`n"+($includes -join "`r`n"))
$hs=$hs.Replace('#include "InstructionOpcode.h"', '#include "InstructionOpcode.h"'+"`r`n"+'namespace YAKSys_Hybrid_CPU::Core { class AtomicMicroOp; class NopMicroOp; class LoadStoreMicroOp; class LoadMicroOp; class StoreMicroOp; }')
[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false))
