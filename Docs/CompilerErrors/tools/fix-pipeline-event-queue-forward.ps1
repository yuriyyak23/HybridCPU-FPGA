$path='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\IPipelineEventQueue.h'
$s=[IO.File]::ReadAllText($path); $marker="namespace YAKSys_Hybrid_CPU::Core::Pipeline`r`n{"
if(-not $s.Contains($marker)){throw 'Namespace marker not found'}
$s=$s.Replace($marker,$marker+"`r`n`tstruct PipelineEvent;")
[IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
