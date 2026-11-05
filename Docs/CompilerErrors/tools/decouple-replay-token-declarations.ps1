$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$jobs=@(
 @{ h='CloseToHSL\\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'; cpp='CloseToHSL\\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'; inc='../../Pipeline/MicroOps/Replay/ReplayToken.h' },
 @{ h='CloseToHSL\\Core\Pipeline\MicroOps\Memory\MicroOp.LoadStore.h'; cpp='CloseToHSL\\Core\Pipeline\MicroOps\Memory\MicroOp.LoadStore.cpp'; inc='../Replay/ReplayToken.h' }
)
foreach($job in $jobs){$line='#include "'+$job.inc+'"';$p=Join-Path $root $job.h;$s=Get-Content $p -Raw;$s=$s.Replace($line,'namespace HybridCPU_ISE::Core { class ReplayToken; }');Set-Content $p $s -Encoding UTF8 -NoNewline;$p=Join-Path $root $job.cpp;$s=Get-Content $p -Raw;if(!$s.Contains($line)){$own='#include "'+[System.IO.Path]::GetFileName($job.h)+'"';$s=$s.Replace($own,$own+"`r`n"+$line)};Set-Content $p $s -Encoding UTF8 -NoNewline}
