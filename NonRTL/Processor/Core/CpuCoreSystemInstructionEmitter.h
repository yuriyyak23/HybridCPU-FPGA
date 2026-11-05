#pragma once

#include "../../Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"

using namespace HybridCPU_ISE::Arch;

namespace YAKSys_Hybrid_CPU
{
	class CpuCoreSystemInstructionEmitter final
	{
	public:
		static VLIW_Instruction EncodeNope();


	};
}
