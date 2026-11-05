#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.BitCount.CtzInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <bit>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitCount
{

	unsigned short CtzInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::CTZ);
	}

	bool CtzInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool CtzInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long CtzInstruction::Execute(unsigned long long source)
	{
		return source == 0 ? 64ULL : static_cast<unsigned long long>(std::countr_zero(source));
	}
}
