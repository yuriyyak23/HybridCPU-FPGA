#pragma once
#include "../../../Pipeline/Scheduling/SlotPlacementMetadata.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VirtualizationLaneBindingDecision : unsigned char
	{
		Allowed = 0,
		DeniedUnvalidatedDescriptor = 1,
		DeniedVmxOwnedLaneResource = 2,
		DeniedWrongSlotClass = 3,
		DeniedWrongPinnedLane = 4
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VirtualizationLaneBindingRequest(SlotPlacementMetadata Placement, bool DescriptorValidated, bool IsCompatibilityFrontend, bool IsLaneResourceOwnedByDescriptor);


struct VirtualizationLaneBindingRequest final
{
    SlotPlacementMetadata Placement{};
    bool DescriptorValidated = false;
    bool IsCompatibilityFrontend = false;
    bool IsLaneResourceOwnedByDescriptor = false;
    VirtualizationLaneBindingRequest(SlotPlacementMetadata placement, bool descriptorValidated, bool compatibilityFrontend, bool descriptorOwnsLane)
        : Placement(placement), DescriptorValidated(descriptorValidated), IsCompatibilityFrontend(compatibilityFrontend), IsLaneResourceOwnedByDescriptor(descriptorOwnsLane) {}
};
}
