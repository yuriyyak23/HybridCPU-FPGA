#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.Extension.ZextHInstruction.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::Extension
{

	unsigned short ZextHInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::ZEXT_H);
	}

	bool ZextHInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool ZextHInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long ZextHInstruction::Execute(unsigned long long source)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ushort)source);
		return static_cast<unsigned short>(source);
	}
}
