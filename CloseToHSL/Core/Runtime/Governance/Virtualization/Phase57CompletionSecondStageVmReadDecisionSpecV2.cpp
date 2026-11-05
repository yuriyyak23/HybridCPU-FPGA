#include "Phase57CompletionSecondStageVmReadDecisionSpecV2.h"
#include "CompletionSecondStageVmReadDecisionValidatorV2.h"
#include "Phase57CompletionSecondStageVmReadE0Contract.h"
#include "VirtualizationDecisionContractsV2.h"
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core
{
	std::shared_ptr<VirtualizationDecisionSpecV2> Phase57CompletionSecondStageVmReadDecisionSpecV2::getInstance()
	{
		return GetInstance();
	}

	std::vector<unsigned char> Phase57CompletionSecondStageVmReadDecisionSpecV2::getCanonicalBytes()
	{
		return GetCanonicalBytes();
	}

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase57CompletionSecondStageVmReadDecisionSpecV2::GetInstance()
	{
		static const auto instance = Create();
		return instance;
	}

	std::vector<unsigned char> Phase57CompletionSecondStageVmReadDecisionSpecV2::GetCanonicalBytes()
	{
		static const auto bytes = VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(GetInstance());
		return bytes;
	}

	bool Phase57CompletionSecondStageVmReadDecisionSpecV2::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase57CompletionSecondStageVmReadDecisionSpecV2::getProductionCompositionAuthorized()
	{
		return false;
	}

	std::shared_ptr<VirtualizationDecisionSpecV2> Phase57CompletionSecondStageVmReadDecisionSpecV2::Create()
	{
		std::shared_ptr<VirtualizationDecisionSpecV2> spec = std::make_shared<VirtualizationDecisionSpecV2>(VirtualizationDecisionSpecV2{CompletionSecondStageVmReadDecisionValidatorV2::SchemaVersion, CompletionSecondStageVmReadDecisionValidatorV2::ExpectedDecisionId, CompletionSecondStageVmReadDecisionValidatorV2::ExpectedOperationNamespace, 0, 0, 0, CompletionSecondStageVmReadDecisionValidatorV2::ExpectedOperationId, VirtualizationDecisionOwnerClassV2::NeutralRuntimeOwner, CompletionSecondStageVmReadDecisionValidatorV2::ExpectedSourceOwnerId, 1, 1, 1, L"VmcsFieldSelectorExactFrozenIdFromCanonicalSourceRegister", L"X0ReservedNoAuthority", L"ArchitecturalDestinationRegisterX1ToX31CanonicalRenameIdentity", VirtualizationDecisionResultAbiV2::ScalarU64ToDestinationRegister, VirtualizationDecisionEffectClassV2::ArchitecturalRegisterResultOnly, VirtualizationCapabilityRequirementV2::None, 0, false, VirtualizationDelegationPolicyV2::NonDelegable, VirtualizationRevocationPolicyV2::GovernanceRevocable, VirtualizationCapabilityMigrationClassV2::None, VirtualizationEvidenceVisibilityV2::GuestVisibleReadOnly, VirtualizationProjectionPolicyV2::ExactReadOnlyFieldSet, VirtualizationExecutionEvidenceRequirementV2::FieldConformanceProof, VirtualizationDomainRequirementV2::ExecutionDomainBound, true, false, false, VirtualizationAddressSpaceRequirementV2::None, VirtualizationSecureDomainPolicyV2::Deny, VirtualizationCancellationPolicyV2::SquashBeforeRetireZeroArchitecturalEffect, VirtualizationReplayPolicyV2::AttemptBoundReceiptNoReplayReuse, VirtualizationOperationMigrationPolicyV2::DrainOnly, VirtualizationCompletionEvidenceClassV2::None, VirtualizationCompletionMigrationClassV2::None, VirtualizationProjectionPolicyV2::NeverProject, VirtualizationCompletionPolicyV2::None, VirtualizationRetirePolicyV2::CanonicalRetireCoordinatorArchitecturalRegisterCommit, VirtualizationAdjacentLeafPolicyV2::DenyAllExceptExactFieldSet, VirtualizationCrossNamespacePolicyV2::DenyCrossNamespaceReuse, CreateOwnerMap(), std::wstring(L"0", 64), VirtualizationDecisionOperationClassV2::ReadOnlyArchitecturalVmReadScalarDelivery, VirtualizationDecisionAuthorityPlaneV2::CompletionObservationReadProjection, Phase57CompletionSecondStageVmReadE0Contract::getExactFieldIds(), VirtualizationDecisionMutationClassV2::UnderlyingVirtualizationStateReadOnly, CompletionSecondStageVmReadDecisionValidatorV2::ExpectedDependencyContract, true, true});
// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
//		spec = spec with { SpecDigest = VirtualizationDecisionCanonicalEncoderV2.ComputeSpecDigest(spec) };
		spec = spec->WithSpecDigest(VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(spec));

		if (spec->SpecDigest != ExpectedSpecDigest)
		{
			throw InvalidOperationException(std::format(L"Phase 57 immutable SpecV2 digest mismatch: {0:s}", spec->SpecDigest));
		}
		return spec;
	}

	std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> Phase57CompletionSecondStageVmReadDecisionSpecV2::CreateOwnerMap()
	{
		return {Entry(L"VmcsField.GuestPhysicalAddress", L"NeutralCompletionObservationSnapshot.Facts.FaultAddress", L"ExactPresentGuestPhysicalAddressSemantic"), Entry(L"VmcsField.EptViolationQualification", L"NeutralCompletionObservationSnapshot.Facts.FaultAuxiliary", L"ExactReasonQualificationAuxiliaryAndProvenanceMapping"),};
	}

	std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> Phase57CompletionSecondStageVmReadDecisionSpecV2::Entry(const std::wstring &field, const std::wstring &source, const std::wstring &evidence)
	{
		return std::make_shared<VirtualizationDecisionOwnerMapEntryV2>(field, L"DomainCompletionObservationOwner", source, L"None", std::format(L"GuestVisibleReadOnly+ExactCpuSecondStageProducerSemanticTuple+{0:s}+CompletionGenerationRestoreOwnerScopeAttemptEventDigestTranslationProvenanceProof", evidence), L"DrainOnly", L"DenyOnMissingOrForeignProducerAbsentOrIncompatibleReasonQualificationAddressAuxiliaryProvenanceStaleGenerationRestoreScopeAttemptEventDigestReplayDestinationOrCanonicalCarrierGate");
	}
}
