$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$paths=@('Pipeline\Scheduling\Smt\MicroOpScheduler.ResearchVirtualizationCanonicalComposition.h','Pipeline\Certificates\YAKSys_Hybrid_CPU.Core.RuntimeLegalityService.h','Runtime\Events\VmRead\GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition.h','Runtime\Events\VmRead\PinBasedControlsVmReadScalarDeliveryCanonicalComposition.h','Runtime\Events\VmRead\MemoryOwnedVmReadScalarDeliveryCanonicalComposition.h','Runtime\Events\VmRead\CompletionSecondStageVmReadScalarDeliveryCanonicalComposition.h','Runtime\Events\VmRead\CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition.h')
foreach($relative in $paths) {
  $h=Join-Path $root $relative; $cpp=[IO.Path]::ChangeExtension($h,'.cpp')
  $hs=[IO.File]::ReadAllText($h); $cs=[IO.File]::ReadAllText($cpp)
  $m=[regex]::Match($hs,'(?m)^#include "[^"\r\n]*MicroOp\.IO\.h"\r?$')
  if(-not $m.Success){throw "Missing include: $relative"}
  $inc=$m.Value.TrimEnd("`r")
  $name=[IO.Path]::GetFileName($h); $own='#include "'+$name+'"'
  if(-not $cs.Contains($own)){throw "Missing own include: $cpp"}
  $hs=$hs.Replace($inc,'namespace YAKSys_Hybrid_CPU::Core { class VmxMicroOp; }')
  $cs=$cs.Replace($own,$own+"`r`n"+$inc)
  [IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false))
  [IO.File]::WriteAllText($cpp,$cs,[Text.UTF8Encoding]::new($false))
}
