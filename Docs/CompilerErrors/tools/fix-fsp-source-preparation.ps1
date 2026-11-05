$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\State';$h=Join-Path $dir 'AdmissionState.h';$c=Join-Path $dir 'AdmissionState.cpp';$hs=[IO.File]::ReadAllText($h);$cs=[IO.File]::ReadAllText($c)
foreach($name in @('SmtCandidates','AssistCandidates')) {
  $old='const std::vector<std::shared_ptr<MicroOp>> '+$name+' =';$new='std::vector<std::shared_ptr<MicroOp>> '+$name+' ='; if(-not $hs.Contains($old)){throw "Missing array $name"};$hs=$hs.Replace($old,$new)
  $hs=$hs.Replace('std::vector<std::shared_ptr<MicroOp>> get'+$name+'() const;','std::vector<std::shared_ptr<MicroOp>>& get'+$name+'();')
  $cs=$cs.Replace('std::vector<std::shared_ptr<MicroOp>> FspSourcePreparation::get'+$name+'() const','std::vector<std::shared_ptr<MicroOp>>& FspSourcePreparation::get'+$name+'()')
}
$old="`t{`r`n`t`tScheduler = scheduler;`r`n`t`tBundlePc = bundlePc;`r`n`t`tSourceStateEpoch = sourceStateEpoch;`r`n`t}"; if(-not $cs.Contains($old)){throw 'Constructor body missing'}
$cs=$cs.Replace($old,"`t`t: Scheduler(std::move(scheduler)), BundlePc(bundlePc), SourceStateEpoch(sourceStateEpoch)`r`n`t{`r`n`t}")
$cs=$cs.Replace('#include "AdmissionState.h"',"#include `"AdmissionState.h`"`r`n#include <utility>")
[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false))
