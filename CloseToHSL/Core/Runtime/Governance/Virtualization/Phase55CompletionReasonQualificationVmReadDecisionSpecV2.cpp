#include "Phase55CompletionReasonQualificationVmReadDecisionSpecV2.h"
#include "CompletionReasonQualificationVmReadDecisionValidatorV2.h"
#include "Phase55CompletionReasonQualificationVmReadE0Contract.h"
#include "VirtualizationDecisionContractsV2.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase55CompletionReasonQualificationVmReadDecisionSpecV2::getInstance()
	{
		return GetInstance();
	}

	std::vector<unsigned char> Phase55CompletionReasonQualificationVmReadDecisionSpecV2::getCanonicalBytes()
	{
		return GetCanonicalBytes();
	}

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase55CompletionReasonQualificationVmReadDecisionSpecV2::GetInstance()
	{
		static const auto instance = Create();
		return instance;
	}

	std::vector<unsigned char> Phase55CompletionReasonQualificationVmReadDecisionSpecV2::GetCanonicalBytes()
	{
		static const auto bytes = VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(GetInstance());
		return bytes;
	}

	bool Phase55CompletionReasonQualificationVmReadDecisionSpecV2::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase55CompletionReasonQualificationVmReadDecisionSpecV2::getProductionCompositionAuthorized()
	{
		return false;
	}

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase55CompletionReasonQualificationVmReadDecisionSpecV2::Create()
	{
		std::shared_ptr<VirtualizationDecisionSpecV2> spec = std::make_shared<VirtualizationDecisionSpecV2>(CompletionReasonQualificationVmReadDecisionValidatorV2::SchemaVersion, CompletionReasonQualificationVmReadDecisionValidatorV2::ExpectedDecisionId, CompletionReasonQualificationVmReadDecisionValidatorV2::ExpectedOperationNamespace, 0, 0, 0, CompletionReasonQualificationVmReadDecisionValidatorV2::ExpectedOperationId, VirtualizationDecisionOwnerClassV2::NeutralRuntimeOwner, CompletionReasonQualificationVmReadDecisionValidatorV2::ExpectedSourceOwnerId, 1, 1, 1, L"VmcsFieldSelectorExactFrozenIdFromCanonicalSourceRegister", L"X0ReservedNoAuthority", L"ArchitecturalDestinationRegisterX1ToX31CanonicalRenameIdentity", VirtualizationDecisionResultAbiV2::ScalarU64ToDestinationRegister, VirtualizationDecisionEffectClassV2::ArchitecturalRegisterResultOnly, VirtualizationCapabilityRequirementV2::None, 0, false, VirtualizationDelegationPolicyV2::NonDelegable, VirtualizationRevocationPolicyV2::GovernanceRevocable, VirtualizationCapabilityMigrationClassV2::None, VirtualizationEvidenceVisibilityV2::GuestVisibleReadOnly, VirtualizationProjectionPolicyV2::ExactReadOnlyFieldSet, VirtualizationExecutionEvidenceRequirementV2::FieldConformanceProof, VirtualizationDomainRequirementV2::ExecutionDomainBound, true, false, false, VirtualizationAddressSpaceRequirementV2::None, VirtualizationSecureDomainPolicyV2::Deny, VirtualizationCancellationPolicyV2::SquashBeforeRetireZeroArchitecturalEffect, VirtualizationReplayPolicyV2::AttemptBoundReceiptNoReplayReuse, VirtualizationOperationMigrationPolicyV2::DrainOnly, VirtualizationCompletionEvidenceClassV2::None, VirtualizationCompletionMigrationClassV2::None, VirtualizationProjectionPolicyV2::NeverProject, VirtualizationCompletionPolicyV2::None, VirtualizationRetirePolicyV2::CanonicalRetireCoordinatorArchitecturalRegisterCommit, VirtualizationAdjacentLeafPolicyV2::DenyAllExceptExactFieldSet, VirtualizationCrossNamespacePolicyV2::DenyCrossNamespaceReuse, CreateOwnerMap(), std::wstring(L"0", 64), VirtualizationDecisionOperationClassV2::ReadOnlyArchitecturalVmReadScalarDelivery, VirtualizationDecisionAuthorityPlaneV2::CompletionObservationReadProjection, Phase55CompletionReasonQualificationVmReadE0Contract::getExactFieldIds(), VirtualizationDecisionMutationClassV2::UnderlyingVirtualizationStateReadOnly, CompletionReasonQualificationVmReadDecisionValidatorV2::ExpectedDependencyContract, true, true);
// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
//		spec = spec with { SpecDigest = VirtualizationDecisionCanonicalEncoderV2.ComputeSpecDigest(spec) };
		spec = spec->WithSpecDigest(VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(spec));

		if (spec->SpecDigest != ExpectedSpecDigest)
		{
			throw InvalidOperationException(std::format(L"Phase 55 immutable SpecV2 digest mismatch: {0:s}", spec->SpecDigest));
		}
		return spec;
	}

	std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> Phase55CompletionReasonQualificationVmReadDecisionSpecV2::CreateOwnerMap()
	{
		return {Entry(L"VmcsField.ExitReason", L"NeutralCompletionObservationSnapshot.Facts.Reason", L"OwnerApprovedReasonMapping"), Entry(L"VmcsField.ExitQualification", L"NeutralCompletionObservationSnapshot.Facts.Qualification", L"ExactReasonBoundQualification"),};
	}

	std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> Phase55CompletionReasonQualificationVmReadDecisionSpecV2::Entry(const std::wstring &field, const std::wstring &source, const std::wstring &evidence)
	{
		return std::make_shared<VirtualizationDecisionOwnerMapEntryV2>(field, L"DomainCompletionObservationOwner", source, L"None", std::format(L"GuestVisibleReadOnly+ExactCpuTranslationProducerSemanticTuple+{0:s}+CompletionGenerationRestoreOwnerScopeAttemptEventDigestProof", evidence), L"DrainOnly", L"DenyOnMissingOrForeignProducerAbsentOrIncompatibleReasonQualificationStaleGenerationRestoreScopeAttemptEventDigestReplayDestinationOrCanonicalCarrierGate");
	}
}
