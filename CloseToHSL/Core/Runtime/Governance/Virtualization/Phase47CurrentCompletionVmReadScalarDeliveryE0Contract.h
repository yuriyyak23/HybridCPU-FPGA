#pragma once

#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "../../Completion/Records/CompletionRecord.h"
#include "VmReadScalarDeliveryE0FindingV2.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class CurrentCompletionProducerE0Disposition : unsigned char
	{
		DeniedNoArchitecturalCommitEvidence = 0,
		DeniedVmCallSpecificAuthority = 1,
		DeniedCompatibilityFactoryIsNotAuthority = 2,
		DeniedNoFieldValidityContract = 3
	};

	struct CurrentCompletionProducerE0Entry final
	{
		CompletionRecordClass RecordClass;
		CurrentCompletionProducerE0Disposition Disposition;
		std::wstring Reason;

		CurrentCompletionProducerE0Entry(CompletionRecordClass recordClass, CurrentCompletionProducerE0Disposition disposition, std::wstring reason)
			: RecordClass(recordClass), Disposition(disposition), Reason(std::move(reason))
		{
		}

		bool IsEligible() const noexcept { return false; }
	};

	struct CurrentCompletionFieldValidityE0Entry final
	{
		VmcsField Field;
		bool IsValidForAnyProducer;
		std::wstring DenialReason;

		CurrentCompletionFieldValidityE0Entry(VmcsField field, bool isValidForAnyProducer, std::wstring denialReason)
			: Field(field), IsValidForAnyProducer(isValidForAnyProducer), DenialReason(std::move(denialReason))
		{
		}
	};

	class Phase47CurrentCompletionVmReadScalarDeliveryE0Contract final
	{
	private:
		static std::shared_ptr<CurrentCompletionFieldValidityE0Entry> DeniedField(VmcsField field);

		static inline const std::vector<unsigned short> ExactFieldIds = {
			static_cast<unsigned short>(VmcsField::ExitReason), static_cast<unsigned short>(VmcsField::ExitQualification),
			static_cast<unsigned short>(VmcsField::GuestPhysicalAddress), static_cast<unsigned short>(VmcsField::EptViolationQualification)
		};
		static inline const std::vector<std::shared_ptr<CurrentCompletionProducerE0Entry>> ProducerMatrix = {
			std::make_shared<CurrentCompletionProducerE0Entry>(CompletionRecordClass::None, CurrentCompletionProducerE0Disposition::DeniedNoArchitecturalCommitEvidence, L"Empty records are not an architecturally-visible completion."),
			std::make_shared<CurrentCompletionProducerE0Entry>(CompletionRecordClass::Trap, CurrentCompletionProducerE0Disposition::DeniedNoArchitecturalCommitEvidence, L"TrapCompletionPublicationFenceResult is a forgeable value result and no canonical retire commit issues nonforgeable current-completion visibility evidence."),
			std::make_shared<CurrentCompletionProducerE0Entry>(CompletionRecordClass::Event, CurrentCompletionProducerE0Disposition::DeniedVmCallSpecificAuthority, L"DomainHypercallCompletionOwner Event/E5/E6 authority is operation-specific and forbidden as this VMREAD source."),
			std::make_shared<CurrentCompletionProducerE0Entry>(CompletionRecordClass::MemoryTranslationFault, CurrentCompletionProducerE0Disposition::DeniedNoArchitecturalCommitEvidence, L"No canonical architecturally-visible completion commit binds this class to a domain/context current snapshot."),
			std::make_shared<CurrentCompletionProducerE0Entry>(CompletionRecordClass::DmaFault, CurrentCompletionProducerE0Disposition::DeniedNoArchitecturalCommitEvidence, L"No canonical architecturally-visible completion commit binds this class to a domain/context current snapshot."),
			std::make_shared<CurrentCompletionProducerE0Entry>(CompletionRecordClass::VectorStreamFault, CurrentCompletionProducerE0Disposition::DeniedNoArchitecturalCommitEvidence, L"No canonical architecturally-visible completion commit binds this class to a domain/context current snapshot."),
			std::make_shared<CurrentCompletionProducerE0Entry>(CompletionRecordClass::LaneFault, CurrentCompletionProducerE0Disposition::DeniedNoArchitecturalCommitEvidence, L"No canonical architecturally-visible completion commit binds this class to a domain/context current snapshot."),
			std::make_shared<CurrentCompletionProducerE0Entry>(CompletionRecordClass::SecurityViolation, CurrentCompletionProducerE0Disposition::DeniedNoArchitecturalCommitEvidence, L"No canonical architecturally-visible completion commit binds this class to a domain/context current snapshot."),
			std::make_shared<CurrentCompletionProducerE0Entry>(CompletionRecordClass::CompatibilityExit, CurrentCompletionProducerE0Disposition::DeniedCompatibilityFactoryIsNotAuthority, L"CompatibilityExit is publicly constructible through a forgeable fence result and has no production commit caller or owner-issued visibility evidence.")
		};
		static inline const std::vector<std::shared_ptr<CurrentCompletionFieldValidityE0Entry>> FieldValidityMatrix = {
			DeniedField(VmcsField::ExitReason), DeniedField(VmcsField::ExitQualification),
			DeniedField(VmcsField::GuestPhysicalAddress), DeniedField(VmcsField::EptViolationQualification)
		};
		static inline const std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> Findings = {
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(1, L"CanonicalIngress", L"VMREAD->VmxCompatibilityAdmissionService.AdmitVmReadProjection->ReadCompatibilityProjection"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(2, L"CallerAuthorityGap", L"VmxCompatibilityVmReadAdmissionRequest.CompletionIsCallerProvided"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(3, L"ConstructibilityGap", L"CompletionRecordAndCompatibilityExitFactoryDoNotProveOwnerIssuedCurrentVisibility"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(4, L"CommitPointGap", L"NoProductionArchitecturallyVisibleNeutralCompletionCommitPointIssuesNonforgeableCurrentCompletionVisibilityEvidence"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(5, L"RegistryGap", L"NoDomainContextScopedLiveCurrentCompletionRegistryExists"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(6, L"GenerationGap", L"NoRuntimeOwnedNonZeroCompletionGenerationExists"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(7, L"AtomicCaptureGap", L"NoAtomicRecordValueOwnerDomainContextGenerationFieldCaptureExists"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(8, L"FieldValidityGap", L"CompletionRecordHasScalarZerosButNoPerFieldPresenceOrValidityContract"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(9, L"ProducerEligibility", L"NoExistingCompletionRecordClassIsEligibleForCurrentCompletionVMREADSource"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(10, L"VmCallIsolation", L"DomainHypercallCompletionOwnerEventE5E6AreForbiddenAndNotReusable"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(11, L"TrapIsolation", L"TrapRouteFenceBooleansAndTrapCompletionPublicationAreNotVMREADPermission"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(12, L"Migration", L"SourceCandidateRecomputedCompletion+RestoreClearsOldSnapshot;ReceiptCandidateDrainOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(13, L"ResultEffectCandidate", L"ScalarU64ToDestinationRegister+ArchitecturalRegisterResultOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(14, L"CapabilityCandidate", L"NoneOnlyAfterCanonicalReadOnlyAdmissionAndSourceAuthorityExist"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(15, L"OwnerCreation", L"DeniedUntilCanonicalCommitEvidenceAndFieldValidityPrerequisitesExist"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(16, L"SpecMaterialization", L"DeniedBecauseE0OwnerFreshnessAndFieldValidityAreUnresolved"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(17, L"AcceptanceMaterialization", L"DeniedBecauseNoExactSpecV2CanBeAccepted"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(18, L"ProductionComposition", L"NotAuthorized"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(19, L"NoFallback", L"NoCallerCompletionCompatibilityFactoryVmcsBackingStoreVmxRetireEffectBackendCompletionOrDirectArchitecturalWrite"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(20, L"Disposition", L"BlockedE0WithoutDomainCurrentCompletionOwnerOrVmReadSpecificSurrogateAuthority")
		};

	public:
		static inline const std::wstring ProposedDecisionId = L"D2-HV-VMREAD-SCALAR-DELIVERY-V1-CURRENT-COMPLETION-0004";
		static inline const std::wstring OperationNamespace = L"HybridCPU.VMREAD.ScalarDelivery.v1";
		static inline const std::wstring OperationId = L"DELIVER_CURRENT_COMPLETION_FIELDS_SCALAR_V1";
		static inline const std::wstring BlockerCode = L"MissingNonforgeableArchitecturallyVisibleCompletionCommitPointAndFieldValidity";

		static std::vector<unsigned short> getExactFieldIds();
		static std::vector<std::shared_ptr<CurrentCompletionProducerE0Entry>> getProducerMatrix();
		static std::vector<std::shared_ptr<CurrentCompletionFieldValidityE0Entry>> getFieldValidityMatrix();
		static std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> getFindings();
		static bool getCanonicalCommitPointProven();
		static bool getAnyProducerEligible();
		static bool getFieldValidityProven();
		static bool getDomainCurrentCompletionOwnerCreated();
		static bool getSpecV2Materialized();
		static bool getAcceptanceRecordV2Materialized();
		static bool getRuntimeAuthorityGranted();
		static bool getSourceValueAvailable();
		static bool getResultReceiptIssued();
		static bool getRegisterWritebackAuthorized();
		static bool getRetireCommitAuthorized();
		static bool getBackendExecutionAuthorized();
		static bool getCompletionPublicationAuthorized();
		static bool getProductionCompositionAuthorized();

	};
}
