#include "VLIW_Bundle.h"
#include "tangible_exception_helper.h"
#include <stdexcept>

namespace HybridCPU_ISE::Arch
{

	VLIW_Instruction VLIW_Bundle::GetInstruction(int index)
	{
		switch (index)
		{
			case 0:
				return inst0;
			case 1:
				return inst1;
			case 2:
				return inst2;
			case 3:
				return inst3;
			case 4:
				return inst4;
			case 5:
				return inst5;
			case 6:
				return inst6;
			case 7:
				return inst7;
			default:
				throw ArgumentOutOfRangeException(L"Bundle index must be 0-7 (index)");
		}
	}

	void VLIW_Bundle::SetInstruction(int index, VLIW_Instruction instruction)
	{
		instruction.ValidateRetiredPolicyGapBitForProductionIngress();
		SetInstructionUnchecked(index, instruction);
	}

	void VLIW_Bundle::SetInstructionUnchecked(int index, VLIW_Instruction instruction)
	{
		switch (index)
		{
			case 0:
				inst0 = instruction;
				break;
			case 1:
				inst1 = instruction;
				break;
			case 2:
				inst2 = instruction;
				break;
			case 3:
				inst3 = instruction;
				break;
			case 4:
				inst4 = instruction;
				break;
			case 5:
				inst5 = instruction;
				break;
			case 6:
				inst6 = instruction;
				break;
			case 7:
				inst7 = instruction;
				break;
			default:
				throw ArgumentOutOfRangeException(L"Bundle index must be 0-7 (index)");
		}
	}

	bool VLIW_Bundle::TryWriteBytes(std::span<unsigned char> destination)
	{
		if (destination.size() < 256)
		{
			return false;
		}

		inst0.TryWriteBytes(destination.subspan(0, 32));
		inst1.TryWriteBytes(destination.subspan(32, 32));
		inst2.TryWriteBytes(destination.subspan(64, 32));
		inst3.TryWriteBytes(destination.subspan(96, 32));
		inst4.TryWriteBytes(destination.subspan(128, 32));
		inst5.TryWriteBytes(destination.subspan(160, 32));
		inst6.TryWriteBytes(destination.subspan(192, 32));
		inst7.TryWriteBytes(destination.subspan(224, 32));
		return true;
	}

	bool VLIW_Bundle::TryReadBytes(std::span<const unsigned char> source, int offset)
	{
		if (offset < 0) throw std::out_of_range("offset");
		const auto start = static_cast<std::size_t>(offset);
		if (start > source.size() || source.size() - start < 256)
		{
			return false;
		}

		inst0.TryReadBytes(source, offset);
		inst1.TryReadBytes(source, offset + 32);
		inst2.TryReadBytes(source, offset + 64);
		inst3.TryReadBytes(source, offset + 96);
		inst4.TryReadBytes(source, offset + 128);
		inst5.TryReadBytes(source, offset + 160);
		inst6.TryReadBytes(source, offset + 192);
		inst7.TryReadBytes(source, offset + 224);

		return true;
	}
}
