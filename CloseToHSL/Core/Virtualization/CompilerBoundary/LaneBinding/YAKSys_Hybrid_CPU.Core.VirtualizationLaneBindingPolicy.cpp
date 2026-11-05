#include "YAKSys_Hybrid_CPU.Core.VirtualizationLaneBindingPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SlotPlacementMetadata VirtualizationLaneBindingPolicy::getCompatibilityFrontendPlacement()
	{
		return CompatibilityFrontendPlacement;
	}

	SlotPlacementMetadata VirtualizationLaneBindingPolicy::getLane6DescriptorPlacement()
	{
		return Lane6DescriptorPlacement;
	}

	VirtualizationLaneBindingDecision VirtualizationLaneBindingPolicy::Evaluate(std::shared_ptr<VirtualizationLaneBindingRequest> request)
	{
		if (!request->DescriptorValidated)
		{
			return VirtualizationLaneBindingDecision::DeniedUnvalidatedDescriptor;
		}

		if (!request->IsCompatibilityFrontend && !request->IsLaneResourceOwnedByDescriptor)
		{
			return VirtualizationLaneBindingDecision::DeniedVmxOwnedLaneResource;
		}

		SlotPlacementMetadata expected = request->IsCompatibilityFrontend ? getCompatibilityFrontendPlacement() : getLane6DescriptorPlacement();

		if (request->Placement.RequiredSlotClass != expected.RequiredSlotClass || request->Placement.PinningKind != SlotPinningKind::HardPinned)
		{
			return VirtualizationLaneBindingDecision::DeniedWrongSlotClass;
		}

		if (request->Placement.PinnedLaneId != expected.PinnedLaneId)
		{
			return VirtualizationLaneBindingDecision::DeniedWrongPinnedLane;
		}

		return VirtualizationLaneBindingDecision::Allowed;
	}

	bool VirtualizationLaneBindingPolicy::CanBind(std::shared_ptr<VirtualizationLaneBindingRequest> request)
	{
		return Evaluate(request) == VirtualizationLaneBindingDecision::Allowed;
	}
}
