#include "SlotClassCapacity.h"

namespace YAKSys_Hybrid_CPU::Core {
	bool SlotClassCapacityState::HasFreeCapacity(SlotClass slotClass) const
	{
		int free = GetFreeCapacity(slotClass);
		if (free <= 0)
		{
			return false;
		}

		// For aliased classes, subtract the occupancy of co-aliased classes
		// that compete for the same physical lane(s).
		auto aliased = SlotClassLaneMap::GetAliasedClasses(slotClass);
		int aliasedOccupied = 0;
		for (auto alias : aliased)
		{
			aliasedOccupied += GetOccupied(alias);
		}

		return free - aliasedOccupied > 0;
	}

	int SlotClassCapacityState::GetFreeCapacity(SlotClass slotClass) const
	{
		int temp_var136;
		switch (slotClass)
		{
			case SlotClass::AluClass:
				temp_var136 = AluTotal - AluOccupied;
				break;
			case SlotClass::LsuClass:
				temp_var136 = LsuTotal - LsuOccupied;
				break;
			case SlotClass::DmaStreamClass:
				temp_var136 = DmaStreamTotal - DmaStreamOccupied;
				break;
			case SlotClass::MatrixTileStreamClass:
				temp_var136 = MatrixTileStreamTotal - MatrixTileStreamOccupied;
				break;
			case SlotClass::BranchControl:
				temp_var136 = BranchControlTotal - BranchControlOccupied;
				break;
			case SlotClass::SystemSingleton:
				temp_var136 = SystemSingletonTotal - SystemSingletonOccupied;
				break;
			default:
				temp_var136 = 0;
				break;
		}
		return temp_var136;
	}

	int SlotClassCapacityState::GetOccupied(SlotClass slotClass) const
	{
		int temp_var137;
		switch (slotClass)
		{
			case SlotClass::AluClass:
				temp_var137 = AluOccupied;
				break;
			case SlotClass::LsuClass:
				temp_var137 = LsuOccupied;
				break;
			case SlotClass::DmaStreamClass:
				temp_var137 = DmaStreamOccupied;
				break;
			case SlotClass::MatrixTileStreamClass:
				temp_var137 = MatrixTileStreamOccupied;
				break;
			case SlotClass::BranchControl:
				temp_var137 = BranchControlOccupied;
				break;
			case SlotClass::SystemSingleton:
				temp_var137 = SystemSingletonOccupied;
				break;
			default:
				temp_var137 = 0;
				break;
		}
		return temp_var137;
	}

	void SlotClassCapacityState::IncrementOccupancy(SlotClass slotClass)
	{
		switch (slotClass)
		{
			case SlotClass::AluClass:
				AluOccupied++;
				break;
			case SlotClass::LsuClass:
				LsuOccupied++;
				break;
			case SlotClass::DmaStreamClass:
				DmaStreamOccupied++;
				break;
			case SlotClass::MatrixTileStreamClass:
				MatrixTileStreamOccupied++;
				break;
			case SlotClass::BranchControl:
				BranchControlOccupied++;
				break;
			case SlotClass::SystemSingleton:
				SystemSingletonOccupied++;
				break;
			// Unclassified ops do not consume typed capacity
		}
	}

	void SlotClassCapacityState::InitializeFromLaneMap()
	{
		AluTotal = static_cast<unsigned char>(SlotClassLaneMap::GetClassCapacity(SlotClass::AluClass));
		LsuTotal = static_cast<unsigned char>(SlotClassLaneMap::GetClassCapacity(SlotClass::LsuClass));
		DmaStreamTotal = static_cast<unsigned char>(SlotClassLaneMap::GetClassCapacity(SlotClass::DmaStreamClass));
		MatrixTileStreamTotal = static_cast<unsigned char>(SlotClassLaneMap::GetClassCapacity(SlotClass::MatrixTileStreamClass));
		BranchControlTotal = static_cast<unsigned char>(SlotClassLaneMap::GetClassCapacity(SlotClass::BranchControl));
		SystemSingletonTotal = static_cast<unsigned char>(SlotClassLaneMap::GetClassCapacity(SlotClass::SystemSingleton));

		ResetOccupancy();
	}

	void SlotClassCapacityState::ResetOccupancy()
	{
		AluOccupied = 0;
		LsuOccupied = 0;
		DmaStreamOccupied = 0;
		MatrixTileStreamOccupied = 0;
		BranchControlOccupied = 0;
		SystemSingletonOccupied = 0;
	}


}
