$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\Capabilities'
$p=Join-Path $r 'AcceleratorShapeCapability.h'; $s=[IO.File]::ReadAllText($p)
if(-not $s.Contains('unsigned char::MaxValue')){throw 'Expected rank default missing'}
$s=$s.Replace('#include <memory>',"#include <memory>`r`n#include <limits>").Replace('unsigned char::MaxValue','std::numeric_limits<unsigned char>::max()')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'AcceleratorOperationCapability.h'; $s=[IO.File]::ReadAllText($p)
$m='supportedShapes = const std::vector<AcceleratorShapeCapability>()'
if(-not $s.Contains($m)){throw 'Expected shape default missing'}
$s=$s.Replace($m,'supportedShapes = {}'); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
