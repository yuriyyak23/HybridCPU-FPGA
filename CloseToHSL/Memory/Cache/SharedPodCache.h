#pragma once

#include <vector>
#include <span>
#include <tuple>
#include <cmath>
#include <memory>

namespace HybridCPU_ISE::CloseToHSL::Memory::Cache
{
	/// <summary>
	/// L2 Shared Pod Cache — shared memory within a Pod of 16 cores (req.md §2).
	///
	/// Purpose: Model a per-Pod shared cache that sits between L1 (per-core TCDM/StreamRegisterFile)
	/// and L3 (Global Distributed SRAM). When a core gets an L2 miss, its VLIW slots become
	/// available for FSP donation — the scheduler injects ALU ops from sibling cores whose
	/// data is already resident in L2.
	///
	/// Architecture:
	/// - Fixed number of cache lines (set-associative, tag-based)
	/// - Per-line domain tag for Singularity-style isolation (req.md §4)
	/// - Miss signal exposed to PodController/Scheduler for FSP integration
	/// - HLS-compatible: fixed-size arrays, deterministic lookup
	///
	/// Simplification: Direct-mapped for HLS, with tag + valid + dirty bits.
	/// </summary>
	class SharedPodCache : public std::enable_shared_from_this<SharedPodCache>
	{
	private:
		long long TotalLookups = 0;
		long long Hits = 0;
		long long Misses = 0;

		/// <summary>Number of cache lines (must be power of 2)</summary>
	public:
		static constexpr int NUM_LINES = 256;

		/// <summary>Cache line size in bytes (matches AXI burst width)</summary>
		static constexpr int LINE_SIZE_BYTES = 256;

		/// <summary>Tag bits shift: log2(NUM_LINES * LINE_SIZE_BYTES)</summary>
	private:
		static constexpr int INDEX_BITS = 8; // log2(256)

		/// <summary>Offset bits: log2(LINE_SIZE_BYTES)</summary>
		static constexpr int OFFSET_BITS = 8; // log2(256)

		/// <summary>Cache line metadata (HLS: register array)</summary>
	private:
		class CacheLine
		{
		public:
			unsigned long long Tag = 0;
			bool Valid = false;
			bool Dirty = false;
			unsigned long long DomainTag = 0;
			int OwnerCoreId = 0;
			long long LastAccessCycle = 0;
		};

	private:
		std::vector<CacheLine> _lines;
		std::vector<std::vector<unsigned char>> _data;
		long long _currentCycle = 0;

		/// <summary>Total L2 lookups</summary>
	public:
		long long getTotalLookups() const;
	private:
		void setTotalLookups(long long value);

		/// <summary>L2 cache hits</summary>
	public:
		long long getHits() const;
	private:
		void setHits(long long value);

		/// <summary>L2 cache misses</summary>
	public:
		long long getMisses() const;
	private:
		void setMisses(long long value);

		// Phase §2: Per-domain access counters for fairness monitoring.
		// Tracks how many L2 accesses each domain makes per measurement window.
		// A significant imbalance between domains may indicate a timing side-channel
		// (e.g., one domain evicting another's lines to probe access patterns).
		// HLS: 16 × 32-bit saturating counters (1 per possible domain slot).
		static constexpr int MAX_DOMAIN_COUNTERS = 16;
		std::vector<long long> _domainAccessCounts;
		std::vector<long long> _domainHitCounts;
		std::vector<long long> _domainMissCounts;

		/// <summary>
		/// Per-core miss-pending flag (16 cores per Pod).
		/// When true, indicates this core is waiting on an L2 miss and its
		/// VLIW slots should be treated as donor-eligible by FSP (req.md §2).
		/// HLS: 16-bit register.
		/// </summary>
		std::vector<bool> _coreMissPending;

		/// <summary>
		/// Bitmask of cores with pending L2 misses.
		/// Consumed by MicroOpScheduler to identify donor slots.
		/// </summary>
	public:
        unsigned short getMissPendingMask() const;
        SharedPodCache();

		/// <summary>
		/// Lookup an address in L2 cache.
		/// Returns true on hit, false on miss. On miss, sets the requesting core's
		/// miss-pending flag so FSP can donate its empty slots.
		/// </summary>
		/// <param name="address">Physical address to lookup</param>
		/// <param name="localCoreId">Requesting core within the Pod (0–15)</param>
		/// <param name="domainTag">Domain tag for isolation check</param>
		/// <returns>True if L2 hit, false if L2 miss</returns>
		bool Lookup(unsigned long long address, int localCoreId, unsigned long long domainTag);

		/// <summary>
		/// Fill a cache line after an L3/HBM4 fetch completes.
		/// Clears the core's miss-pending flag.
		/// </summary>
		/// <param name="address">Address of the line to fill</param>
		/// <param name="data">Data from L3/HBM4</param>
		/// <param name="localCoreId">Core that requested the fill</param>
		/// <param name="domainTag">Domain tag for the line</param>
		void Fill(unsigned long long address, std::span<const unsigned char> data, int localCoreId, unsigned long long domainTag);

		/// <summary>
		/// Invalidate a cache line (e.g., on coherence action or domain revocation).
		/// </summary>
		void Invalidate(unsigned long long address);

		/// <summary>
		/// Check if a specific core has a pending L2 miss (for FSP slot donation).
		/// </summary>
		bool IsCoreMissPending(int localCoreId);

		/// <summary>
		/// Set or clear the miss-pending flag for a core.
		/// </summary>
	private:
		void SetCoreMissPending(int localCoreId, bool pending);

		/// <summary>
		/// Clear all miss-pending flags (e.g., on Pod reset or barrier).
		/// </summary>
	public:
		void ClearAllMissPending();

		/// <summary>
		/// Advance the internal cycle counter.
		/// </summary>
		void AdvanceCycle();

		/// <summary>
		/// Reset all cache state.
		/// </summary>
		void Reset();

		/// <summary>
		/// Phase §2: Get per-domain access statistics for fairness monitoring.
		/// Returns (totalAccesses, hits, misses) for a given domain slot.
		/// The scheduler can compare these across domains to detect timing side-channel
		/// patterns (e.g., one domain causing disproportionate evictions of another).
		/// HLS: direct register read, zero latency.
		/// </summary>
		/// <param name="domainSlot">Domain slot index (domainTag mod 16)</param>
		/// <returns>Tuple of (accesses, hits, misses)</returns>
// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		public (long Accesses, long Hits, long Misses) GetDomainAccessCounts(int domainSlot)
//		{
//			if ((uint)domainSlot >= MAX_DOMAIN_COUNTERS)
//				return (0, 0, 0);
//			return (_domainAccessCounts[domainSlot], _domainHitCounts[domainSlot], _domainMissCounts[domainSlot]);
//		}

		/// <summary>
		/// Phase §2: Reset per-domain fairness counters (e.g., at the start of each
		/// measurement window). Allows periodic fairness evaluation without
		/// accumulating stale data from earlier execution phases.
		/// </summary>
		std::tuple<long long, long long, long long> GetDomainAccessCounts(int domainSlot) const;
		void ResetDomainCounters();

	private:
		static int GetIndex(unsigned long long address);

		static unsigned long long GetTag(unsigned long long address);
	};
}
