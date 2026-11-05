$base=Split-Path -Parent (Split-Path -Parent $PSScriptRoot); $folder=Join-Path $base 'CloseToHSL\\Core\Pipeline\Scheduling\SlotLegality'
$h=Join-Path $folder 'SlotClassDefinitions.h'; $c=Join-Path $folder 'SlotClassDefinitions.cpp'; $cap=Join-Path $folder 'SlotClassCapacity.cpp'; $project=Join-Path $base 'DPCPPConsoleApplication1.vcxproj'; $newfile=Join-Path $folder 'SlotClassLaneMap.cpp'
$header=[IO.File]::ReadAllText($h); $cpp=[IO.File]::ReadAllText($c); $ct=[IO.File]::ReadAllText($cap); $proj=[IO.File]::ReadAllText($project)
if($header.Contains('static ReadOnlySpan<SlotClass> GetAliasedClasses')) {
 $header=$header.Replace('#include <vector>', '#include <span>' + "`r`n" + '#include <vector>')
 $header=$header.Replace('static ReadOnlySpan<SlotClass> GetAliasedClasses','static std::span<const SlotClass> GetAliasedClasses')
 $header=$header.Replace('inline static unsigned char GetLaneMask','static unsigned char GetLaneMask').Replace('inline static int GetClassCapacity','static int GetClassCapacity').Replace('inline static bool HasAliasedLanes','static bool HasAliasedLanes')
 $pattern='(?s)\s*inline unsigned char SlotClassLaneMap::GetLaneMask\(.*?(?=\s*SlotClass TypedSlotBundleFacts::getSlot0Class\(\) const)'
 if([regex]::Matches($cpp,$pattern).Count -ne 1){throw 'Expected exactly one lane map block'}
 $cpp=[regex]::Replace($cpp,$pattern,"`r`n")
 $ct=$ct.Replace('ReadOnlySpan<SlotClass> aliased = SlotClassLaneMap::GetAliasedClasses(slotClass);','auto aliased = SlotClassLaneMap::GetAliasedClasses(slotClass);')
 $anchor='<ClCompile Include="CloseToHSL\\Core\Pipeline\Scheduling\SlotLegality\SlotClassDefinitions.cpp" />'
 if(!$proj.Contains($anchor)){throw 'Project anchor missing'}
 $proj=$proj.Replace($anchor,$anchor+"`r`n"+'    <ClCompile Include="CloseToHSL\\Core\Pipeline\Scheduling\SlotLegality\SlotClassLaneMap.cpp" />')
 [IO.File]::WriteAllText($h,$header,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($c,$cpp,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($cap,$ct,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($project,$proj,[Text.UTF8Encoding]::new($false))
} else {Write-Output 'Header already converted'}
if(!(Test-Path -LiteralPath $newfile)){throw 'Expected SlotClassLaneMap.cpp to be installed alongside this script'}