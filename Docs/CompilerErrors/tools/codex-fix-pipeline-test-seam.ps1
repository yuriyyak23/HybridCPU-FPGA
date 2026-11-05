$ErrorActionPreference='Stop'
$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$src=Join-Path $root 'CloseToHSL\\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'
$hdr=Join-Path $root 'CloseToHSL\\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'
$s=[IO.File]::ReadAllText($src)
$begin=$s.IndexOf('bool Processor::CPU_Core::TestCheckPipelineHazardForExplicitExecuteLaneNotReady()')
$end=$s.IndexOf('inline void Processor::CPU_Core::ApplyControlFlowDominanceBeforeRedirect()', $begin)
if($begin -lt 0 -or $end -lt 0){throw 'Hazard test block boundaries not found.'}
$block=$s.Substring($begin,$end-$begin)
$replacements=[ordered]@{'pipeID->'='getRuntime()->getDecode()->Decode.';'pipeEX->'='getRuntime()->getExecution()->Execute.';'pipeMEM->'='getRuntime()->getMemoryPipeline()->Memory.';'pipeWB->'='getRuntime()->getMemoryPipeline()->WriteBack.';'pipeEX::SetLane'='getRuntime()->getExecution()->Execute.SetLane';'pipeMEM::SetLane'='getRuntime()->getMemoryPipeline()->Memory.SetLane'}
foreach($key in $replacements.Keys){$block=$block.Replace($key,$replacements[$key])}
$s=$s.Substring(0,$begin)+$block+$s.Substring($end)
[IO.File]::WriteAllText($src,$s,[Text.UTF8Encoding]::new($false))
$h=[IO.File]::ReadAllText($hdr)
$old="`t`t`tclass LiveAdapterWritebackReservation;`n`t`tpublic:"
$new="`t`t`tclass LiveAdapterWritebackReservation;`n`t`t`tclass PipelineCycleStallDecision;`n`t`tpublic:"
if($h.Contains($old)){$h=$h.Replace($old,$new);[IO.File]::WriteAllText($hdr,$h,[Text.UTF8Encoding]::new($false))}elseif(-not $h.Contains($new)){throw 'CPU_Core forward-declaration anchor not found.'}
'Pipeline hazard test seam fix applied (runtime stage owners and nested decision type).'