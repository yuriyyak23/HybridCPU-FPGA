$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$header=Join-Path $root 'CloseToHSL\\Core\Decoder\CanonicalDecodedContracts.h'
$source=Join-Path $root 'CloseToHSL\\Core\Decoder\CanonicalDecodedContracts.cpp'
$project=Join-Path $root 'DPCPPConsoleApplication1.vcxproj'
$h=[IO.File]::ReadAllText($header); $c=[IO.File]::ReadAllText($source); $p=[IO.File]::ReadAllText($project)
if (!$h.Contains('#include "Sha256Digest.h"')) { $h=$h.Replace('#include "CanonicalDecodeContext.h"', '#include "CanonicalDecodeContext.h"'+"`r`n"+'#include "Sha256Digest.h"') }
$h=$h.Replace('static std::wstring Hash(ReadOnlySpan<unsigned char> content);','static std::wstring Hash(std::span<const unsigned char> content);')
$pattern='(?s)std::wstring CanonicalPayloadSnapshot::Hash\(ReadOnlySpan<unsigned char> content\)\s*\{.*?\}'
$c=[regex]::Replace($c,$pattern,'std::wstring CanonicalPayloadSnapshot::Hash(std::span<const unsigned char> content) { return Sha256Hex(content); }')
$anchor='<ClCompile Include="CloseToHSL\\Core\Decoder\CanonicalDecodedContracts.cpp" />'
if (!$p.Contains('<ClCompile Include="CloseToHSL\\Core\Decoder\Sha256Digest.cpp" />')) {
 if (!$p.Contains($anchor)) {throw 'Project source anchor missing'}
 $p=$p.Replace($anchor,$anchor+"`r`n"+'    <ClCompile Include="CloseToHSL\\Core\Decoder\Sha256Digest.cpp" />')
}
if (!$p.Contains('<AdditionalDependencies>bcrypt.lib;%(AdditionalDependencies)</AdditionalDependencies>')) {
 $p=$p.Replace('<SubSystem>Console</SubSystem>', '<SubSystem>Console</SubSystem>'+"`r`n"+'      <AdditionalDependencies>bcrypt.lib;%(AdditionalDependencies)</AdditionalDependencies>')
}
[IO.File]::WriteAllText($header,$h,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($source,$c,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($project,$p,[Text.UTF8Encoding]::new($false)); Write-Output 'Connected BCrypt SHA-256'
