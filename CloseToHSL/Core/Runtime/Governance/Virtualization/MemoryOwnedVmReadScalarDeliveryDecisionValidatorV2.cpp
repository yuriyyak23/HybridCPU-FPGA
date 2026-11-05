#include "MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2.h"
#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include "Phase45MemoryOwnedVmReadScalarDeliveryE0Contract.h"
#include <algorithm>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	// Keep comparisons and collection handling in native C++ containers.

	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ValidateSpecShape(std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec)
	{
		if (!spec || !*spec)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedMissingSpec, L"Memory-owned SpecV2 is missing.");
		}
		const auto &value = **spec;
		if (value.SchemaVersion != SchemaVersion || value.DecisionId != ExpectedDecisionId || value.OperationNamespace != ExpectedOperationNamespace || value.OperationId != ExpectedOperationId)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedIdentity, L"Decision identity mismatched.");
		}
		if (!ValidateExactProfile(*spec))
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedProfile, L"Memory-owned scalar profile mismatched.");
		}
		if (!ValidateOwnerMap(value.OwnerMap))
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedOwnerMap, L"Memory owner/value map mismatched.");
		}
		if (!VirtualizationDecisionCanonicalEncoderV2::IsCanonicalSha256(value.SpecDigest) || value.SpecDigest != VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(*spec))
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedCanonicalDigest, L"Spec digest is not canonical.");
		}
		return std::make_shared<VmReadScalarDeliveryDecisionValidationResultV2>(VmReadScalarDeliveryDecisionValidationDecisionV2::ExactPolicyShape, L"Exact memory-owned policy shape is canonical; acceptance and runtime authority remain absent.");
	}

	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::Validate(std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> &spec, std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> &acceptance, std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> &evidence)
	{
		if (!spec || !*spec || !acceptance || !*acceptance || !evidence || !*evidence || !(**evidence).CodeOwners)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedMissingArtifact, L"Spec, later acceptance and evidence are required.");
		}
		const auto &specValue = **spec;
		const auto &acceptanceValue = **acceptance;
		const auto &evidenceValue = **evidence;
		std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> shape = ValidateSpecShape(spec);
		if (!shape->IsExactPolicyShape)
		{
			return shape;
		}
		if (acceptanceValue.SpecCommitSha != evidenceValue.ResolvedSpecCommitSha || acceptanceValue.SpecCommitSha == evidenceValue.AcceptanceContainingCommitSha || !VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(acceptanceValue.SpecCommitSha) || !VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(evidenceValue.AcceptanceContainingCommitSha))
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedProvenance, L"Acceptance must bind an earlier immutable spec.");
		}
		if (acceptanceValue.SchemaVersion != SchemaVersion || acceptanceValue.DecisionId != ExpectedDecisionId || !FixedEquals(acceptanceValue.SpecDigest, specValue.SpecDigest) || !FixedEquals(acceptanceValue.AcceptanceDigest, VirtualizationDecisionCanonicalEncoderV2::ComputeAcceptanceDigest(*acceptance)) || !FixedEquals(evidenceValue.SpecCanonicalBytes, VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(*spec)) || !FixedEquals(evidenceValue.SpecBytesAtCommit, VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(*spec)) || !FixedEquals(evidenceValue.AcceptanceCanonicalBytes, VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(*acceptance)))
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedAcceptance, L"Acceptance bytes or digests mismatched.");
		}
		if (acceptanceValue.AcceptanceState != VirtualizationDecisionAcceptanceStateV2::Accepted || acceptanceValue.AcceptancePolicyVersion != 1 || acceptanceValue.AcceptedBy != L"@yaksysdev")
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedAcceptance, L"Acceptance principal or state mismatched.");
		}
		if (!Review(acceptanceValue.OwnerReviewEvidence, VirtualizationDecisionReviewRoleV2::OwnerReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, *spec, *acceptance) || !Review(acceptanceValue.ArchitectureReviewEvidence, VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, *spec, *acceptance))
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedReview, L"Required reviews mismatched.");
		}
		if (!evidenceValue.CodeOwners->FilePresent || evidenceValue.CodeOwners->BlobSha != acceptanceValue.CodeOwnersBlobSha)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedCodeOwners, L"Required CODEOWNERS coverage is missing.");
		}
		for (const auto &scope : RequiredCodeOwnersScopes)
		{
			const bool covered = std::any_of(evidenceValue.CodeOwners->Rules.begin(), evidenceValue.CodeOwners->Rules.end(),
				[&](const auto &rule) { return rule && rule->Scope == scope && rule->Principal == acceptanceValue.AcceptedBy; });
			if (!covered)
			{
				return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedCodeOwners, L"Required CODEOWNERS coverage is missing.");
			}
		}
		if (!evidenceValue.Revocations.empty() || !evidenceValue.Supersessions.empty() || acceptanceValue.SupersedesDecisionId || acceptanceValue.SupersedesAcceptanceDigest)
		{
			return Deny(VmReadScalarDeliveryDecisionValidationDecisionV2::DeniedLineage, L"Earlier VMREAD decisions must remain unchanged.");
		}
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return new(VmReadScalarDeliveryDecisionValidationDecisionV2.AcceptedPolicyObject, "Exact memory-owned D2 is governance policy only; production composition remains unauthorized.", new AcceptedVmReadScalarDeliveryDecisionV2( spec.DecisionId, spec.SpecDigest, acceptance.AcceptanceDigest, acceptance.SpecCommitSha, spec.OperationNamespace, spec.ExactFieldIds!.Value, spec.OwnerId, spec.OwnerPolicyVersion, spec.OwnerEpoch, spec.ResultAbi, spec.EffectClass, spec.OperationMigrationPolicy, spec.RetirePolicy));
		return std::make_shared<VmReadScalarDeliveryDecisionValidationResultV2>(
			VmReadScalarDeliveryDecisionValidationDecisionV2::AcceptedPolicyObject,
			L"Exact memory-owned D2 is governance policy only; production composition remains unauthorized.",
			std::make_shared<AcceptedVmReadScalarDeliveryDecisionV2>(AcceptedVmReadScalarDeliveryDecisionV2{
				specValue.DecisionId, specValue.SpecDigest, acceptanceValue.AcceptanceDigest, acceptanceValue.SpecCommitSha,
				specValue.OperationNamespace, specValue.ExactFieldIds.value_or(std::vector<unsigned short>{}),
				specValue.OwnerId, specValue.OwnerPolicyVersion, specValue.OwnerEpoch, specValue.ResultAbi,
				specValue.EffectClass, specValue.OperationMigrationPolicy, specValue.RetirePolicy}));
	}

	bool MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ValidateExactProfile(std::shared_ptr<VirtualizationDecisionSpecV2> spec)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		return spec->LeafWidth == 0 && spec->InvalidLeaf == 0 && spec->NumericLeaf == 0 && spec->OwnerClass == VirtualizationDecisionOwnerClassV2::NeutralRuntimeOwner && spec->OwnerId == ExpectedSourceOwnerId && spec->OwnerPolicyVersion == 1 && spec->OwnerEpoch == 1 && spec->OperandAbiVersion == 1 && spec->Rs1Contract == L"VmcsFieldSelectorExactFrozenIdFromCanonicalSourceRegister" && spec->Rs2Contract == L"X0ReservedNoAuthority" && spec->RdContract == L"ArchitecturalDestinationRegisterX1ToX31CanonicalRenameIdentity" && spec->ResultAbi == VirtualizationDecisionResultAbiV2::ScalarU64ToDestinationRegister && spec->EffectClass == VirtualizationDecisionEffectClassV2::ArchitecturalRegisterResultOnly && spec->CapabilityRequirement == VirtualizationCapabilityRequirementV2::None && spec->CapabilityMask == 0 && !spec->RequiresTypedGrant && spec->DelegationPolicy == VirtualizationDelegationPolicyV2::NonDelegable && spec->RevocationPolicy == VirtualizationRevocationPolicyV2::GovernanceRevocable && spec->CapabilityMigrationClass == VirtualizationCapabilityMigrationClassV2::None && spec->EvidenceVisibility == VirtualizationEvidenceVisibilityV2::GuestVisibleReadOnly && spec->FrontendProjectionPolicy == VirtualizationProjectionPolicyV2::ExactReadOnlyFieldSet && spec->ExecutionEvidenceRequirement == VirtualizationExecutionEvidenceRequirementV2::FieldConformanceProof && spec->DomainRequirement == VirtualizationDomainRequirementV2::ExecutionDomainAndAddressSpaceBound && spec->RequireNonZeroDomainTag && spec->RequiresMemoryDomain && spec->RequiresIoDomain && spec->AddressSpaceRequirement == VirtualizationAddressSpaceRequirementV2::ExactNonZeroAddressSpaceTag && spec->SecureDomainPolicy == VirtualizationSecureDomainPolicyV2::Deny && spec->CancellationPolicy == VirtualizationCancellationPolicyV2::SquashBeforeRetireZeroArchitecturalEffect && spec->ReplayPolicy == VirtualizationReplayPolicyV2::AttemptBoundReceiptNoReplayReuse && spec->OperationMigrationPolicy == VirtualizationOperationMigrationPolicyV2::DrainOnly && spec->CompletionEvidenceClass == VirtualizationCompletionEvidenceClassV2::None && spec->CompletionMigrationClass == VirtualizationCompletionMigrationClassV2::None && spec->CompletionProjectionPolicy == VirtualizationProjectionPolicyV2::NeverProject && spec->CompletionPolicy == VirtualizationCompletionPolicyV2::None && spec->RetirePolicy == VirtualizationRetirePolicyV2::CanonicalRetireCoordinatorArchitecturalRegisterCommit && spec->AdjacentLeafPolicy == VirtualizationAdjacentLeafPolicyV2::DenyAllExceptExactFieldSet && spec->CrossNamespacePolicy == VirtualizationCrossNamespacePolicyV2::DenyCrossNamespaceReuse && spec->OperationClass == VirtualizationDecisionOperationClassV2::ReadOnlyArchitecturalVmReadScalarDelivery && spec->AuthorityPlane == VirtualizationDecisionAuthorityPlaneV2::MemoryAddressSpaceReadProjection && spec->MutationClass == VirtualizationDecisionMutationClassV2::UnderlyingVirtualizationStateReadOnly && spec->DependencyContract == ExpectedDependencyContract && spec->VmcsMetadataOnly && spec->RequiresConformanceProof && spec->ExactFieldIds.has_value() && *spec->ExactFieldIds == Phase45MemoryOwnedVmReadScalarDeliveryE0Contract::getExactFieldIds();
	}

	bool MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ValidateOwnerMap(const std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> &map)
	{
		if (map.empty() || map.size() != 4)
		{
			return false;
		}
		const std::vector<std::wstring> fields = {L"VmcsField.GuestCr3", L"VmcsField.EptPointer", L"VmcsField.Vpid", L"VmcsField.Cr3TargetCount"};
		for (const auto &field : fields)
		{
			const auto found = std::find_if(map.begin(), map.end(), [&](const auto &entry)
			{
				return entry && entry->FieldOrOperation == field;
			});
			if (found == map.end())
			{
				return false;
			}
			const auto &entry = **found;
			if (entry.Owner != L"MemoryDomainDescriptor" || entry.ValueSource.rfind(L"MemoryDomainReadOnlyTranslationView.", 0) != 0 ||
				entry.CapabilityPolicy != L"None" || entry.MigrationClass != L"DrainOnly" ||
				entry.EvidenceClass.find(L"RuntimeOwnedAddressSpaceGenerationProof") == std::wstring::npos)
			{
				return false;
			}
		}
		return true;
	}

	bool MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::Review(std::shared_ptr<VirtualizationDecisionReviewEvidenceV2> review, VirtualizationDecisionReviewRoleV2 role, VirtualizationDecisionReviewAuthorityPlaneV2 plane, std::shared_ptr<VirtualizationDecisionSpecV2> spec, std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> acceptance)
	{
		return review != nullptr && review->Role == role && review->AuthorityPlane == plane && review->State == VirtualizationDecisionReviewStateV2::Completed && review->Principal == acceptance->AcceptedBy && review->ReviewedDecisionId == spec->DecisionId && FixedEquals(review->ReviewedSpecDigest, spec->SpecDigest) && review->ReviewedSpecCommitSha == acceptance->SpecCommitSha && !StringHelper::isEmptyOrWhiteSpace(review->EvidenceId);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool FixedEquals(string? left, string? right)
	bool MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::FixedEquals(const std::wstring &left, const std::wstring &right)
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

	bool MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::FixedEquals(const std::vector<unsigned char> &left, const std::vector<unsigned char> &right)
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

	std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::Deny(VmReadScalarDeliveryDecisionValidationDecisionV2 decision, const std::wstring &reason)
	{
		return std::make_shared<VmReadScalarDeliveryDecisionValidationResultV2>(decision, reason);
	}
}
