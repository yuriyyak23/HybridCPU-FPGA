$d='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\Descriptors'
$p=Join-Path $d 'ExecutionDomain\YAKSys_Hybrid_CPU.Core.ExecutionDomainDescriptor.cpp'; $s=[IO.File]::ReadAllText($p).Replace('->HasAnyMaterializedGuestArchitecturalState','->getHasAnyMaterializedGuestArchitecturalState()'); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $d 'MemoryDomain\YAKSys_Hybrid_CPU.Core.MemoryDomainDescriptor.cpp'; $s=[IO.File]::ReadAllText($p).Replace('->IsValid','->isValid()'); [IO.File]::WriteAllText($p,$s)
