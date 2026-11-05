#pragma once

#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "VmReadScalarDeliveryE0FindingV2.h"
#include <memory>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{
	class Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract final
	{
	private:
		static inline const std::vector<unsigned short> ExactFieldIds = {
			static_cast<unsigned short>(VmcsField::GuestPc),
			static_cast<unsigned short>(VmcsField::GuestSp),
			static_cast<unsigned short>(VmcsField::GuestFlags)
		};
		static inline const std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> Findings = {
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(1, L"CanonicalIngress", L"VMREAD->VmxCompatibilityAdmissionService.AdmitVmReadProjection->ReadCompatibilityProjection"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(2, L"SourceOwner", L"ExecutionDomainDescriptorOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(3, L"ValueSource", L"MaterializedExecutionDomainReadOnlyStateView.GuestPc|GuestSp|GuestFlagsOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(4, L"RuntimeBoundary", L"PreserveFullDomainRuntimeReadCompatibilityProjection+CapabilityNone+GuestVisibleCompatibilityAliasEvidence"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(5, L"Materialization", L"ExactFieldMaterialized+GuestArchitecturalStateEvidence+ImmutableDescriptorViewSnapshotRequired"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(6, L"EpochGap", L"CurrentProjectionCarriesStateEpochButDoesNotValidateCurrentEpoch;FutureProductionMustAtomicallyBindAndRevalidateSourceEpoch"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(7, L"ResultAbi", L"ScalarU64ToDestinationRegister+ArchitecturalRegisterResultOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(8, L"Receipt", L"OpaqueAttemptD2FieldSourceEpochValueDestinationReplayRestoreProfileGenerationBoundSingleUse"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(9, L"Transport", L"CanonicalPRFRenameWritebackOnly+RetireRecord.RegisterWrite+RetireCoordinatorOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(10, L"MigrationRollback", L"DrainOnly+NoReceiptSerialization+DisableBindingAndInvalidateOutstandingReceipts"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(11, L"SquashReplayRestore", L"ZeroArchitecturalEffectBeforeRetire+InvalidateOnReplaySquashRestoreOrProfileGenerationChange"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(12, L"NoFallback", L"NoVMCSScalarOrBackingStore+NoDirectArchitecturalWrite+NoVmxRetireEffectAuthority"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(13, L"NoReuse", L"NoVMCALLCapabilityD2O1E2ToE7E5E6+NoTrapOrBackendCompletion"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(14, L"AdjacentDenial", L"DenyVMWRITEGuestCrFieldsMemoryOwnedFieldsHostAliasesHostCr3CompatibilityControlsAndAllOtherFields"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(15, L"Activation", L"DefaultDisabled+GovernancePolicyIsNotCapabilityAdmissionSourceAuthorityReceiptOrRetireGrant")
		};

	public:
		static std::vector<unsigned short> getExactFieldIds();
		static std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> getFindings();
		static bool getRuntimeAuthorityGranted();
		static bool getSourceValueAvailable();
		static bool getResultReceiptIssued();
		static bool getRegisterWritebackAuthorized();
		static bool getRetireCommitAuthorized();
		static bool getBackendExecutionAuthorized();
		static bool getUnderlyingVirtualizationMutationAuthorized();
		static bool getCompletionPublicationAuthorized();
		static bool getProductionCompositionAuthorized();
	};
}
