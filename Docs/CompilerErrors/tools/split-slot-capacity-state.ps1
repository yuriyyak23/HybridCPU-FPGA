$base=Split-Path -Parent (Split-Path -Parent $PSScriptRoot); $dir=Join-Path $base 'CloseToHSL\\Core\Pipeline\Scheduling\SlotLegality'
$h=Join-Path $dir 'SlotClassCapacity.h'; $cpp=Join-Path $dir 'SlotClassCapacity.cpp'; $state=Join-Path $dir 'SlotClassCapacityState.cpp'; $project=Join-Path $base 'DPCPPConsoleApplication1.vcxproj'
$ht=[IO.File]::ReadAllText($h); $ct=[IO.File]::ReadAllText($cpp); $pt=[IO.File]::ReadAllText($project)
if(Test-Path -LiteralPath $state){Write-Output 'Already split';exit 0}
$pattern='(?s)(?<block>\tbool SlotClassCapacityState::HasFreeCapacity\(.*?\n\tvoid SlotClassCapacityState::ResetOccupancy\(\)\s*\{[^{}]*\}\s*)(?=\s*// C# TO C\+\+ CONVERTER WARNING: Nullable reference types)'
$m=[regex]::Match($ct,$pattern); if(!$m.Success){throw 'Capacity-state block not found'}
$block=$m.Groups['block'].Value.Replace('inline int SlotClassCapacityState::','int SlotClassCapacityState::')
$out='#include "SlotClassCapacity.h"'+"`r`n`r`n"+'namespace YAKSys_Hybrid_CPU::Core {'+"`r`n"+$block+"`r`n}"+"`r`n"
$ct=[regex]::Replace($ct,$pattern,''); $ct=$ct.Replace('#include "SlotClassCapacity.h"','#include "SlotClassCapacity.h"'+"`r`n"+'#include "../../MicroOps/Types/MicroOp.h"')
$ht=$ht.Replace('#include "../../MicroOps/Types/MicroOp.h"','');$ht=$ht.Replace('inline int GetFreeCapacity','int GetFreeCapacity').Replace('inline int GetOccupied','int GetOccupied')
$anchor='<ClCompile Include="CloseToHSL\\Core\Pipeline\Scheduling\SlotLegality\SlotClassCapacity.cpp" />'
if(!$pt.Contains($anchor)){throw 'Project anchor missing'}
$pt=$pt.Replace($anchor,$anchor+"`r`n"+'    <ClCompile Include="CloseToHSL\\Core\Pipeline\Scheduling\SlotLegality\SlotClassCapacityState.cpp" />')
[IO.File]::WriteAllText($state,$out,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($cpp,$ct,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($project,$pt,[Text.UTF8Encoding]::new($false)); Write-Output 'Split state methods and removed header dependency'