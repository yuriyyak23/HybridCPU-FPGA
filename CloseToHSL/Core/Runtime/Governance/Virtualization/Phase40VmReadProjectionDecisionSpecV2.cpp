#include "Phase40VmReadProjectionDecisionSpecV2.h"
#include "Phase40VmReadProjectionE0Contract.h"
#include "VirtualizationDecisionContractsV2.h"
#include "VirtualizationDecisionValidatorV2.h"
#include "VmReadProjectionDecisionValidatorV2.h"

namespace YAKSys_Hybrid_CPU::Core
{
	std::shared_ptr<VirtualizationDecisionSpecV2> Phase40VmReadProjectionDecisionSpecV2::getInstance()
	{
		return GetInstance();
}
	std::vector<unsigned char> Phase40VmReadProjectionDecisionSpecV2::getCanonicalBytes()
	{
		return GetCanonicalBytes();
}

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase40VmReadProjectionDecisionSpecV2::GetInstance()
	{
		static const auto instance = Create();
		return instance;
	}

	std::vector<unsigned char> Phase40VmReadProjectionDecisionSpecV2::GetCanonicalBytes()
	{
		static const auto bytes = VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(GetInstance());
		return bytes;
	}
	bool Phase40VmReadProjectionDecisionSpecV2::getRuntimeAuthorityGranted()
	{
		return false;
}
	bool Phase40VmReadProjectionDecisionSpecV2::getProjectionValueAvailable()
	{
		return false;
}
	bool Phase40VmReadProjectionDecisionSpecV2::getCapabilityGranted()
	{
		return false;
}
	bool Phase40VmReadProjectionDecisionSpecV2::getBackendExecutionAuthorized()
	{
		return false;
}
	bool Phase40VmReadProjectionDecisionSpecV2::getMutationAuthorized()
	{
		return false;
}
	bool Phase40VmReadProjectionDecisionSpecV2::getCompletionPublicationAuthorized()
	{
		return false;
}
	bool Phase40VmReadProjectionDecisionSpecV2::getRetirePublicationAuthorized()
	{
		return false;
}
	std::shared_ptr<VirtualizationDecisionSpecV2> Phase40VmReadProjectionDecisionSpecV2::Create()
	{
		std::shared_ptr<VirtualizationDecisionSpecV2> spec = std::make_shared<VirtualizationDecisionSpecV2>(VirtualizationDecisionSpecV2{VirtualizationDecisionValidatorV2::CurrentSchemaVersion, VmReadProjectionDecisionValidatorV2::ExpectedDecisionId, VmReadProjectionDecisionValidatorV2::ExpectedOperationNamespace, 0, 0, 0, VmReadProjectionDecisionValidatorV2::ExpectedOperationId, VirtualizationDecisionOwnerClassV2::NeutralRuntimeOwner, VmReadProjectionDecisionValidatorV2::ExpectedOwnerId, 1, 1, 1, L"VmcsFieldSelectorExactFrozenId", L"X0ReservedNoAuthority", L"ArchitecturalDestinationRegisterScalar64", VirtualizationDecisionResultAbiV2::ArchitecturalScalar64, VirtualizationDecisionEffectClassV2::ReadOnlyProjectionNoStateMutation, VirtualizationCapabilityRequirementV2::None, 0, false, VirtualizationDelegationPolicyV2::NonDelegable, VirtualizationRevocationPolicyV2::GovernanceRevocable, VirtualizationCapabilityMigrationClassV2::None, VirtualizationEvidenceVisibilityV2::GuestVisibleReadOnly, VirtualizationProjectionPolicyV2::ExactReadOnlyFieldSet, VirtualizationExecutionEvidenceRequirementV2::FieldConformanceProof, VirtualizationDomainRequirementV2::ExecutionDomainAndAddressSpaceBound, true, false, false, VirtualizationAddressSpaceRequirementV2::ExactNonZeroAddressSpaceTag, VirtualizationSecureDomainPolicyV2::Deny, VirtualizationCancellationPolicyV2::NotApplicableReadOnlyProjection, VirtualizationReplayPolicyV2::NotApplicableReadOnlyProjection, VirtualizationOperationMigrationPolicyV2::RevalidatedAfterRestore, VirtualizationCompletionEvidenceClassV2::None, VirtualizationCompletionMigrationClassV2::None, VirtualizationProjectionPolicyV2::NeverProject, VirtualizationCompletionPolicyV2::None, VirtualizationRetirePolicyV2::None, VirtualizationAdjacentLeafPolicyV2::DenyAllExceptExactFieldSet, VirtualizationCrossNamespacePolicyV2::DenyCrossNamespaceReuse, CreateOwnerMap(), std::wstring(L"0", 64), VirtualizationDecisionOperationClassV2::ReadOnlyArchitecturalVmReadCompatibilityProjection, VirtualizationDecisionAuthorityPlaneV2::PrivilegedExecutionStateReadProjection, Phase40VmReadProjectionE0Contract::getExactFieldIds(), VirtualizationDecisionMutationClassV2::ReadOnly, L"JointDescriptorLegalityGuestCr0AndGuestCr4", true, true});
        spec = spec->WithSpecDigest(VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(spec));
        return spec;
}
	std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> Phase40VmReadProjectionDecisionSpecV2::CreateOwnerMap()
	{
		return {Entry(L"VmcsField.GuestCr0", L"PrivilegedExecutionStateDescriptor.GuestCr0"), Entry(L"VmcsField.GuestCr4", L"PrivilegedExecutionStateDescriptor.GuestCr4"),};
}
	std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> Phase40VmReadProjectionDecisionSpecV2::Entry(const std::wstring &field, const std::wstring &valueSource)
	{
		return std::make_shared<VirtualizationDecisionOwnerMapEntryV2>(VirtualizationDecisionOwnerMapEntryV2{field, L"PrivilegedExecutionStateOwnerPolicy", valueSource, L"None", L"GuestVisibleReadOnlyProjection+FieldConformanceProof", L"RevalidatedAfterRestore", L"DenyOnAnyMissingOwnerDomainAddressSpaceEpochBitsVisibilityMigrationConformanceGate"});
}
}
