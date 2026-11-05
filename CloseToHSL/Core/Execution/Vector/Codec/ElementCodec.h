#pragma once
#include <span>

#include "../../../../../Arch/DataTypeEnum.h"
#include <format>

using namespace HybridCPU_ISE::Arch;


namespace YAKSys_Hybrid_CPU::Execution
{
	/// <summary>
	/// Unified codec for loading and storing elements of all data types.
	/// Handles endianness, sign-extension, and floating-point bit patterns.
	///
	/// Design goals:
	/// - Single point of truth for element encoding/decoding
	/// - HLS-friendly: no dynamic allocation, inline methods
	/// - Testable: clear contracts for all data type conversions
	/// - RVV-compatible: supports all element widths (8, 16, 32, 64 bits)
	/// </summary>
	class ElementCodec final
	{
		/// <summary>
		/// Get element size in bytes from DataType enum.
		/// Delegates to Arch.DataTypeUtils for consistency.
		/// </summary>
	public:
		static int SizeOf(DataTypeEnum dt);

		/// <summary>
		/// Load a signed integer element from memory.
		/// Sign-extends to 64-bit for uniform processing.
		/// Little-endian byte order.
		/// </summary>
		static long long LoadI(std::span<const unsigned char> mem, int off, DataTypeEnum dt);

		/// <summary>
		/// Load an unsigned integer element from memory.
		/// Zero-extends to 64-bit for uniform processing.
		/// Little-endian byte order.
		/// </summary>
		static unsigned long long LoadU(std::span<const unsigned char> mem, int off, DataTypeEnum dt);

		/// <summary>
		/// Load a floating-point element from memory.
		/// Converts to double (64-bit) for uniform processing.
		/// Little-endian byte order.
		/// </summary>
		static double LoadF(std::span<const unsigned char> mem, int off, DataTypeEnum dt);

		/// <summary>
		/// Store a signed integer element to memory.
		/// Truncates from 64-bit to target width.
		/// Little-endian byte order.
		/// </summary>
		static void StoreI(std::span<unsigned char> mem, int off, DataTypeEnum dt, long long value);

		/// <summary>
		/// Store an unsigned integer element to memory.
		/// Truncates from 64-bit to target width.
		/// Little-endian byte order.
		/// </summary>
		static void StoreU(std::span<unsigned char> mem, int off, DataTypeEnum dt, unsigned long long value);

		/// <summary>
		/// Store a floating-point element to memory.
		/// Converts from double (64-bit) and writes appropriate width.
		/// Little-endian byte order.
		/// </summary>
		static void StoreF(std::span<unsigned char> mem, int off, DataTypeEnum dt, double value);

		/// <summary>
		/// Load element as raw bits (ulong), regardless of type.
		/// Useful for bitwise operations that work uniformly across types.
		/// Zero-pads to 64-bit for sub-64-bit types.
		/// </summary>
		static unsigned long long LoadRaw(std::span<const unsigned char> mem, int off, DataTypeEnum dt);

		/// <summary>
		/// Store element as raw bits (ulong), regardless of type.
		/// Truncates to appropriate width.
		/// </summary>
		static void StoreRaw(std::span<unsigned char> mem, int off, DataTypeEnum dt, unsigned long long value);

		// ========================================================================
		// FP16/BF16 Conversion Helpers
		// ========================================================================

		/// <summary>
		/// Convert IEEE 754 FP16 (half precision) to float.
		/// FP16 format: 1 sign bit, 5 exponent bits, 10 mantissa bits.
		/// </summary>
	private:
		static float ConvertFP16ToFloat(unsigned short fp16);

		/// <summary>
		/// Convert float to IEEE 754 FP16 (half precision).
		/// Rounds to nearest even, handles overflow/underflow.
		/// </summary>
		static unsigned short ConvertFloatToFP16(double value);

		/// <summary>
		/// Convert BFloat16 to float.
		/// BF16 format: 1 sign bit, 8 exponent bits, 7 mantissa bits (truncated FP32).
		/// </summary>
		static float ConvertBF16ToFloat(unsigned short bf16);

		/// <summary>
		/// Convert float to BFloat16.
		/// Simply truncates FP32 mantissa (takes upper 16 bits).
		/// </summary>
		static unsigned short ConvertFloatToBF16(double value);

		// ========================================================================
		// FP8 (NVFP8) Conversion Helpers
		// ========================================================================

		/// <summary>
		/// Encode IEEE 754 Float64/Float32 to NVIDIA E4M3 (8-bit float).
		/// E4M3 format: 1 sign bit, 4 exponent bits, 3 mantissa bits. Bias = 7.
		/// Flush-to-zero for underflow; saturate to max finite for overflow.
		/// NaN encoding: exp=15, mantissa=7. No infinity in E4M3.
		/// </summary>
	public:
		static unsigned char EncodeE4M3(double value);

		/// <summary>
		/// Encode IEEE 754 Float64/Float32 to NVIDIA E5M2 (8-bit float).
		/// E5M2 format: 1 sign bit, 5 exponent bits, 2 mantissa bits. Bias = 15.
		/// Flush-to-zero for underflow; overflow maps to infinity.
		/// Infinity encoding: exp=31, mantissa=0. NaN: exp=31, mantissa!=0.
		/// </summary>
		static unsigned char EncodeE5M2(double value);

		/// <summary>
		/// Decode NVIDIA E4M3 (8-bit float) to IEEE 754 Float32.
		/// E4M3 format: 1 sign bit, 4 exponent bits, 3 mantissa bits.
		/// Hardware decoder for HLS synthesis (no branches).
		/// Implements flush-to-zero for denormals.
		/// </summary>
		static float DecodeE4M3(unsigned char value);

		/// <summary>
		/// Decode NVIDIA E5M2 (8-bit float) to IEEE 754 Float32.
		/// E5M2 format: 1 sign bit, 5 exponent bits, 2 mantissa bits.
		/// Hardware decoder for HLS synthesis (no branches).
		/// Implements flush-to-zero for denormals.
		/// </summary>
		static float DecodeE5M2(unsigned char value);
	};
}

