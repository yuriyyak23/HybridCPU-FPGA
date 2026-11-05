$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Observation\DomainCompletionObservationOwner.h'
$s=[IO.File]::ReadAllText($p)
$old='#include "../Commit/ArchitecturalCompletionCommitOwner.h"'
if (-not $s.Contains($old)) { throw 'Expected include missing' }
$s=$s.Replace($old,'')
$s=$s.Replace('namespace YAKSys_Hybrid_CPU::Core' + "`r`n" + '{','namespace YAKSys_Hybrid_CPU::Core' + "`r`n" + '{' + "`r`n`tclass ArchitecturalCompletionCommitOwner;`r`n`tclass ArchitecturalCompletionReceiptBinding;`r`n`tstruct CompletionObservationScope;`r`n`tstruct NeutralCompletionObservationSnapshot;`r`n`tstruct CompletionObservationResult;`r`n`tstruct NeutralCompletionFieldResult;")
[IO.File]::WriteAllText($p,$s)
$cpp='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Observation\DomainCompletionObservationOwner.cpp'
$c=[IO.File]::ReadAllText($cpp)
$inc='#include "../Commit/ArchitecturalCompletionCommitOwner.h"'
if(-not $c.Contains($inc)) { $c=$c.Replace('#include "DomainCompletionObservationOwner.h"', '#include "DomainCompletionObservationOwner.h"' + "`r`n" + $inc); [IO.File]::WriteAllText($cpp,$c) }
