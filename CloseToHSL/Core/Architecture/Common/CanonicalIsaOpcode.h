#pragma once
#include "InstructionOpcode.h"
#include <stdexcept>

namespace YAKSys_Hybrid_CPU
{
	// Canonical ISA opcode identity shared by the decoder and runtime without
	// requiring either component to include the full Processor definition.
	struct CanonicalIsaOpcode final
	{
		unsigned short Value = 0;
		explicit CanonicalIsaOpcode(unsigned short value = 0) : Value(value) {}
		static CanonicalIsaOpcode FromInstructionsEnum(InstructionsEnum opcode)
		{
			return CanonicalIsaOpcode(static_cast<unsigned short>(opcode));
		}
		static CanonicalIsaOpcode FromRawValue(unsigned int rawOpcode)
		{
			if (rawOpcode > 0xffffu) throw std::out_of_range("rawOpcode");
			return CanonicalIsaOpcode(static_cast<unsigned short>(rawOpcode));
		}
		InstructionsEnum ToInstructionsEnum() const { return static_cast<InstructionsEnum>(Value); }
	};
}
