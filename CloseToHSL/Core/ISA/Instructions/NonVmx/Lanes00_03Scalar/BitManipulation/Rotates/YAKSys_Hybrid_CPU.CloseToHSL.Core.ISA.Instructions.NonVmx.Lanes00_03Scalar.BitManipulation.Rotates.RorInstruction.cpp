#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.Rotates.RorInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::Rotates
{

	unsigned short RorInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::ROR);
	}

	bool RorInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool RorInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long RorInstruction::Execute(unsigned long long source, unsigned long long shiftSource)
	{
		int amount = static_cast<int>(shiftSource & ShiftMask);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return amount == 0 ? source : unchecked((source >> amount) | (source << (XLen - amount)));
		return amount == 0 ? source : (source >> amount) | (source << (XLen - amount));
	}
}
