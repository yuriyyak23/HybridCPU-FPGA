#pragma once

#include "IDecoderFrontend.h"
#include "../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include <span>
#include <vector>
#include <memory>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	/// <summary>
	/// Residual adapter/test-support helper that projects a decoder frontend's canonical
	/// bundle contract into occupied-slot <see cref="InstructionIR"/> values.
	/// This helper intentionally lives outside <see cref="IDecoderFrontend"/>, so the
	/// runtime-relevant decoder contract stays centered on <c>DecodeInstructionBundle(...)</c>.
	/// </summary>
	class DecoderFrontendOccupiedInstructionProjection final
	{
	public:
		static std::vector<std::shared_ptr<InstructionIR>> DecodeOccupiedInstructions(std::shared_ptr<IDecoderFrontend> frontend, std::span<const VLIW_Instruction> bundle);
	};
}

