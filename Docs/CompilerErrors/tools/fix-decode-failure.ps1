$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$h=Join-Path $root 'CloseToHSL\\Core\Decoder\CanonicalDecodedContracts.h';$p=Join-Path $root 'DPCPPConsoleApplication1.vcxproj'
$ht=[IO.File]::ReadAllText($h);$pt=[IO.File]::ReadAllText($p)
if (!$ht.Contains('#include "DecodeFailure.h"')) {
 $ht=$ht.Replace('#include "SemanticInstructionKey.h"','#include "SemanticInstructionKey.h"'+"`r`n"+'#include "DecodeFailure.h"')
 $pattern='(?s)\tenum class DecodeFailureCode\s*:\s*unsigned short\s*\{.*?\n\t\};'
 if([regex]::Matches($ht,$pattern).Count -ne 1){throw 'DecodeFailureCode not found'}
 $ht=[regex]::Replace($ht,$pattern,'')
}
$anchor='<ClCompile Include="CloseToHSL\\Core\Decoder\SemanticInstructionKey.cpp" />'
if (!$pt.Contains('<ClCompile Include="CloseToHSL\\Core\Decoder\DecodeFailure.cpp" />')) {if(!$pt.Contains($anchor)){throw 'Project anchor missing'}; $pt=$pt.Replace($anchor,$anchor+"`r`n"+'    <ClCompile Include="CloseToHSL\\Core\Decoder\DecodeFailure.cpp" />')}
[IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($p,$pt,[Text.UTF8Encoding]::new($false));Write-Output 'Installed DecodeFailure'
