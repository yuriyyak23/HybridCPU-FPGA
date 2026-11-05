$path='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Safety\YAKSys_Hybrid_CPU.Core.SecureComputeIssuePermitOwner.h';$s=[IO.File]::ReadAllText($path)
$old='class SecureComputeIssuePermitOwner(SafetyVerifier issuer, Object gate) final';if(-not $s.Contains($old)){throw 'Class declaration missing'};$s=$s.Replace($old,'class SecureComputeIssuePermitOwner final')
$start=$s.IndexOf('class SecureComputeIssuePermitOwner final');$mark=$s.IndexOf("`tprivate:",$start);if($mark -lt 0){throw 'Private marker missing'}
$s=$s.Insert($mark+"`tprivate:".Length,"`r`n`t`tstd::weak_ptr<SafetyVerifier> issuer;`r`n`t`tstd::mutex& gate;")
$mark=$s.IndexOf("`tpublic:",$start);$s=$s.Insert($mark+"`tpublic:".Length,"`r`n`t`tSecureComputeIssuePermitOwner(std::shared_ptr<SafetyVerifier> owner, std::mutex& ownerGate) : issuer(std::move(owner)), gate(ownerGate) {}")
$s=$s.Replace(' = [];',' = {};').Replace('#include <memory>',"#include <memory>`r`n#include <utility>")
[IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
