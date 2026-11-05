#include "YAKSys_Hybrid_CPU.Core.VirtualizationSystemSingletonSchedulingRule.h"
#include "../../../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../LaneBinding/YAKSys_Hybrid_CPU.Core.VirtualizationLaneBindingPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	VirtualizationSchedulingDecision VirtualizationSystemSingletonSchedulingRule::Evaluate(std::shared_ptr<VirtualizationSystemSingletonSchedulingRequest> request)
	{
		if (!request->DescriptorValidated)
		{
			return VirtualizationSchedulingDecision::DeniedUnvalidatedDescriptor;
		}

		if (request->Placement.RequiredSlotClass != SlotClass::SystemSingleton)
		{
			return VirtualizationSchedulingDecision::DeniedNotSystemSingleton;
		}

		if (request->Placement.PinningKind != SlotPinningKind::HardPinned)
		{
			return VirtualizationSchedulingDecision::DeniedNotHardPinned;
		}

		if (request->Placement.PinnedLaneId != VirtualizationLaneBindingPolicy::Lane7Id)
		{
			return VirtualizationSchedulingDecision::DeniedNotLane7;
		}

		if (request->SystemSingletonCount > 1)
		{
			return VirtualizationSchedulingDecision::DeniedMultipleSystemSingletons;
		}

		return VirtualizationSchedulingDecision::Allowed;
	}

	bool VirtualizationSystemSingletonSchedulingRule::CanSchedule(std::shared_ptr<VirtualizationSystemSingletonSchedulingRequest> request)
	{
		return Evaluate(request) == VirtualizationSchedulingDecision::Allowed;
	}
}
