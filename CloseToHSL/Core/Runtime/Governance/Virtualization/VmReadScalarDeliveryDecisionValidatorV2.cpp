#include "VmReadScalarDeliveryDecisionValidatorV2.h"
#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include "Phase41VmReadScalarDeliveryE0Contract.h"
#include <exception>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> VmReadScalarDeliveryDecisionValidatorV2::ValidateSpecShape(
		std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec)
	{
		if (!spec || !*spec)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedMissingSpec, L"The scalar-delivery SpecV2 is missing.");
		}
		const auto &value = **spec;
		if (value.SchemaVersion != SchemaVersion || value.DecisionId != ExpectedDecisionId ||
			value.OperationNamespace != ExpectedOperationNamespace || value.OperationId != ExpectedOperationId)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedIdentity, L"Decision, namespace, operation or schema identity mismatched.");
		}
		if (!ValidateExactProfile(*spec))
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedProfile, L"The scalar-delivery policy is wider than the authorized exact profile.");
		}
		if (!ValidateOwnerMap(value.OwnerMap))
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedOwnerMap, L"Both fields must retain the existing privileged execution-state source owner.");
		}
		if (!VirtualizationDecisionCanonicalEncoderV2::IsCanonicalSha256(value.SpecDigest) ||
			!FixedEquals(value.SpecDigest, VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(*spec)))
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedCanonicalDigest, L"The scalar-delivery spec digest is not canonical.");
		}
		return std::make_shared<VmReadScalarDeliveryDecisionValidationResultV2>(
			VmReadScalarDeliveryDecisionValidationDecisionV2::ExactPolicyShape,
			L"Exact scalar-delivery policy shape is canonical; later acceptance and all runtime authorities remain absent.");
	}

	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> VmReadScalarDeliveryDecisionValidatorV2::Validate(
		std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec,
		std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> &acceptance,
		std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> &evidence)
	{
		if (!spec || !*spec || !acceptance || !*acceptance || !evidence || !*evidence || !(**evidence).CodeOwners)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedMissingArtifact, L"SpecV2, later AcceptanceRecordV2 and repository evidence are required.");
		}
		const auto &specValue = **spec;
		const auto &acceptanceValue = **acceptance;
		const auto &evidenceValue = **evidence;

		std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> shape = ValidateSpecShape(spec);
		if (!shape->IsExactPolicyShape)
		{
			return shape;
		}

		if (!VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(acceptanceValue.SpecCommitSha) ||
			!VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(evidenceValue.ResolvedSpecCommitSha) ||
			!VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(evidenceValue.AcceptanceContainingCommitSha) ||
			acceptanceValue.SpecCommitSha != evidenceValue.ResolvedSpecCommitSha ||
			acceptanceValue.SpecCommitSha == evidenceValue.AcceptanceContainingCommitSha)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedProvenance, L"Acceptance must bind exact earlier spec bytes and cannot name its containing commit.");
		}

		try
		{
			if (acceptanceValue.SchemaVersion != SchemaVersion || acceptanceValue.DecisionId != ExpectedDecisionId ||
				!FixedEquals(acceptanceValue.SpecDigest, specValue.SpecDigest) ||
				!FixedEquals(acceptanceValue.AcceptanceDigest, VirtualizationDecisionCanonicalEncoderV2::ComputeAcceptanceDigest(*acceptance)) ||
				!FixedEquals(evidenceValue.SpecCanonicalBytes, VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(*spec)) ||
				!FixedEquals(evidenceValue.SpecBytesAtCommit, VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(*spec)) ||
				!FixedEquals(evidenceValue.AcceptanceCanonicalBytes, VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(*acceptance)))
			{
				return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedAcceptance, L"Acceptance bytes or digests do not bind the exact scalar-delivery spec.");
			}
		}
		catch (const std::exception &)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedAcceptance, L"Acceptance canonical fields are malformed.");
		}

		if (acceptanceValue.AcceptanceState != VirtualizationDecisionAcceptanceStateV2::Accepted ||
			acceptanceValue.AcceptancePolicyVersion != 1 || acceptanceValue.AcceptedBy != L"@yaksysdev")
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedAcceptance, L"Acceptance state, policy version or principal is not exact.");
		}

		if (!ValidateReview(acceptanceValue.OwnerReviewEvidence, VirtualizationDecisionReviewRoleV2::OwnerReviewRole,
			VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, *spec, *acceptance) ||
			!ValidateReview(acceptanceValue.ArchitectureReviewEvidence, VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole,
				VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, *spec, *acceptance))
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedReview, L"Neutral source-owner and architecture reviews must bind the same earlier spec.");
		}

		const auto &codeOwners = evidenceValue.CodeOwners;
		if (!codeOwners->FilePresent || codeOwners->BlobSha != acceptanceValue.CodeOwnersBlobSha)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedCodeOwners, L"Repository attribution is missing for an exact source, carrier or retire scope.");
		}
		for (const auto &scope : RequiredCodeOwnersScopes)
		{
			bool covered = false;
			for (const auto &rule : codeOwners->Rules)
			{
				if (rule && rule->Scope == scope && rule->Principal == acceptanceValue.AcceptedBy)
				{
					covered = true;
					break;
				}
			}
			if (!covered)
			{
				return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedCodeOwners, L"Repository attribution is missing for an exact source, carrier or retire scope.");
			}
		}

		if (!evidenceValue.Revocations.empty() || !evidenceValue.Supersessions.empty() ||
			acceptanceValue.SupersedesDecisionId || acceptanceValue.SupersedesAcceptanceDigest)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedLineage, L"This first scalar-delivery acceptance has no revocation or supersession lineage.");
		}

		const auto exactFieldIds = specValue.ExactFieldIds.value_or(std::vector<unsigned short>{});
		auto accepted = std::make_shared<AcceptedVmReadScalarDeliveryDecisionV2>(AcceptedVmReadScalarDeliveryDecisionV2{
			specValue.DecisionId, specValue.SpecDigest, acceptanceValue.AcceptanceDigest, acceptanceValue.SpecCommitSha,
			specValue.OperationNamespace, exactFieldIds, specValue.OwnerId, specValue.OwnerPolicyVersion,
			specValue.OwnerEpoch, specValue.ResultAbi, specValue.EffectClass, specValue.OperationMigrationPolicy, specValue.RetirePolicy});
		return std::make_shared<VmReadScalarDeliveryDecisionValidationResultV2>(
			VmReadScalarDeliveryDecisionValidationDecisionV2::AcceptedPolicyObject,
			L"Exact scalar-delivery D2 is accepted as governance policy only; no receipt, writeback or retire authority is issued.", accepted);
	}

	bool VmReadScalarDeliveryDecisionValidatorV2::ValidateExactProfile(std::shared_ptr<VirtualizationDecisionSpecV2> spec)
	{
		if (!spec || !spec->ExactFieldIds || *spec->ExactFieldIds != Phase41VmReadScalarDeliveryE0Contract::getExactFieldIds())
		{
			return false;
		}
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
			spec->DomainRequirement == VirtualizationDomainRequirementV2::ExecutionDomainAndAddressSpaceBound &&
			spec->RequireNonZeroDomainTag && !spec->RequiresMemoryDomain && !spec->RequiresIoDomain &&
			spec->AddressSpaceRequirement == VirtualizationAddressSpaceRequirementV2::ExactNonZeroAddressSpaceTag &&
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
			spec->AuthorityPlane == VirtualizationDecisionAuthorityPlaneV2::PrivilegedExecutionStateSourceCanonicalRegisterDelivery &&
			spec->MutationClass == VirtualizationDecisionMutationClassV2::UnderlyingVirtualizationStateReadOnly &&
			spec->DependencyContract == ExpectedDependencyContract && spec->VmcsMetadataOnly && spec->RequiresConformanceProof;
	}

	bool VmReadScalarDeliveryDecisionValidatorV2::ValidateOwnerMap(
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
			if (!entry) return false;
			if (entry->FieldOrOperation == L"VmcsField.GuestCr0") cr0 = entry;
			else if (entry->FieldOrOperation == L"VmcsField.GuestCr4") cr4 = entry;
			else return false;
		}
		return cr0 && cr4 && cr0->Owner == L"PrivilegedExecutionStateOwnerPolicy" && cr4->Owner == cr0->Owner &&
			cr0->ValueSource == L"PrivilegedExecutionStateDescriptor.GuestCr0" &&
			cr4->ValueSource == L"PrivilegedExecutionStateDescriptor.GuestCr4" &&
			cr0->CapabilityPolicy == L"None" && cr4->CapabilityPolicy == L"None" &&
			cr0->MigrationClass == L"DrainOnly" && cr4->MigrationClass == L"DrainOnly" &&
			cr0->EvidenceClass == cr4->EvidenceClass && cr0->DenialReason == cr4->DenialReason;
	}

	bool VmReadScalarDeliveryDecisionValidatorV2::ValidateReview(
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

	bool VmReadScalarDeliveryDecisionValidatorV2::FixedEquals(const std::wstring &left, const std::wstring &right)
	{
		if (left.empty() || right.empty() || left.size() != right.size()) return false;
		unsigned char difference = 0;
		for (std::size_t i = 0; i < left.size(); ++i)
		{
			const auto a = static_cast<unsigned char>(left[i] <= 0x7f ? left[i] : L'?');
			const auto b = static_cast<unsigned char>(right[i] <= 0x7f ? right[i] : L'?');
			difference = static_cast<unsigned char>(difference | (a ^ b));
		}
		return difference == 0;
	}

	bool VmReadScalarDeliveryDecisionValidatorV2::FixedEquals(const std::vector<unsigned char> &left, const std::vector<unsigned char> &right)
	{
		if (left.empty() || right.empty() || left.size() != right.size()) return false;
		unsigned char difference = 0;
		for (std::size_t i = 0; i < left.size(); ++i)
		{
			difference = static_cast<unsigned char>(difference | (left[i] ^ right[i]));
		}
		return difference == 0;
	}

	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> VmReadScalarDeliveryDecisionValidatorV2::Deny(
		VmReadScalarDeliveryDecisionValidationDecisionV2 decision, const std::wstring &reason)
	{
		return std::make_shared<VmReadScalarDeliveryDecisionValidationResultV2>(decision, reason);
	}
}
