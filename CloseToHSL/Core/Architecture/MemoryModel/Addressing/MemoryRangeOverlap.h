#pragma once

#include <limits>

namespace YAKSys_Hybrid_CPU::Core::Memory
{
	/// <summary>
	/// Deterministic half-open range helpers for modeled cache/SRF invalidation.
	/// Overflowing ranges are treated as malformed instead of wrapping.
	/// </summary>
	class MemoryRangeOverlap final
	{
	public:
		static bool IsNonEmptyWellFormed(unsigned long long address, unsigned long long length);

		static bool TryGetEndExclusive(unsigned long long address, unsigned long long length, unsigned long long &endExclusive);

		static bool RangesOverlap(unsigned long long leftAddress, unsigned long long leftLength, unsigned long long rightAddress, unsigned long long rightLength);
	};
}
