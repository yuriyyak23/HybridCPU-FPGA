$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Types\MicroOp.h'
$s=[IO.File]::ReadAllText($p)
$m='#include "../../Scheduling/SlotPlacementMetadata.h"'
if (-not $s.Contains($m) -or $s.Contains('TryGetArchitecturalOwnerVtId(')) { throw 'Unexpected header state' }
$s=$s.Replace($m,$m+"`r`n"+'#include "../../../Architecture/Registers/Architectural/RegisterIdentity.h"')
$m='        int getOwnerThreadId() const { return OwnerThreadId; }'
if (-not $s.Contains($m)) { throw 'Missing owner accessor' }
$s=$s.Replace($m,$m+"`r`n"+'        bool TryGetArchitecturalOwnerVtId(VtId& ownerVtId) const { return VtId::TryCreate(OwnerThreadId, ownerVtId); }')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
