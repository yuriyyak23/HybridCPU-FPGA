#include "YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"

using namespace YAKSys_Hybrid_CPU::Arch::Generated;

namespace YAKSys_Hybrid_CPU::Arch
{

	std::optional<OpcodeInfo> OpcodeRegistry::GetInfo(unsigned int opCode)
	{
		for (const auto &info : Opcodes)
		{
			if (info.OpCode == opCode)
			{
				return info;
			}
		}
		return std::nullopt;
	}

	bool OpcodeRegistry::TryGetMnemonic(unsigned int opCode, std::wstring &mnemonic)
	{
		std::optional<OpcodeInfo> info = GetInfo(opCode);
		if (!info)
		{
			mnemonic = L"";
			return false;
		}

		mnemonic = info.value().Mnemonic;
		return true;
	}

	std::wstring OpcodeRegistry::GetMnemonicOrHex(unsigned int opCode)
	{
		std::wstring mnemonic;
		return TryGetMnemonic(opCode, mnemonic) ? mnemonic : std::format(L"0x{:X}", opCode);
	}

	bool OpcodeRegistry::IsSystemDeviceCommandOpcode(unsigned int opCode)
	{
		return (opCode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_QUERY_CAPS || opCode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_SUBMIT || opCode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_POLL || opCode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_WAIT || opCode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_CANCEL || opCode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_FENCE || opCode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_STATUS);
	}

	bool OpcodeRegistry::TryGetPublishedSemantics(unsigned int opCode, InstructionClass &instructionClass, SerializationClass &serializationClass)
	{
		std::optional<OpcodeInfo> info = GetInfo(opCode);
		if (!info)
		{
			instructionClass = static_cast<YAKSys_Hybrid_CPU::Arch::InstructionClass>(0);
			serializationClass = static_cast<YAKSys_Hybrid_CPU::Arch::SerializationClass>(0);
			return false;
		}

		instructionClass = info.value().InstructionClass;
		serializationClass = info.value().SerializationClass;
		return true;
	}

	bool OpcodeRegistry::TryGetPublishedSemantics(Processor::CPU_Core::InstructionsEnum opcode, InstructionClass &instructionClass, SerializationClass &serializationClass)
	{
		return TryGetPublishedSemantics(static_cast<unsigned int>(opcode), instructionClass, serializationClass);
	}

	bool OpcodeRegistry::IsVectorOp(unsigned int opCode)
	{
		const std::optional<OpcodeInfo> info = GetInfo(opCode);
		return info.has_value() && info->isVector();
	}

	bool OpcodeRegistry::RequiresVectorPayloadProjection(unsigned int opCode)
	{
		if (IsVectorConfigSystemSingletonOpcode(opCode))
		{
			return false;
		}

		if (IsVectorOp(opCode))
		{
			return true;
		}

		const auto opcode = static_cast<Processor::CPU_Core::InstructionsEnum>(opCode);
		return opcode == Processor::CPU_Core::InstructionsEnum::VLOAD ||
			opcode == Processor::CPU_Core::InstructionsEnum::VSTORE;
	}

	bool OpcodeRegistry::HasStaticRelativeControlFlowTarget(unsigned int opCode)
	{
		const auto opcode = static_cast<Processor::CPU_Core::InstructionsEnum>(opCode);
		return opcode == Processor::CPU_Core::InstructionsEnum::JAL ||
			opcode == Processor::CPU_Core::InstructionsEnum::BEQ ||
			opcode == Processor::CPU_Core::InstructionsEnum::BNE ||
			opcode == Processor::CPU_Core::InstructionsEnum::BLT ||
			opcode == Processor::CPU_Core::InstructionsEnum::BGE ||
			opcode == Processor::CPU_Core::InstructionsEnum::BLTU ||
			opcode == Processor::CPU_Core::InstructionsEnum::BGEU;
	}

	bool OpcodeRegistry::SupportsMasking(unsigned int opCode)
	{
		const std::optional<OpcodeInfo> info = GetInfo(opCode);
		return info.has_value() && info->getSupportsMasking();
	}

	bool OpcodeRegistry::SupportsSaturatingAddPolicy(unsigned int opCode)
	{
		return opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VADD);
	}

	bool OpcodeRegistry::UsesPackedArchRegisterWord1(Processor::CPU_Core::InstructionsEnum opcode)
	{
		Processor::CPU_Core::InstructionsEnum semanticOpcode = opcode;

		if (IsVectorOp(static_cast<unsigned int>(semanticOpcode)) && !IsVectorConfigSystemSingletonOpcode(semanticOpcode))
		{
			return false;
		}

		InstructionClass instructionClass;
		SerializationClass serializationClass;
		if (!TryGetPublishedSemantics(semanticOpcode, instructionClass, serializationClass))
		{
			// Only a narrow classifier-only allowlist is permitted here.
			// Vector/matrix contours also fall back to classifier metadata, but they keep
			// pointer-based Word1/Word2 transport and must never be reinterpreted through
			// the packed architectural register ABI.
			return UsesClassifierOnlyPackedArchRegisterWord1(semanticOpcode);
		}

		return (instructionClass == InstructionClass::ScalarAlu || instructionClass == InstructionClass::Memory || instructionClass == InstructionClass::ControlFlow || instructionClass == InstructionClass::Atomic || instructionClass == InstructionClass::System || instructionClass == InstructionClass::Csr || instructionClass == InstructionClass::SmtVt || instructionClass == InstructionClass::Vmx);
	}

	bool OpcodeRegistry::UsesPackedArchRegisterWord1(unsigned int opcode)
	{
		unsigned int semanticOpcode = opcode;

		if (IsVectorOp(semanticOpcode) && !IsVectorConfigSystemSingletonOpcode(semanticOpcode))
		{
			return false;
		}

		InstructionClass instructionClass;
		SerializationClass serializationClass;
		if (!TryGetPublishedSemantics(semanticOpcode, instructionClass, serializationClass))
		{
			return UsesClassifierOnlyPackedArchRegisterWord1(semanticOpcode);
		}

		return (instructionClass == InstructionClass::ScalarAlu || instructionClass == InstructionClass::Memory || instructionClass == InstructionClass::ControlFlow || instructionClass == InstructionClass::Atomic || instructionClass == InstructionClass::System || instructionClass == InstructionClass::Csr || instructionClass == InstructionClass::SmtVt || instructionClass == InstructionClass::Vmx);
	}

	bool OpcodeRegistry::IsMathOrVectorOp(unsigned int opCode)
	{
		const std::optional<OpcodeInfo> info = GetInfo(opCode);
		return info.has_value() && info->isMathOrVector();
	}

	bool OpcodeRegistry::IsControlFlowOp(unsigned int opCode)
	{
		const std::optional<OpcodeInfo> info = GetInfo(opCode);
		return info.has_value() && info->isControlFlow();
	}

	bool OpcodeRegistry::IsComparisonOp(unsigned int opCode)
	{
		const std::optional<OpcodeInfo> info = GetInfo(opCode);
		return info.has_value() && info->Category == OpcodeCategory::Comparison &&
			(info->Flags & InstructionFlags::MaskManipulation) == InstructionFlags::None;
	}

	bool OpcodeRegistry::IsMaskManipOp(unsigned int opCode)
	{
		const std::optional<OpcodeInfo> info = GetInfo(opCode);
		return info.has_value() && (info->Flags & InstructionFlags::MaskManipulation) != InstructionFlags::None;
	}

	bool OpcodeRegistry::IsWidenNarrowConvertOp(unsigned int opCode)
	{
		const std::optional<OpcodeInfo> info = GetInfo(opCode);
		return info.has_value() && (info->Flags & InstructionFlags::WidenNarrowConvert) != InstructionFlags::None;
	}

	bool OpcodeRegistry::IsScanOp(unsigned int opCode)
	{
		const std::optional<OpcodeInfo> info = GetInfo(opCode);
		return info.has_value() && (info->Flags & InstructionFlags::Scan) != InstructionFlags::None;
	}

	bool OpcodeRegistry::IsFmaOp(unsigned int opCode)
	{
		const std::optional<OpcodeInfo> info = GetInfo(opCode);
		if (!info)
		{
			return false;
		}
		return (info->Flags & InstructionFlags::ThreeOperand) != InstructionFlags::None &&
			(info->Flags & InstructionFlags::FloatingPoint) != InstructionFlags::None;
	}

	bool OpcodeRegistry::IsReductionOp(unsigned int opCode)
	{
		const std::optional<OpcodeInfo> info = GetInfo(opCode);
		if (!info)
		{
			return false;
		}
		// Exclude mask-pop (VPOPC) which also carries Reduction but is a mask-manipulation op.
		if ((info->Flags & InstructionFlags::MaskManipulation) != InstructionFlags::None)
		{
			return false;
		}
		return (info->Flags & InstructionFlags::Reduction) != InstructionFlags::None;
	}

	bool OpcodeRegistry::IsVectorConfigSystemSingletonOpcode(Processor::CPU_Core::InstructionsEnum opcode)
	{
		return (opcode == Processor::CPU_Core::InstructionsEnum::VSETVL || opcode == Processor::CPU_Core::InstructionsEnum::VSETVLI || opcode == Processor::CPU_Core::InstructionsEnum::VSETIVLI);
	}

	bool OpcodeRegistry::IsVectorConfigSystemSingletonOpcode(unsigned int opcode)
	{
		return (opcode == Processor::CPU_Core::IsaOpcodeValues::VSETVL || opcode == Processor::CPU_Core::IsaOpcodeValues::VSETVLI || opcode == Processor::CPU_Core::IsaOpcodeValues::VSETIVLI);
	}

	bool OpcodeRegistry::UsesClassifierOnlyPackedArchRegisterWord1(Processor::CPU_Core::InstructionsEnum opcode)
	{
		return (opcode == Processor::CPU_Core::InstructionsEnum::VSETVEXCPMASK || opcode == Processor::CPU_Core::InstructionsEnum::VSETVEXCPPRI);
	}

	bool OpcodeRegistry::UsesClassifierOnlyPackedArchRegisterWord1(unsigned int opcode)
	{
		return (opcode == Processor::CPU_Core::IsaOpcodeValues::VSETVEXCPMASK || opcode == Processor::CPU_Core::IsaOpcodeValues::VSETVEXCPPRI);
	}
}

