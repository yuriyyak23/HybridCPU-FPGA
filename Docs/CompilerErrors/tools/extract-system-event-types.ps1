$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$h=Join-Path $root 'Pipeline\MicroOps\System\MicroOp.System.h'; $dest=Join-Path $root 'Pipeline\MicroOps\System\SystemEventTypes.h'
if(Test-Path $dest){throw 'Destination exists'}
$s=[IO.File]::ReadAllText($h); $start=$s.IndexOf("`tenum class SystemEventKind")
$end=$s.IndexOf("`t/// <summary>`r`n`t/// System-event micro-operation",$start)
if($start -lt 0 -or $end -lt 0){throw 'Type block boundary missing'}
$block=$s.Substring($start,$end-$start)
$new="#pragma once`r`nnamespace YAKSys_Hybrid_CPU::Core`r`n{`r`n"+$block+"}`r`n"
$s=$s.Remove($start,$end-$start).Replace('#pragma once',"#pragma once`r`n#include `"SystemEventTypes.h`"")
[IO.File]::WriteAllText($dest,$new,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($h,$s,[Text.UTF8Encoding]::new($false))
$changes=@(@('Architecture\Common\YAKSys_Hybrid_CPU.Processor.h','../../Pipeline/MicroOps/System/'),@('Execution\YAKSys_Hybrid_CPU.Core.Execution.ExecutionDispatcherV4.h','../Pipeline/MicroOps/System/'))
foreach($item in $changes){
 $path=Join-Path $root $item[0]; $s=[IO.File]::ReadAllText($path); $old='#include "'+$item[1]+'MicroOp.System.h"'; $new='#include "'+$item[1]+'SystemEventTypes.h"'
 if(-not $s.Contains($old)){throw "Missing include: $path"}
 $s=$s.Replace($old,$new)
 if($item[0] -like 'Architecture*'){$s=$s.Replace($new,$new+"`r`n"+'namespace YAKSys_Hybrid_CPU::Core { class SysEventMicroOp; }')}
 [IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
 $cpp=[IO.Path]::ChangeExtension($path,'.cpp'); $s=[IO.File]::ReadAllText($cpp); $own='#include "'+[IO.Path]::GetFileName($path)+'"'
 if(-not $s.Contains($own)){throw "Missing own include: $cpp"}
 $s=$s.Replace($own,$own+"`r`n"+$old); [IO.File]::WriteAllText($cpp,$s,[Text.UTF8Encoding]::new($false))
}
