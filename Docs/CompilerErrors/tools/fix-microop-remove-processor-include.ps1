$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Types\MicroOp.h'
$lines=[System.Collections.Generic.List[string]]::new()
[IO.File]::ReadAllLines($p) | ForEach-Object { if($_ -notmatch '^\s*#include "\.\./\.\./\.\./Architecture/Common/YAKSys_Hybrid_CPU\.Processor\.h"\s*$'){$lines.Add($_)} }
[IO.File]::WriteAllLines($p,$lines,[Text.UTF8Encoding]::new($false))
$text=[IO.File]::ReadAllText($p)
$text=$text.Replace('namespace YAKSys_Hybrid_CPU { class Processor; }','namespace YAKSys_Hybrid_CPU { class Processor { public: class CPU_Core; }; }')
[IO.File]::WriteAllText($p,$text,[Text.UTF8Encoding]::new($false))
if(Select-String -Quiet -Path $p -Pattern 'YAKSys_Hybrid_CPU\.Processor\.h'){throw 'Full Processor header must not be included here'}
