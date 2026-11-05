#include "SafetyMaskBuilder.h"
#include "../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core
{

	SafetyMask128 SafetyMaskBuilder::BuildFromClass(InstructionClass instrClass, SerializationClass serialClass)
	{
		unsigned long long low = 1ULL << static_cast<int>(instrClass);
		unsigned long long high = 1ULL << static_cast<int>(serialClass);
		return SafetyMask128(low, high);
	}

	SafetyMask128 SafetyMaskBuilder::BuildFromOpcode(unsigned int opcode)
	{
		std::optional<OpcodeInfo> info = OpcodeRegistry::GetInfo(opcode);
		if (!info)
		{
			return SafetyMask128::Zero;
		}

		InstructionClass instrClass = ClassifyOpcodeInfo(info.value());
		SerializationClass serialClass = SerializationClassForInstruction(instrClass);
		return BuildFromClass(instrClass, serialClass);
	}

	SafetyMask128 SafetyMaskBuilder::BuildFromInternalOp(std::shared_ptr<InternalOp> op)
	{
		InstructionClass instrClass = InternalOpCategoryToInstructionClass(op->getCategory());
		SerializationClass serialClass = SerializationClassForInternalOp(op);
		return BuildFromClass(instrClass, serialClass);
	}

	InstructionClass SafetyMaskBuilder::ClassifyOpcodeInfo(OpcodeInfo info)
	{
		if ((static_cast<int>(info.Flags) & static_cast<int>(InstructionFlags::Privileged)) != 0)
		{
			return InstructionClass::System;
		}
		if ((static_cast<int>(info.Flags) & (static_cast<int>(InstructionFlags::MemoryRead) | static_cast<int>(InstructionFlags::MemoryWrite))) != 0 &&
			(static_cast<int>(info.Flags) & static_cast<int>(InstructionFlags::Atomic)) != 0)
		{
			return InstructionClass::Atomic;
		}
		if ((static_cast<int>(info.Flags) & (static_cast<int>(InstructionFlags::MemoryRead) | static_cast<int>(InstructionFlags::MemoryWrite))) != 0)
		{
			return InstructionClass::Memory;
		}
		if (info.isControlFlow())
		{
			return InstructionClass::ControlFlow;
		}
		if (info.isVector())
		{
			return InstructionClass::ScalarAlu; // vectors share the ALU slot class
		}
		return InstructionClass::ScalarAlu;
	}

	InstructionClass SafetyMaskBuilder::InternalOpCategoryToInstructionClass(InternalOpCategory category)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return category switch
	//			{
	//				InternalOpCategory.Computation  => InstructionClass.ScalarAlu,
	//				InternalOpCategory.MemoryAccess => InstructionClass.Memory,
	//				InternalOpCategory.ControlFlow  => InstructionClass.ControlFlow,
	//				InternalOpCategory.Atomic       => InstructionClass.Atomic,
	//				InternalOpCategory.Csr          => InstructionClass.Csr,
	//				InternalOpCategory.SysEvent     => InstructionClass.SmtVt,
	//				InternalOpCategory.VmxEvent     => InstructionClass.Vmx,
	//				_                               => InstructionClass.ScalarAlu,
	//			};
		switch (category)
		{
		case InternalOpCategory::Computation: return InstructionClass::ScalarAlu;
		case InternalOpCategory::MemoryAccess: return InstructionClass::Memory;
		case InternalOpCategory::ControlFlow: return InstructionClass::ControlFlow;
		case InternalOpCategory::Atomic: return InstructionClass::Atomic;
		case InternalOpCategory::Csr: return InstructionClass::Csr;
		case InternalOpCategory::SysEvent: return InstructionClass::SmtVt;
		case InternalOpCategory::VmxEvent: return InstructionClass::Vmx;
		default: return InstructionClass::ScalarAlu;
		}
	}

	SerializationClass SafetyMaskBuilder::SerializationClassForInstruction(InstructionClass instrClass)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return instrClass switch
	//			{
	//				InstructionClass.ScalarAlu   => SerializationClass.Free,
	//				InstructionClass.Memory      => SerializationClass.MemoryOrdered,
	//				InstructionClass.ControlFlow => SerializationClass.Free,
	//				InstructionClass.Atomic      => SerializationClass.AtomicSerial,
	//				InstructionClass.System      => SerializationClass.FullSerial,
	//				InstructionClass.Csr         => SerializationClass.CsrOrdered,
	//				InstructionClass.SmtVt       => SerializationClass.FullSerial,
	//				InstructionClass.Vmx         => SerializationClass.VmxSerial,
	//				_                            => SerializationClass.Free,
	//			};
		switch (instrClass)
		{
		case InstructionClass::ScalarAlu: return SerializationClass::Free;
		case InstructionClass::Memory: return SerializationClass::MemoryOrdered;
		case InstructionClass::ControlFlow: return SerializationClass::Free;
		case InstructionClass::Atomic: return SerializationClass::AtomicSerial;
		case InstructionClass::System: return SerializationClass::FullSerial;
		case InstructionClass::Csr: return SerializationClass::CsrOrdered;
		case InstructionClass::SmtVt: return SerializationClass::FullSerial;
		case InstructionClass::Vmx: return SerializationClass::VmxSerial;
		default: return SerializationClass::Free;
		}
	}

	SerializationClass SafetyMaskBuilder::SerializationClassForInternalOp(std::shared_ptr<InternalOp> op)
	{
		// Atomics with aq/rl flags need full serial ordering
		if (op->getCategory() == InternalOpCategory::Atomic && op->hasOrdering())
		{
			return SerializationClass::AtomicSerial;
		}

		return SerializationClassForInstruction(InternalOpCategoryToInstructionClass(op->getCategory()));
	}
}

