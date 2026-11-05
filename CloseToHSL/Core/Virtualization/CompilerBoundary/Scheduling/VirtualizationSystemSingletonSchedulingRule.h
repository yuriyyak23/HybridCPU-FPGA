#pragma once
#include "../../../Pipeline/Scheduling/SlotPlacementMetadata.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VirtualizationSchedulingDecision : unsigned char
	{
		Allowed = 0,
		DeniedUnvalidatedDescriptor = 1,
		DeniedNotSystemSingleton = 2,
		DeniedNotHardPinned = 3,
		DeniedNotLane7 = 4,
		DeniedMultipleSystemSingletons = 5
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VirtualizationSystemSingletonSchedulingRequest(SlotPlacementMetadata Placement, bool DescriptorValidated, byte SystemSingletonCount);


struct VirtualizationSystemSingletonSchedulingRequest final
{
    SlotPlacementMetadata Placement{};
    bool DescriptorValidated = false;
    unsigned char SystemSingletonCount = 0;
    VirtualizationSystemSingletonSchedulingRequest(SlotPlacementMetadata placement, bool descriptorValidated, unsigned char singletonCount)
        : Placement(placement), DescriptorValidated(descriptorValidated), SystemSingletonCount(singletonCount) {}
};
}
