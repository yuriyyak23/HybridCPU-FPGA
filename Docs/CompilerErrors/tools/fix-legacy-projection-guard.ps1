$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$h=Join-Path $root 'CloseToHSL\\Core\Decoder\Rf06ExecutionContracts.h';$c=Join-Path $root 'CloseToHSL\\Core\Decoder\Rf06ExecutionContracts.cpp';$p=Join-Path $root 'DPCPPConsoleApplication1.vcxproj'
$ht=[IO.File]::ReadAllText($h);$ct=[IO.File]::ReadAllText($c);$pt=[IO.File]::ReadAllText($p)
$pattern='(?s)\tclass LegacyProjectionGuard final\s*\{.*?\n\t\};'
if([regex]::Matches($ht,$pattern).Count -eq 1){$ht=[regex]::Replace($ht,$pattern,'')}
$pattern='(?s)\tstd::shared_ptr<AdmissionProjectionFingerprint> LegacyProjectionGuard::Capture\(.*?(?=\s*unsigned long long OperationAttemptIssuer::IssueNextAttempt\()'
if([regex]::Matches($ct,$pattern).Count -eq 1){$ct=[regex]::Replace($ct,$pattern,'')}
$anchor='<ClCompile Include="CloseToHSL\\Core\Decoder\DecodeFailure.cpp" />'
if(!$pt.Contains('<ClCompile Include="CloseToHSL\\Core\Decoder\LegacyProjectionGuard.cpp" />')){if(!$pt.Contains($anchor)){throw 'Project anchor missing'};$pt=$pt.Replace($anchor,$anchor+"`r`n"+'    <ClCompile Include="CloseToHSL\\Core\Decoder\LegacyProjectionGuard.cpp" />')}
[IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$ct,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($p,$pt,[Text.UTF8Encoding]::new($false));Write-Output 'Installed legacy projection guard'
