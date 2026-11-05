$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Safety'
$header = Join-Path $root 'YAKSys_Hybrid_CPU.Core.SafetyVerifier.h'
$source = Join-Path $root 'YAKSys_Hybrid_CPU.Core.SafetyVerifier.cpp'
$utf8 = [System.Text.UTF8Encoding]::new($false)
$h = [IO.File]::ReadAllText($header)
$include = '#include "../MicroOps/Types/MicroOp.IO.h"'
if (-not $h.Contains($include)) { throw 'Expected header include not found.' }
$h = $h.Replace($include, 'namespace YAKSys_Hybrid_CPU::Core { class VmxMicroOp; }')
$c = [IO.File]::ReadAllText($source)
$first = '#include "YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"'
if (-not $c.Contains($first)) { throw 'Expected source include not found.' }
$c = $c.Replace($first, $first + "`r`n" + $include)
[IO.File]::WriteAllText($header, $h, $utf8)
[IO.File]::WriteAllText($source, $c, $utf8)
