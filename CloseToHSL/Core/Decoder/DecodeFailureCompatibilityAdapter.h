#pragma once

#include "../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../../../NonRTL/Arch/InvalidOpcodeException.h"
#include <string>
#include <memory>
#include <span>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	struct DecodeFailure;

	/// <summary>
	/// Retained public exception contract for RF-05. This adapter is intentionally
	/// narrow: it converts only expected typed decode rejections. Programming and
	/// invariant exceptions pass through untouched.
	/// </summary>
	class DecodeFailureCompatibilityAdapter final
	{
	public:
		static std::shared_ptr<InvalidOpcodeException> ToInvalidOpcodeException(std::shared_ptr<DecodeFailure> failure, std::span<const VLIW_Instruction> rawSlots);

		static std::shared_ptr<InvalidOpcodeException> ToInvalidOpcodeException(std::shared_ptr<DecodeFailure> failure, const VLIW_Instruction &rawSlot);

	private:
		static std::shared_ptr<InvalidOpcodeException> Create(std::shared_ptr<DecodeFailure> failure, const std::wstring &opcodeIdentifier, unsigned short opcode);
	};

}
