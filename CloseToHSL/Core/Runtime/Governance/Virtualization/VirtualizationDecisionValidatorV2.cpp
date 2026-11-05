#include "VirtualizationDecisionValidatorV2.h"
#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include <algorithm>
#include <array>
#include <cstdlib>
#include <exception>
#include <cwctype>
#include <format>
#include <unordered_set>

namespace YAKSys_Hybrid_CPU::Core
{
namespace
{
    bool OrdinalIgnoreCaseEqual(const std::wstring& left, const std::wstring& right)
    {
        if (left.size() != right.size()) return false;
        auto fold = [](wchar_t c) { return c >= L'A' && c <= L'Z' ? static_cast<wchar_t>(c + (L'a' - L'A')) : c; };
        for (size_t i = 0; i < left.size(); ++i) if (fold(left[i]) != fold(right[i])) return false;
        return true;
    }

    bool ContainsOrdinalIgnoreCase(const std::wstring& value, const std::wstring& token)
    {
        if (token.empty()) return true;
        auto fold = [](wchar_t c) { return c >= L'A' && c <= L'Z' ? static_cast<wchar_t>(c + (L'a' - L'A')) : c; };
        if (token.size() > value.size()) return false;
        for (size_t start = 0; start <= value.size() - token.size(); ++start)
        {
            size_t i = 0;
            for (; i < token.size(); ++i) if (fold(value[start + i]) != fold(token[i])) break;
            if (i == token.size()) return true;
        }
        return false;
    }

    bool IsWhiteSpaceOnly(const std::wstring& value)
    {
        return value.empty() || std::all_of(value.begin(), value.end(), [](wchar_t c) { return std::iswspace(c) != 0; });
    }
}

std::shared_ptr<VirtualizationDecisionValidationResultV2> VirtualizationDecisionValidatorV2::Validate(
    const std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>>& spec,
    const std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>>& acceptance,
    const std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>>& evidence)
{
    if (!spec || !acceptance || !evidence || !spec.value() || !acceptance.value() || !evidence.value() || !evidence.value()->CodeOwners)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedMissingArtifact, L"SpecV2, AcceptanceRecordV2 and repository evidence are all required.");

    const auto& s = spec.value();
    const auto& a = acceptance.value();
    const auto& e = evidence.value();
    if (s->SchemaVersion != CurrentSchemaVersion || a->SchemaVersion != CurrentSchemaVersion)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedSchemaVersion, L"Only D2 v2 governance artifacts are supported.");

    if (!VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(a->SpecCommitSha) ||
        !VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(a->CodeOwnersBlobSha) ||
        !VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(e->ResolvedSpecCommitSha) ||
        !VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(e->AcceptanceContainingCommitSha))
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedMalformedCommitSha, L"Spec, CODEOWNERS and containing commit identities must be canonical 40-hex SHAs.");

    if (a->SpecCommitSha == e->AcceptanceContainingCommitSha)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedSelfReferentialCommitSha, L"SpecCommitSha must name the earlier commit containing the immutable spec bytes, not the acceptance record's containing commit.");
    if (a->SpecCommitSha != e->ResolvedSpecCommitSha)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedSpecBytesAtCommitMismatch, L"The repository resolver did not resolve exact spec bytes at the recorded SpecCommitSha.");

    auto canonicalFailure = ValidateCanonicalArtifacts(s, a, e);
    if (canonicalFailure) return canonicalFailure.value();
    if (s->DecisionId != ExpectedDecisionId || a->DecisionId != s->DecisionId)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedDecisionIdentity, L"DecisionId does not identify the exact Phase 38 first slice.");

    auto ownerFailure = ValidateOwnerAndAbi(s); if (ownerFailure) return ownerFailure.value();
    auto policyFailure = ValidatePolicies(s); if (policyFailure) return policyFailure.value();
    auto ownerMapFailure = ValidateOwnerMap(s->OwnerMap); if (ownerMapFailure) return ownerMapFailure.value();
    auto namespaceFailure = ValidateNamespaceAndLeaf(s, e->ExistingAllocations); if (namespaceFailure) return namespaceFailure.value();
    auto acceptanceFailure = ValidateAcceptance(s, a, e->CodeOwners); if (acceptanceFailure) return acceptanceFailure.value();

    try
    {
        auto lineageFailure = ValidateLineage(a, e->Revocations, e->Supersessions);
        if (lineageFailure) return lineageFailure.value();
    }
    catch (const std::exception&)
    {
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedInvalidLineage, L"Lineage contains malformed fields that cannot be canonically validated.");
    }

    auto adjacentPolicy = std::make_shared<VirtualizationDecisionAdjacentPolicySnapshotV2>(s->AdjacentLeafPolicy, s->CrossNamespacePolicy);
    auto acceptedPolicy = std::make_shared<AcceptedVirtualizationDecision>(s->DecisionId, s->SpecDigest, a->AcceptanceDigest,
        a->SpecCommitSha, s->OperationNamespace, s->NumericLeaf, s->OwnerId, s->OwnerPolicyVersion, s->OwnerEpoch,
        s->EffectClass, std::move(adjacentPolicy));
    return std::make_shared<VirtualizationDecisionValidationResultV2>(VirtualizationDecisionValidationDecisionV2::AcceptedPolicyObject,
        L"The exact D2 v2 structure is valid as immutable governance policy only; no runtime authority is granted.", std::move(acceptedPolicy));
}

std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> VirtualizationDecisionValidatorV2::ValidateCanonicalArtifacts(
    const std::shared_ptr<VirtualizationDecisionSpecV2>& spec,
    const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>& acceptance,
    const std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>& evidence)
{
    if (!VirtualizationDecisionCanonicalEncoderV2::IsCanonicalSha256(spec->SpecDigest) ||
        !VirtualizationDecisionCanonicalEncoderV2::IsCanonicalSha256(acceptance->SpecDigest) ||
        !VirtualizationDecisionCanonicalEncoderV2::IsCanonicalSha256(acceptance->AcceptanceDigest))
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedMalformedDigest, L"All D2 digests must be canonical lowercase SHA-256 values.");
    try
    {
        const auto computedSpecDigest = VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(spec);
        if (!FixedEquals(spec->SpecDigest, computedSpecDigest) || !FixedEquals(acceptance->SpecDigest, spec->SpecDigest))
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedSpecDigestMismatch, L"SpecDigest does not reproduce the canonical spec payload or its acceptance reference.");
        const auto computedAcceptanceDigest = VirtualizationDecisionCanonicalEncoderV2::ComputeAcceptanceDigest(acceptance);
        if (!FixedEquals(acceptance->AcceptanceDigest, computedAcceptanceDigest))
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedAcceptanceDigestMismatch, L"AcceptanceDigest does not reproduce the canonical acceptance payload.");
        const auto canonicalSpec = VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(spec);
        if (!FixedEquals(canonicalSpec, evidence->SpecCanonicalBytes))
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedNonCanonicalSpecBytes, L"Provided spec bytes are not the v2 canonical binary form.");
        const auto canonicalAcceptance = VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(acceptance);
        if (!FixedEquals(canonicalAcceptance, evidence->AcceptanceCanonicalBytes))
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedNonCanonicalAcceptanceBytes, L"Provided acceptance bytes are not the v2 canonical binary form.");
        if (!FixedEquals(canonicalSpec, evidence->SpecBytesAtCommit))
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedSpecBytesAtCommitMismatch, L"The bytes resolved at SpecCommitSha differ from the accepted canonical spec bytes.");
    }
    catch (const std::exception&)
    {
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedMalformedDigest, L"A digest, commit identity or canonical field cannot be encoded.");
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> VirtualizationDecisionValidatorV2::ValidateOwnerAndAbi(const std::shared_ptr<VirtualizationDecisionSpecV2>& spec)
{
    if (spec->OwnerId == 0 || spec->OwnerClass != VirtualizationDecisionOwnerClassV2::NeutralRuntimeOwner ||
        spec->OwnerId != ExpectedOwnerId || spec->OwnerPolicyVersion != 1 || spec->OwnerEpoch != 1)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedOwner, L"The neutral HCOWNR allocation and owner policy/epoch must match Phase 38 exactly.");
    if (spec->OperandAbiVersion != 1 || spec->Rs1Contract != L"ArchitecturalRegisterFullNumericLeafValue" ||
        spec->Rs2Contract != L"X0" || spec->RdContract != L"X0NoResult" ||
        spec->ResultAbi != VirtualizationDecisionResultAbiV2::NoPayload || spec->EffectClass != VirtualizationDecisionEffectClassV2::NoStateNoPayload)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedAbi, L"Operand, result and no-state/no-payload ABI must match the exact Phase 38 profile.");
    return std::nullopt;
}

std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> VirtualizationDecisionValidatorV2::ValidatePolicies(const std::shared_ptr<VirtualizationDecisionSpecV2>& spec)
{
    struct PolicyCheck { unsigned int value; unsigned int maxDefined; std::wstring_view name; };
    const std::array policies = {
        PolicyCheck{static_cast<unsigned int>(spec->CapabilityRequirement), 2, L"VirtualizationCapabilityRequirementV2"},
        PolicyCheck{static_cast<unsigned int>(spec->DelegationPolicy), 1, L"VirtualizationDelegationPolicyV2"},
        PolicyCheck{static_cast<unsigned int>(spec->RevocationPolicy), 2, L"VirtualizationRevocationPolicyV2"},
        PolicyCheck{static_cast<unsigned int>(spec->CapabilityMigrationClass), 2, L"VirtualizationCapabilityMigrationClassV2"},
        PolicyCheck{static_cast<unsigned int>(spec->EvidenceVisibility), 2, L"VirtualizationEvidenceVisibilityV2"},
        PolicyCheck{static_cast<unsigned int>(spec->FrontendProjectionPolicy), 2, L"VirtualizationProjectionPolicyV2"},
        PolicyCheck{static_cast<unsigned int>(spec->ExecutionEvidenceRequirement), 2, L"VirtualizationExecutionEvidenceRequirementV2"},
        PolicyCheck{static_cast<unsigned int>(spec->DomainRequirement), 2, L"VirtualizationDomainRequirementV2"},
        PolicyCheck{static_cast<unsigned int>(spec->AddressSpaceRequirement), 2, L"VirtualizationAddressSpaceRequirementV2"},
        PolicyCheck{static_cast<unsigned int>(spec->SecureDomainPolicy), 1, L"VirtualizationSecureDomainPolicyV2"},
        PolicyCheck{static_cast<unsigned int>(spec->CancellationPolicy), 3, L"VirtualizationCancellationPolicyV2"},
        PolicyCheck{static_cast<unsigned int>(spec->ReplayPolicy), 3, L"VirtualizationReplayPolicyV2"},
        PolicyCheck{static_cast<unsigned int>(spec->OperationMigrationPolicy), 2, L"VirtualizationOperationMigrationPolicyV2"},
        PolicyCheck{static_cast<unsigned int>(spec->CompletionEvidenceClass), 2, L"VirtualizationCompletionEvidenceClassV2"},
        PolicyCheck{static_cast<unsigned int>(spec->CompletionMigrationClass), 2, L"VirtualizationCompletionMigrationClassV2"},
        PolicyCheck{static_cast<unsigned int>(spec->CompletionProjectionPolicy), 2, L"VirtualizationProjectionPolicyV2"},
        PolicyCheck{static_cast<unsigned int>(spec->CompletionPolicy), 2, L"VirtualizationCompletionPolicyV2"},
        PolicyCheck{static_cast<unsigned int>(spec->RetirePolicy), 3, L"VirtualizationRetirePolicyV2"},
        PolicyCheck{static_cast<unsigned int>(spec->AdjacentLeafPolicy), 2, L"VirtualizationAdjacentLeafPolicyV2"},
        PolicyCheck{static_cast<unsigned int>(spec->CrossNamespacePolicy), 2, L"VirtualizationCrossNamespacePolicyV2"}};
    for (const auto& policy : policies)
    {
        if (policy.value > policy.maxDefined)
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedUnknownPolicy, std::format(L"Unknown policy value for {} is denied.", policy.name));
        if (policy.value == 0)
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedPolicyMissing, std::format(L"Missing policy value for {} is denied.", policy.name));
    }
    if (spec->CapabilityRequirement != VirtualizationCapabilityRequirementV2::DomainGrantedVmCallProbeNoStateV1 || spec->CapabilityMask != ExpectedCapabilityMask ||
        !spec->RequiresTypedGrant || spec->DelegationPolicy != VirtualizationDelegationPolicyV2::NonDelegable ||
        spec->RevocationPolicy != VirtualizationRevocationPolicyV2::RuntimeRevocable || spec->CapabilityMigrationClass != VirtualizationCapabilityMigrationClassV2::DomainLocal ||
        spec->EvidenceVisibility != VirtualizationEvidenceVisibilityV2::HostOnly || spec->FrontendProjectionPolicy != VirtualizationProjectionPolicyV2::NeverProject ||
        spec->ExecutionEvidenceRequirement != VirtualizationExecutionEvidenceRequirementV2::None || spec->DomainRequirement != VirtualizationDomainRequirementV2::ExecutionDomainBound ||
        !spec->RequireNonZeroDomainTag || spec->RequiresMemoryDomain || spec->RequiresIoDomain || spec->AddressSpaceRequirement != VirtualizationAddressSpaceRequirementV2::None ||
        spec->SecureDomainPolicy != VirtualizationSecureDomainPolicyV2::Deny || spec->CancellationPolicy != VirtualizationCancellationPolicyV2::DenyBeforeExecution ||
        spec->ReplayPolicy != VirtualizationReplayPolicyV2::DenyAttemptReplay || spec->OperationMigrationPolicy != VirtualizationOperationMigrationPolicyV2::DrainOnly ||
        spec->CompletionEvidenceClass != VirtualizationCompletionEvidenceClassV2::HostOwnedRuntimeEvidence || spec->CompletionMigrationClass != VirtualizationCompletionMigrationClassV2::HostOwnedNonMigratable ||
        spec->CompletionProjectionPolicy != VirtualizationProjectionPolicyV2::NeverProject || spec->CompletionPolicy != VirtualizationCompletionPolicyV2::AtomicE3ToCompletionRecordAndE5 ||
        spec->RetirePolicy != VirtualizationRetirePolicyV2::PreciseE5BoundNoStateRetire || spec->AdjacentLeafPolicy != VirtualizationAdjacentLeafPolicyV2::DenyAllExceptExact ||
        spec->CrossNamespacePolicy != VirtualizationCrossNamespacePolicyV2::AllowDistinctFrozenCompatibilityNamespaceOnly)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedPolicyProfile, L"The complete capability/domain/evidence/cancellation/replay/completion/retire policy profile must match Phase 38 exactly.");
    return std::nullopt;
}

std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> VirtualizationDecisionValidatorV2::ValidateOwnerMap(const std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>>& ownerMap)
{
    if (ownerMap.empty() || ownerMap.size() != RequiredOwnerMap.size())
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedOwnerMapIncomplete, L"The complete Phase 38 owner map is required.");
    std::unordered_set<std::wstring> fields;
    for (const auto& entry : ownerMap)
    {
        if (!entry || IsWhiteSpaceOnly(entry->FieldOrOperation) || IsWhiteSpaceOnly(entry->Owner) || IsWhiteSpaceOnly(entry->ValueSource) ||
            IsWhiteSpaceOnly(entry->CapabilityPolicy) || IsWhiteSpaceOnly(entry->EvidenceClass) || IsWhiteSpaceOnly(entry->MigrationClass) || IsWhiteSpaceOnly(entry->DenialReason))
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedOwnerMapIncomplete, L"Every owner-map entry requires owner, source, capability, evidence, migration and denial fields.");
        if (!fields.insert(entry->FieldOrOperation).second)
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedOwnerMapMismatch, L"Owner-map fields must be unique.");
    }
    for (const auto& [field, expectedOwner] : RequiredOwnerMap)
    {
        auto entry = std::find_if(ownerMap.begin(), ownerMap.end(), [&](const auto& item) { return item->FieldOrOperation == field; });
        if (entry == ownerMap.end() || (*entry)->Owner != expectedOwner)
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedOwnerMapMismatch, std::format(L"Owner-map field {} is missing or assigned to the wrong neutral owner.", field));
    }
    for (const auto& entry : ownerMap)
        if (ContainsOrdinalIgnoreCase(entry->Owner, L"CompatibilityFrontend") || ContainsOrdinalIgnoreCase(entry->Owner, L"VMX") || ContainsOrdinalIgnoreCase(entry->Owner, L"VMCS"))
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedOwnerMapMismatch, L"Compatibility vocabulary cannot own a D2 policy field.");
    return std::nullopt;
}

std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> VirtualizationDecisionValidatorV2::ValidateNamespaceAndLeaf(const std::shared_ptr<VirtualizationDecisionSpecV2>& spec, const std::vector<std::shared_ptr<VirtualizationNamespaceAllocationV2>>& allocations)
{
    if (spec->NumericLeaf == 0 || spec->InvalidLeaf != 0)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedZeroLeaf, L"Leaf zero is invalid and cannot be allocated.");
    if (spec->OperationNamespace != ExpectedOperationNamespace || spec->OperationId != ExpectedOperationId || spec->LeafWidth != 16 || spec->NumericLeaf != 1)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedNamespaceRule, L"Namespace, width, operation and numeric leaf must match the exact Phase 38 ABI.");
    for (const auto& allocation : allocations)
    {
        if (!allocation || allocation->NumericLeaf == 0 || IsWhiteSpaceOnly(allocation->OperationNamespace))
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedNamespaceRule, L"Malformed existing allocation evidence is denied.");
        const bool sameNamespace = allocation->OperationNamespace == spec->OperationNamespace;
        if (sameNamespace && allocation->LeafWidth != spec->LeafWidth)
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedNamespaceRule, L"One operation namespace cannot carry conflicting leaf widths.");
        if (sameNamespace && allocation->NumericLeaf == spec->NumericLeaf)
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedDuplicateLeaf, L"The exact runtime namespace/leaf is already allocated.");
        if (sameNamespace && std::abs(static_cast<int>(allocation->NumericLeaf) - static_cast<int>(spec->NumericLeaf)) == 1)
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedAdjacentLeaf, L"Adjacent leaves remain denied by the exact first-slice policy.");
        if (!sameNamespace && allocation->NumericLeaf == spec->NumericLeaf && allocation->NamespaceClass != VirtualizationNamespaceClassV2::FrozenCompatibility)
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedCrossNamespaceLeaf, L"Numeric reuse is allowed only for a distinct frozen compatibility namespace.");
        if (allocation->DecisionId == spec->DecisionId)
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedDuplicateLeaf, L"DecisionId is already bound to another allocation.");
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> VirtualizationDecisionValidatorV2::ValidateAcceptance(const std::shared_ptr<VirtualizationDecisionSpecV2>& spec, const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>& acceptance, const std::shared_ptr<VirtualizationCodeOwnersEvidenceV2>& codeOwners)
{
    if (acceptance->AcceptanceState != VirtualizationDecisionAcceptanceStateV2::Accepted)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedAcceptanceState, L"Draft, revoked or superseded acceptance-record states are not active acceptance.");
    if (acceptance->AcceptancePolicyVersion != 1 || !IsRepositoryPrincipal(acceptance->AcceptedBy))
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedAcceptanceState, L"Acceptance policy version and attributable accepting principal are required.");
    if (!codeOwners->FilePresent || codeOwners->BlobSha != acceptance->CodeOwnersBlobSha || codeOwners->Rules.empty())
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedCodeOwners, L"A matching repository CODEOWNERS blob and rules are required.");
    for (const auto& scope : RequiredCodeOwnersScopes)
    {
        const bool covered = std::any_of(codeOwners->Rules.begin(), codeOwners->Rules.end(), [&](const auto& rule) {
            return rule && rule->Scope == scope && rule->Principal == acceptance->AcceptedBy;
        });
        if (!covered) return Deny(VirtualizationDecisionValidationDecisionV2::DeniedCodeOwners, std::format(L"Required CODEOWNERS scope {} is absent or does not match the accepting repository principal.", scope));
    }
    auto ownerReviewFailure = ValidateReview(acceptance->OwnerReviewEvidence, VirtualizationDecisionReviewRoleV2::OwnerReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, spec, acceptance->SpecCommitSha);
    if (ownerReviewFailure) return ownerReviewFailure.value();
    auto architectureReviewFailure = ValidateReview(acceptance->ArchitectureReviewEvidence, VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, spec, acceptance->SpecCommitSha);
    if (architectureReviewFailure) return architectureReviewFailure.value();
    if (!acceptance->OwnerReviewEvidence || !acceptance->ArchitectureReviewEvidence ||
        acceptance->OwnerReviewEvidence->Principal != acceptance->AcceptedBy || acceptance->ArchitectureReviewEvidence->Principal != acceptance->AcceptedBy)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedReviewMismatch, L"Owner review, architecture review and acceptance must name the same CODEOWNERS-attributed repository principal.");
    return std::nullopt;
}

bool VirtualizationDecisionValidatorV2::IsRepositoryPrincipal(const std::wstring& principal)
{
    if (principal.size() <= 1 || principal.front() != L'@') return false;
    for (size_t index = 1; index < principal.size(); ++index)
    {
        const wchar_t value = principal[index];
        if (!((value >= L'a' && value <= L'z') || (value >= L'A' && value <= L'Z') || (value >= L'0' && value <= L'9') || value == L'-' || value == L'_' || value == L'/')) return false;
    }
    return !OrdinalIgnoreCaseEqual(principal, L"@CompatibilityFrontend");
}

std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> VirtualizationDecisionValidatorV2::ValidateReview(const std::shared_ptr<VirtualizationDecisionReviewEvidenceV2>& review, VirtualizationDecisionReviewRoleV2 requiredRole, VirtualizationDecisionReviewAuthorityPlaneV2 requiredPlane, const std::shared_ptr<VirtualizationDecisionSpecV2>& spec, const std::wstring& specCommitSha)
{
    if (!review || review->Role != requiredRole)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedReviewRole, std::format(L"The {} evidence is missing or role-mismatched.", requiredRole == VirtualizationDecisionReviewRoleV2::OwnerReviewRole ? L"OwnerReviewRole" : L"ArchitectureReviewRole"));
    if (review->AuthorityPlane == VirtualizationDecisionReviewAuthorityPlaneV2::CompatibilityFrontend)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedCompatibilityReview, L"CompatibilityFrontend cannot satisfy owner or architecture review.");
    if (review->AuthorityPlane != requiredPlane || review->State != VirtualizationDecisionReviewStateV2::Completed || IsWhiteSpaceOnly(review->Principal) ||
        IsWhiteSpaceOnly(review->EvidenceId) || review->ReviewedDecisionId != spec->DecisionId || !FixedEquals(review->ReviewedSpecDigest, spec->SpecDigest) ||
        review->ReviewedSpecCommitSha != specCommitSha)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedReviewMismatch, L"The review evidence does not bind the exact spec identity, digest and commit.");
    return std::nullopt;
}

std::optional<std::shared_ptr<VirtualizationDecisionValidationResultV2>> VirtualizationDecisionValidatorV2::ValidateLineage(const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>& acceptance, const std::vector<std::shared_ptr<VirtualizationDecisionRevocationEvidenceV2>>& revocations, const std::vector<std::shared_ptr<VirtualizationDecisionSupersessionEvidenceV2>>& supersessions)
{
    for (const auto& evidence : revocations)
    {
        if (!evidence || !evidence->Record) return Deny(VirtualizationDecisionValidationDecisionV2::DeniedInvalidLineage, L"Revocation lineage is malformed, noncanonical or has the wrong digest.");
        const auto& record = evidence->Record;
        if (record->SchemaVersion != CurrentSchemaVersion || !VirtualizationDecisionCanonicalEncoderV2::IsCanonicalSha256(record->RevocationDigest) ||
            !FixedEquals(record->RevocationDigest, VirtualizationDecisionCanonicalEncoderV2::ComputeRevocationDigest(record)) ||
            !FixedEquals(evidence->CanonicalBytes, VirtualizationDecisionCanonicalEncoderV2::EncodeRevocation(record)))
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedInvalidLineage, L"Revocation lineage is malformed, noncanonical or has the wrong digest.");
        if (record->DecisionId != acceptance->DecisionId) continue;
        if (!FixedEquals(record->AcceptanceDigest, acceptance->AcceptanceDigest) || record->Sequence == 0 || IsWhiteSpaceOnly(record->RevocationId) || IsWhiteSpaceOnly(record->RevokedBy) || IsWhiteSpaceOnly(record->Reason))
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedInvalidLineage, L"Revocation lineage does not bind the exact accepted record.");
        if (record->State == VirtualizationDecisionRevocationStateV2::Effective)
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedRevoked, L"The accepted record has an effective immutable revocation.");
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedInvalidLineage, L"A draft revocation cannot be treated as active lineage evidence.");
    }
    for (const auto& evidence : supersessions)
    {
        if (!evidence || !evidence->Record) return Deny(VirtualizationDecisionValidationDecisionV2::DeniedInvalidLineage, L"Supersession lineage is malformed, self-referential, noncanonical or has the wrong digest.");
        const auto& record = evidence->Record;
        if (record->SchemaVersion != CurrentSchemaVersion || !VirtualizationDecisionCanonicalEncoderV2::IsCanonicalSha256(record->SupersessionDigest) ||
            !FixedEquals(record->SupersessionDigest, VirtualizationDecisionCanonicalEncoderV2::ComputeSupersessionDigest(record)) ||
            !FixedEquals(evidence->CanonicalBytes, VirtualizationDecisionCanonicalEncoderV2::EncodeSupersession(record)) || record->Sequence == 0 ||
            IsWhiteSpaceOnly(record->SupersessionId) || IsWhiteSpaceOnly(record->SupersededBy) || record->SupersededDecisionId == record->SupersedingDecisionId ||
            FixedEquals(record->SupersededAcceptanceDigest, record->SupersedingAcceptanceDigest))
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedInvalidLineage, L"Supersession lineage is malformed, self-referential, noncanonical or has the wrong digest.");
        if (record->SupersededDecisionId == acceptance->DecisionId)
        {
            if (!FixedEquals(record->SupersededAcceptanceDigest, acceptance->AcceptanceDigest))
                return Deny(VirtualizationDecisionValidationDecisionV2::DeniedInvalidLineage, L"Supersession lineage does not bind the exact superseded acceptance record.");
            if (record->State == VirtualizationDecisionSupersessionStateV2::Effective)
                return Deny(VirtualizationDecisionValidationDecisionV2::DeniedSuperseded, L"The accepted record has an effective immutable supersession.");
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedInvalidLineage, L"A draft supersession cannot be treated as active lineage evidence.");
        }
    }
    const bool hasSupersedesDecision = acceptance->SupersedesDecisionId != nullptr;
    const bool hasSupersedesDigest = acceptance->SupersedesAcceptanceDigest != nullptr;
    if (hasSupersedesDecision != hasSupersedesDigest)
        return Deny(VirtualizationDecisionValidationDecisionV2::DeniedInvalidLineage, L"Superseding acceptance lineage must provide both prior DecisionId and AcceptanceDigest.");
    if (hasSupersedesDecision)
    {
        if (*acceptance->SupersedesDecisionId == acceptance->DecisionId || !VirtualizationDecisionCanonicalEncoderV2::IsCanonicalSha256(*acceptance->SupersedesAcceptanceDigest))
            return Deny(VirtualizationDecisionValidationDecisionV2::DeniedInvalidLineage, L"Acceptance supersession lineage is self-referential or malformed.");
        const bool matching = std::any_of(supersessions.begin(), supersessions.end(), [&](const auto& item) {
            return item && item->Record && item->Record->State == VirtualizationDecisionSupersessionStateV2::Effective &&
                item->Record->SupersededDecisionId == *acceptance->SupersedesDecisionId && FixedEquals(item->Record->SupersededAcceptanceDigest, *acceptance->SupersedesAcceptanceDigest) &&
                item->Record->SupersedingDecisionId == acceptance->DecisionId && FixedEquals(item->Record->SupersedingAcceptanceDigest, acceptance->AcceptanceDigest);
        });
        if (!matching) return Deny(VirtualizationDecisionValidationDecisionV2::DeniedInvalidLineage, L"The superseding acceptance record lacks one matching effective immutable lineage record.");
    }
    return std::nullopt;
}

bool VirtualizationDecisionValidatorV2::FixedEquals(const std::wstring& left, const std::wstring& right)
{
    if (left.size() != right.size()) return false;
    unsigned char difference = 0;
    for (size_t i = 0; i < left.size(); ++i)
    {
        const auto l = static_cast<unsigned char>(left[i] <= 0x7f ? left[i] : L'?');
        const auto r = static_cast<unsigned char>(right[i] <= 0x7f ? right[i] : L'?');
        difference = static_cast<unsigned char>(difference | (l ^ r));
    }
    return difference == 0;
}

bool VirtualizationDecisionValidatorV2::FixedEquals(const std::vector<unsigned char>& left, const std::vector<unsigned char>& right)
{
    if (left.size() != right.size()) return false;
    unsigned char difference = 0;
    for (size_t i = 0; i < left.size(); ++i) difference = static_cast<unsigned char>(difference | (left[i] ^ right[i]));
    return difference == 0;
}

std::shared_ptr<VirtualizationDecisionValidationResultV2> VirtualizationDecisionValidatorV2::Deny(VirtualizationDecisionValidationDecisionV2 decision, const std::wstring& reason)
{
    return std::make_shared<VirtualizationDecisionValidationResultV2>(decision, reason, nullptr);
}
}
