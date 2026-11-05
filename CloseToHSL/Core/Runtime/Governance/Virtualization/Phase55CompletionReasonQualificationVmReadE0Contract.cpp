#include "Phase55CompletionReasonQualificationVmReadE0Contract.h"

namespace YAKSys_Hybrid_CPU::Core
{
	const std::vector<unsigned short> &Phase55CompletionReasonQualificationVmReadE0Contract::ExactFieldIds()
	{
		static const std::vector<unsigned short> value = {static_cast<unsigned short>(VmcsField::ExitReason), static_cast<unsigned short>(VmcsField::ExitQualification)};
		return value;
	}

	const std::vector<unsigned short> &Phase55CompletionReasonQualificationVmReadE0Contract::ExplicitlyDeniedFieldIds()
	{
		static const std::vector<unsigned short> value = {static_cast<unsigned short>(VmcsField::GuestPhysicalAddress), static_cast<unsigned short>(VmcsField::EptViolationQualification)};
		return value;
	}

	const std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> &Phase55CompletionReasonQualificationVmReadE0Contract::Findings()
	{
		static const std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> value = {
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(1, L"Authority", L"DomainCompletionObservationOwnerReadOnlySnapshotOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(2, L"Producer", L"ExactCanonicalCpuInstructionTranslationFaultProducerOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(3, L"MappedReasons", L"AccessDeniedOrOwnerScopeMismatchToFrozenSecurityPolicyViolation"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(4, L"Qualification", L"ExactReasonAccessKindAccessSizeEncodingReturnedUnchanged"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(5, L"PairBinding", L"ExitReasonAndExitQualificationRequireTheSameOwnerApprovedSemanticTuple"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(6, L"AbsentVersusZero", L"PresentBitsRequiredAndZeroQualificationCannotBeSynthesized"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(7, L"GuestPhysicalAddress", L"DeniedVirtualAddressIsNotGuestPhysicalAddress"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(8, L"EptViolationQualification", L"DeniedTranslationFaultAuxiliaryIsNotSecondStageTranslationViolation"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(9, L"Lifecycle", L"SnapshotGenerationRestoreOwnerScopeAttemptEventAndDigestBoundReceipt"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(10, L"Delivery", L"ExistingE1CarrierReceiptPRFWritebackRetireRegisterPathOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(11, L"NoFallback", L"NoInferenceZeroIommuNestedExceptionVmcsBackingOrVmxAuthority"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(12, L"Activation", L"ExactPolicyOptInDefaultDisabledAndNoAdjacentFieldActivation")};
		return value;
	}

	std::vector<unsigned short> Phase55CompletionReasonQualificationVmReadE0Contract::getExactFieldIds()
	{
		return ExactFieldIds();
	}

	std::vector<unsigned short> Phase55CompletionReasonQualificationVmReadE0Contract::getExplicitlyDeniedFieldIds()
	{
		return ExplicitlyDeniedFieldIds();
	}

	std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> Phase55CompletionReasonQualificationVmReadE0Contract::getFindings()
	{
		return Findings();
	}

	bool Phase55CompletionReasonQualificationVmReadE0Contract::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase55CompletionReasonQualificationVmReadE0Contract::getCompletionCreationAuthorized()
	{
		return false;
	}

	bool Phase55CompletionReasonQualificationVmReadE0Contract::getVmxAuthorityGranted()
	{
		return false;
	}

	bool Phase55CompletionReasonQualificationVmReadE0Contract::getGuestPhysicalAddressAuthorized()
	{
		return false;
	}

	bool Phase55CompletionReasonQualificationVmReadE0Contract::getEptViolationQualificationAuthorized()
	{
		return false;
	}

	bool Phase55CompletionReasonQualificationVmReadE0Contract::getProductionCompositionAuthorized()
	{
		return false;
	}
}
