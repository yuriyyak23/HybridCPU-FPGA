#pragma once

#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "VmReadScalarDeliveryE0FindingV2.h"
#include <string>
#include <memory>
#include <utility>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CurrentCompletionFieldE0Disposition : unsigned char
	{
		BlockedMissingOwnerApprovedProjectionMapping = 0,
		BlockedAbsentForExactProducer = 1,
		BlockedSemanticMismatch = 2,
		BlockedNoEligibleProducer = 3
	};

struct CurrentCompletionFieldE0Entry final
{
	VmcsField Field;
	CurrentCompletionFieldE0Disposition Disposition;
	std::wstring NeutralSource;
	std::wstring DenialReason;

	CurrentCompletionFieldE0Entry(VmcsField field, CurrentCompletionFieldE0Disposition disposition, std::wstring neutralSource, std::wstring denialReason)
		: Field(field), Disposition(disposition), NeutralSource(std::move(neutralSource)), DenialReason(std::move(denialReason)) {}

	bool IsD2Eligible() const noexcept { return false; }
};

	class Phase49CurrentCompletionVmReadScalarDeliveryE0Contract final
	{
	private:
		static inline const std::vector<unsigned short> ExactFieldIds = {
			static_cast<unsigned short>(VmcsField::ExitReason), static_cast<unsigned short>(VmcsField::ExitQualification),
			static_cast<unsigned short>(VmcsField::GuestPhysicalAddress), static_cast<unsigned short>(VmcsField::EptViolationQualification)
		};
		static inline const std::vector<std::shared_ptr<CurrentCompletionFieldE0Entry>> FieldMatrix = {
			std::make_shared<CurrentCompletionFieldE0Entry>(VmcsField::ExitReason, CurrentCompletionFieldE0Disposition::BlockedMissingOwnerApprovedProjectionMapping, L"Present neutral TrapEntry reason from CanonicalPipelineTrapEntryProducer", L"A neutral cause code is not an owner-approved VMX ExitReason mapping."),
			std::make_shared<CurrentCompletionFieldE0Entry>(VmcsField::ExitQualification, CurrentCompletionFieldE0Disposition::BlockedAbsentForExactProducer, L"Qualification is explicitly absent and producer policy disallows it", L"The only registered production producer cannot provide qualification."),
			std::make_shared<CurrentCompletionFieldE0Entry>(VmcsField::GuestPhysicalAddress, CurrentCompletionFieldE0Disposition::BlockedSemanticMismatch, L"TrapEntry permits only VirtualAddress semantic", L"GuestPhysicalAddress requires a present GuestPhysicalAddress semantic fact."),
			std::make_shared<CurrentCompletionFieldE0Entry>(VmcsField::EptViolationQualification, CurrentCompletionFieldE0Disposition::BlockedNoEligibleProducer, L"Auxiliary is explicitly absent and producer policy permits None only", L"No registered production producer supplies second-stage translation-violation auxiliary data.")
		};
		static inline const std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> Findings = {
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(1, L"P48Foundation", L"CanonicalCommitReceiptObservationGenerationPresenceAndSemanticDenialAreProvenNeutralFoundations"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(2, L"ExactProducer", L"CanonicalPipelineTrapEntryProducerIsTheOnlyRegisteredProductionProducer"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(3, L"ReasonCoverage", L"PresentNeutralTrapCauseExistsButNoOwnerApprovedExitReasonMappingExists"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(4, L"QualificationCoverage", L"ExactProducerPolicyDisallowsQualificationAndProductionFactIsAbsent"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(5, L"AddressCoverage", L"ExactProducerAllowsVirtualAddressOnlyGuestPhysicalSemanticIsUnavailable"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(6, L"AuxiliaryCoverage", L"NoSecondStageTranslationViolationAuxiliaryProducerIsRegistered"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(7, L"GroupAtomicity", L"ExactFourFieldOperationCannotBePartiallyAuthorized"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(8, L"CallerIsolation", L"CallerProvidedCompletionAndGenerationRemainForbidden"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(9, L"CompatibilityIsolation", L"CompletionRecordCompatibilityFactoryAndVmcsBackingStoreAreNotAuthority"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(10, L"VmCallIsolation", L"DomainHypercallCompletionOwnerE5E6CannotBeReused"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(11, L"TrapIsolation", L"TrapRouteAndFenceResultsCannotAuthorizeProjection"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(12, L"NoFallback", L"AbsentOrSemanticMismatchMustDenyAndSuccessfulZeroFallbackIsForbidden"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(13, L"SpecMaterialization", L"DeniedBecauseExactProducerFieldCoverageAndMappingAreIncomplete"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(14, L"AcceptanceMaterialization", L"DeniedBecauseNoExactSpecV2Exists"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(15, L"ProductionComposition", L"NotAuthorized"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(16, L"Disposition", L"BlockedE0WithoutVmReadSpecificSurrogateAuthority")
		};

	public:
		static inline const std::wstring ProposedDecisionId = L"D2-HV-VMREAD-SCALAR-DELIVERY-V1-CURRENT-COMPLETION-0004";
		static inline const std::wstring OperationNamespace = L"HybridCPU.VMREAD.ScalarDelivery.v1";
		static inline const std::wstring OperationId = L"DELIVER_CURRENT_COMPLETION_FIELDS_SCALAR_V1";
		static inline const std::wstring BlockerCode = L"IncompleteExactProducerFieldSemanticCoverageAndProjectionMapping";

		static std::vector<unsigned short> getExactFieldIds();

		static std::vector<std::shared_ptr<CurrentCompletionFieldE0Entry>> getFieldMatrix();

		static std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> getFindings();

		static bool getCanonicalCommitPointProven();
		static bool getNeutralObservationOwnerProven();
		static bool getExplicitPresenceAndSemanticContractProven();
		static bool getExactProductionProducerRegistered();
		static bool getExactFourFieldCoverageProven();
		static bool getOwnerApprovedProjectionMappingProven();
		static bool getSpecV2Materialized();
		static bool getAcceptanceRecordV2Materialized();
		static bool getRuntimeAuthorityGranted();
		static bool getProductionCompositionAuthorized();
	};

}
