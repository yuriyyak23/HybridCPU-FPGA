#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.Extension.SextHInstruction.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::Extension
{

	unsigned short SextHInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::SEXT_H);
	}

	bool SextHInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool SextHInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long SextHInstruction::Execute(unsigned long long source)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)(long)(short)(ushort)source);
		return static_cast<unsigned long long>(static_cast<long long>(static_cast<short>(static_cast<unsigned short>(source))));
	}
}
