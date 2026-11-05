$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Types\MicroOp.h'
$s=[IO.File]::ReadAllText($p)
if($s -notmatch 'SetHardPinnedPlacement' -or $s -match '(?s)\}\s*;\s*\}\s*$'){throw 'Unexpected header state'}
$s += "`r`n    // Remaining execution and retire methods require conversion from MicroOp.cs.`r`n};`r`n}`r`n"
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
