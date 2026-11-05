$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Safety';$h=Join-Path $root 'SafetyVerifier.VirtualizationOperationAdmission.h';$v=Join-Path $root 'YAKSys_Hybrid_CPU.Core.SafetyVerifier.h';$s=[IO.File]::ReadAllText($h);$m="namespace YAKSys_Hybrid_CPU::Core`r`n{";if(-not $s.Contains($m)){throw 'Namespace missing'};$s=$s.Replace($m,$m+"`r`n`tclass VirtualizationOperationAdmissionCertificate;")
$marker="`tenum class VirtualizationE2State : unsigned char";$pos=$s.IndexOf($marker);if($pos -lt 0){throw 'State marker missing'}
$block=@"
    struct VirtualizationOperationAdmissionResult final {
        VirtualizationOperationAdmissionDecision Decision;
        std::shared_ptr<VirtualizationOperationAdmissionCertificate> Certificate;
        std::wstring Reason;
        bool IsIssued() const noexcept { return false; }
        bool BackendExecutionAuthorized() const noexcept { return false; }
        bool CompletionPublicationAuthorized() const noexcept { return false; }
        bool RetirePublicationAuthorized() const noexcept { return false; }
    };

"@
$s=$s.Insert($pos,$block)
$marker="`tenum class VirtualizationE2ConsumptionDecision : unsigned char";$pos=$s.IndexOf($marker);if($pos -lt 0){throw 'Consumption enum missing'}
$block=@"
    struct VirtualizationE2Result final {
        VirtualizationE2Decision Decision;
        std::shared_ptr<VirtualizationOperationAdmissionCertificate> Certificate;
        std::wstring Reason;
        bool IsIssued() const noexcept { return Decision == VirtualizationE2Decision::Issued && Certificate != nullptr; }
        bool IsLive() const noexcept { return IsIssued(); }
        bool BackendExecutionAuthorized() const noexcept { return false; }
        bool CompletionPublicationAuthorized() const noexcept { return false; }
        bool RetirePublicationAuthorized() const noexcept { return false; }
    };

"@
$s=$s.Insert($pos,$block)
$marker="`tclass VirtualizationE2Digest final";$pos=$s.IndexOf($marker);if($pos -lt 0){throw 'Digest class missing'}
$block=@"
    struct VirtualizationE2ConsumptionResult final {
        VirtualizationE2ConsumptionDecision Decision;
        std::wstring Reason;
        bool IsConsumed() const noexcept { return Decision == VirtualizationE2ConsumptionDecision::Consumed; }
    };

"@
$s=$s.Insert($pos,$block);$vs=[IO.File]::ReadAllText($v);$m='private sealed record LiveVirtualizationAdmission(ulong AttemptId, ulong IssuerGeneration);';if(-not $vs.Contains($m)){throw 'Live E1 marker missing'};$vs=$vs.Replace($m,$m+"`r`n`t`tstruct LiveVirtualizationAdmission final { unsigned long long AttemptId; unsigned long long IssuerGeneration; };")
[IO.File]::WriteAllText($h,$s,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($v,$vs,[Text.UTF8Encoding]::new($false))
