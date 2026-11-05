$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExecutionFaultContract.h'
$s=[IO.File]::ReadAllText($p)
$m='namespace YAKSys_Hybrid_CPU::Core'
if (-not $s.Contains($m) -or $s.Contains('class ArgumentOutOfRangeException;')) { throw 'Unexpected header state' }
$s=$s.Replace($m,"class InvalidOperationException;`r`nclass ArgumentOutOfRangeException;`r`n`r`n"+$m)
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
