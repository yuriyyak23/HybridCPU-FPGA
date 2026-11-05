#pragma once

#include <string>
#include <any>
#include <format>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// 128-bit safety mask for extended resource tracking (Phase: Safety Tags & Certificates).
	///
	/// Purpose: Support larger resource spaces as the system scales beyond 64 resources.
	/// - Low 64 bits: Register groups, memory domains, LSU classes
	/// - High 64 bits: GRLB channels (DMA, Stream, Accelerators), future extensions
	///
	/// Design goals:
	/// - Hardware-agnostic: suitable for HLS synthesis
	/// - Efficient: inline operations, minimal overhead
	/// - Scalable: supports up to 128 distinct resource types
	/// </summary>
	class SafetyMask128
	{
		/// <summary>
		/// Low 64 bits (0-63):
		/// - Bits 0-15:  Register read groups (16 groups of 4 registers each)
		/// - Bits 16-31: Register write groups (16 groups of 4 registers each)
		/// - Bits 32-47: Memory domain IDs (16 possible domains)
		/// - Bits 48:    Load operation (LSU read channel)
		/// - Bits 49:    Store operation (LSU write channel)
		/// - Bits 50:    Atomic operation (LSU atomic channel)
		/// - Bits 51-54: DMA channels (4 channels)
		/// - Bits 55-58: Stream engines (4 engines)
		/// - Bits 59-62: Custom accelerators (4 accelerators)
		/// - Bit 63:     Reserved
		/// </summary>
	public:
		unsigned long long Low = 0;

		/// <summary>
		/// High 64 bits (64-127):
		/// - Bits 64-95:  Extended GRLB channels (32 channels for future hardware)
		/// - Bits 96-111: Extended memory domains (16 additional domains)
		/// - Bits 112-127: Reserved for future resource types
		/// </summary>
		unsigned long long High = 0;

		/// <summary>
		/// Create a 128-bit safety mask from low and high components
		/// </summary>
		SafetyMask128(unsigned long long low, unsigned long long high);

		/// <summary>
		/// Create a 128-bit safety mask from a 64-bit mask (for backward compatibility)
		/// </summary>
		SafetyMask128(unsigned long long mask);

		/// <summary>
		/// Check if the mask is zero (uninitialized)
		/// </summary>
		bool isZero() const;

		/// <summary>
		/// Check if the mask is non-zero (properly initialized)
		/// </summary>
		bool isNonZero() const;

		/// <summary>
		/// Bitwise OR operator for combining masks
		/// </summary>
		SafetyMask128 operator | (SafetyMask128 b) const;

		/// <summary>
		/// Bitwise AND operator for checking conflicts
		/// </summary>
		SafetyMask128 operator &(SafetyMask128 b) const;

		/// <summary>
		/// Equality operator
		/// </summary>
		bool operator == (SafetyMask128 b) const;

		/// <summary>
		/// Inequality operator
		/// </summary>
		bool operator != (SafetyMask128 b) const;

		/// <summary>
		/// Implicit conversion from ulong (backward compatibility)
		/// </summary>

		/// <summary>
		/// Check if this mask conflicts with another mask
		/// Returns true if any bits overlap (conflict exists), accounting for Read/Write hazards
		/// </summary>
		bool ConflictsWith(SafetyMask128 other) const;

		/// <summary>
		/// Check if this mask has no conflicts with another mask
		/// Returns true if no bits overlap (safe to execute in parallel)
		/// </summary>
		bool NoConflictsWith(SafetyMask128 other) const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
		bool Equals(std::any obj) const;

		bool Equals(SafetyMask128 other) const;

		int GetHashCode() const;

		std::wstring ToString() const;

		/// <summary>
		/// Zero mask (no resources)
		/// </summary>
		static const SafetyMask128 Zero;

		/// <summary>
		/// All ones mask (all resources)
		/// </summary>
		static const SafetyMask128 All;

		SafetyMask128() = default;
	};
inline const SafetyMask128 SafetyMask128::Zero = SafetyMask128(0, 0);
inline const SafetyMask128 SafetyMask128::All = SafetyMask128(~0ULL, ~0ULL);
}
