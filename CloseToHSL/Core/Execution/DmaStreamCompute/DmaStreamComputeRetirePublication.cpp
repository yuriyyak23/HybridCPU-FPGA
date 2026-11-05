#include "DmaStreamComputeRetirePublication.h"
#include "DmaStreamComputeTokenStore.h"
#include "DmaStreamComputeDescriptor.h"

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	std::shared_ptr<DmaStreamComputeRetirePublicationMetadata> DmaStreamComputeRetirePublicationMetadata::FromActiveEntry(std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry, unsigned long long architecturalInstructionAge, int ownerThreadId, unsigned long long completionCycle)
	{
		ArgumentNullException::ThrowIfNull(entry);
		ArgumentNullException::ThrowIfNull(entry->Token);
		std::shared_ptr<DmaStreamComputeDescriptor> descriptor = entry->Token->getDescriptor();
		ArgumentNullException::ThrowIfNull(descriptor);
		ArgumentNullException::ThrowIfNull(descriptor->OwnerBinding);
		ArgumentNullException::ThrowIfNull(descriptor->OwnerGuardDecision);
		const auto& owner = descriptor->OwnerBinding;
		const int resolvedOwnerThreadId = ownerThreadId >= 0 ? ownerThreadId : static_cast<int>(owner->OwnerVirtualThreadId);
		return std::make_shared<DmaStreamComputeRetirePublicationMetadata>(entry->Handle.TokenId, entry->Metadata.IssuingPc, entry->Metadata.BundleId, architecturalInstructionAge, entry->Metadata.SlotIndex, entry->Metadata.LaneIndex, SlotClass::DmaStreamClass, SlotClass::DmaStreamClass, resolvedOwnerThreadId, owner->OwnerVirtualThreadId, owner->OwnerContextId, owner->OwnerCoreId, owner->OwnerPodId, owner->OwnerDomainTag, descriptor->OwnerGuardDecision->RuntimeOwnerContext.ActiveDomainCertificate, owner->DeviceId, descriptor->DescriptorReference.DescriptorAddress, descriptor->DescriptorIdentityHash, descriptor->NormalizedFootprintHash, entry->Metadata.IssueCycle, completionCycle, entry->Metadata.ReplayEpoch);
	}


	std::shared_ptr<DmaStreamComputeRetireObservation> DmaStreamComputeRetirePublication::ObserveFutureRetire(std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry, unsigned long long architecturalInstructionAge, int ownerThreadId, unsigned long long completionCycle)
	{
		return ObserveRetire(entry, architecturalInstructionAge, ownerThreadId, completionCycle, DmaStreamComputeRetirePublicationSurfaceKind::ExplicitTestFutureSeam);
	}

	std::shared_ptr<DmaStreamComputeRetireObservation> DmaStreamComputeRetirePublication::ObserveProductionRetire(std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry, unsigned long long architecturalInstructionAge, int ownerThreadId, unsigned long long completionCycle)
	{
		return ObserveRetire(entry, architecturalInstructionAge, ownerThreadId, completionCycle, DmaStreamComputeRetirePublicationSurfaceKind::ProductionLane6RuntimeRetire);
	}

	std::shared_ptr<DmaStreamComputeRetireObservation> DmaStreamComputeRetirePublication::ObserveRetire(std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry, unsigned long long architecturalInstructionAge, int ownerThreadId, unsigned long long completionCycle, DmaStreamComputeRetirePublicationSurfaceKind surfaceKind)
	{
		ArgumentNullException::ThrowIfNull(entry);

		std::shared_ptr<DmaStreamComputeRetirePublicationMetadata> metadata = DmaStreamComputeRetirePublicationMetadata::FromActiveEntry(entry, architecturalInstructionAge, ownerThreadId, completionCycle);
		std::shared_ptr<DmaStreamComputeToken> token = entry->Token;
		const DmaStreamComputeTokenState state = token->getState();
		DmaStreamComputeRetireObservationKind kind;
		switch (state)
		{
		case DmaStreamComputeTokenState::Admitted:
		case DmaStreamComputeTokenState::Issued:
		case DmaStreamComputeTokenState::ReadsComplete:
		case DmaStreamComputeTokenState::ComputeComplete:
			kind = DmaStreamComputeRetireObservationKind::StallOrDrain;
			break;
		case DmaStreamComputeTokenState::CommitPending:
			kind = DmaStreamComputeRetireObservationKind::CommitReadyMemoryNotVisible;
			break;
		case DmaStreamComputeTokenState::Faulted:
			kind = DmaStreamComputeRetireObservationKind::FaultPublicationCandidate;
			break;
		case DmaStreamComputeTokenState::Canceled:
			kind = DmaStreamComputeRetireObservationKind::CancellationSuppressesPublication;
			break;
		case DmaStreamComputeTokenState::Committed:
			kind = DmaStreamComputeRetireObservationKind::IllegalCommittedBeforeRetireObservation;
			break;
		default:
			kind = DmaStreamComputeRetireObservationKind::StallOrDrain;
			break;
		}

		const std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> fault = token->getLastFault();
		return std::make_shared<DmaStreamComputeRetireObservation>(kind, state, metadata, fault, surfaceKind);
	}

	std::shared_ptr<DmaStreamComputeCommitResult> DmaStreamComputeRetirePublication::NormalizeBackendExceptionToTokenFault(std::shared_ptr<DmaStreamComputeToken> token, std::runtime_error exception, unsigned long long faultAddress, bool isWrite, DmaStreamComputeFaultSourcePhase sourcePhase)
	{
		ArgumentNullException::ThrowIfNull(token);
		ArgumentNullException::ThrowIfNull(exception);

		std::shared_ptr<DmaStreamComputeFaultRecord> fault = DmaStreamComputeFaultRecord::FromBackendException(token->getDescriptor(), exception, faultAddress, isWrite, sourcePhase);
		return token->PublishFault(fault);
	}

	DmaStreamComputeFaultPriorityClass DmaStreamComputeRetirePublication::ResolveSameTokenFaultPriority(DmaStreamComputeTokenFaultKind faultKind, DmaStreamComputeFaultSourcePhase sourcePhase)
	{
		if (sourcePhase == DmaStreamComputeFaultSourcePhase::Cancellation)
		{
			return DmaStreamComputeFaultPriorityClass::PostIssueCancellation;
		}

		if (sourcePhase == DmaStreamComputeFaultSourcePhase::CarrierDecode)
		{
			return DmaStreamComputeFaultPriorityClass::CarrierDecodeAndTypedSlot;
		}

		if ((sourcePhase == DmaStreamComputeFaultSourcePhase::DescriptorParse || sourcePhase == DmaStreamComputeFaultSourcePhase::Admission))
		{
			return DmaStreamComputeFaultPriorityClass::DescriptorAdmission;
		}

		if (sourcePhase == DmaStreamComputeFaultSourcePhase::Commit)
		{
			return (faultKind == DmaStreamComputeTokenFaultKind::DomainViolation || faultKind == DmaStreamComputeTokenFaultKind::OwnerContextViolation || faultKind == DmaStreamComputeTokenFaultKind::DmaDeviceFault) ? DmaStreamComputeFaultPriorityClass::CommitGuard : DmaStreamComputeFaultPriorityClass::CommitPhysicalWriteRollback;
		}

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return faultKind switch
	//			{
	//				DmaStreamComputeTokenFaultKind.DescriptorDecodeFault or DmaStreamComputeTokenFaultKind.ExecutionDisabled => DmaStreamComputeFaultPriorityClass.CarrierDecodeAndTypedSlot,
	//ignore
	//
	//				DmaStreamComputeTokenFaultKind.UnsupportedAbiOrOperation or DmaStreamComputeTokenFaultKind.AlignmentFault or DmaStreamComputeTokenFaultKind.AliasOverlapFault or DmaStreamComputeTokenFaultKind.DomainViolation or DmaStreamComputeTokenFaultKind.OwnerContextViolation => DmaStreamComputeFaultPriorityClass.DescriptorAdmission,
	//ignore
	//ignore
	//ignore
	//ignore
	//
	//				DmaStreamComputeTokenFaultKind.TranslationFault or DmaStreamComputeTokenFaultKind.PermissionFault or DmaStreamComputeTokenFaultKind.DmaDeviceFault or DmaStreamComputeTokenFaultKind.MemoryFault => DmaStreamComputeFaultPriorityClass.RuntimeReadBackendIommu,
	//ignore
	//ignore
	//ignore
	//
	//				DmaStreamComputeTokenFaultKind.PartialCompletionFault => DmaStreamComputeFaultPriorityClass.ComputeStageCoverage,
	//
	//				DmaStreamComputeTokenFaultKind.ReplayInvalidationBeforeCommit => DmaStreamComputeFaultPriorityClass.PostIssueCancellation,
	//
	//				_ => DmaStreamComputeFaultPriorityClass.DescriptorAdmission
	//			};
		switch (faultKind)
		{
		case DmaStreamComputeTokenFaultKind::DescriptorDecodeFault:
		case DmaStreamComputeTokenFaultKind::ExecutionDisabled:
			return DmaStreamComputeFaultPriorityClass::CarrierDecodeAndTypedSlot;
		case DmaStreamComputeTokenFaultKind::UnsupportedAbiOrOperation:
		case DmaStreamComputeTokenFaultKind::AlignmentFault:
		case DmaStreamComputeTokenFaultKind::AliasOverlapFault:
		case DmaStreamComputeTokenFaultKind::DomainViolation:
		case DmaStreamComputeTokenFaultKind::OwnerContextViolation:
			return DmaStreamComputeFaultPriorityClass::DescriptorAdmission;
		case DmaStreamComputeTokenFaultKind::TranslationFault:
		case DmaStreamComputeTokenFaultKind::PermissionFault:
		case DmaStreamComputeTokenFaultKind::DmaDeviceFault:
		case DmaStreamComputeTokenFaultKind::MemoryFault:
			return DmaStreamComputeFaultPriorityClass::RuntimeReadBackendIommu;
		case DmaStreamComputeTokenFaultKind::PartialCompletionFault:
			return DmaStreamComputeFaultPriorityClass::ComputeStageCoverage;
		case DmaStreamComputeTokenFaultKind::ReplayInvalidationBeforeCommit:
			return DmaStreamComputeFaultPriorityClass::PostIssueCancellation;
		default:
			return DmaStreamComputeFaultPriorityClass::DescriptorAdmission;
		}
	}

	DmaStreamComputeFaultPriorityClass DmaStreamComputeRetirePublication::ResolveValidationFaultPriority(DmaStreamComputeValidationFault validationFault)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return validationFault switch
	//			{
	//				DmaStreamComputeValidationFault.QuotaAdmissionReject or DmaStreamComputeValidationFault.BackpressureAdmissionReject or DmaStreamComputeValidationFault.TokenCapAdmissionReject => DmaStreamComputeFaultPriorityClass.QuotaBackpressureTokenCap,
	//ignore
	//ignore
	//
	//				DmaStreamComputeValidationFault.DescriptorCarrierDecodeFault => DmaStreamComputeFaultPriorityClass.CarrierDecodeAndTypedSlot,
	//
	//				DmaStreamComputeValidationFault.None => throw new ArgumentException(
	//						"Successful validation does not have a DSC fault priority.", nameof(validationFault)), _ => DmaStreamComputeFaultPriorityClass.DescriptorAdmission
	//
	//			};
		switch (validationFault)
		{
		case DmaStreamComputeValidationFault::QuotaAdmissionReject:
		case DmaStreamComputeValidationFault::BackpressureAdmissionReject:
		case DmaStreamComputeValidationFault::TokenCapAdmissionReject:
			return DmaStreamComputeFaultPriorityClass::QuotaBackpressureTokenCap;
		case DmaStreamComputeValidationFault::DescriptorCarrierDecodeFault:
			return DmaStreamComputeFaultPriorityClass::CarrierDecodeAndTypedSlot;
		case DmaStreamComputeValidationFault::None:
			throw std::invalid_argument("Successful validation does not have a DSC fault priority.");
		default:
			return DmaStreamComputeFaultPriorityClass::DescriptorAdmission;
		}
	}
}
