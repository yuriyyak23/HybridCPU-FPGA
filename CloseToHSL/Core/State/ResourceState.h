#pragma once

#include "../Pipeline/Safety/ResourceBitset.h"
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Storage-only containment for the existing per-core GRLB, token,
	/// scoreboard-reference and synchronization bookkeeping. Existing CPU_Core
	/// methods retain acquire, release, reset and diagnostic authority.
	/// </summary>
	class ResourceState final : public std::enable_shared_from_this<ResourceState>
	{
	public:
		ResourceBitset GlobalResourceLocks;
		unsigned long long TokenGeneration = 0;
		std::vector<unsigned long long> ResourceTokens = std::vector<unsigned long long>(128);
		unsigned long long StructuralStalls = 0;
		std::vector<unsigned long long> ResourceUsageCounts = std::vector<unsigned long long>(128);
		std::vector<unsigned long long> ResourceContentionCounts = std::vector<unsigned long long>(128);
		std::vector<unsigned char> ReadCounters = std::vector<unsigned char>(16);
		unsigned long long SyncCounter = 0;
		std::vector<unsigned int> GrlbBanks = std::vector<unsigned int>(4);
		std::vector<unsigned long long> BankContentionCounts = std::vector<unsigned long long>(4);
	};

}
