$ErrorActionPreference = 'Stop'
$p = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\Descriptors\MatMulDescriptor.cpp'
$s = [IO.File]::ReadAllText($p)
$old = '#include "MatMulDescriptor.h"'
$new = $old + "`r`n" + '#include "../Auth/AcceleratorOwnerDomainGuard.h"'
if (-not $s.Contains($old) -or $s.Contains('#include "../Auth/AcceleratorOwnerDomainGuard.h"')) { throw 'Unexpected include state' }
$s = $s.Replace($old, $new)
$old = 'if (commandDescriptor)'
if (-not $s.Contains($old)) { throw 'Expected optional check missing' }
$s = $s.Replace($old, 'if (commandDescriptor && *commandDescriptor)')
$old = 'if (!commandDescriptor->OwnerGuardDecision->IsAllowed)'
if (-not $s.Contains($old)) { throw 'Expected guard check missing' }
$s = $s.Replace($old, 'if (!commandDescriptor->OwnerGuardDecision || !commandDescriptor->OwnerGuardDecision->IsAllowed)')
[IO.File]::WriteAllText($p, $s, [Text.UTF8Encoding]::new($false))
