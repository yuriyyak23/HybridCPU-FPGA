#include "SlotAssignment.h"

namespace YAKSys_Hybrid_CPU::Core::Pipeline::Scheduling
{

	const std::vector<SlotAdmission> SlotAssignment::getEntries() const
	{
		return _entries;
	}

	bool SlotAssignment::getHasAssignments() const
	{
		return _entries.size() > 0;
	}

	int SlotAssignment::getCount() const
	{
		return _entries.size();
	}

	void SlotAssignment::Add(int slotIndex, int donorVtId)
	{
		_entries.push_back(SlotAdmission(slotIndex, donorVtId));
	}

	SlotAdmission::SlotAdmission(int slotIndex, int donorVtId) : SlotIndex(slotIndex), DonorVtId(donorVtId)
	{
	}
}
