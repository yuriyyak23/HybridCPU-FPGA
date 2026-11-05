$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\Descriptors\AcceleratorDescriptorParser.h'
$s=[IO.File]::ReadAllText($p)
$line='using namespace YAKSys_Hybrid_CPU::Arch;'
if (-not $s.Contains($line)) { throw 'Expected namespace directive missing' }
$s=$s.Replace($line + "`r`n", '')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
