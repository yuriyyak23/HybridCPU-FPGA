#include "VmxDirtyLogProjectionTypes.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public VmxDirtyLogSnapshot(bool enabled, uint pageSize, ulong guestPhysicalBase, ulong guestPhysicalLimitExclusive, ulong generation, ulong writeSequence, bool overflowed, IReadOnlyList<ulong> dirtyPageIndices)
	VmxDirtyLogSnapshot::VmxDirtyLogSnapshot(bool enabled, unsigned int pageSize, unsigned long long guestPhysicalBase, unsigned long long guestPhysicalLimitExclusive, unsigned long long generation, unsigned long long writeSequence, bool overflowed, const std::vector<unsigned long long> &dirtyPageIndices)
		: Enabled(enabled), PageSize(pageSize), GuestPhysicalBase(guestPhysicalBase),
		  GuestPhysicalLimitExclusive(guestPhysicalLimitExclusive), Generation(generation),
		  WriteSequence(writeSequence), Overflowed(overflowed), DirtyPageIndices(dirtyPageIndices)
	{
	}

	bool VmxDirtyLogSnapshot::getEnabled() const
	{
		return Enabled;
	}

	unsigned int VmxDirtyLogSnapshot::getPageSize() const
	{
		return PageSize;
	}

	unsigned long long VmxDirtyLogSnapshot::getGuestPhysicalBase() const
	{
		return GuestPhysicalBase;
	}

	unsigned long long VmxDirtyLogSnapshot::getGuestPhysicalLimitExclusive() const
	{
		return GuestPhysicalLimitExclusive;
	}

	unsigned long long VmxDirtyLogSnapshot::getGeneration() const
	{
		return Generation;
	}

	unsigned long long VmxDirtyLogSnapshot::getWriteSequence() const
	{
		return WriteSequence;
	}

	bool VmxDirtyLogSnapshot::getOverflowed() const
	{
		return Overflowed;
	}

	const std::vector<unsigned long long> VmxDirtyLogSnapshot::getDirtyPageIndices() const
	{
		return DirtyPageIndices;
	}

	bool VmxDirtyLogSnapshot::ContainsGuestPhysicalAddress(unsigned long long address)
	{
		if (!getEnabled() || address < getGuestPhysicalBase() || getPageSize() == 0)
		{
			return false;
		}

		unsigned long long pageIndex = (address - getGuestPhysicalBase()) / getPageSize();
		for (int index = 0; index < getDirtyPageIndices().size(); index++)
		{
			if (getDirtyPageIndices()[index] == pageIndex)
			{
				return true;
			}
		}

		return false;
	}
}
