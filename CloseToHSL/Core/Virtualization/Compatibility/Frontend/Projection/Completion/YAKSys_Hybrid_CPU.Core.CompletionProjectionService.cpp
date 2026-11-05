#include "YAKSys_Hybrid_CPU.Core.CompletionProjectionService.h"

namespace YAKSys_Hybrid_CPU::Core
{

	namespace { bool IsDefinedVmExitReason(unsigned int value) { switch (static_cast<VmExitReason>(value)) {
        case VmExitReason::None: return true;
        case VmExitReason::ExternalInterrupt: return true;
        case VmExitReason::TripleFault: return true;
        case VmExitReason::InitSignal: return true;
        case VmExitReason::Hlt: return true;
        case VmExitReason::VmCall: return true;
        case VmExitReason::VmxOff: return true;
        case VmExitReason::InvalidGuestState: return true;
        case VmExitReason::EntryFailMsrLoading: return true;
        case VmExitReason::EntryFailMachineCheck: return true;
        case VmExitReason::EptViolation: return true;
        case VmExitReason::EptMisconfiguration: return true;
        case VmExitReason::VmFunc: return true;
        case VmExitReason::InstructionIntercept: return true;
        case VmExitReason::CsrIntercept: return true;
        case VmExitReason::MemoryIntercept: return true;
        case VmExitReason::VmxOperationIntercept: return true;
        case VmExitReason::LaneOperationIntercept: return true;
        case VmExitReason::VirtualTimer: return true;
        case VmExitReason::VmxPreemptionTimerExpired: return true;
        case VmExitReason::VirtualInterrupt: return true;
        case VmExitReason::DmaDescriptorFault: return true;
        case VmExitReason::IommuFault: return true;
        case VmExitReason::DmaPermissionFault: return true;
        case VmExitReason::DmaAbort: return true;
        case VmExitReason::DmaReplay: return true;
        case VmExitReason::VectorException: return true;
        case VmExitReason::StreamDescriptorFault: return true;
        case VmExitReason::StreamReplayRequired: return true;
        case VmExitReason::Lane7TokenFault: return true;
        case VmExitReason::Lane7BackendUnavailable: return true;
        case VmExitReason::Lane7QuotaExceeded: return true;
        case VmExitReason::SecurityPolicyViolation: return true;
        default: return false;
    } } }

	std::shared_ptr<VmxCompletionProjection> CompletionProjectionService::ProjectToVmx(std::shared_ptr<CompletionRecord> record)
	{
		if (!CanProjectToVmx(record))
		{
			return VmxCompletionProjection::None;
		}

		return std::make_shared<VmxCompletionProjection>(ProjectReason(record), record->getQualification(), record->getFaultAddress(), record->getFaultAux());
	}

	VmExitReason CompletionProjectionService::ProjectReason(std::shared_ptr<CompletionRecord> record)
	{
		if (!CanProjectToVmx(record))
		{
			return VmExitReason::None;
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
		if (IsDefinedVmExitReason(record->getReasonCode()))
		{
			return static_cast<VmExitReason>(record->getReasonCode());
		}

		return VmExitReason::SecurityPolicyViolation;
	}

	bool CompletionProjectionService::CanProjectToVmx(std::shared_ptr<CompletionRecord> record)
	{
		return record != nullptr && record->isCompatibilityProjectionSource();
	}
}
