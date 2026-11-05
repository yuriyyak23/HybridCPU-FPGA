#include "VmReadProjectionDecisionValidatorV2.h"
#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include "VirtualizationDecisionValidatorV2.h"
#include "Phase40VmReadProjectionE0Contract.h"
#include <algorithm>
#include <exception>

namespace YAKSys_Hybrid_CPU::Core
{
	const std::vector<std::wstring> &VmReadProjectionDecisionValidatorV2::GetRequiredCodeOwnersScopes()
	{
		static const std::vector<std::wstring> scopes = {
			L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Governance/Virtualization/",
			L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/SecureCompute/Descriptors/ExecutionState/",
			L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/SecureCompute/Policies/ExecutionState/",
			L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/SecureCompute/Compatibility/Projection/",
			L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/Compatibility/Frontend/Projection/VmcsRead/",
			L"/HybridCPU_ISE/docs/ref2/VirtualizationActivationPlan/"};
		return scopes;
	}

	std::shared_ptr<VmReadProjectionDecisionValidationResultV2> VmReadProjectionDecisionValidatorV2::Validate(
		std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec,
		std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> &acceptance,
		std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> &evidence)
	{
		if (!spec || !*spec || !acceptance || !*acceptance || !evidence || !*evidence || !(**evidence).CodeOwners)
		{
			return Deny(VmReadProjectionDecisionValidationDecisionV2::DeniedMissingArtifact, L"SpecV2, later AcceptanceRecordV2 and repository evidence are required.");
		}

		const auto &specValue = **spec;
		const auto &acceptanceValue = **acceptance;
		const auto &evidenceValue = **evidence;
		if (!ValidateProvenance(*acceptance, *evidence))
		{
			return Deny(VmReadProjectionDecisionValidationDecisionV2::DeniedProvenance, L"Acceptance must resolve exact earlier spec bytes and cannot name its containing commit.");
		}

		try
		{
			if (specValue.SchemaVersion != VirtualizationDecisionValidatorV2::CurrentSchemaVersion ||
				acceptanceValue.SchemaVersion != VirtualizationDecisionValidatorV2::CurrentSchemaVersion ||
				!VirtualizationDecisionCanonicalEncoderV2::IsCanonicalSha256(specValue.SpecDigest) ||
				!FixedEquals(specValue.SpecDigest, VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(*spec)) ||
				!FixedEquals(acceptanceValue.SpecDigest, specValue.SpecDigest) ||
				!FixedEquals(acceptanceValue.AcceptanceDigest, VirtualizationDecisionCanonicalEncoderV2::ComputeAcceptanceDigest(*acceptance)) ||
				!FixedEquals(evidenceValue.SpecCanonicalBytes, VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(*spec)) ||
				!FixedEquals(evidenceValue.SpecBytesAtCommit, VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(*spec)) ||
				!FixedEquals(evidenceValue.AcceptanceCanonicalBytes, VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(*acceptance)))
			{
				return Deny(VmReadProjectionDecisionValidationDecisionV2::DeniedCanonicalArtifact, L"Spec or acceptance bytes/digests are not their exact canonical V2 forms.");
			}
		}
		catch (const std::exception &)
		{
			return Deny(VmReadProjectionDecisionValidationDecisionV2::DeniedCanonicalArtifact, L"Canonical artifact fields are malformed.");
		}

		if (specValue.DecisionId != ExpectedDecisionId || acceptanceValue.DecisionId != ExpectedDecisionId ||
			specValue.OperationNamespace != ExpectedOperationNamespace || specValue.OperationId != ExpectedOperationId)
		{
			return Deny(VmReadProjectionDecisionValidationDecisionV2::DeniedIdentity, L"Decision, namespace and operation identity must match the exact VMREAD profile.");
		}

		if (!ValidateExactProfile(*spec))
		{
			return Deny(VmReadProjectionDecisionValidationDecisionV2::DeniedProfile, L"The VMREAD policy is wider than the exact read-only GuestCr0/GuestCr4 profile.");
		}

		if (!ValidateOwnerMap(specValue.OwnerMap))
		{
			return Deny(VmReadProjectionDecisionValidationDecisionV2::DeniedOwnerMap, L"Both fields must bind the same neutral owner/source class and exact field-local value source.");
		}

		if (!ValidateAcceptance(*spec, *acceptance))
		{
			return Deny(VmReadProjectionDecisionValidationDecisionV2::DeniedAcceptance, L"Acceptance does not bind the exact immutable spec with accepted governance state.");
		}

		if (!ValidateReviews(*spec, *acceptance))
		{
			return Deny(VmReadProjectionDecisionValidationDecisionV2::DeniedReview, L"Neutral-owner and architecture reviews must bind the same earlier spec commit and digest.");
		}

		if (!ValidateCodeOwners(*acceptance, evidenceValue.CodeOwners))
		{
			return Deny(VmReadProjectionDecisionValidationDecisionV2::DeniedCodeOwners, L"Repository attribution is missing for one or more exact owner/projection scopes.");
		}

		if (!evidenceValue.Revocations.empty() || !evidenceValue.Supersessions.empty() || acceptanceValue.SupersedesDecisionId || acceptanceValue.SupersedesAcceptanceDigest)
		{
			return Deny(VmReadProjectionDecisionValidationDecisionV2::DeniedLineage, L"This first VMREAD D2 acceptance has no revocation or supersession lineage.");
		}

		const auto exactFieldIds = specValue.ExactFieldIds.value_or(std::vector<unsigned short>{});
		std::shared_ptr<AcceptedVmReadProjectionDecisionV2> accepted = std::make_shared<AcceptedVmReadProjectionDecisionV2>(AcceptedVmReadProjectionDecisionV2{
			specValue.DecisionId, specValue.SpecDigest, acceptanceValue.AcceptanceDigest, acceptanceValue.SpecCommitSha,
			specValue.OperationNamespace, exactFieldIds, specValue.OwnerId, specValue.OwnerPolicyVersion,
			specValue.OwnerEpoch, specValue.MutationClass});

		return std::make_shared<VmReadProjectionDecisionValidationResultV2>(VmReadProjectionDecisionValidationDecisionV2::AcceptedPolicyObject,
			L"Exact GuestCr0/GuestCr4 D2 is accepted as immutable governance policy only; no runtime authority is granted.", accepted);
	}

	bool VmReadProjectionDecisionValidatorV2::ValidateProvenance(
		std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance,
		std::shared_ptr<VirtualizationDecisionValidationEvidenceV2> evidence)
	{
		return acceptance && evidence &&
			VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(acceptance->SpecCommitSha) &&
			VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(acceptance->CodeOwnersBlobSha) &&
			VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(evidence->ResolvedSpecCommitSha) &&
			VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(evidence->AcceptanceContainingCommitSha) &&
			acceptance->SpecCommitSha == evidence->ResolvedSpecCommitSha &&
			acceptance->SpecCommitSha != evidence->AcceptanceContainingCommitSha;
	}

	bool VmReadProjectionDecisionValidatorV2::ValidateExactProfile(std::shared_ptr<VirtualizationDecisionSpecV2> spec)
	{
		if (!spec || !spec->ExactFieldIds || *spec->ExactFieldIds != Phase40VmReadProjectionE0Contract::getExactFieldIds())
		{
			return false;
		}

		return spec->LeafWidth == 0 && spec->InvalidLeaf == 0 && spec->NumericLeaf == 0 &&
			spec->OwnerClass == VirtualizationDecisionOwnerClassV2::NeutralRuntimeOwner && spec->OwnerId == ExpectedOwnerId &&
			spec->OwnerPolicyVersion == 1 && spec->OwnerEpoch == 1 && spec->OperandAbiVersion == 1 &&
			spec->Rs1Contract == L"VmcsFieldSelectorExactFrozenId" && spec->Rs2Contract == L"X0ReservedNoAuthority" &&
			spec->RdContract == L"ArchitecturalDestinationRegisterScalar64" &&
			spec->ResultAbi == VirtualizationDecisionResultAbiV2::ArchitecturalScalar64 &&
			spec->EffectClass == VirtualizationDecisionEffectClassV2::ReadOnlyProjectionNoStateMutation &&
			spec->CapabilityRequirement == VirtualizationCapabilityRequirementV2::None && spec->CapabilityMask == 0 &&
			!spec->RequiresTypedGrant && spec->DelegationPolicy == VirtualizationDelegationPolicyV2::NonDelegable &&
			spec->RevocationPolicy == VirtualizationRevocationPolicyV2::GovernanceRevocable &&
			spec->CapabilityMigrationClass == VirtualizationCapabilityMigrationClassV2::None &&
			spec->EvidenceVisibility == VirtualizationEvidenceVisibilityV2::GuestVisibleReadOnly &&
			spec->FrontendProjectionPolicy == VirtualizationProjectionPolicyV2::ExactReadOnlyFieldSet &&
			spec->ExecutionEvidenceRequirement == VirtualizationExecutionEvidenceRequirementV2::FieldConformanceProof &&
			spec->DomainRequirement == VirtualizationDomainRequirementV2::ExecutionDomainAndAddressSpaceBound &&
			spec->RequireNonZeroDomainTag && !spec->RequiresMemoryDomain && !spec->RequiresIoDomain &&
			spec->AddressSpaceRequirement == VirtualizationAddressSpaceRequirementV2::ExactNonZeroAddressSpaceTag &&
			spec->SecureDomainPolicy == VirtualizationSecureDomainPolicyV2::Deny &&
			spec->CancellationPolicy == VirtualizationCancellationPolicyV2::NotApplicableReadOnlyProjection &&
			spec->ReplayPolicy == VirtualizationReplayPolicyV2::NotApplicableReadOnlyProjection &&
			spec->OperationMigrationPolicy == VirtualizationOperationMigrationPolicyV2::RevalidatedAfterRestore &&
			spec->CompletionEvidenceClass == VirtualizationCompletionEvidenceClassV2::None &&
			spec->CompletionMigrationClass == VirtualizationCompletionMigrationClassV2::None &&
			spec->CompletionProjectionPolicy == VirtualizationProjectionPolicyV2::NeverProject &&
			spec->CompletionPolicy == VirtualizationCompletionPolicyV2::None && spec->RetirePolicy == VirtualizationRetirePolicyV2::None &&
			spec->AdjacentLeafPolicy == VirtualizationAdjacentLeafPolicyV2::DenyAllExceptExactFieldSet &&
			spec->CrossNamespacePolicy == VirtualizationCrossNamespacePolicyV2::DenyCrossNamespaceReuse &&
			spec->OperationClass == VirtualizationDecisionOperationClassV2::ReadOnlyArchitecturalVmReadCompatibilityProjection &&
			spec->AuthorityPlane == VirtualizationDecisionAuthorityPlaneV2::PrivilegedExecutionStateReadProjection &&
			spec->MutationClass == VirtualizationDecisionMutationClassV2::ReadOnly &&
			spec->DependencyContract == L"JointDescriptorLegalityGuestCr0AndGuestCr4" &&
			spec->VmcsMetadataOnly && spec->RequiresConformanceProof;
	}

	bool VmReadProjectionDecisionValidatorV2::ValidateOwnerMap(
		const std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> &ownerMap)
	{
		if (ownerMap.size() != 2)
		{
			return false;
		}
		std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> cr0;
		std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> cr4;
		for (const auto &entry : ownerMap)
		{
			if (!entry)
			{
				return false;
			}
			if (entry->FieldOrOperation == L"VmcsField.GuestCr0") cr0 = entry;
			else if (entry->FieldOrOperation == L"VmcsField.GuestCr4") cr4 = entry;
			else return false;
		}
		return cr0 && cr4 && cr0->Owner == L"PrivilegedExecutionStateOwnerPolicy" && cr4->Owner == cr0->Owner &&
			cr0->ValueSource == L"PrivilegedExecutionStateDescriptor.GuestCr0" &&
			cr4->ValueSource == L"PrivilegedExecutionStateDescriptor.GuestCr4" &&
			cr0->CapabilityPolicy == L"None" && cr4->CapabilityPolicy == cr0->CapabilityPolicy &&
			cr0->EvidenceClass == cr4->EvidenceClass && cr0->MigrationClass == L"RevalidatedAfterRestore" &&
			cr4->MigrationClass == cr0->MigrationClass && cr0->DenialReason == cr4->DenialReason;
	}

	bool VmReadProjectionDecisionValidatorV2::ValidateAcceptance(
		std::shared_ptr<VirtualizationDecisionSpecV2> spec,
		std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance)
	{
		return spec && acceptance && acceptance->AcceptanceState == VirtualizationDecisionAcceptanceStateV2::Accepted &&
			acceptance->AcceptancePolicyVersion == 1 && acceptance->AcceptedBy == L"@yaksysdev" &&
			FixedEquals(acceptance->SpecDigest, spec->SpecDigest) &&
			VirtualizationDecisionCanonicalEncoderV2::IsCanonicalSha256(acceptance->AcceptanceDigest);
	}

	bool VmReadProjectionDecisionValidatorV2::ValidateReviews(
		std::shared_ptr<VirtualizationDecisionSpecV2> spec,
		std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance)
	{
		return spec && acceptance &&
			ValidateReview(acceptance->OwnerReviewEvidence, VirtualizationDecisionReviewRoleV2::OwnerReviewRole,
				VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, spec, acceptance) &&
			ValidateReview(acceptance->ArchitectureReviewEvidence, VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole,
				VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, spec, acceptance);
	}

	bool VmReadProjectionDecisionValidatorV2::ValidateReview(
		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> review,
		VirtualizationDecisionReviewRoleV2 role,
		VirtualizationDecisionReviewAuthorityPlaneV2 plane,
		std::shared_ptr<VirtualizationDecisionSpecV2> spec,
		std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance)
	{
		return review && spec && acceptance && review->Role == role && review->AuthorityPlane == plane &&
			review->State == VirtualizationDecisionReviewStateV2::Completed && review->Principal == acceptance->AcceptedBy &&
			review->ReviewedDecisionId == spec->DecisionId && FixedEquals(review->ReviewedSpecDigest, spec->SpecDigest) &&
			review->ReviewedSpecCommitSha == acceptance->SpecCommitSha && !StringHelper::isEmptyOrWhiteSpace(review->EvidenceId);
	}

	bool VmReadProjectionDecisionValidatorV2::ValidateCodeOwners(
		std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance,
		std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> codeOwners)
	{
		if (!acceptance || !codeOwners || !codeOwners->FilePresent || codeOwners->BlobSha != acceptance->CodeOwnersBlobSha)
		{
			return false;
		}
		for (const auto &scope : GetRequiredCodeOwnersScopes())
		{
			const bool covered = std::any_of(codeOwners->Rules.begin(), codeOwners->Rules.end(),
				[&](const auto &rule) { return rule && rule->Scope == scope && rule->Principal == acceptance->AcceptedBy; });
			if (!covered)
			{
				return false;
			}
		}
		return true;
	}

	bool VmReadProjectionDecisionValidatorV2::FixedEquals(const std::wstring &left, const std::wstring &right)
	{
		if (left.empty() || right.empty() || left.size() != right.size())
		{
			return false;
		}
		unsigned char difference = 0;
		for (std::size_t i = 0; i < left.size(); ++i)
		{
			const auto leftByte = static_cast<unsigned char>(left[i] <= 0x7f ? left[i] : L'?');
			const auto rightByte = static_cast<unsigned char>(right[i] <= 0x7f ? right[i] : L'?');
			difference = static_cast<unsigned char>(difference | (leftByte ^ rightByte));
		}
		return difference == 0;
	}

	bool VmReadProjectionDecisionValidatorV2::FixedEquals(const std::vector<unsigned char> &left, const std::vector<unsigned char> &right)
	{
		if (left.empty() || right.empty() || left.size() != right.size())
		{
			return false;
		}
		unsigned char difference = 0;
		for (std::size_t i = 0; i < left.size(); ++i)
		{
			difference = static_cast<unsigned char>(difference | (left[i] ^ right[i]));
		}
		return difference == 0;
	}

	std::shared_ptr<VmReadProjectionDecisionValidationResultV2> VmReadProjectionDecisionValidatorV2::Deny(
		VmReadProjectionDecisionValidationDecisionV2 decision, const std::wstring &reason)
	{
		return std::make_shared<VmReadProjectionDecisionValidationResultV2>(decision, reason, nullptr);
	}
}
