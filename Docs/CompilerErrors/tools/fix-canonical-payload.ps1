$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$h=Join-Path $root 'CloseToHSL\\Core\Decoder\CanonicalDecodedContracts.h'
$c=Join-Path $root 'CloseToHSL\\Core\Decoder\CanonicalDecodedContracts.cpp'
$proj=Join-Path $root 'DPCPPConsoleApplication1.vcxproj'
$ht=[IO.File]::ReadAllText($h); $ct=[IO.File]::ReadAllText($c); $pt=[IO.File]::ReadAllText($proj)
if (!$ht.Contains('#include "CanonicalPayloadSnapshot.h"')) {
 $ht=$ht.Replace('#include "CanonicalDecodeContext.h"', '#include "CanonicalDecodeContext.h"'+"`r`n"+'#include "CanonicalPayloadSnapshot.h"')
 $pattern='(?s)\tclass CanonicalPayloadSnapshot final\s*\{.*?\n\t\};'
 if ([regex]::Matches($ht,$pattern).Count -ne 1) {throw 'Unexpected snapshot declaration'}
 $ht=[regex]::Replace($ht,$pattern,'')
 $pattern='(?s)\tCanonicalPayloadSnapshot::CanonicalPayloadSnapshot\(.*?(?=\s*// C# TO C\+\+ CONVERTER WARNING: Nullable reference types have no equivalent in C\+\+:\s*// ORIGINAL LINE: public static CanonicalPayloadSnapshot FromObject)'
 if ([regex]::Matches($ct,$pattern).Count -ne 1) {throw 'Snapshot prefix not found'}
 $ct=[regex]::Replace($ct,$pattern,'')
 $pattern='(?s)\tstd::shared_ptr<CanonicalPayloadSnapshot> CanonicalPayloadSnapshot::FromBytes\(.*?(?=\s*SemanticInstructionKey::SemanticInstructionKey\()'
 if ([regex]::Matches($ct,$pattern).Count -ne 1) {throw 'Snapshot implementation block not found'}
 $ct=[regex]::Replace($ct,$pattern,'')
}
$anchor='<ClCompile Include="CloseToHSL\\Core\Decoder\Sha256Digest.cpp" />'
if (!$pt.Contains('<ClCompile Include="CloseToHSL\\Core\Decoder\CanonicalPayloadSnapshot.cpp" />')) {
 if (!$pt.Contains($anchor)) {throw 'Project anchor missing'}
 $pt=$pt.Replace($anchor,$anchor+"`r`n"+'    <ClCompile Include="CloseToHSL\\Core\Decoder\CanonicalPayloadSnapshot.cpp" />')
}
[IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$ct,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($proj,$pt,[Text.UTF8Encoding]::new($false));Write-Output 'Separated snapshot byte operations'
