#include "Phase41VmReadScalarDeliveryDecisionSpecV2.h"
#include "Phase41VmReadScalarDeliveryE0Contract.h"
#include "VirtualizationDecisionContractsV2.h"
#include "VmReadScalarDeliveryDecisionValidatorV2.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase41VmReadScalarDeliveryDecisionSpecV2::getInstance()
	{
		return GetInstance();
	}

	std::vector<unsigned char> Phase41VmReadScalarDeliveryDecisionSpecV2::getCanonicalBytes()
	{
		return GetCanonicalBytes();
	}

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase41VmReadScalarDeliveryDecisionSpecV2::GetInstance()
	{
		static const auto instance = Create();
		return instance;
	}

	std::vector<unsigned char> Phase41VmReadScalarDeliveryDecisionSpecV2::GetCanonicalBytes()
	{
		static const auto bytes = VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(GetInstance());
		return bytes;
	}

	bool Phase41VmReadScalarDeliveryDecisionSpecV2::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryDecisionSpecV2::getSourceValueAvailable()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryDecisionSpecV2::getResultReceiptIssued()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryDecisionSpecV2::getRegisterWritebackAuthorized()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryDecisionSpecV2::getRetireCommitAuthorized()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryDecisionSpecV2::getBackendExecutionAuthorized()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryDecisionSpecV2::getUnderlyingVirtualizationMutationAuthorized()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryDecisionSpecV2::getCompletionPublicationAuthorized()
	{
		return false;
	}

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase41VmReadScalarDeliveryDecisionSpecV2::Create()
	{
		std::shared_ptr<VirtualizationDecisionSpecV2> spec = std::make_shared<VirtualizationDecisionSpecV2>(VmReadScalarDeliveryDecisionValidatorV2::SchemaVersion, VmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId, VmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationNamespace, 0, 0, 0, VmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationId, VirtualizationDecisionOwnerClassV2::NeutralRuntimeOwner, VmReadScalarDeliveryDecisionValidatorV2::ExpectedSourceOwnerId, 1, 1, 1, L"VmcsFieldSelectorExactFrozenIdFromCanonicalSourceRegister", L"X0ReservedNoAuthority", L"ArchitecturalDestinationRegisterX1ToX31CanonicalRenameIdentity", VirtualizationDecisionResultAbiV2::ScalarU64ToDestinationRegister, VirtualizationDecisionEffectClassV2::ArchitecturalRegisterResultOnly, VirtualizationCapabilityRequirementV2::None, 0, false, VirtualizationDelegationPolicyV2::NonDelegable, VirtualizationRevocationPolicyV2::GovernanceRevocable, VirtualizationCapabilityMigrationClassV2::None, VirtualizationEvidenceVisibilityV2::GuestVisibleReadOnly, VirtualizationProjectionPolicyV2::ExactReadOnlyFieldSet, VirtualizationExecutionEvidenceRequirementV2::FieldConformanceProof, VirtualizationDomainRequirementV2::ExecutionDomainAndAddressSpaceBound, true, false, false, VirtualizationAddressSpaceRequirementV2::ExactNonZeroAddressSpaceTag, VirtualizationSecureDomainPolicyV2::Deny, VirtualizationCancellationPolicyV2::SquashBeforeRetireZeroArchitecturalEffect, VirtualizationReplayPolicyV2::AttemptBoundReceiptNoReplayReuse, VirtualizationOperationMigrationPolicyV2::DrainOnly, VirtualizationCompletionEvidenceClassV2::None, VirtualizationCompletionMigrationClassV2::None, VirtualizationProjectionPolicyV2::NeverProject, VirtualizationCompletionPolicyV2::None, VirtualizationRetirePolicyV2::CanonicalRetireCoordinatorArchitecturalRegisterCommit, VirtualizationAdjacentLeafPolicyV2::DenyAllExceptExactFieldSet, VirtualizationCrossNamespacePolicyV2::DenyCrossNamespaceReuse, CreateOwnerMap(), std::wstring(L"0", 64), VirtualizationDecisionOperationClassV2::ReadOnlyArchitecturalVmReadScalarDelivery, VirtualizationDecisionAuthorityPlaneV2::PrivilegedExecutionStateSourceCanonicalRegisterDelivery, Phase41VmReadScalarDeliveryE0Contract::getExactFieldIds(), VirtualizationDecisionMutationClassV2::UnderlyingVirtualizationStateReadOnly, VmReadScalarDeliveryDecisionValidatorV2::ExpectedDependencyContract, true, true);
		spec = spec->WithSpecDigest(VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(spec));

		if (spec->SpecDigest != ExpectedSpecDigest)
		{
			throw InvalidOperationException(L"The VMREAD scalar-delivery SpecV2 canonical digest drifted.");
		}

		return spec;
	}

	std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> Phase41VmReadScalarDeliveryDecisionSpecV2::CreateOwnerMap()
	{
		return {Entry(L"VmcsField.GuestCr0", L"PrivilegedExecutionStateDescriptor.GuestCr0"), Entry(L"VmcsField.GuestCr4", L"PrivilegedExecutionStateDescriptor.GuestCr4"),};
	}

	std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> Phase41VmReadScalarDeliveryDecisionSpecV2::Entry(const std::wstring &field, const std::wstring &valueSource)
	{
		return std::make_shared<VirtualizationDecisionOwnerMapEntryV2>(field, L"PrivilegedExecutionStateOwnerPolicy", valueSource, L"None", L"GuestVisibleReadOnlyProjection+FieldConformanceProof+OpaqueAttemptBoundVmReadScalarResultReceipt", L"DrainOnly", L"DenyOnAnyMissingOldProjectionD2OwnerDomainAddressSpaceEpochBitsEvidenceConformanceAttemptReplayDestinationOrCanonicalCarrierGate");
	}
}
