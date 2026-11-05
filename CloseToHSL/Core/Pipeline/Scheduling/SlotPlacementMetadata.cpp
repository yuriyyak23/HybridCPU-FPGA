#include "SlotPlacementMetadata.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SlotPlacementMetadata SlotPlacementMetadata::getDefault()
	{
		SlotPlacementMetadata temp_var = SlotPlacementMetadata();
		temp_var.RequiredSlotClass = SlotClass::AluClass;
		temp_var.PinningKind = SlotPinningKind::ClassFlexible;
		temp_var.PinnedLaneId = 0;
		temp_var.DomainTag = 0;
		return temp_var;
	}
}
