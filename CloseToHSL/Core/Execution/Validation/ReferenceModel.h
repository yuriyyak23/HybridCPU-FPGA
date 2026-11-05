#pragma once

#include "../../../../NonRTL/Arch/InvalidOpcodeException.h"
#include "../../../../Arch/DataTypeEnum.h"
#include <string>
#include <cmath>
#include <format>
#include <span>

using namespace HybridCPU_ISE::Arch;
using YAKSys_Hybrid_CPU::Arch::InvalidOpcodeException;


namespace YAKSys_Hybrid_CPU::Execution
{
	/// <summary>
	/// ReferenceModel: Simple, unoptimized implementation of vector operations.
	/// Used for self-checking and validation of the optimized VectorALU.
	///
	/// Design principles:
	/// - Simplicity over performance
	/// - Readability over optimization
	/// - Correctness is paramount
	/// - No SIMD, no intrinsics, no tricks
	/// - Uses native float/double operations
	/// - One element at a time (no vectorization)
	///
	/// Usage:
	/// - Enable self-check mode in Processor
	/// - Compare VectorALU results with ReferenceModel
	/// - Report discrepancies for debugging
	///
	/// Note: This model is intentionally slow and should only be used
	/// in debug/test builds for validation purposes.
	/// </summary>
	class ReferenceModel final
	{
		/// <summary>
		/// Reference implementation of binary floating-point operations.
		/// Uses straightforward float/double arithmetic for validation.
		/// </summary>
	public:
		static double BinaryFloat(unsigned int opCode, double a, double b);

		/// <summary>
		/// Reference implementation of FMA for 32-bit floats.
		/// Uses MathF.FusedMultiplyAdd for single-precision validation.
		/// </summary>
		static float FMAFloat32(unsigned int opCode, float d, float s1, float s2);

		/// <summary>
		/// Reference implementation of FMA for 64-bit doubles.
		/// Uses Math.FusedMultiplyAdd for double-precision validation.
		/// </summary>
		static double FMAFloat64(unsigned int opCode, double d, double s1, double s2);

		/// <summary>
		/// Reference implementation of unary floating-point operations.
		/// </summary>
		static double UnaryFloat(unsigned int opCode, double a);

		/// <summary>
		/// Reference implementation of binary integer operations.
		/// Supports both signed and unsigned arithmetic.
		/// </summary>
		static long long BinarySignedInt(unsigned int opCode, long long a, long long b);

		/// <summary>
		/// Reference implementation of binary unsigned integer operations.
		/// </summary>
		static unsigned long long BinaryUnsignedInt(unsigned int opCode, unsigned long long a, unsigned long long b);

	private:
		template<typename T>
		static T ThrowUnknownReferenceOpcode(const std::wstring &operationClass, unsigned int opCode)
		{
			std::wstring opcodeIdentifier = std::format(L"0x{:X}", opCode);
			throw InvalidOpcodeException(std::format(L"ReferenceModel {} rejected unsupported opcode {}. Reference validation must fail closed instead of projecting a neutral value.", operationClass, opcodeIdentifier), opcodeIdentifier, -1, false);
		}

		/// <summary>
		/// Compare two floating-point values with tolerance.
		/// Returns true if values are "close enough" considering floating-point precision.
		/// </summary>
	public:
		static bool AreCloseFloat32(float a, float b, float relativeTolerance = 1e-5f);

		/// <summary>
		/// Compare two double-precision values with tolerance.
		/// </summary>
		static bool AreCloseFloat64(double a, double b, double relativeTolerance = 1e-10);

		/// <summary>
		/// Compare two narrow floating-point values decoded to double.
		/// Used for FLOAT16, BFLOAT16, FLOAT8_E4M3, FLOAT8_E5M2 validation.
		/// Tolerance is derived from the mantissa bit width of the target format.
		/// </summary>
		static bool AreCloseNarrowFloat(double a, double b, DataTypeEnum dataType);

		/// <summary>
		/// Validate VectorALU result against ReferenceModel.
		/// Used for self-checking in debug mode.
		/// </summary>
		/// <param name="opCode">Operation code</param>
		/// <param name="dataType">Element data type</param>
		/// <param name="actual">Actual result from VectorALU</param>
		/// <param name="expected">Expected result from ReferenceModel</param>
		/// <param name="elementIndex">Index of element being validated</param>
		/// <returns>True if results match within tolerance</returns>
		static bool ValidateResult(unsigned int opCode, DataTypeEnum dataType, std::span<const unsigned char> actual, std::span<const unsigned char> expected, int elementIndex);
	};
}



