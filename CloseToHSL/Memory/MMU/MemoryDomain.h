#pragma once

#include <limits>

namespace HybridCPU_ISE::CloseToHSL::Memory::MMU
{
	/// <summary>
	/// Атрибуты домена памяти (флаги доступа).
	/// Memory domain attributes for Singularity-style SIP isolation.
	/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum MemoryDomainFlags
	enum class MemoryDomainFlags
	{
		None = 0,
		Read = 1,
		Write = 2,
		ReadWrite = Read | Write, // Convenience combination
		Execute = 4,
		Shared = 8 // Can be accessed by multiple threads (with locks)
	};

	/// <summary>
	/// Описатель домена памяти потока.
	/// Memory domain descriptor for thread isolation (Singularity SIP-style).
	///
	/// Each hardware thread (0-15) has an isolated memory domain with:
	/// - Dedicated address range (BaseAddress + Size)
	/// - Access permissions (Read/Write/Execute)
	/// - Formal non-overlap guarantees
	///
	/// Design principles:
	/// - HLS-compatible: simple struct, no complex inheritance
	/// - Verifiable: Contains() and Overlaps() support formal proofs
	/// - Efficient: All operations are O(1) comparisons
	/// </summary>
	class MemoryDomain
	{
	public:
		int ThreadId = 0;
		unsigned long long BaseAddress = 0;
		unsigned long long Size = 0;
		MemoryDomainFlags Flags = static_cast<MemoryDomainFlags>(0);

		/// <summary>
		/// Check if an address range is fully contained within this domain.
		/// Used for access validation before memory operations.
		/// </summary>
		/// <param name="address">Starting address to check</param>
		/// <param name="length">Length of the range in bytes</param>
		/// <returns>True if [address, address+length) is fully within domain</returns>
		bool Contains(unsigned long long address, unsigned long long length);

		/// <summary>
		/// Check if an address range overlaps with this domain.
		/// Used for domain allocation to ensure non-overlapping domains.
		/// </summary>
		/// <param name="address">Starting address of range to check</param>
		/// <param name="length">Length of the range in bytes</param>
		/// <returns>True if ranges overlap, false if disjoint</returns>
		bool Overlaps(unsigned long long address, unsigned long long length);
	};
}
