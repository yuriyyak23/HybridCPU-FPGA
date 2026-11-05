#pragma once

#include <memory>
#include <array>
#include "../../Core/Runtime/Memory/Translation/NestedTlbTag.h"

using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::CloseToHSL::Memory::MMU
{
	/// <summary>
	/// HLS-compatible Translation Lookaside Buffer (fully-associative CAM).
	///
	/// <para>Caches VPN в†' PPN translations to avoid costly 2-level page walks
	/// on every memory access. In hardware, all 16 comparisons execute in
	/// parallel (Content-Addressable Memory).</para>
	///
	/// <para><b>HLS characteristics:</b></para>
	/// <list type="bullet">
	///   <item>16-entry fully-associative CAM: ~200 LUT (Xilinx UltraScale+).</item>
	///   <item>Single-cycle hit latency (<see cref="HIT_LATENCY_CYCLES"/> = 1).</item>
	///   <item>Miss penalty: <see cref="MISS_PENALTY_CYCLES"/> = 8 (2-level page walk).</item>
	///   <item>LRU replacement: 3-bit age counter per entry, updated on every access.</item>
	///   <item>Zero heap allocation вЂ" <see cref="InlineArray"/>-backed fixed storage.</item>
	///   <item><see cref="FlushDomain"/>: 16 AND-masks in 1 cycle.</item>
	/// </list>
	/// </summary>
	class TLB
	{
	private:
		unsigned long long Hits = 0;
		unsigned long long Misses = 0;

		/// <summary>Number of TLB entries (HLS: maps to 16-entry CAM).</summary>
		static constexpr int TLB_ENTRIES = 16;

		/// <summary>Miss penalty in cycles (2-level page walk through memory).</summary>
	public:
		static constexpr int MISS_PENALTY_CYCLES = 8;

		/// <summary>Hit latency (single-cycle CAM lookup in hardware).</summary>
		static constexpr int HIT_LATENCY_CYCLES = 1;

		/// <summary>TLB entry: VPN в†' PPN mapping with permissions and domain tag.</summary>
	public:
		class TlbEntry
		{
			/// <summary>Virtual Page Number (VA &gt;&gt; 12).</summary>
		public:
			unsigned long long VPN = 0;

			/// <summary>Physical Page Number (PA &gt;&gt; 12).</summary>
			unsigned long long PPN = 0;

			/// <summary>Permission bits (bit 1 = Read, bit 2 = Write).</summary>
			unsigned char Permissions = 0;

			unsigned char MemoryType = 0;

			/// <summary>Valid bit вЂ" entry is active.</summary>
			bool Valid = false;

			/// <summary>Domain/ASID tag for per-thread/device isolation.</summary>
			int DomainId = 0;

			bool IsNested = false;

			std::shared_ptr<NestedTlbTag> NestedTag;

			unsigned long long NestedGuestPPN = 0;

			/// <summary>LRU age counter for replacement (higher = older).</summary>
			unsigned char LruAge = 0;
		};

	private:
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [InlineArray(TLB_ENTRIES)] private struct TlbEntryArray
		using TlbEntryArray = std::array<TlbEntry, TLB_ENTRIES>;

	private:
		TlbEntryArray _entries;

		/// <summary>Number of TLB hits since last flush.</summary>
	public:
		unsigned long long getHits() const;
	private:
		void setHits(unsigned long long value);

		/// <summary>Number of TLB misses since last flush.</summary>
	public:
		unsigned long long getMisses() const;
	private:
		void setMisses(unsigned long long value);

		/// <summary>
		/// Lookup VPN in TLB. O(1) in hardware (parallel CAM compare).
		/// In software: linear scan of 16 entries (small enough to stay in L1).
		/// </summary>
		/// <param name="virtualAddress">Full virtual address.</param>
		/// <param name="domainId">Thread/ASID/device ID for isolation.</param>
		/// <param name="physicalAddress">Translated physical address on hit.</param>
		/// <param name="permissions">Cached permission bits on hit.</param>
		/// <returns>True on hit, false on miss (caller must do page walk).</returns>
	public:
		bool TryTranslate(unsigned long long virtualAddress, int domainId, unsigned long long &physicalAddress, unsigned char &permissions);

		/// <summary>
		/// Insert entry after page walk (replaces LRU victim or first invalid slot).
		/// Called by IOMMU on TLB miss after successful page walk.
		/// </summary>
		/// <param name="virtualAddress">Virtual address that was translated.</param>
		/// <param name="physicalAddress">Resulting physical address from page walk.</param>
		/// <param name="permissions">Permission bits (bit 1 = Read, bit 2 = Write).</param>
		/// <param name="domainId">Domain/ASID for this mapping.</param>
		void Insert(unsigned long long virtualAddress, unsigned long long physicalAddress, unsigned char permissions, int domainId);

		bool TryTranslateNested(unsigned long long guestVirtualAddress, std::shared_ptr<AddressSpaceId> addressSpace, unsigned long long &hostPhysicalAddress, unsigned long long &guestPhysicalAddress, unsigned char &permissions, unsigned char &memoryType, std::shared_ptr<NestedTlbTag> &tag);

		void InsertNested(unsigned long long guestVirtualAddress, unsigned long long guestPhysicalAddress, unsigned long long hostPhysicalAddress, unsigned char permissions, unsigned char memoryType, std::shared_ptr<AddressSpaceId> addressSpace);

		/// <summary>
		/// Invalidate all entries for a given domain (context switch / domain free).
		/// HLS: 16 parallel AND-masks, single cycle.
		/// </summary>
		/// <param name="domainId">Domain whose entries to invalidate.</param>
		void FlushDomain(int domainId);

		int FlushNestedAll();

		int FlushNestedBySecondStageRoot(unsigned long long secondStageRootIdentity);

		int FlushNestedByAddressSpaceTag(unsigned short addressSpaceTag);

		int FlushNestedByDomainTag(unsigned short domainTag);

		int FlushNestedSingleAddress(std::shared_ptr<AddressSpaceId> addressSpace, unsigned long long guestVirtualAddress);

		int CountNestedEntries() const;

		/// <summary>
		/// Full TLB flush (global page table change). Resets counters.
		/// </summary>
		void FlushAll();

	private:
		void UpdateLru(int hitIndex);

		int FindVictimIndex();
	};
}
