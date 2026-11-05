#include "SlotClassCapacity.h"
#include "../../MicroOps/Types/MicroOp.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static SlotClassCapacityState ComputeFromBundle(MicroOp?[] bundle, int bundleWidth = 8)
	SlotClassCapacityState SlotClassCapacity::ComputeFromBundle(std::vector<std::shared_ptr<MicroOp>> &bundle, int bundleWidth)
	{
		SlotClassCapacityState state = SlotClassCapacityState();
		state.InitializeFromLaneMap();

		int limit = bundle.size() < bundleWidth ? bundle.size() : bundleWidth;
		for (int i = 0; i < limit; i++)
		{
			if (bundle[i] != nullptr)
			{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: state.IncrementOccupancy(bundle[i]!.Placement.RequiredSlotClass);
				state.IncrementOccupancy(bundle[i]->getPlacement().RequiredSlotClass);
			}
		}

		return state;
	}
}
