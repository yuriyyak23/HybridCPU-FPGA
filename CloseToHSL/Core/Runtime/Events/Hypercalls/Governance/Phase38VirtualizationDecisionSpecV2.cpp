#include "Phase38VirtualizationDecisionSpecV2.h"
#include "../../../Governance/Virtualization/VirtualizationDecisionContractsV2.h"
#include "../../../Governance/Virtualization/VirtualizationDecisionValidatorV2.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase38VirtualizationDecisionSpecV2::getInstance()
	{
		static const auto instance = Create();
		return instance;
}
	std::vector<unsigned char> Phase38VirtualizationDecisionSpecV2::getCanonicalBytes()
	{
		static const auto bytes = VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(getInstance());
		return bytes;
}
	bool Phase38VirtualizationDecisionSpecV2::getRuntimeAuthorityGranted()
	{
		return false;
}
	bool Phase38VirtualizationDecisionSpecV2::getBackendExecutionAuthorized()
	{
		return false;
}
	bool Phase38VirtualizationDecisionSpecV2::getCompletionPublicationAuthorized()
	{
		return false;
}
	bool Phase38VirtualizationDecisionSpecV2::getRetirePublicationAuthorized()
	{
		return false;
}
	std::shared_ptr<VirtualizationDecisionSpecV2> Phase38VirtualizationDecisionSpecV2::Create()
	{
		std::shared_ptr<VirtualizationDecisionSpecV2> spec = std::make_shared<VirtualizationDecisionSpecV2>(VirtualizationDecisionValidatorV2::CurrentSchemaVersion, VirtualizationDecisionValidatorV2::ExpectedDecisionId, VirtualizationDecisionValidatorV2::ExpectedOperationNamespace, 16, 0, 1, VirtualizationDecisionValidatorV2::ExpectedOperationId, VirtualizationDecisionOwnerClassV2::NeutralRuntimeOwner, VirtualizationDecisionValidatorV2::ExpectedOwnerId, 1, 1, 1, L"ArchitecturalRegisterFullNumericLeafValue", L"X0", L"X0NoResult", VirtualizationDecisionResultAbiV2::NoPayload, VirtualizationDecisionEffectClassV2::NoStateNoPayload, VirtualizationCapabilityRequirementV2::DomainGrantedVmCallProbeNoStateV1, VirtualizationDecisionValidatorV2::ExpectedCapabilityMask, true, VirtualizationDelegationPolicyV2::NonDelegable, VirtualizationRevocationPolicyV2::RuntimeRevocable, VirtualizationCapabilityMigrationClassV2::DomainLocal, VirtualizationEvidenceVisibilityV2::HostOnly, VirtualizationProjectionPolicyV2::NeverProject, VirtualizationExecutionEvidenceRequirementV2::None, VirtualizationDomainRequirementV2::ExecutionDomainBound, true, false, false, VirtualizationAddressSpaceRequirementV2::None, VirtualizationSecureDomainPolicyV2::Deny, VirtualizationCancellationPolicyV2::DenyBeforeExecution, VirtualizationReplayPolicyV2::DenyAttemptReplay, VirtualizationOperationMigrationPolicyV2::DrainOnly, VirtualizationCompletionEvidenceClassV2::HostOwnedRuntimeEvidence, VirtualizationCompletionMigrationClassV2::HostOwnedNonMigratable, VirtualizationProjectionPolicyV2::NeverProject, VirtualizationCompletionPolicyV2::AtomicE3ToCompletionRecordAndE5, VirtualizationRetirePolicyV2::PreciseE5BoundNoStateRetire, VirtualizationAdjacentLeafPolicyV2::DenyAllExceptExact, VirtualizationCrossNamespacePolicyV2::AllowDistinctFrozenCompatibilityNamespaceOnly, CreateOwnerMap(), std::wstring(L"0", 64));
        spec = spec->WithSpecDigest(VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(spec));
        return spec;
}
	std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> Phase38VirtualizationDecisionSpecV2::CreateOwnerMap()
	{
		return {Entry(L"Operation", L"DomainHypercallRuntimeOwner"), Entry(L"OperandAbi", L"CanonicalOperandSnapshotOwner"), Entry(L"CapabilityAdmission", L"RuntimeCapabilityOwner"), Entry(L"DomainAdmission", L"RuntimeDomainOwner"), Entry(L"ExecutionAdmission", L"SafetyVerifier"), Entry(L"CancellationReplay", L"DomainHypercallRuntimeOwner"), Entry(L"CompletionPublication", L"NeutralCompletionOwner"), Entry(L"RetirePublication", L"CanonicalRetireOwner"), Entry(L"MigrationRestore", L"CheckpointRestoreOwner"), Entry(L"AdjacentLeafDenial", L"VirtualizationDecisionValidatorV2"),};
}
	std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> Phase38VirtualizationDecisionSpecV2::Entry(const std::wstring &field, const std::wstring &owner)
	{
		return std::make_shared<VirtualizationDecisionOwnerMapEntryV2>(field, owner, L"Phase38ExactValueSource", L"ExactTypedGrantOrNone", L"HostOnlyOrNone", L"DrainOnlyOrHostOwnedNonMigratable", L"DenyOnMissingOrMismatch");
}
}
