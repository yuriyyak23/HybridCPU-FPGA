#include "OpcodeInfo.Types.h"

namespace YAKSys_Hybrid_CPU::Arch
{

	OpcodeInfo::OpcodeInfo(unsigned int opCode, const std::wstring &mnemonic, OpcodeCategory category, unsigned char operandCount, InstructionFlags flags, unsigned char executionLatency, unsigned char memoryBandwidth, const std::optional<YAKSys_Hybrid_CPU::Arch::InstructionClass> &instructionClass, const std::optional<YAKSys_Hybrid_CPU::Arch::SerializationClass> &serializationClass) : OpCode(opCode), Mnemonic(mnemonic), Category(category), OperandCount(operandCount), Flags(flags), ExecutionLatency(executionLatency), MemoryBandwidth(memoryBandwidth), InstructionClass(instructionClass.value_or(ResolveInstructionClass(category))), SerializationClass(serializationClass.value_or(ResolveSerializationClass(category, flags)))
	{
	}

	bool OpcodeInfo::isVector() const
	{
		return (static_cast<int>(Flags) & static_cast<int>(InstructionFlags::Vector)) != 0;
	}

	bool OpcodeInfo::getSupportsMasking() const
	{
		return (static_cast<int>(Flags) & static_cast<int>(InstructionFlags::Maskable)) != 0;
	}

	bool OpcodeInfo::getSupportsReduction() const
	{
		return (static_cast<int>(Flags) & static_cast<int>(InstructionFlags::Reduction)) != 0;
	}

	bool OpcodeInfo::getSupportsIndexed() const
	{
		return (static_cast<int>(Flags) & static_cast<int>(InstructionFlags::Indexed)) != 0;
	}

	bool OpcodeInfo::isControlFlow() const
	{
		return Category == OpcodeCategory::ControlFlow;
	}

	bool OpcodeInfo::isMathOrVector() const
	{
		return Category == OpcodeCategory::Scalar || Category == OpcodeCategory::Vector || Category == OpcodeCategory::Comparison || Category == OpcodeCategory::BitManip;
	}

	bool OpcodeInfo::getModifiesFlags() const
	{
		return (static_cast<int>(Flags) & static_cast<int>(InstructionFlags::ModifiesFlags)) != 0;
	}

	InstructionClass OpcodeInfo::ResolveInstructionClass(OpcodeCategory category)
	{
		switch (category) {
		case OpcodeCategory::Memory: return InstructionClass::Memory;
		case OpcodeCategory::ControlFlow: return InstructionClass::ControlFlow;
		case OpcodeCategory::System: case OpcodeCategory::Privileged: return InstructionClass::System;
		case OpcodeCategory::Atomic: return InstructionClass::Atomic;
		default: return InstructionClass::ScalarAlu;
		}
	}

	SerializationClass OpcodeInfo::ResolveSerializationClass(OpcodeCategory category, InstructionFlags flags)
	{
		if (category == OpcodeCategory::Atomic) return SerializationClass::AtomicSerial;
		if (category == OpcodeCategory::Memory && (static_cast<int>(flags) & static_cast<int>(InstructionFlags::MemoryWrite)) != 0) return SerializationClass::MemoryOrdered;
		if (category == OpcodeCategory::Privileged) return SerializationClass::FullSerial;
		return SerializationClass::Free;
	}
}
