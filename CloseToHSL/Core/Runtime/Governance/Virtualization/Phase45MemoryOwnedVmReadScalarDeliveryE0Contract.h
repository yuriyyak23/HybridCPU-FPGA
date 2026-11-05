#pragma once

#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "VmReadScalarDeliveryE0FindingV2.h"
#include <string>
#include <memory>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{

	class Phase45MemoryOwnedVmReadScalarDeliveryE0Contract final
	{
	private:
		static inline const std::vector<unsigned short> ExactFieldIds = {
			static_cast<unsigned short>(VmcsField::GuestCr3), static_cast<unsigned short>(VmcsField::EptPointer),
			static_cast<unsigned short>(VmcsField::Vpid), static_cast<unsigned short>(VmcsField::Cr3TargetCount)
		};
		static inline const std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> Findings = {
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(1, L"CanonicalIngress", L"VMREAD->VmxCompatibilityAdmissionService.AdmitVmReadProjection->ReadCompatibilityProjection"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(2, L"AuthorityPlane", L"MemoryAddressSpaceReadProjection"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(3, L"SourceOwner", L"MemoryDomainDescriptor+CanonicalMemoryDomainRuntimeOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(4, L"ValueSource", L"MaterializedMemoryDomainReadOnlyTranslationViewOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(5, L"RuntimeBoundary", L"FullDomainRuntime+CapabilityNone+FieldSpecificGuestVisibleEvidence"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(6, L"GenerationGap", L"LegacyAddressSpaceGenerationWasCallerProvidedAndNotCurrentAcrossOwnershipTagTargetReplacementOrRestore"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(7, L"GenerationClosure", L"MemoryDomainRuntimeNowIssuesNonZeroCurrentGenerationAndAtomicallyCapturesOwnerValueDomainAddressSpaceGenerationField"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(8, L"GuestCr3", L"AddressSpaceRootOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(9, L"EptPointer", L"SecondStageRootOnlyWhenTranslationEnabledRootNonZeroAndMemoryDomainOwnsSecondStage"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(10, L"Vpid", L"AddressSpaceTagOnlyWhenTaggingEnabledAndTagNonZero"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(11, L"Cr3TargetCount", L"CanonicalAddressSpaceTargetCountWithinExactBound"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(12, L"ResultEffect", L"ScalarU64ToDestinationRegister+ArchitecturalRegisterResultOnlyCandidate"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(13, L"MigrationRollback", L"DrainOnly+NoReceiptSealPhysicalDestinationOrOutputSerialization+DefaultDisabled"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(14, L"Invalidation", L"ReplaySquashRestoreDisableRevocationInvalidateOutstandingReceipt+NoRetireSourceReread"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(15, L"NoFallback", L"NoVMCSBackingStoreDirectArchitecturalWriteVmxRetireEffectBackendCompletionOrVMCALLReuse"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(16, L"AdjacentDenial", L"DenyGuestCr0GuestCr4GuestPcGuestSpGuestFlagsCompletionHostCr3ControlsVMWRITENestedSecureComputeTransactionsIommuIoDeviceLaneStreamCompiler")
		};

	public:
		static std::vector<unsigned short> getExactFieldIds();

		static std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> getFindings();

		static inline const std::wstring GenerationPrerequisiteCommit = L"3cb896e37fc7b5775099bf34ca9082e488a73dd3";
		static inline const std::wstring GenerationPrerequisiteTree = L"840d48603162146de36e51d65bca7d6ebe151d8c";
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
