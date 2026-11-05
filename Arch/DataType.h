#pragma once

#include "DataTypeEnum.h"
#include <format>

using namespace HybridCPU_ISE::Arch;


namespace YAKSys_Hybrid_CPU::Arch
{
	/// <summary>
	/// Data type utilities for Hybrid CPU architecture.
	/// <see cref="DataTypeEnum"/> in this directory is the canonical source of truth;
	/// compat containers project into it but do not own it.
	/// Provides type classification and sizing for hardware-oriented code generation.
	/// Compatible with HLS synthesis (no dynamic allocations, pure functions).
	/// </summary>
	class DataTypeUtils final
	{
		/// <summary>
		/// Get element size in bytes for a given data type.
		/// HLS-friendly: inline, no branches that depend on runtime values.
		/// </summary>
	public:
		static int SizeOf(DataTypeEnum dataType);

		/// <summary>
		/// Check if data type is signed integer.
		/// </summary>
		static bool IsSignedInteger(DataTypeEnum dataType);

		/// <summary>
		/// Check if data type is unsigned integer.
		/// </summary>
		static bool IsUnsignedInteger(DataTypeEnum dataType);

		/// <summary>
		/// Check if data type is floating point.
		/// </summary>
		static bool IsFloatingPoint(DataTypeEnum dataType);

		/// <summary>
		/// Check if data type is any integer type (signed or unsigned).
		/// </summary>
		static bool IsInteger(DataTypeEnum dataType);

		/// <summary>
		/// Get the natural bit width of a data type (8, 16, 32, or 64).
		/// Used for shift operations and mask generation.
		/// </summary>
		static int BitWidth(DataTypeEnum dataType);

		/// <summary>
		/// Returns true if the data type is a recognized architecture-defined value.
		/// </summary>
		static bool IsValid(DataTypeEnum dataType);
	};
}
