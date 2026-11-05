#pragma once

#include <vector>
#include <memory>

namespace HybridCPU_ISE::CloseToHSL::Memory::Cache
{
	/// <summary>
	/// L3 Global Distributed Cache — shared SRAM buffer distributed across NoC nodes (req.md §2).
	///
	/// Purpose: Acts as a large shared buffer between L2 (per-Pod) and HBM4 DRAM.
	/// Data from HBM4 is loaded here via large AXI4-burst DMA transactions,
	/// then distributed to Pods via NoC. Similar to AMD Infinity Cache.
	///
	/// Architecture:
	/// - Distributed across NoC nodes (each Pod's router holds a slice)
	/// - Home node determined by address hashing (maps address → NoC coordinate)
	/// - HLS: tag+valid arrays per node, fixed-size
	///
	/// Current implementation: Stub model with basic hit/miss tracking.
	/// Full implementation would distribute slices across NoC_XY_Router instances.
	/// </summary>
	class GlobalDistributedCache : public std::enable_shared_from_this<GlobalDistributedCache>
	{
	private:
		long long TotalLookups = 0;
		long long Hits = 0;
		long long Misses = 0;

		/// <summary>Total number of lines across all slices</summary>
	public:
		static constexpr int TOTAL_LINES = 4096;

		/// <summary>Cache line size (matches HBM4 burst width)</summary>
		static constexpr int LINE_SIZE_BYTES = 256;

		/// <summary>Number of NoC nodes (64 Pods)</summary>
	private:
		static constexpr int NUM_SLICES = 64;

		/// <summary>Lines per slice</summary>
		static constexpr int LINES_PER_SLICE = TOTAL_LINES / NUM_SLICES;

		static constexpr int OFFSET_BITS = 8; // log2(256)

		/// <summary>Per-line metadata</summary>
	private:
		class L3Line
		{
		public:
			unsigned long long Tag = 0;
			bool Valid = false;
			unsigned long long DomainTag = 0;
			long long LastAccessCycle = 0;
		};

	private:
		// The collection identity stays fixed after construction, while each line's
		// metadata remains mutable just like elements of the C# readonly array.
		std::vector<L3Line> _lines;
		long long _currentCycle = 0;

		/// <summary>Total L3 lookups</summary>
	public:
		long long getTotalLookups() const;
	private:
		void setTotalLookups(long long value);

		/// <summary>L3 cache hits</summary>
	public:
		long long getHits() const;
	private:
		void setHits(long long value);

		/// <summary>L3 cache misses (require HBM4 fetch)</summary>
	public:
		long long getMisses() const;
	private:
		void setMisses(long long value);

	public:
		GlobalDistributedCache();

		/// <summary>
		/// Determine which NoC slice (Pod) is the home node for a given address.
		/// Uses address hashing to distribute lines evenly across slices.
		/// </summary>
		inline static int GetHomeSlice(unsigned long long address);

		/// <summary>
		/// Lookup an address in L3 cache.
		/// </summary>
		/// <param name="address">Physical address</param>
		/// <returns>True on L3 hit, false on L3 miss (needs HBM4 fetch)</returns>
		inline bool Lookup(unsigned long long address);

		/// <summary>
		/// Fill a line after HBM4 burst fetch completes.
		/// </summary>
		inline void Fill(unsigned long long address, unsigned long long domainTag);

		/// <summary>
		/// Invalidate a line.
		/// </summary>
		inline void Invalidate(unsigned long long address);

		/// <summary>
		/// Advance the internal cycle counter.
		/// </summary>
		void AdvanceCycle();

		/// <summary>
		/// Reset all cache state.
		/// </summary>
		void Reset();

	private:
		inline static int GetLineIndex(unsigned long long address);

		inline static unsigned long long GetTag(unsigned long long address);
	};
}
