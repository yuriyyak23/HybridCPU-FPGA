$path='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Safety\SecureComputeIssuePermitOwner.h'
$s=[IO.File]::ReadAllText($path); $marker='using namespace HybridCPU::ExternalRuntime::Contracts;'
if(-not $s.Contains($marker)){throw 'Expected namespace marker missing'}
$s=$s.Replace($marker,'class Guid;' + "`r`n" + 'namespace HybridCPU::ExternalRuntime::Contracts { class ExternalSecureExecutionReservation; }' + "`r`n" + $marker)
[IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
