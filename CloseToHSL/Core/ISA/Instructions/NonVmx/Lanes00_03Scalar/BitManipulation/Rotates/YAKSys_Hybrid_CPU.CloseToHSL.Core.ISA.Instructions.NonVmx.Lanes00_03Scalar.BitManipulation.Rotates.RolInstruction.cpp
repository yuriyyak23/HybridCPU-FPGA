#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.Rotates.RolInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::Rotates
{

	unsigned short RolInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::ROL);
	}

	bool RolInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool RolInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long RolInstruction::Execute(unsigned long long source, unsigned long long shiftSource)
	{
		int amount = static_cast<int>(shiftSource & ShiftMask);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return amount == 0 ? source : unchecked((source << amount) | (source >> (XLen - amount)));
		return amount == 0 ? source : (source << amount) | (source >> (XLen - amount));
	}
}
