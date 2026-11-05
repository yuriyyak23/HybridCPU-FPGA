$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Events\Hypercalls\DomainHypercallCanonicalComposition.h'; $s=[IO.File]::ReadAllText($p).Replace('SafetyVerifier::VirtualizationOperationAdmissionCertificate','VirtualizationOperationAdmissionCertificate').Replace('SafetyVerifier::VirtualizationAdmissionCertificate','VirtualizationAdmissionCertificate'); $anchor='class DomainHypercallCanonicalComposition final'; $def=@"
class SmtBundleMetadata4Way;
struct DomainHypercallCompositionResult final {
    const DomainHypercallCompositionDecision Decision;
    const std::shared_ptr<VirtualizationOperationAdmissionCertificate> E2;
    const std::wstring Reason;
    DomainHypercallCompositionResult(DomainHypercallCompositionDecision decision, std::shared_ptr<VirtualizationOperationAdmissionCertificate> e2, std::wstring reason) : Decision(decision), E2(std::move(e2)), Reason(std::move(reason)) {}
    bool isPrepared() const noexcept { return Decision == DomainHypercallCompositionDecision::Prepared && E2 != nullptr; }
    bool backendExecutionAuthorized() const noexcept { return false; }
    bool completionPublicationAuthorized() const noexcept { return false; }
    bool retirePublicationAuthorized() const noexcept { return false; }
};

"@; if(-not $s.Contains('struct DomainHypercallCompositionResult final')) {$s=$s.Replace($anchor,$def+$anchor)}; if(-not $s.Contains('#include <utility>')) {$s=$s.Replace('#include <memory>',"#include <memory>`r`n#include <utility>")}; [IO.File]::WriteAllText($p,$s)
