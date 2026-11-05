#pragma once

#include <string>
#include <any>
#include <format>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// 128-bit resource bitset for Global Resource Lock Bitset (GRLB) - Phase 8 Extended.
	///
	/// Purpose: Support larger resource spaces as the system scales beyond 64 resources.
	/// - Low 64 bits (0-63): Register groups, memory domains, LSU channels, DMA channels
	/// - High 64 bits (64-127): Stream engines, accelerators, extended resources
	///
	/// Design goals:
	/// - Hardware-agnostic: suitable for HLS synthesis (maps to two 64-bit registers)
	/// - Efficient: inline operations, minimal overhead, single-cycle logic in hardware
	/// - Scalable: supports up to 128 distinct resource types
	/// - ABA-safe: used with token tracking to prevent resource re-acquisition bugs
	///
	/// Bit layout (128 bits total):
	/// Low 64 bits (0-63):
	/// - Bits 0-15:  Register read groups (16 groups of 4 registers each)
	/// - Bits 16-31: Register write groups (16 groups of 4 registers each)
	/// - Bits 32-47: Memory domain IDs (16 possible domains)
	/// - Bit 48:     Load operation (LSU read channel)
	/// - Bit 49:     Store operation (LSU write channel)
	/// - Bit 50:     Atomic operation (LSU atomic channel)
	/// - Bits 51-54: DMA channels 0-3 (4 channels)
	/// - Bits 55-58: Stream engines 0-3 (4 engines)
	/// - Bits 59-62: Custom accelerators 0-3 (4 accelerators)
	/// - Bit 63:     Reserved
	///
	/// High 64 bits (64-127):
	/// - Bits 64-67: DMA channels 4-7 (4 additional channels)
	/// - Bits 68-71: Stream engines 4-7 (4 additional engines)
	/// - Bits 72-75: Custom accelerators 4-7 (4 additional accelerators)
	/// - Bits 76-79: Additional LSU channels
	/// - Bits 80-95: Extended memory domains (16 additional domains)
	/// - Bits 96-127: Reserved for future resource types
	/// </summary>
	class ResourceBitset
	{
		/// <summary>
		/// Low 64 bits (resources 0-63)
		/// Registers, LSU channels, basic DMA/Stream/Accel
		/// </summary>
	public:
		unsigned long long Low = 0;

		/// <summary>
		/// High 64 bits (resources 64-127)
		/// Extended DMA, StreamEngine, Accelerators, future resources
		/// </summary>
		unsigned long long High = 0;

		/// <summary>
		/// Create a 128-bit resource bitset from low and high components
		/// </summary>
		ResourceBitset(unsigned long long low, unsigned long long high);

		/// <summary>
		/// Create a 128-bit resource bitset from a 64-bit mask (for backward compatibility)
		/// </summary>
		ResourceBitset(unsigned long long mask);

		/// <summary>
		/// Check if the bitset is zero (no resources)
		/// </summary>
		bool isZero() const;

		/// <summary>
		/// Check if the bitset is non-zero (has resources)
		/// </summary>
		bool isNonZero() const;

		/// <summary>
		/// Bitwise OR operator for combining bitsets
		/// Hardware: Two parallel OR gates (single cycle)
		/// </summary>
		ResourceBitset operator | (ResourceBitset b);

		/// <summary>
		/// Bitwise AND operator for checking conflicts
		/// Hardware: Two parallel AND gates (single cycle)
		/// </summary>
		ResourceBitset operator &(ResourceBitset b);

		/// <summary>
		/// Bitwise NOT operator for clearing resources
		/// Hardware: Two parallel NOT gates (single cycle)
		/// </summary>
		ResourceBitset operator ~();

		/// <summary>
		/// Equality operator
		/// </summary>
		bool operator == (ResourceBitset b);

		/// <summary>
		/// Inequality operator
		/// </summary>
		bool operator != (ResourceBitset b);

		/// <summary>
		/// Implicit conversion from ulong (backward compatibility)
		/// Allows: ResourceBitset mask = 0x1UL;
		/// </summary>

		/// <summary>
		/// Explicit conversion to ulong (backward compatibility for tests)
		/// Converts to Low 64 bits, discarding High bits.
		/// Use only for legacy code that expects 64-bit masks.
		/// </summary>
		explicit operator unsigned long long();

		/// <summary>
		/// Check if this bitset conflicts with another bitset
		/// Returns true if any structural hazards or data hazards (RAW/WAR/WAW) exist.
		/// Resolves the RAR safety: pure read-read overlaps do not trigger a conflict.
		/// </summary>
		bool ConflictsWith(ResourceBitset other) const;

		/// <summary>
		/// Check if this bitset has no conflicts with another bitset
		/// </summary>
		bool NoConflictsWith(ResourceBitset other) const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override readonly bool Equals(Object? obj)
		bool Equals(std::any obj) const;

		bool Equals(ResourceBitset other) const ;

		int GetHashCode() const;

		std::wstring ToString() const;

		// ===== GRLB Bank Partitioning (Plan 06) =====

		/// <summary>Number of GRLB banks.</summary>
		static constexpr int GRLB_BANK_COUNT = 4;

		/// <summary>
		/// Extract a 32-bit bank slice from this 128-bit bitset.
		/// Bank 0: bits [0..31], Bank 1: bits [32..63],
		/// Bank 2: bits [64..95], Bank 3: bits [96..127].
		/// HLS: pure wire select, zero combinational logic.
		/// </summary>
		unsigned int GetBank(int bankIndex) const;

		/// <summary>
		/// Return a bitmask of banks touched by this bitset (bits 0–3).
		/// HLS: 4 OR-reduce gates, single-cycle.
		/// </summary>
		unsigned char ActiveBanks() const;

		/// <summary>
		/// Zero bitset (no resources)
		/// </summary>
		static const ResourceBitset Zero;

		/// <summary>
		/// All ones bitset (all resources)
		/// </summary>
		static const ResourceBitset All;

		ResourceBitset() = default;
	};
inline const ResourceBitset ResourceBitset::Zero = ResourceBitset(0, 0);
inline const ResourceBitset ResourceBitset::All = ResourceBitset(~0ULL, ~0ULL);
}
