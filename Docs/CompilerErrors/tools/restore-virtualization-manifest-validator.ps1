$path='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Events\Hypercalls\VirtualizationOperationDecisionManifest.cpp';$backup='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\CompilerErrors\docs\snapshots\VirtualizationOperationDecisionManifest.before-restore.cpp.txt';if(Test-Path $backup){throw 'Backup exists'};Copy-Item $path $backup
$content=@"
#include "VirtualizationOperationDecisionManifest.h"
#include <algorithm>
#include <stdexcept>
#include <unordered_set>

namespace YAKSys_Hybrid_CPU::Core {
namespace {
bool IsWhiteSpace(wchar_t c) {
    return (c >= 0x09 && c <= 0x0d) || c == 0x20 || c == 0x85 || c == 0xa0 || c == 0x1680 ||
        (c >= 0x2000 && c <= 0x200a) || c == 0x2028 || c == 0x2029 || c == 0x202f || c == 0x205f || c == 0x3000;
}
bool IsBlank(const std::wstring& value) { return value.empty() || std::all_of(value.begin(),value.end(),IsWhiteSpace); }
bool IsHex(wchar_t c) { return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F'); }
wchar_t FoldHex(wchar_t c) { return (c >= L'A' && c <= L'F') ? static_cast<wchar_t>(c + (L'a' - L'A')) : c; }
}
std::shared_ptr<VirtualizationOperationDecisionValidationResult> VirtualizationOperationDecisionManifestValidator::Validate(
    std::shared_ptr<VirtualizationOperationDecisionManifest> manifest, std::shared_ptr<VirtualizationDecisionAttributionEvidence> attribution) {
    if (!manifest || !attribution) throw std::invalid_argument("manifest or attribution");
    using D = VirtualizationOperationDecisionValidationDecision;
    if (manifest->SchemaVersion != CurrentSchemaVersion) return Deny(D::DeniedSchemaVersion,L"The D2 schema version is missing or unsupported.");
    if (manifest->State != VirtualizationOperationDecisionState::Accepted) return Deny(D::DeniedDecisionNotAccepted,L"Only an attributable accepted decision artifact can satisfy D2.");
    if (IsBlank(manifest->DecisionId) || IsBlank(manifest->OperationName)) return Deny(D::DeniedDecisionIdentity,L"The decision and operation identities must be explicit.");
    if (manifest->OwnerSource != NeutralHypercallBackendOwnerSource::NeutralRuntimeOwner || IsBlank(manifest->DecisionOwner) || manifest->DecisionOwner != attribution->DecisionOwner) return Deny(D::DeniedNeutralOwnerAttribution,L"D2 requires an attributable neutral runtime owner independent of compatibility planes.");
    const auto& sha = manifest->AcceptedCommitSha; const auto& approvedSha = attribution->AcceptedCommitSha;
    if (!IsCommitSha(sha) || sha.size() != approvedSha.size() || !std::equal(sha.begin(),sha.end(),approvedSha.begin(),[](wchar_t a,wchar_t b){return FoldHex(a)==FoldHex(b);})) return Deny(D::DeniedAcceptedCommitSha,L"The accepted decision must be bound to one matching repository commit SHA.");
    if (!attribution->CodeOwnersRulePresent || !attribution->CodeOwnersRuleMatched) return Deny(D::DeniedCodeOwnersRule,L"No matching repository CODEOWNERS attribution was proven.");
    if (manifest->RequiredReviewers.empty() || !attribution->RequiredReviewersApproved) return Deny(D::DeniedRequiredReview,L"Required owner review is absent or unproven.");
    if (!std::all_of(manifest->RequiredReviewers.begin(),manifest->RequiredReviewers.end(),[&](const auto& reviewer){return std::find(attribution->ApprovedReviewers.begin(),attribution->ApprovedReviewers.end(),reviewer)!=attribution->ApprovedReviewers.end();})) return Deny(D::DeniedReviewerMismatch,L"The attributed approvals do not cover every required reviewer.");
    if (attribution->CompatibilityFrontendSelfApproved) return Deny(D::DeniedCompatibilitySelfApproval,L"The VMX compatibility plane cannot approve its own runtime authority.");
    if (IsBlank(manifest->ValueSource) || IsBlank(manifest->CapabilityPolicy) || IsBlank(manifest->EvidenceClass) || IsBlank(manifest->MigrationClass) || IsBlank(manifest->DenialReason)) return Deny(D::DeniedOwnerMapIncomplete,L"The complete owner/value/capability/evidence/migration/denial map is required.");
    if (manifest->ExactNumericLeaves.empty()) return Deny(D::DeniedExactLeafCardinality,L"D2 requires exactly one owner-accepted numeric leaf; none is supplied by this substrate.");
    std::unordered_set<unsigned long long> distinct(manifest->ExactNumericLeaves.begin(),manifest->ExactNumericLeaves.end());
    if (distinct.size() != manifest->ExactNumericLeaves.size()) return Deny(D::DeniedDuplicateExactLeaf,L"D2 rejects duplicate numeric leaves; this substrate allocates no leaf.");
    if (manifest->ExactNumericLeaves.size() != 1) return Deny(D::DeniedExactLeafCardinality,L"D2 requires exactly one owner-accepted numeric leaf; a range or set is not accepted.");
    return std::make_shared<VirtualizationOperationDecisionValidationResult>(D::ValidGovernanceArtifactOnly,L"The D2 artifact shape and attribution are valid governance evidence only; runtime authority is not granted.");
}
bool VirtualizationOperationDecisionManifestValidator::IsCommitSha(const std::wstring& value) { return value.size()==40 && std::all_of(value.begin(),value.end(),IsHex); }
std::shared_ptr<VirtualizationOperationDecisionValidationResult> VirtualizationOperationDecisionManifestValidator::Deny(VirtualizationOperationDecisionValidationDecision decision,const std::wstring& reason) { return std::make_shared<VirtualizationOperationDecisionValidationResult>(decision,reason); }
}
"@
[IO.File]::WriteAllText($path,$content,[Text.UTF8Encoding]::new($false))
