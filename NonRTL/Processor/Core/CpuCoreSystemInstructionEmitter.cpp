#include "CpuCoreSystemInstructionEmitter.h"
#include "../../../CloseToHSL/Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU
{

	VLIW_Instruction CpuCoreSystemInstructionEmitter::EncodeNope()
	{
		VLIW_Instruction temp_var = VLIW_Instruction();
		temp_var.setOpCode(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::Nope));
		return temp_var;
	}
}
