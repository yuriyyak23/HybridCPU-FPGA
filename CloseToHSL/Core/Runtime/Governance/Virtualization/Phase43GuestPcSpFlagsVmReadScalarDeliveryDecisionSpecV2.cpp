#include "Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2.h"
#include "GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2.h"
#include "Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract.h"
#include "VirtualizationDecisionContractsV2.h"
#include "tangible_exception_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getInstance()
	{
		return GetInstance();
	}

	std::vector<unsigned char> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getCanonicalBytes()
	{
		return GetCanonicalBytes();
	}

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::GetInstance()
	{
		static const auto instance = Create();
		return instance;
	}

	std::vector<unsigned char> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::GetCanonicalBytes()
	{
		static const auto bytes = VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(GetInstance());
		return bytes;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getSourceValueAvailable()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getResultReceiptIssued()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getRegisterWritebackAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getRetireCommitAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getBackendExecutionAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getUnderlyingVirtualizationMutationAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getCompletionPublicationAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getProductionCompositionAuthorized()
	{
		return false;
	}

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::Create()
	{
		std::shared_ptr<VirtualizationDecisionSpecV2> spec = std::make_shared<VirtualizationDecisionSpecV2>(GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::SchemaVersion, GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId, GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationNamespace, 0, 0, 0, GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationId, VirtualizationDecisionOwnerClassV2::NeutralRuntimeOwner, GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ExpectedSourceOwnerId, 1, 1, 1, L"VmcsFieldSelectorExactFrozenIdFromCanonicalSourceRegister", L"X0ReservedNoAuthority", L"ArchitecturalDestinationRegisterX1ToX31CanonicalRenameIdentity", VirtualizationDecisionResultAbiV2::ScalarU64ToDestinationRegister, VirtualizationDecisionEffectClassV2::ArchitecturalRegisterResultOnly, VirtualizationCapabilityRequirementV2::None, 0, false, VirtualizationDelegationPolicyV2::NonDelegable, VirtualizationRevocationPolicyV2::GovernanceRevocable, VirtualizationCapabilityMigrationClassV2::None, VirtualizationEvidenceVisibilityV2::GuestVisibleReadOnly, VirtualizationProjectionPolicyV2::ExactReadOnlyFieldSet, VirtualizationExecutionEvidenceRequirementV2::FieldConformanceProof, VirtualizationDomainRequirementV2::ExecutionDomainAndAddressSpaceBound, true, true, true, VirtualizationAddressSpaceRequirementV2::ExactNonZeroAddressSpaceTag, VirtualizationSecureDomainPolicyV2::Deny, VirtualizationCancellationPolicyV2::SquashBeforeRetireZeroArchitecturalEffect, VirtualizationReplayPolicyV2::AttemptBoundReceiptNoReplayReuse, VirtualizationOperationMigrationPolicyV2::DrainOnly, VirtualizationCompletionEvidenceClassV2::None, VirtualizationCompletionMigrationClassV2::None, VirtualizationProjectionPolicyV2::NeverProject, VirtualizationCompletionPolicyV2::None, VirtualizationRetirePolicyV2::CanonicalRetireCoordinatorArchitecturalRegisterCommit, VirtualizationAdjacentLeafPolicyV2::DenyAllExceptExactFieldSet, VirtualizationCrossNamespacePolicyV2::DenyCrossNamespaceReuse, CreateOwnerMap(), std::wstring(L"0", 64), VirtualizationDecisionOperationClassV2::ReadOnlyArchitecturalVmReadScalarDelivery, VirtualizationDecisionAuthorityPlaneV2::ExecutionDomainReadOnlyStateCanonicalRegisterDelivery, Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getExactFieldIds(), VirtualizationDecisionMutationClassV2::UnderlyingVirtualizationStateReadOnly, GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ExpectedDependencyContract, true, true);

// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
//		spec = spec with { SpecDigest = VirtualizationDecisionCanonicalEncoderV2.ComputeSpecDigest(spec) };
		spec = spec->WithSpecDigest(VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(spec));

		if (spec->SpecDigest != ExpectedSpecDigest)
		{
			throw InvalidOperationException(std::format(L"Phase 43 SpecV2 digest is {0:s}.", spec->SpecDigest));
		}
		return spec;
	}

	std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::CreateOwnerMap()
	{
		return {Entry(L"VmcsField.GuestPc", L"ExecutionDomainReadOnlyStateView.GuestPc"), Entry(L"VmcsField.GuestSp", L"ExecutionDomainReadOnlyStateView.GuestSp"), Entry(L"VmcsField.GuestFlags", L"ExecutionDomainReadOnlyStateView.GuestFlags"),};
	}

	std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::Entry(const std::wstring &field, const std::wstring &source)
	{
		return std::make_shared<VirtualizationDecisionOwnerMapEntryV2>(field, L"ExecutionDomainDescriptor", source, L"None", L"GuestVisibleReadOnlyCompatibilityProjection+MaterializedFieldAndSourceStateProof+OpaqueAttemptBoundReceipt", L"DrainOnly", L"DenyOnMissingOwnerMaterializationDomainAddressSpaceSourceEpochEvidenceD2AttemptReplayRestoreProfileDestinationOrCanonicalCarrierGate");
	}
}
