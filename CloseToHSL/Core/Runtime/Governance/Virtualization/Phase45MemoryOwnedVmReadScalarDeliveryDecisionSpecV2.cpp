#include "Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2.h"
#include "MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2.h"
#include "Phase45MemoryOwnedVmReadScalarDeliveryE0Contract.h"
#include "VirtualizationDecisionContractsV2.h"
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core
{
	std::shared_ptr<VirtualizationDecisionSpecV2> Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::getInstance()
	{
		return GetInstance();
	}

	std::vector<unsigned char> Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::getCanonicalBytes()
	{
		return GetCanonicalBytes();
	}

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::GetInstance()
	{
		static const auto instance = Create();
		return instance;
	}

	std::vector<unsigned char> Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::GetCanonicalBytes()
	{
		static const auto bytes = VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(GetInstance());
		return bytes;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::getSourceValueAvailable()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::getResultReceiptIssued()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::getProductionCompositionAuthorized()
	{
		return false;
	}

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::Create()
	{
		std::shared_ptr<VirtualizationDecisionSpecV2> spec = std::make_shared<VirtualizationDecisionSpecV2>(VirtualizationDecisionSpecV2{MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::SchemaVersion, MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId, MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationNamespace, 0, 0, 0, MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationId, VirtualizationDecisionOwnerClassV2::NeutralRuntimeOwner, MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ExpectedSourceOwnerId, 1, 1, 1, L"VmcsFieldSelectorExactFrozenIdFromCanonicalSourceRegister", L"X0ReservedNoAuthority", L"ArchitecturalDestinationRegisterX1ToX31CanonicalRenameIdentity", VirtualizationDecisionResultAbiV2::ScalarU64ToDestinationRegister, VirtualizationDecisionEffectClassV2::ArchitecturalRegisterResultOnly, VirtualizationCapabilityRequirementV2::None, 0, false, VirtualizationDelegationPolicyV2::NonDelegable, VirtualizationRevocationPolicyV2::GovernanceRevocable, VirtualizationCapabilityMigrationClassV2::None, VirtualizationEvidenceVisibilityV2::GuestVisibleReadOnly, VirtualizationProjectionPolicyV2::ExactReadOnlyFieldSet, VirtualizationExecutionEvidenceRequirementV2::FieldConformanceProof, VirtualizationDomainRequirementV2::ExecutionDomainAndAddressSpaceBound, true, true, true, VirtualizationAddressSpaceRequirementV2::ExactNonZeroAddressSpaceTag, VirtualizationSecureDomainPolicyV2::Deny, VirtualizationCancellationPolicyV2::SquashBeforeRetireZeroArchitecturalEffect, VirtualizationReplayPolicyV2::AttemptBoundReceiptNoReplayReuse, VirtualizationOperationMigrationPolicyV2::DrainOnly, VirtualizationCompletionEvidenceClassV2::None, VirtualizationCompletionMigrationClassV2::None, VirtualizationProjectionPolicyV2::NeverProject, VirtualizationCompletionPolicyV2::None, VirtualizationRetirePolicyV2::CanonicalRetireCoordinatorArchitecturalRegisterCommit, VirtualizationAdjacentLeafPolicyV2::DenyAllExceptExactFieldSet, VirtualizationCrossNamespacePolicyV2::DenyCrossNamespaceReuse, CreateOwnerMap(), std::wstring(L"0", 64), VirtualizationDecisionOperationClassV2::ReadOnlyArchitecturalVmReadScalarDelivery, VirtualizationDecisionAuthorityPlaneV2::MemoryAddressSpaceReadProjection, Phase45MemoryOwnedVmReadScalarDeliveryE0Contract::getExactFieldIds(), VirtualizationDecisionMutationClassV2::UnderlyingVirtualizationStateReadOnly, MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ExpectedDependencyContract, true, true});
// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
//		spec = spec with { SpecDigest = VirtualizationDecisionCanonicalEncoderV2.ComputeSpecDigest(spec) };
		spec = spec->WithSpecDigest(VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(spec));

		if (spec->SpecDigest != ExpectedSpecDigest)
		{
			throw std::invalid_argument("Phase 45 immutable SpecV2 digest mismatch.");
		}

		return spec;
	}

	std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::CreateOwnerMap()
	{
		return {Entry(L"VmcsField.GuestCr3", L"MemoryDomainReadOnlyTranslationView.AddressSpaceRoot", L"GuestArchitecturalState"), Entry(L"VmcsField.EptPointer", L"MemoryDomainReadOnlyTranslationView.SecondStageRoot", L"CompatibilityAlias+OwnedValidSecondStage"), Entry(L"VmcsField.Vpid", L"MemoryDomainReadOnlyTranslationView.AddressSpaceTag", L"CompatibilityAlias+EnabledNonZeroTag"), Entry(L"VmcsField.Cr3TargetCount", L"MemoryDomainReadOnlyTranslationView.AddressSpaceTargetCount", L"CompatibilityAlias+CanonicalTargetCount"),};
	}

	std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::Entry(const std::wstring &field, const std::wstring &source, const std::wstring &evidence)
	{
		return std::make_shared<VirtualizationDecisionOwnerMapEntryV2>(field, L"MemoryDomainDescriptor", source, L"None", std::format(L"GuestVisibleReadOnly+{0:s}+MaterializedFieldSourceStateProof+RuntimeOwnedAddressSpaceGenerationProof", evidence), L"DrainOnly", L"DenyOnMissingOwnerMaterializationDomainAddressSpaceCurrentGenerationFieldEvidenceD2AttemptReplayRestoreProfileDestinationOrCanonicalCarrierGate");
	}
}


