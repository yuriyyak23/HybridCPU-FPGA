$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$h=Join-Path $root 'CloseToHSL\\Core\Decoder\CanonicalDecodedContracts.h'; $c=Join-Path $root 'CloseToHSL\\Core\Decoder\CanonicalDecodedContracts.cpp'; $proj=Join-Path $root 'DPCPPConsoleApplication1.vcxproj'
$ht=[IO.File]::ReadAllText($h); $ct=[IO.File]::ReadAllText($c); $pt=[IO.File]::ReadAllText($proj)
if (!$ht.Contains('#include "SemanticInstructionKey.h"')) {
 $ht=$ht.Replace('#include "CanonicalPayloadSnapshot.h"','#include "CanonicalPayloadSnapshot.h"'+"`r`n"+'#include "SemanticInstructionKey.h"')
 $pattern='(?s)\tclass SemanticInstructionKey\s*\{.*?\n\t\};'
 if([regex]::Matches($ht,$pattern).Count -ne 1){throw 'Key declaration missing'}
 $ht=[regex]::Replace($ht,$pattern,'')
 $pattern='(?s)\tSemanticInstructionKey::SemanticInstructionKey\(.*?(?=\s*CanonicalScalarLoadAddressPlan::CanonicalScalarLoadAddressPlan\()'
 if([regex]::Matches($ct,$pattern).Count -ne 1){throw 'Key definitions boundary missing'}
 $ct=[regex]::Replace($ct,$pattern,'')
}
$anchor='<ClCompile Include="CloseToHSL\\Core\Decoder\CanonicalPayloadSnapshot.cpp" />'
if(!$pt.Contains('<ClCompile Include="CloseToHSL\\Core\Decoder\SemanticInstructionKey.cpp" />')) {if(!$pt.Contains($anchor)){throw 'Project anchor missing'}; $pt=$pt.Replace($anchor,$anchor+"`r`n"+'    <ClCompile Include="CloseToHSL\\Core\Decoder\SemanticInstructionKey.cpp" />')}
[IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$ct,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($proj,$pt,[Text.UTF8Encoding]::new($false));Write-Output 'Separated semantic key'
