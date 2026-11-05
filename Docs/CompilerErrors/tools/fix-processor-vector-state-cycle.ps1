$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$h=Join-Path $root 'Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'; $c=Join-Path $root 'Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'
$hs=[IO.File]::ReadAllText($h); $cs=[IO.File]::ReadAllText($c)
$includes=@('#include "../../Pipeline/MicroOps/Vector/VectorMicroOps.Memory.h"','#include "../../Pipeline/MicroOps/Vector/VectorMicroOps.Data.h"','#include "../../State/CoreRuntimeState.h"')
foreach($inc in $includes){if(-not $hs.Contains($inc)){throw "Missing $inc"};$hs=$hs.Replace($inc,'')}
$own='#include "YAKSys_Hybrid_CPU.Processor.h"'; if(-not $cs.Contains($own)){throw 'Missing own include'}
$hs=$hs.Replace('#include "InstructionOpcode.h"','#include "InstructionOpcode.h"'+"`r`n"+'namespace YAKSys_Hybrid_CPU::Core { class VectorTransferMicroOp; class CoreRuntimeState; }')
$cs=$cs.Replace($own,$own+"`r`n"+($includes -join "`r`n"))
$v=Join-Path $root 'Pipeline\MicroOps\Vector\VectorMicroOps.h'; $vs=[IO.File]::ReadAllText($v); $unused='#include "../../../State/ExecutionState.h"'
if(-not $vs.Contains($unused)){throw 'Missing vector state include'}
$vs=$vs.Replace($unused,'')
[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($v,$vs,[Text.UTF8Encoding]::new($false))
