#include "DecodeFailureCompatibilityAdapter.h"
#include "DecodeFailure.h"
#include "../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "CanonicalDecodedContracts.h"
#include <format>
#include <limits>
#include <stdexcept>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	namespace
	{
		unsigned short CheckedOpcode(VLIW_Instruction const &instruction)
		{
			const auto rawOpcode = instruction.getOpCode();
			if (rawOpcode > std::numeric_limits<unsigned short>::max())
			{
				throw std::overflow_error("Opcode value does not fit in UInt16.");
			}
			return static_cast<unsigned short>(rawOpcode);
		}
	}

	std::shared_ptr<InvalidOpcodeException> DecodeFailureCompatibilityAdapter::ToInvalidOpcodeException(std::shared_ptr<DecodeFailure> failure, std::span<const VLIW_Instruction> rawSlots)
	{
		ArgumentNullException::ThrowIfNull(failure);
		std::wstring opcodeIdentifier = failure->Field;
		unsigned short opcode = 0;
		if (static_cast<unsigned int>(failure->SlotIndex) < static_cast<unsigned int>(rawSlots.size()))
		{
			opcode = CheckedOpcode(rawSlots[static_cast<size_t>(failure->SlotIndex)]);
			opcodeIdentifier = OpcodeRegistry::GetMnemonicOrHex(opcode);
		}

		return Create(failure, opcodeIdentifier, opcode);
	}

	std::shared_ptr<InvalidOpcodeException> DecodeFailureCompatibilityAdapter::ToInvalidOpcodeException(std::shared_ptr<DecodeFailure> failure, const VLIW_Instruction &rawSlot)
	{
		ArgumentNullException::ThrowIfNull(failure);
		unsigned short opcode = CheckedOpcode(rawSlot);
		return Create(failure, OpcodeRegistry::GetMnemonicOrHex(opcode), opcode);
	}

	std::shared_ptr<InvalidOpcodeException> DecodeFailureCompatibilityAdapter::Create(std::shared_ptr<DecodeFailure> failure, const std::wstring &opcodeIdentifier, unsigned short opcode)
	{
		// std::wstring equality is exact and case-sensitive, matching StringComparison.Ordinal.
		bool isProhibited = failure->Code == DecodeFailureCode::ProhibitedOpcode || failure->Field == L"Word3[50]";
		std::wstring message;
		switch (failure->Code)
		{
		case DecodeFailureCode::ProhibitedOpcode:
			message = std::format(L"Opcode '{}' (slot {}) is not part of ISA v4 canonical surface. ", opcodeIdentifier, failure->SlotIndex) +
				L"Pseudo-ops and hint opcodes must not appear in the hardware instruction stream. See IsaV4Surface.ProhibitedOpcodes.";
			break;
		case DecodeFailureCode::UnsupportedOpcode:
			message = std::format(L"Opcode '{}' (slot {}) uses unsupported optional scalar contour. {}", opcode, failure->SlotIndex, failure->Message);
			break;
		case DecodeFailureCode::ReservedEncoding:
			message = failure->Field == L"Reserved" ? VLIW_Instruction::GetReservedWord0ViolationMessage(failure->SlotIndex) : failure->Message;
			break;
		case DecodeFailureCode::Sideband:
			if (opcode == Processor::CPU_Core::IsaOpcodeValues::DmaStreamCompute && failure->Field == L"DmaStreamComputeDescriptor")
				message = std::format(L"Slot {}: DmaStreamCompute native decode requires typed decoded sideband descriptor payload.", failure->SlotIndex);
			else if (opcode == Processor::CPU_Core::IsaOpcodeValues::DmaStreamCompute && failure->Field == L"slot")
				message = std::format(L"Slot {}: DmaStreamCompute is a lane6 DMA/stream instruction and cannot decode on any other VLIW slot.", failure->SlotIndex);
			else
				message = failure->Message;
			break;
		default:
			message = failure->Message;
			break;
		}
		return std::make_shared<InvalidOpcodeException>(message, opcodeIdentifier, failure->SlotIndex, isProhibited);
	}
}
