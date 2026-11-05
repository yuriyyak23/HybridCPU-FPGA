$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Certificates\ReplayPhaseSubstrate.h'
$s=[IO.File]::ReadAllText($p)
$pat='LegalityCertificateCacheEvaluation\(TResult legalityResult, LegalityCertificateCacheTelemetry telemetry\)\s*\{\s*LegalityResult = legalityResult;\s*Telemetry = telemetry;\s*\}'
if(-not [regex]::IsMatch($s,$pat)){throw 'constructor anchor missing'}
$s=[regex]::Replace($s,$pat,'LegalityCertificateCacheEvaluation(TResult legalityResult, LegalityCertificateCacheTelemetry telemetry) : LegalityResult(legalityResult), Telemetry(telemetry) {}')
[IO.File]::WriteAllText($p,$s)
