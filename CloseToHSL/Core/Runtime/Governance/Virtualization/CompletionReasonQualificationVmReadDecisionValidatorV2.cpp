#include "CompletionReasonQualificationVmReadDecisionValidatorV2.h"
#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include "Phase55CompletionReasonQualificationVmReadE0Contract.h"
#include <algorithm>
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	const std::vector<std::wstring> &CompletionReasonQualificationVmReadDecisionValidatorV2::GetRequiredCodeOwnersScopes()
	{
		static const std::vector<std::wstring> scopes = {
			L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Governance/Virtualization/",
			L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Completion/",
			L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/Compatibility/Frontend/Projection/Completion/",
			L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/Retire/",
			L"/HybridCPU_ISE/docs/ref2/VirtualizationActivationPlan/"};
		return scopes;
	}
	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> CompletionReasonQualificationVmReadDecisionValidatorV2::ValidateSpecShape(
		std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec)
	{
		if (!spec || !*spec)
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedMissingSpec, L"Completion projection SpecV2 is missing.");

		const auto &value = **spec;
		if (value.SchemaVersion != SchemaVersion || value.DecisionId != ExpectedDecisionId ||
			value.OperationNamespace != ExpectedOperationNamespace || value.OperationId != ExpectedOperationId)
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedIdentity, L"Decision identity mismatched.");
		if (!ValidateProfile(*spec))
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedProfile, L"Exact completion projection profile mismatched.");
		if (!ValidateOwnerMap(value.OwnerMap))
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedOwnerMap, L"Exact completion owner/value map mismatched.");
		if (!VirtualizationDecisionCanonicalEncoderV2::IsCanonicalSha256(value.SpecDigest) ||
			value.SpecDigest != VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(*spec))
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedCanonicalDigest, L"Spec digest is not canonical.");

		return std::make_shared<VmReadScalarDeliveryDecisionValidationResultV2>(
			VmReadScalarDeliveryDecisionValidationDecisionV2::ExactPolicyShape,
			L"Exact reason/qualification mapping is canonical governance policy; runtime composition remains unauthorized.");
	}

	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> CompletionReasonQualificationVmReadDecisionValidatorV2::Validate(
		std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec,
		std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> &acceptance,
		std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> &evidence)
	{
		if (!spec || !*spec || !acceptance || !*acceptance || !evidence || !*evidence || !(**evidence).CodeOwners)
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedMissingArtifact, L"Spec, later acceptance and evidence are required.");

		const auto &specValue = **spec;
		const auto &acceptanceValue = **acceptance;
		const auto &evidenceValue = **evidence;
		auto shape = ValidateSpecShape(spec);
		if (!shape->IsExactPolicyShape) return shape;

		if (acceptanceValue.SpecCommitSha != evidenceValue.ResolvedSpecCommitSha ||
			acceptanceValue.SpecCommitSha == evidenceValue.AcceptanceContainingCommitSha ||
			!VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(acceptanceValue.SpecCommitSha) ||
			!VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(evidenceValue.AcceptanceContainingCommitSha))
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedProvenance, L"Acceptance must bind an earlier immutable spec commit.");

		if (acceptanceValue.SchemaVersion != SchemaVersion || acceptanceValue.DecisionId != ExpectedDecisionId ||
			!FixedEquals(acceptanceValue.SpecDigest, specValue.SpecDigest) ||
			!FixedEquals(acceptanceValue.AcceptanceDigest, VirtualizationDecisionCanonicalEncoderV2::ComputeAcceptanceDigest(*acceptance)) ||
			!FixedEquals(evidenceValue.SpecCanonicalBytes, VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(*spec)) ||
			!FixedEquals(evidenceValue.SpecBytesAtCommit, VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(*spec)) ||
			!FixedEquals(evidenceValue.AcceptanceCanonicalBytes, VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(*acceptance)))
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedAcceptance, L"Acceptance bytes or digests mismatched.");

		if (acceptanceValue.AcceptanceState != VirtualizationDecisionAcceptanceStateV2::Accepted ||
			acceptanceValue.AcceptancePolicyVersion != 1 || acceptanceValue.AcceptedBy != L"@yaksysdev")
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedAcceptance, L"Acceptance principal or state mismatched.");

		if (!Review(acceptanceValue.OwnerReviewEvidence, VirtualizationDecisionReviewRoleV2::OwnerReviewRole,
			VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, *spec, *acceptance) ||
			!Review(acceptanceValue.ArchitectureReviewEvidence, VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole,
				VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, *spec, *acceptance))
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedReview, L"Required reviews mismatched.");

		const auto &codeOwners = evidenceValue.CodeOwners;
		if (!codeOwners->FilePresent || codeOwners->BlobSha != acceptanceValue.CodeOwnersBlobSha)
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedCodeOwners, L"Required CODEOWNERS coverage is missing.");
		for (const auto &scope : GetRequiredCodeOwnersScopes())
		{
			const bool covered = std::any_of(codeOwners->Rules.begin(), codeOwners->Rules.end(), [&](const auto &rule) {
				return rule && rule->Scope == scope && rule->Principal == acceptanceValue.AcceptedBy;
			});
			if (!covered)
				return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedCodeOwners, L"Required CODEOWNERS coverage is missing.");
		}

		if (!evidenceValue.Revocations.empty() || !evidenceValue.Supersessions.empty() ||
			acceptanceValue.SupersedesDecisionId || acceptanceValue.SupersedesAcceptanceDigest)
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedLineage, L"Earlier VMREAD decisions must remain unchanged.");

		const auto exactFieldIds = specValue.ExactFieldIds.value_or(std::vector<unsigned short>{});
		auto accepted = std::make_shared<AcceptedVmReadScalarDeliveryDecisionV2>(AcceptedVmReadScalarDeliveryDecisionV2{
			specValue.DecisionId, specValue.SpecDigest, acceptanceValue.AcceptanceDigest, acceptanceValue.SpecCommitSha,
			specValue.OperationNamespace, exactFieldIds, specValue.OwnerId, specValue.OwnerPolicyVersion,
			specValue.OwnerEpoch, specValue.ResultAbi, specValue.EffectClass, specValue.OperationMigrationPolicy, specValue.RetirePolicy});
		return std::make_shared<VmReadScalarDeliveryDecisionValidationResultV2>(
			VmReadScalarDeliveryDecisionValidationDecisionV2::AcceptedPolicyObject,
			L"Exact completion reason/qualification D2 accepted as compatibility-only policy.", std::move(accepted));
	}

	bool CompletionReasonQualificationVmReadDecisionValidatorV2::ValidateProfile(std::shared_ptr<VirtualizationDecisionSpecV2> spec)
	{
		if (!spec || !spec->ExactFieldIds || *spec->ExactFieldIds != Phase55CompletionReasonQualificationVmReadE0Contract::getExactFieldIds())
			return false;
		return spec->LeafWidth == 0 && spec->InvalidLeaf == 0 && spec->NumericLeaf == 0 &&
			spec->OwnerClass == VirtualizationDecisionOwnerClassV2::NeutralRuntimeOwner && spec->OwnerId == ExpectedSourceOwnerId &&
			spec->OwnerPolicyVersion == 1 && spec->OwnerEpoch == 1 && spec->OperandAbiVersion == 1 &&
			spec->Rs1Contract == L"VmcsFieldSelectorExactFrozenIdFromCanonicalSourceRegister" &&
			spec->Rs2Contract == L"X0ReservedNoAuthority" &&
			spec->RdContract == L"ArchitecturalDestinationRegisterX1ToX31CanonicalRenameIdentity" &&
			spec->ResultAbi == VirtualizationDecisionResultAbiV2::ScalarU64ToDestinationRegister &&
			spec->EffectClass == VirtualizationDecisionEffectClassV2::ArchitecturalRegisterResultOnly &&
			spec->CapabilityRequirement == VirtualizationCapabilityRequirementV2::None && spec->CapabilityMask == 0 &&
			!spec->RequiresTypedGrant && spec->DelegationPolicy == VirtualizationDelegationPolicyV2::NonDelegable &&
			spec->RevocationPolicy == VirtualizationRevocationPolicyV2::GovernanceRevocable &&
			spec->CapabilityMigrationClass == VirtualizationCapabilityMigrationClassV2::None &&
			spec->EvidenceVisibility == VirtualizationEvidenceVisibilityV2::GuestVisibleReadOnly &&
			spec->FrontendProjectionPolicy == VirtualizationProjectionPolicyV2::ExactReadOnlyFieldSet &&
			spec->ExecutionEvidenceRequirement == VirtualizationExecutionEvidenceRequirementV2::FieldConformanceProof &&
			spec->DomainRequirement == VirtualizationDomainRequirementV2::ExecutionDomainBound &&
			spec->RequireNonZeroDomainTag && !spec->RequiresMemoryDomain && !spec->RequiresIoDomain &&
			spec->AddressSpaceRequirement == VirtualizationAddressSpaceRequirementV2::None &&
			spec->SecureDomainPolicy == VirtualizationSecureDomainPolicyV2::Deny &&
			spec->CancellationPolicy == VirtualizationCancellationPolicyV2::SquashBeforeRetireZeroArchitecturalEffect &&
			spec->ReplayPolicy == VirtualizationReplayPolicyV2::AttemptBoundReceiptNoReplayReuse &&
			spec->OperationMigrationPolicy == VirtualizationOperationMigrationPolicyV2::DrainOnly &&
			spec->CompletionEvidenceClass == VirtualizationCompletionEvidenceClassV2::None &&
			spec->CompletionMigrationClass == VirtualizationCompletionMigrationClassV2::None &&
			spec->CompletionProjectionPolicy == VirtualizationProjectionPolicyV2::NeverProject &&
			spec->CompletionPolicy == VirtualizationCompletionPolicyV2::None &&
			spec->RetirePolicy == VirtualizationRetirePolicyV2::CanonicalRetireCoordinatorArchitecturalRegisterCommit &&
			spec->AdjacentLeafPolicy == VirtualizationAdjacentLeafPolicyV2::DenyAllExceptExactFieldSet &&
			spec->CrossNamespacePolicy == VirtualizationCrossNamespacePolicyV2::DenyCrossNamespaceReuse &&
			spec->OperationClass == VirtualizationDecisionOperationClassV2::ReadOnlyArchitecturalVmReadScalarDelivery &&
			spec->AuthorityPlane == VirtualizationDecisionAuthorityPlaneV2::CompletionObservationReadProjection &&
			spec->MutationClass == VirtualizationDecisionMutationClassV2::UnderlyingVirtualizationStateReadOnly &&
			spec->DependencyContract == ExpectedDependencyContract && spec->VmcsMetadataOnly && spec->RequiresConformanceProof;
	}

	bool CompletionReasonQualificationVmReadDecisionValidatorV2::ValidateOwnerMap(
		const std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> &map)
	{
		if (map.size() != 2) return false;
		bool hasReason = false;
		bool hasQualification = false;
		for (const auto &entry : map)
		{
			if (!entry || entry->Owner != L"DomainCompletionObservationOwner" ||
				entry->ValueSource.find(L"NeutralCompletionObservationSnapshot.") != 0 ||
				entry->CapabilityPolicy != L"None" || entry->MigrationClass != L"DrainOnly" ||
				entry->EvidenceClass.find(L"ExactCpuTranslationProducerSemanticTuple") == std::wstring::npos)
				return false;
			hasReason = hasReason || entry->FieldOrOperation == L"VmcsField.ExitReason";
			hasQualification = hasQualification || entry->FieldOrOperation == L"VmcsField.ExitQualification";
		}
		return hasReason && hasQualification;
	}

	bool CompletionReasonQualificationVmReadDecisionValidatorV2::Review(
		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> review, VirtualizationDecisionReviewRoleV2 role,
		VirtualizationDecisionReviewAuthorityPlaneV2 plane, std::shared_ptr<VirtualizationDecisionSpecV2> spec,
		std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance)
	{
		return review && spec && acceptance && review->Role == role && review->AuthorityPlane == plane &&
			review->State == VirtualizationDecisionReviewStateV2::Completed && review->Principal == acceptance->AcceptedBy &&
			review->ReviewedDecisionId == spec->DecisionId && FixedEquals(review->ReviewedSpecDigest, spec->SpecDigest) &&
			review->ReviewedSpecCommitSha == acceptance->SpecCommitSha && !StringHelper::isEmptyOrWhiteSpace(review->EvidenceId);
	}

	bool CompletionReasonQualificationVmReadDecisionValidatorV2::FixedEquals(const std::wstring &left, const std::wstring &right)
	{
		if (left.size() != right.size()) return false;
		unsigned char difference = 0;
		for (std::size_t index = 0; index < left.size(); ++index)
		{
			const auto lhs = static_cast<unsigned char>(left[index] <= 0x7f ? left[index] : L'?');
			const auto rhs = static_cast<unsigned char>(right[index] <= 0x7f ? right[index] : L'?');
			difference = static_cast<unsigned char>(difference | (lhs ^ rhs));
		}
		return difference == 0;
	}

	bool CompletionReasonQualificationVmReadDecisionValidatorV2::FixedEquals(
		const std::vector<unsigned char> &left, const std::vector<unsigned char> &right)
	{
		if (left.size() != right.size()) return false;
		unsigned char difference = 0;
		for (std::size_t index = 0; index < left.size(); ++index)
			difference = static_cast<unsigned char>(difference | (left[index] ^ right[index]));
		return difference == 0;
	}

	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> CompletionReasonQualificationVmReadDecisionValidatorV2::Deny(
		VmReadScalarDeliveryDecisionValidationDecisionV2 decision, const std::wstring &reason)
	{
		return std::make_shared<VmReadScalarDeliveryDecisionValidationResultV2>(decision, reason);
	}
}
