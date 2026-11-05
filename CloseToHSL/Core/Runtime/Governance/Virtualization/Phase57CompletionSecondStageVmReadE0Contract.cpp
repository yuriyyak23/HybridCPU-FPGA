#include "Phase57CompletionSecondStageVmReadE0Contract.h"

namespace YAKSys_Hybrid_CPU::Core
{
	const std::vector<unsigned short> &Phase57CompletionSecondStageVmReadE0Contract::ExactFieldIds()
	{
		static const std::vector<unsigned short> value = {static_cast<unsigned short>(VmcsField::GuestPhysicalAddress), static_cast<unsigned short>(VmcsField::EptViolationQualification)};
		return value;
	}

	const std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> &Phase57CompletionSecondStageVmReadE0Contract::Findings()
	{
		static const std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> value = {
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(1, L"Authority", L"DomainCompletionObservationOwnerReadOnlyCommittedSnapshotOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(2, L"Producer", L"ExactCanonicalCpuSecondStageTranslationFaultProducerOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(3, L"Gpa", L"PresentGuestPhysicalAddressSemanticCopiedExactlyIncludingZero"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(4, L"Auxiliary", L"PresentSecondStageTranslationViolationSemanticDecodedExactly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(5, L"Reason", L"SecondStageViolationOrSecondStageMisconfigurationMustMatchAuxiliaryKind"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(6, L"Qualification", L"ReasonAccessKindAccessSizeMustMatchAuxiliaryTuple"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(7, L"CompatibilityMapping", L"AccessBitMisconfigurationBitAndPageWalkLevelMappedExplicitly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(8, L"Provenance", L"MemoryOperationTranslationOwnerMemoryDomainAddressSpaceGenerationAndIdentityRequired"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(9, L"Lifecycle", L"SnapshotGenerationRestoreOwnerScopeAttemptEventDigestAndProvenanceBoundReceipt"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(10, L"Delivery", L"ExistingE1CarrierReceiptPRFWritebackRetireRegisterPathOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(11, L"NoFallback", L"NoInferenceZeroFallbackIommuNestedExceptionVmcsBackingOrNeutralEncodingPassThrough"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(12, L"Activation", L"ExactPolicyOptInDefaultDisabledAndNoAdjacentFieldActivation")};
		return value;
	}

	std::vector<unsigned short> Phase57CompletionSecondStageVmReadE0Contract::getExactFieldIds()
	{
		return ExactFieldIds();
	}

	std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> Phase57CompletionSecondStageVmReadE0Contract::getFindings()
	{
		return Findings();
	}

	bool Phase57CompletionSecondStageVmReadE0Contract::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase57CompletionSecondStageVmReadE0Contract::getCompletionCreationAuthorized()
	{
		return false;
	}

	bool Phase57CompletionSecondStageVmReadE0Contract::getVmxAuthorityGranted()
	{
		return false;
	}

	bool Phase57CompletionSecondStageVmReadE0Contract::getProductionCompositionAuthorized()
	{
		return false;
	}
}
