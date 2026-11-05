#include "ReferenceModel.h"
#include "../ExecutionFaultContract.h"
#include "../../../../Arch/DataType.h"
#include "../Vector/Codec/ElementCodec.h"
#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <bit>
#include <span>
#include <limits>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Execution
{

	double ReferenceModel::BinaryFloat(unsigned int opCode, double a, double b)
{
	switch (opCode)
	{
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VADD): return a + b;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSUB): return a - b;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMUL): return a * b;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDIV): return b != 0.0 ? a / b : std::numeric_limits<double>::quiet_NaN();
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMIN): return std::min(a, b);
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMAX): return std::max(a, b);
	default: return ThrowUnknownReferenceOpcode<double>(L"BinaryFloat", opCode);
	}
}

	float ReferenceModel::FMAFloat32(unsigned int opCode, float d, float s1, float s2)
{
	switch (opCode)
	{
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VFMADD): return std::fma(d, s1, s2);
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VFMSUB): return std::fma(d, s1, -s2);
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VFNMADD): return std::fma(-d, s1, s2);
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VFNMSUB): return std::fma(-d, s1, -s2);
	default: return ThrowUnknownReferenceOpcode<float>(L"FMAFloat32", opCode);
	}
}

	double ReferenceModel::FMAFloat64(unsigned int opCode, double d, double s1, double s2)
{
	switch (opCode)
	{
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VFMADD): return std::fma(d, s1, s2);
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VFMSUB): return std::fma(d, s1, -s2);
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VFNMADD): return std::fma(-d, s1, s2);
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VFNMSUB): return std::fma(-d, s1, -s2);
	default: return ThrowUnknownReferenceOpcode<double>(L"FMAFloat64", opCode);
	}
}

	double ReferenceModel::UnaryFloat(unsigned int opCode, double a)
{
	switch (opCode)
	{
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSQRT): return std::sqrt(a);
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VNOT): return std::bit_cast<double>(~std::bit_cast<unsigned long long>(a));
	default: return ThrowUnknownReferenceOpcode<double>(L"UnaryFloat", opCode);
	}
}

	long long ReferenceModel::BinarySignedInt(unsigned int opCode, long long a, long long b)
{
	switch (opCode)
	{
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VADD): return static_cast<long long>(static_cast<unsigned long long>(a) + static_cast<unsigned long long>(b));
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSUB): return static_cast<long long>(static_cast<unsigned long long>(a) - static_cast<unsigned long long>(b));
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMUL): return static_cast<long long>(static_cast<unsigned long long>(a) * static_cast<unsigned long long>(b));
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDIV): return b == 0 ? 0 : (a == std::numeric_limits<long long>::min() && b == -1 ? a : a / b);
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMOD): return b == 0 || (a == std::numeric_limits<long long>::min() && b == -1) ? 0 : a % b;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VAND): return a & b;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VOR): return a | b;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VXOR): return a ^ b;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMIN): return std::min(a, b);
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMAX): return std::max(a, b);
	default: return ThrowUnknownReferenceOpcode<long long>(L"BinarySignedInt", opCode);
	}
}

	unsigned long long ReferenceModel::BinaryUnsignedInt(unsigned int opCode, unsigned long long a, unsigned long long b)
{
	switch (opCode)
	{
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VADD): return a + b;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSUB): return a - b;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMUL): return a * b;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDIV): return b != 0 ? a / b : 0;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMOD): return b != 0 ? a % b : 0;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VAND): return a & b;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VOR): return a | b;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VXOR): return a ^ b;
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMINU): return std::min(a, b);
	case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMAXU): return std::max(a, b);
	default: return ThrowUnknownReferenceOpcode<unsigned long long>(L"BinaryUnsignedInt", opCode);
	}
}

	bool ReferenceModel::AreCloseFloat32(float a, float b, float relativeTolerance)
	{
		// Handle exact equality (including infinity and zero)
		if (a == b)
		{
			return true;
		}

		// Handle NaN: both must be NaN
		if (std::isnan(a) && std::isnan(b))
		{
			return true;
		}
		if (std::isnan(a) || std::isnan(b))
		{
			return false;
		}

		// Relative difference check
		float diff = std::abs(a - b);
		float absA = std::abs(a);
		float absB = std::abs(b);
		float largest = std::max(absA, absB);

		return diff <= largest * relativeTolerance;
	}

	bool ReferenceModel::AreCloseFloat64(double a, double b, double relativeTolerance)
	{
		// Handle exact equality (including infinity and zero)
		if (a == b)
		{
			return true;
		}

		// Handle NaN: both must be NaN
		if (std::isnan(a) && std::isnan(b))
		{
			return true;
		}
		if (std::isnan(a) || std::isnan(b))
		{
			return false;
		}

		// Relative difference check
		double diff = std::abs(a - b);
		double absA = std::abs(a);
		double absB = std::abs(b);
		double largest = std::max(absA, absB);

		return diff <= largest * relativeTolerance;
	}

	bool ReferenceModel::AreCloseNarrowFloat(double a, double b, DataTypeEnum dataType)
	{
		// Tolerance based on mantissa precision of target format
		double temp_var56;
		switch (dataType)
		{
			case DataTypeEnum::FLOAT16: // 10 mantissa bits
				temp_var56 = 1e-3;
				break;
			case DataTypeEnum::BFLOAT16: // 7 mantissa bits
				temp_var56 = 8e-3;
				break;
			case DataTypeEnum::FLOAT8_E4M3: // 3 mantissa bits
				temp_var56 = 0.125;
				break;
			case DataTypeEnum::FLOAT8_E5M2: // 2 mantissa bits
				temp_var56 = 0.25;
				break;
			default:
				throw ExecutionFaultContract::CreateReferenceModelValidationException(L"dataType", dataType, std::format(L"AreCloseNarrowFloat not applicable to {}", static_cast<int>(dataType)));
		}
		double relativeTolerance = temp_var56;

		if (a == b)
		{
			return true;
		}
		if (std::isnan(a) && std::isnan(b))
		{
			return true;
		}
		if (std::isnan(a) || std::isnan(b))
		{
			return false;
		}

		double diff = std::abs(a - b);
		double largest = std::max(std::abs(a), std::abs(b));

		return diff <= largest * relativeTolerance;
	}

	bool ReferenceModel::ValidateResult(unsigned int opCode, DataTypeEnum dataType, std::span<const unsigned char> actual, std::span<const unsigned char> expected, int elementIndex)
	{
		int elemSize = Arch::DataTypeUtils::SizeOf(dataType);
		int offset = elementIndex * elemSize;

		if (Arch::DataTypeUtils::IsFloatingPoint(dataType))
		{
			if (dataType == DataTypeEnum::FLOAT32)
			{
				float actualVal = std::bit_cast<float>(static_cast<unsigned int>(actual[offset]) | (static_cast<unsigned int>(actual[offset + 1]) << 8) | (static_cast<unsigned int>(actual[offset + 2]) << 16) | (static_cast<unsigned int>(actual[offset + 3]) << 24));
				float expectedVal = std::bit_cast<float>(static_cast<unsigned int>(expected[offset]) | (static_cast<unsigned int>(expected[offset + 1]) << 8) | (static_cast<unsigned int>(expected[offset + 2]) << 16) | (static_cast<unsigned int>(expected[offset + 3]) << 24));
				return AreCloseFloat32(actualVal, expectedVal);
			}
			else if (dataType == DataTypeEnum::FLOAT64)
			{
				double actualVal = std::bit_cast<double>(static_cast<unsigned long long>(actual[offset]) | (static_cast<unsigned long long>(actual[offset + 1]) << 8) | (static_cast<unsigned long long>(actual[offset + 2]) << 16) | (static_cast<unsigned long long>(actual[offset + 3]) << 24) | (static_cast<unsigned long long>(actual[offset + 4]) << 32) | (static_cast<unsigned long long>(actual[offset + 5]) << 40) | (static_cast<unsigned long long>(actual[offset + 6]) << 48) | (static_cast<unsigned long long>(actual[offset + 7]) << 56));
				double expectedVal = std::bit_cast<double>(static_cast<unsigned long long>(expected[offset]) | (static_cast<unsigned long long>(expected[offset + 1]) << 8) | (static_cast<unsigned long long>(expected[offset + 2]) << 16) | (static_cast<unsigned long long>(expected[offset + 3]) << 24) | (static_cast<unsigned long long>(expected[offset + 4]) << 32) | (static_cast<unsigned long long>(expected[offset + 5]) << 40) | (static_cast<unsigned long long>(expected[offset + 6]) << 48) | (static_cast<unsigned long long>(expected[offset + 7]) << 56));
				return AreCloseFloat64(actualVal, expectedVal);
			}
			else
			{
				// Narrow FP types: FLOAT16, BFLOAT16, FLOAT8_E4M3, FLOAT8_E5M2
				// Decode raw bytes to double via ElementCodec, then compare with
				// format-appropriate tolerance.
				double actualVal = ElementCodec::LoadF(actual, offset, dataType);
				double expectedVal = ElementCodec::LoadF(expected, offset, dataType);
				return AreCloseNarrowFloat(actualVal, expectedVal, dataType);
			}
		}
		else
		{
			// For integers, expect exact match
			for (int i = 0; i < elemSize; i++)
			{
				if (actual[offset + i] != expected[offset + i])
				{
					return false;
				}
			}
		}

		return true;
	}
}


