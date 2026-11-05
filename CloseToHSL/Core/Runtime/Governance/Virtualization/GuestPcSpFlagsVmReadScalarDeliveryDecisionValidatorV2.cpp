#include "GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2.h"
#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include "Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract.h"

#include <exception>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2>
	GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ValidateSpecShape(
		std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec)
	{
		if (!spec || !*spec)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedMissingSpec,
				L"The GuestPc/GuestSp/GuestFlags scalar-delivery SpecV2 is missing.");
		}

		const auto &value = **spec;
		if (value.SchemaVersion != SchemaVersion || value.DecisionId != ExpectedDecisionId ||
			value.OperationNamespace != ExpectedOperationNamespace || value.OperationId != ExpectedOperationId)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedIdentity,
				L"Decision, namespace, operation or schema identity mismatched.");
		}
		if (!ValidateExactProfile(*spec))
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedProfile,
				L"The scalar-delivery policy differs from the exact GuestPc/GuestSp/GuestFlags profile.");
		}
		if (!ValidateOwnerMap(value.OwnerMap))
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedOwnerMap,
				L"Every field must retain ExecutionDomainDescriptor and its materialized read-only view source.");
		}
		if (!VirtualizationDecisionCanonicalEncoderV2::IsCanonicalSha256(value.SpecDigest) ||
			!FixedEquals(value.SpecDigest, VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(*spec)))
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedCanonicalDigest,
				L"The scalar-delivery spec digest is not canonical.");
		}
		return std::make_shared<VmReadScalarDeliveryDecisionValidationResultV2>(
			VmReadScalarDeliveryDecisionValidationDecisionV2::ExactPolicyShape,
			L"Exact policy shape is canonical; acceptance and runtime authority remain absent.");
	}

	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2>
	GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::Validate(
		std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec,
		std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> &acceptance,
		std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> &evidence)
	{
		if (!spec || !*spec || !acceptance || !*acceptance || !evidence || !*evidence || !(**evidence).CodeOwners)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedMissingArtifact,
				L"SpecV2, later AcceptanceRecordV2 and repository evidence are required.");
		}

		const auto &specValue = **spec;
		const auto &acceptanceValue = **acceptance;
		const auto &evidenceValue = **evidence;
		const auto acceptancePtr = *acceptance;
		const auto specPtr = *spec;

		auto shape = ValidateSpecShape(spec);
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
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedProvenance,
				L"Acceptance must bind an exact earlier spec and cannot name its own containing commit.");
		}

		try
		{
			if (acceptanceValue.SchemaVersion != SchemaVersion || acceptanceValue.DecisionId != ExpectedDecisionId ||
				!FixedEquals(acceptanceValue.SpecDigest, specValue.SpecDigest) ||
				!FixedEquals(acceptanceValue.AcceptanceDigest, VirtualizationDecisionCanonicalEncoderV2::ComputeAcceptanceDigest(acceptancePtr)) ||
				!FixedEquals(evidenceValue.SpecCanonicalBytes, VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(specPtr)) ||
				!FixedEquals(evidenceValue.SpecBytesAtCommit, VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(specPtr)) ||
				!FixedEquals(evidenceValue.AcceptanceCanonicalBytes, VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(acceptancePtr)))
			{
				return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedAcceptance,
					L"Acceptance bytes or digests do not bind the exact spec.");
			}
		}
		catch (const std::exception &)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedAcceptance,
				L"Acceptance canonical fields are malformed.");
		}

		if (acceptanceValue.AcceptanceState != VirtualizationDecisionAcceptanceStateV2::Accepted ||
			acceptanceValue.AcceptancePolicyVersion != 1 || acceptanceValue.AcceptedBy != L"@yaksysdev")
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedAcceptance,
				L"Acceptance state, policy version or principal is not exact.");
		}
		if (!ValidateReview(acceptanceValue.OwnerReviewEvidence, VirtualizationDecisionReviewRoleV2::OwnerReviewRole,
			VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, specPtr, acceptancePtr) ||
			!ValidateReview(acceptanceValue.ArchitectureReviewEvidence, VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole,
				VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, specPtr, acceptancePtr))
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedReview,
				L"Neutral source-owner and architecture reviews must bind the earlier spec.");
		}

		const auto &codeOwners = evidenceValue.CodeOwners;
		if (!codeOwners->FilePresent || codeOwners->BlobSha != acceptanceValue.CodeOwnersBlobSha)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedCodeOwners,
				L"Required source, carrier, retire or governance ownership is missing.");
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
				return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedCodeOwners,
					L"Required source, carrier, retire or governance ownership is missing.");
			}
		}

		if (!evidenceValue.Revocations.empty() || !evidenceValue.Supersessions.empty() ||
			acceptanceValue.SupersedesDecisionId || acceptanceValue.SupersedesAcceptanceDigest)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedLineage,
				L"This decision must not modify or supersede an earlier VMREAD decision.");
		}

		const auto exactFieldIds = *specValue.ExactFieldIds;
		auto accepted = std::make_shared<AcceptedVmReadScalarDeliveryDecisionV2>(AcceptedVmReadScalarDeliveryDecisionV2{
			specValue.DecisionId, specValue.SpecDigest, acceptanceValue.AcceptanceDigest, acceptanceValue.SpecCommitSha,
			specValue.OperationNamespace, exactFieldIds, specValue.OwnerId, specValue.OwnerPolicyVersion,
			specValue.OwnerEpoch, specValue.ResultAbi, specValue.EffectClass,
			specValue.OperationMigrationPolicy, specValue.RetirePolicy});
		return std::make_shared<VmReadScalarDeliveryDecisionValidationResultV2>(
			VmReadScalarDeliveryDecisionValidationDecisionV2::AcceptedPolicyObject,
			L"Exact GuestPc/GuestSp/GuestFlags D2 is governance policy only; no receipt or runtime authority is issued.", accepted);
	}

	bool GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ValidateExactProfile(
		std::shared_ptr<VirtualizationDecisionSpecV2> spec)
	{
		if (!spec || !spec->ExactFieldIds || *spec->ExactFieldIds != Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getExactFieldIds())
		{
			return false;
		}
		return spec->LeafWidth == 0 && spec->InvalidLeaf == 0 && spec->NumericLeaf == 0 &&
			spec->OwnerClass == VirtualizationDecisionOwnerClassV2::NeutralRuntimeOwner &&
			spec->OwnerId == ExpectedSourceOwnerId && spec->OwnerPolicyVersion == 1 && spec->OwnerEpoch == 1 &&
			spec->OperandAbiVersion == 1 &&
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
			spec->RequireNonZeroDomainTag && spec->RequiresMemoryDomain && spec->RequiresIoDomain &&
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
			spec->AuthorityPlane == VirtualizationDecisionAuthorityPlaneV2::ExecutionDomainReadOnlyStateCanonicalRegisterDelivery &&
			spec->MutationClass == VirtualizationDecisionMutationClassV2::UnderlyingVirtualizationStateReadOnly &&
			spec->DependencyContract == ExpectedDependencyContract && spec->VmcsMetadataOnly && spec->RequiresConformanceProof;
	}

	bool GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ValidateOwnerMap(
		const std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> &map)
	{
		if (map.size() != 3)
		{
			return false;
		}
		const std::vector<std::wstring> fields = {L"GuestPc", L"GuestSp", L"GuestFlags"};
		for (const auto &field : fields)
		{
			bool found = false;
			for (const auto &entry : map)
			{
				if (!entry)
				{
					return false;
				}
				if (entry->FieldOrOperation != L"VmcsField." + field)
				{
					continue;
				}
				if (found || entry->Owner != L"ExecutionDomainDescriptor" ||
					entry->ValueSource != L"ExecutionDomainReadOnlyStateView." + field ||
					entry->CapabilityPolicy != L"None" || entry->MigrationClass != L"DrainOnly" ||
					entry->EvidenceClass.find(L"MaterializedFieldAndSourceStateProof") == std::wstring::npos)
				{
					return false;
				}
				found = true;
			}
			if (!found)
			{
				return false;
			}
		}
		return true;
	}

	bool GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ValidateReview(
		std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> review,
		VirtualizationDecisionReviewRoleV2 role,
		VirtualizationDecisionReviewAuthorityPlaneV2 plane,
		std::shared_ptr<VirtualizationDecisionSpecV2> spec,
		std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance)
	{
		return review && spec && acceptance && review->Role == role && review->AuthorityPlane == plane &&
			review->State == VirtualizationDecisionReviewStateV2::Completed && review->Principal == acceptance->AcceptedBy &&
			review->ReviewedDecisionId == spec->DecisionId && FixedEquals(review->ReviewedSpecDigest, spec->SpecDigest) &&
			review->ReviewedSpecCommitSha == acceptance->SpecCommitSha &&
			!StringHelper::isEmptyOrWhiteSpace(review->EvidenceId);
	}

	bool GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::FixedEquals(
		const std::wstring &left, const std::wstring &right)
	{
		if (left.empty() || right.empty() || left.size() != right.size())
		{
			return false;
		}
		unsigned char difference = 0;
		for (std::size_t index = 0; index < left.size(); ++index)
		{
			const auto leftByte = static_cast<unsigned char>(left[index] <= 0x7f ? left[index] : L'?');
			const auto rightByte = static_cast<unsigned char>(right[index] <= 0x7f ? right[index] : L'?');
			difference = static_cast<unsigned char>(difference | (leftByte ^ rightByte));
		}
		return difference == 0;
	}

	bool GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::FixedEquals(
		const std::vector<unsigned char> &left, const std::vector<unsigned char> &right)
	{
		if (left.empty() || right.empty() || left.size() != right.size())
		{
			return false;
		}
		unsigned char difference = 0;
		for (std::size_t index = 0; index < left.size(); ++index)
		{
			difference = static_cast<unsigned char>(difference | (left[index] ^ right[index]));
		}
		return difference == 0;
	}

	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2>
	GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::Deny(
		VmReadScalarDeliveryDecisionValidationDecisionV2 decision, const std::wstring &reason)
	{
		return std::make_shared<VmReadScalarDeliveryDecisionValidationResultV2>(decision, reason);
	}
}
