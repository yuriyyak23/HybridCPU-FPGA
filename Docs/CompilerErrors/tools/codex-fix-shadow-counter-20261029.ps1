$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'
$s=[IO.File]::ReadAllText($p)
$old='if (pipeCtrl.CycleCount != 0 && pipeCtrl.Enabled)'
if (-not $s.Contains($old)) { throw 'Expected check missing' }
$s=$s.Replace($old,'if (pipeCtrl.getCycleCount() != 0 && pipeCtrl.Enabled)')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
