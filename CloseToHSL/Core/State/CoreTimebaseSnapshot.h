#pragma once

#include <string>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Authoritative runtime timebase exported from the live pipeline contour.
	/// When the pipeline has not yet published timing facts, callers receive an
	/// explicit unavailable snapshot instead of falling back to legacy counters.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct CoreTimebaseSnapshot
	class CoreTimebaseSnapshot
	{
	private:
		const unsigned long long CycleCount = 0;
		const bool IsStalled = false;
		const bool IsAvailable = false;
		const std::wstring UnavailableReason;

	public:
		CoreTimebaseSnapshot(unsigned long long cycleCount, bool isStalled, bool isAvailable, const std::wstring &unavailableReason);

		unsigned long long getCycleCount() const;
		bool isStalled() const;
		bool isAvailable() const;
		std::wstring getUnavailableReason() const;

		CoreTimebaseSnapshot() = default;
	};
}
