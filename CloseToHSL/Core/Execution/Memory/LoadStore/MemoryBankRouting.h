#pragma once

#include "tangible_exception_helper.h"
#include <atomic>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Memory
{
	/// <summary>
	/// Single authority for scheduler-visible memory-bank routing.
	/// Returns an explicit invalid bank contour when the runtime memory
	/// subsystem has not been materialized yet, and records that use so the
	/// uninitialized path cannot remain silent or masquerade as real bank truth.
	/// </summary>
	class MemoryBankRouting final
	{
	public:
		static constexpr int UninitializedSchedulerVisibleBankId = -1;
	private:
		static constexpr int DefaultNumBanks = 16;
		static constexpr int DefaultBankWidthBytes = 4096;
		static inline std::atomic<long long> _schedulerVisibleUninitializedUseCount{0};

	public:
		static int ResolveStandaloneReadOnlyBank(unsigned long long offset, unsigned long long sourceLength);

		static unsigned long long getSchedulerVisibleUninitializedUseCount();

		static bool IsResolvedSchedulerVisibleBankId(int bankId);

		static void ResetTelemetryForTesting();

		static int ResolveSchedulerVisibleBankId(unsigned long long address);

		static int ResolveBankId(unsigned long long address, int bankWidthBytes, int numBanks);
	};
}
