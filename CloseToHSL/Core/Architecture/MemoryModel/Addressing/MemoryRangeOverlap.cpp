#include "MemoryRangeOverlap.h"

namespace YAKSys_Hybrid_CPU::Core::Memory
{

	bool MemoryRangeOverlap::IsNonEmptyWellFormed(unsigned long long address, unsigned long long length)
	{
		return length != 0 && length <= std::numeric_limits<unsigned long long>::max() - address;
	}

	bool MemoryRangeOverlap::TryGetEndExclusive(unsigned long long address, unsigned long long length, unsigned long long &endExclusive)
	{
		if (length == 0 || length > std::numeric_limits<unsigned long long>::max() - address)
		{
			endExclusive = address;
			return false;
		}

		endExclusive = address + length;
		return true;
	}

	bool MemoryRangeOverlap::RangesOverlap(unsigned long long leftAddress, unsigned long long leftLength, unsigned long long rightAddress, unsigned long long rightLength)
	{
		unsigned long long leftEnd;
		unsigned long long rightEnd;
		if (!TryGetEndExclusive(leftAddress, leftLength, leftEnd) || !TryGetEndExclusive(rightAddress, rightLength, rightEnd))
		{
			return false;
		}

		return leftAddress < rightEnd && rightAddress < leftEnd;
	}
}
