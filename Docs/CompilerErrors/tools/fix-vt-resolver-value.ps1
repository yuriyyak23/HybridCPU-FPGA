$ErrorActionPreference='Stop'
$h='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\ExceptionsAndTraps\InvalidVirtualThreadException.h'
$s=[IO.File]::ReadAllText($h)
$marker='#include "../../Execution/ExecutionFaultContract.h"'
if (-not $s.Contains($marker) -or $s.Contains('#include "../Registers/Architectural/RegisterIdentity.h"')) {throw 'Unexpected header state'}
$s=$s.Replace($marker,$marker+"`r`n"+'#include "../Registers/Architectural/RegisterIdentity.h"')
[IO.File]::WriteAllText($h,$s,[Text.UTF8Encoding]::new($false))
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\ExceptionsAndTraps\InvalidVirtualThreadException.cpp'
$s=[IO.File]::ReadAllText($p)
if (-not $s.Contains('std::shared_ptr<VtId> resolvedVtId;') -or -not $s.Contains('return resolvedVtId->Value;')) {throw 'Unexpected resolver state'}
$s=$s.Replace('std::shared_ptr<VtId> resolvedVtId;','VtId resolvedVtId;').Replace('return resolvedVtId->Value;','return resolvedVtId.getValue();')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
