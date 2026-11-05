$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\DmaStreamCompute\DmaStreamComputeToken.h'
$s=[IO.File]::ReadAllText($p); $m='class DmaStreamComputeDescriptor;'
if(-not $s.Contains($m) -or $s.Contains('class DmaStreamComputeValidationResult;')){throw 'Unexpected token state'}
$s=$s.Replace($m,$m+' class DmaStreamComputeValidationResult;'); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
