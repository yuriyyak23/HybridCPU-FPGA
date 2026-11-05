$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$control=Join-Path $root 'Pipeline\MicroOps\Control\MicroOp.Control.h'
$dest=Join-Path $root 'Pipeline\MicroOps\Control\ControlRetireTypes.h'
if(Test-Path $dest){throw 'Destination already exists'}
$s=[IO.File]::ReadAllText($control)
$start=$s.IndexOf("`tenum class CsrStorageSurface")
$end=$s.IndexOf("`t/// <summary>`r`n`t/// CSR (Control & Status Register) micro-operation",$start)
if($start -lt 0 -or $end -lt 0){throw 'Type block boundaries missing'}
$block=$s.Substring($start,$end-$start)
$new="#pragma once`r`n#include `"../../../Diagnostics/TraceEventKind.h`"`r`n`r`nnamespace YAKSys_Hybrid_CPU::Core`r`n{`r`n"+$block+"}`r`n"
$s=$s.Remove($start,$end-$start)
$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"ControlRetireTypes.h`"")
[IO.File]::WriteAllText($dest,$new,[Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText($control,$s,[Text.UTF8Encoding]::new($false))
$changes=@(
 @('Architecture\Common\YAKSys_Hybrid_CPU.Processor.h','../../Pipeline/MicroOps/Control/'),
 @('Execution\YAKSys_Hybrid_CPU.Core.Execution.ExecutionDispatcherV4.h','../Pipeline/MicroOps/Control/')
)
foreach($change in $changes){
 $path=Join-Path $root $change[0]; $text=[IO.File]::ReadAllText($path)
 $old='#include "'+$change[1]+'MicroOp.Control.h"'; $replacement='#include "'+$change[1]+'ControlRetireTypes.h"'
 if(-not $text.Contains($old)){throw "Missing include: $path"}
 $text=$text.Replace($old,$replacement)
 if($change[0] -like 'Architecture*'){$text=$text.Replace($replacement,$replacement+"`r`n"+'namespace YAKSys_Hybrid_CPU::Core { class BranchExecutionPayload; class BranchMicroOp; class CSRMicroOp; }')}
 [IO.File]::WriteAllText($path,$text,[Text.UTF8Encoding]::new($false))
 $cpp=[IO.Path]::ChangeExtension($path,'.cpp'); $cs=[IO.File]::ReadAllText($cpp); $own='#include "'+[IO.Path]::GetFileName($path)+'"'
 if(-not $cs.Contains($own)){throw "Missing own include: $cpp"}
 $cs=$cs.Replace($own,$own+"`r`n"+$old); [IO.File]::WriteAllText($cpp,$cs,[Text.UTF8Encoding]::new($false))
}
