#include "MemoryDomain.h"

namespace HybridCPU_ISE::CloseToHSL::Memory::MMU
{

	bool MemoryDomain::Contains(unsigned long long address, unsigned long long length)
	{
		// Check for overflow in the range calculation
		if (length > 0 && address > (std::numeric_limits<unsigned long long>::max() - length))
		{
			return false;
		}

		// Address must be >= base, and end must be <= domain end
		return address >= BaseAddress && (address + length) <= (BaseAddress + Size);
	}

	bool MemoryDomain::Overlaps(unsigned long long address, unsigned long long length)
	{
		// Check for overflow
		if (length > 0 && address > (std::numeric_limits<unsigned long long>::max() - length))
		{
			return true; // Conservative: assume overlap if overflow
		}

		// Two ranges [a1,a2) and [b1,b2) overlap if: a1 < b2 AND b1 < a2
		unsigned long long end1 = BaseAddress + Size;
		unsigned long long end2 = address + length;
		return BaseAddress < end2 && address < end1;
	}
}
