#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.Extension.SextBInstruction.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::Extension
{

	unsigned short SextBInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::SEXT_B);
	}

	bool SextBInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool SextBInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long SextBInstruction::Execute(unsigned long long source)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)(long)(sbyte)(byte)source);
		return static_cast<unsigned long long>(static_cast<long long>(static_cast<signed char>(static_cast<unsigned char>(source))));
	}
}
