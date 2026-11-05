#include "VmxTrapProjectionMapper.h"
#include "../../../../../Runtime/Events/Traps/TrapRequest.h"
#include "../../Retire/VmxOperationKind.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VmxTrapProjectionMapper> VmxTrapProjectionMapper::getDefault()
	{
		return Default;
	}

	std::shared_ptr<TrapDecision> VmxTrapProjectionMapper::Project(std::shared_ptr<NeutralTrapResult> result)
	{
		return result->ShouldTrap ? TrapDecision::Exit(result->Request, ProjectReason(result)) : TrapDecision::NoExit(result->Request);
	}

	VmExitReason VmxTrapProjectionMapper::ProjectReason(std::shared_ptr<NeutralTrapResult> result)
	{
		return result->Kind == NeutralTrapResultKind::CompatibilityOperationIntercept ? ProjectCompatibilityOperationReason(result->Request) : ProjectReason(result->Kind);
	}

	VmExitReason VmxTrapProjectionMapper::ProjectReason(NeutralTrapResultKind kind)
	{
	switch(kind) {
        case NeutralTrapResultKind::None: return VmExitReason::None;
        case NeutralTrapResultKind::InstructionIntercept: return VmExitReason::InstructionIntercept;
        case NeutralTrapResultKind::CsrIntercept: return VmExitReason::CsrIntercept;
        case NeutralTrapResultKind::CompatibilityOperationIntercept: return VmExitReason::VmxOperationIntercept;
        case NeutralTrapResultKind::MemoryIntercept: return VmExitReason::MemoryIntercept;
        case NeutralTrapResultKind::LaneOperationIntercept: return VmExitReason::LaneOperationIntercept;
        case NeutralTrapResultKind::PreemptionTimerExpired: return VmExitReason::VmxPreemptionTimerExpired;
        case NeutralTrapResultKind::SecurityPolicyViolation: return VmExitReason::SecurityPolicyViolation;
        default: return VmExitReason::SecurityPolicyViolation;
        }
	}

	VmExitReason VmxTrapProjectionMapper::ProjectCompatibilityOperationReason(std::shared_ptr<TrapRequest> request)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return request.TargetKind == TrapTargetKind.CompatibilityOperation && unchecked((byte)request.Target) == (byte)VmxOperationKind.VmCall ? VmExitReason.VmCall : VmExitReason.VmxOperationIntercept;
		return request->TargetKind == TrapTargetKind::CompatibilityOperation && static_cast<unsigned char>(request->Target) == static_cast<unsigned char>(VmxOperationKind::VmCall) ? VmExitReason::VmCall : VmExitReason::VmxOperationIntercept;
	}
}
