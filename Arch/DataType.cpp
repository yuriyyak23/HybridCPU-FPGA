#include "DataType.h"
#include "../tangible_exception_helper.h"

using namespace HybridCPU_ISE::Arch;

namespace YAKSys_Hybrid_CPU::Arch
{

	int DataTypeUtils::SizeOf(DataTypeEnum dataType)
	{
		int temp_var;
		switch (dataType)
		{
			case DataTypeEnum::INT8:
				temp_var = 1;
				break;
			case DataTypeEnum::UINT8:
				temp_var = 1;
				break;
			case DataTypeEnum::FLOAT8_E4M3:
				temp_var = 1;
				break;
			case DataTypeEnum::FLOAT8_E5M2:
				temp_var = 1;
				break;
			case DataTypeEnum::INT16:
				temp_var = 2;
				break;
			case DataTypeEnum::UINT16:
				temp_var = 2;
				break;
			case DataTypeEnum::FLOAT16:
				temp_var = 2;
				break;
			case DataTypeEnum::BFLOAT16:
				temp_var = 2;
				break;
			case DataTypeEnum::INT32:
				temp_var = 4;
				break;
			case DataTypeEnum::UINT32:
				temp_var = 4;
				break;
			case DataTypeEnum::FLOAT32:
				temp_var = 4;
				break;
			case DataTypeEnum::INT64:
				temp_var = 8;
				break;
			case DataTypeEnum::UINT64:
				temp_var = 8;
				break;
			case DataTypeEnum::FLOAT64:
				temp_var = 8;
				break;
			default:
				throw ArgumentOutOfRangeException(std::format("Unknown data type: {} (dataType)", static_cast<unsigned int>(dataType)));
				break;
		}
		return temp_var;
	}

	bool DataTypeUtils::IsSignedInteger(DataTypeEnum dataType)
	{
		return dataType == DataTypeEnum::INT8 || dataType == DataTypeEnum::INT16 || dataType == DataTypeEnum::INT32 || dataType == DataTypeEnum::INT64;
	}

	bool DataTypeUtils::IsUnsignedInteger(DataTypeEnum dataType)
	{
		return dataType == DataTypeEnum::UINT8 || dataType == DataTypeEnum::UINT16 || dataType == DataTypeEnum::UINT32 || dataType == DataTypeEnum::UINT64;
	}

	bool DataTypeUtils::IsFloatingPoint(DataTypeEnum dataType)
	{
		return dataType == DataTypeEnum::FLOAT8_E4M3 || dataType == DataTypeEnum::FLOAT8_E5M2 || dataType == DataTypeEnum::FLOAT16 || dataType == DataTypeEnum::BFLOAT16 || dataType == DataTypeEnum::FLOAT32 || dataType == DataTypeEnum::FLOAT64;
	}

	bool DataTypeUtils::IsInteger(DataTypeEnum dataType)
	{
		return IsSignedInteger(dataType) || IsUnsignedInteger(dataType);
	}

	int DataTypeUtils::BitWidth(DataTypeEnum dataType)
	{
		return SizeOf(dataType) * 8;
	}

	bool DataTypeUtils::IsValid(DataTypeEnum dataType)
	{
		switch (dataType) {
        case DataTypeEnum::INT8: case DataTypeEnum::UINT8: case DataTypeEnum::INT16: case DataTypeEnum::UINT16:
        case DataTypeEnum::INT32: case DataTypeEnum::UINT32: case DataTypeEnum::FLOAT32: case DataTypeEnum::INT64:
        case DataTypeEnum::UINT64: case DataTypeEnum::FLOAT64: case DataTypeEnum::FLOAT16: case DataTypeEnum::BFLOAT16:
        case DataTypeEnum::FLOAT8_E4M3: case DataTypeEnum::FLOAT8_E5M2: return true;
        default: return false;
		}
	}
}
