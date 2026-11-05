$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'
$s=[IO.File]::ReadAllText($p)
foreach ($name in @('pipeCtrl','pipeIF')) {
    $s=$s.Replace($name+'::',$name+'.').Replace($name+'->',$name+'.')
}
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
