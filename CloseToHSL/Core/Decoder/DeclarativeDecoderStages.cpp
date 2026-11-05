#include "DeclarativeDecoderStages.h"
#include "../../../NonRTL/Arch/IsaV4Surface.h"
#include "CanonicalDecodedContracts.h"
#include "CanonicalDecodedInstruction.h"
#include "../../../NonRTL/Arch/InstructionClassifier.h"
#include "../Contracts/CompilerTransport/InstructionSlotMetadata.h"
#include "../../../NonRTL/Arch/Generated/GeneratedIsaShadowCatalog.g.h"
#include "../../../NonRTL/Arch/OpcodeInfo.Types.h"
#include "../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../Arch/CsrAddresses.h"
#include "../Execution/DmaStreamCompute/YAKSys_Hybrid_CPU.Core.Execution.DmaStreamCompute.DmaStreamComputeDescriptorParser.h"
#include "../Execution/ExternalAccelerators/Descriptors/AcceleratorDescriptorParser.h"
#include "CanonicalSecurityOperationIdentity.h"
#include "../../../NonRTL/Arch/SerializationClass.h"
#include "../Pipeline/Metadata/BundleMetadata.h"
#include <cstdint>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Arch::Generated;
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace {
std::vector<unsigned char> SerializeRawWordsLittleEndian(unsigned long long word0, unsigned long long word1,
	unsigned long long word2, unsigned long long word3)
{
	std::vector<unsigned char> bytes(sizeof(unsigned long long) * 4);
	const unsigned long long words[] = {word0, word1, word2, word3};
	for (std::size_t wordIndex = 0; wordIndex < 4; ++wordIndex)
		for (std::size_t byteIndex = 0; byteIndex < sizeof(unsigned long long); ++byteIndex)
			bytes[wordIndex * sizeof(unsigned long long) + byteIndex] = static_cast<unsigned char>(words[wordIndex] >> (byteIndex * 8));
	return bytes;
}
}
namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	const std::vector<std::shared_ptr<EncodingConstraintValidator::RawEncodingRule>> EncodingConstraintValidator::RawFormConstraints = {std::make_shared<RawEncodingRule>(L"reserved-word0-zero", ValidateReservedWord0), std::make_shared<RawEncodingRule>(L"retired-policy-gap-zero", ValidateRetiredPolicyGap), std::make_shared<RawEncodingRule>(L"opcode-flag-legality", ValidateOpcodeFlags), std::make_shared<RawEncodingRule>(L"fence-zero-payload", ValidateFencePayload)};
	const std::vector<std::shared_ptr<EncodingConstraintValidator::IEncodingConstraint>> EncodingConstraintValidator::Constraints = {RegisterConstraint::RequireRs2Zero(L"scalar-word-shift-immediate-rs2-zero", std::vector<unsigned short> {Processor::CPU_Core::IsaOpcodeValues::SLLIW, Processor::CPU_Core::IsaOpcodeValues::SRLIW, Processor::CPU_Core::IsaOpcodeValues::SRAIW}), RegisterConstraint::RequireRs2AndImmediateZero(L"scalar-unary-rs2-and-immediate-zero", std::vector<unsigned short> {Processor::CPU_Core::IsaOpcodeValues::SEXT_W, Processor::CPU_Core::IsaOpcodeValues::ZEXT_W, Processor::CPU_Core::IsaOpcodeValues::CLZ, Processor::CPU_Core::IsaOpcodeValues::CTZ, Processor::CPU_Core::IsaOpcodeValues::CPOP, Processor::CPU_Core::IsaOpcodeValues::SEXT_B, Processor::CPU_Core::IsaOpcodeValues::SEXT_H, Processor::CPU_Core::IsaOpcodeValues::ZEXT_H, Processor::CPU_Core::IsaOpcodeValues::REV8, Processor::CPU_Core::IsaOpcodeValues::BREV8}), RegisterConstraint::RequireImmediateZero(L"scalar-address-generation-immediate-zero", std::vector<unsigned short> {Processor::CPU_Core::IsaOpcodeValues::SH1ADD, Processor::CPU_Core::IsaOpcodeValues::SH2ADD, Processor::CPU_Core::IsaOpcodeValues::SH3ADD, Processor::CPU_Core::IsaOpcodeValues::ADD_UW, Processor::CPU_Core::IsaOpcodeValues::SH1ADD_UW, Processor::CPU_Core::IsaOpcodeValues::SH2ADD_UW, Processor::CPU_Core::IsaOpcodeValues::SH3ADD_UW}), RegisterConstraint::RequireRs2ZeroAndImmediateAtMost(L"scalar-address-generation-imm6", 0x3F, std::vector<unsigned short> {Processor::CPU_Core::IsaOpcodeValues::SLLI_UW}), RegisterConstraint::RequireImmediateZero(L"scalar-carry-less-immediate-zero", std::vector<unsigned short> {Processor::CPU_Core::IsaOpcodeValues::CLMUL, Processor::CPU_Core::IsaOpcodeValues::CLMULH, Processor::CPU_Core::IsaOpcodeValues::CLMULR}), RegisterConstraint::RequireImmediateZero(L"scalar-rotate-immediate-zero", std::vector<unsigned short> {Processor::CPU_Core::IsaOpcodeValues::ROL, Processor::CPU_Core::IsaOpcodeValues::ROR}), RegisterConstraint::RequireRs2ZeroAndImmediateAtMost(L"scalar-rotate-imm6", 0x3F, std::vector<unsigned short> {Processor::CPU_Core::IsaOpcodeValues::ROLI, Processor::CPU_Core::IsaOpcodeValues::RORI}), RegisterConstraint::RequireImmediateZero(L"scalar-boolean-invert-immediate-zero", std::vector<unsigned short> {Processor::CPU_Core::IsaOpcodeValues::ANDN, Processor::CPU_Core::IsaOpcodeValues::ORN, Processor::CPU_Core::IsaOpcodeValues::XNOR}), RegisterConstraint::RequireImmediateZero(L"scalar-bitfield-immediate-zero", std::vector<unsigned short> {Processor::CPU_Core::IsaOpcodeValues::BSET, Processor::CPU_Core::IsaOpcodeValues::BCLR, Processor::CPU_Core::IsaOpcodeValues::BINV, Processor::CPU_Core::IsaOpcodeValues::BEXT}), RegisterConstraint::RequireRs2ZeroAndImmediateAtMost(L"scalar-bitfield-imm6", 0x3F, std::vector<unsigned short> {Processor::CPU_Core::IsaOpcodeValues::BSETI, Processor::CPU_Core::IsaOpcodeValues::BCLRI, Processor::CPU_Core::IsaOpcodeValues::BINVI, Processor::CPU_Core::IsaOpcodeValues::BEXTI}), RegisterConstraint::RequireImmediateZero(L"scalar-minmax-immediate-zero", std::vector<unsigned short> {Processor::CPU_Core::IsaOpcodeValues::MIN, Processor::CPU_Core::IsaOpcodeValues::MAX, Processor::CPU_Core::IsaOpcodeValues::MINU, Processor::CPU_Core::IsaOpcodeValues::MAXU}), RegisterConstraint::RequireImmediateZero(L"scalar-zeroing-select-immediate-zero", std::vector<unsigned short> {Processor::CPU_Core::IsaOpcodeValues::CZERO_NEZ}), RegisterConstraint::RequireRs1Rs2AndImmediateZero(L"counter-read-source-and-immediate-zero", std::vector<unsigned short> {Processor::CPU_Core::IsaOpcodeValues::RDCYCLE}), std::make_shared<DscQueueControlConstraint>(), std::make_shared<ControlFlowTargetTransportConstraint>()};

	RawBundle::RawBundle(std::vector<std::shared_ptr<RawSlot>> slots) : Slots(std::move(slots))
	{
		if (Slots.size() != BundleMetadata::BundleSlotCount)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException(string.Format("Raw bundle must contain exactly {0} slots.", BundleMetadata.BundleSlotCount), nameof(slots));
			throw std::invalid_argument(std::format("Raw bundle must contain exactly {} slots.", BundleMetadata::BundleSlotCount));
		}

	}

	std::vector<std::shared_ptr<RawSlot>> RawBundle::getSlots() const
	{
		return Slots;
	}

	std::shared_ptr<RawBundle> RawBundleReader::Read(std::span<const VLIW_Instruction> bundle)
	{
		if (bundle.size() != BundleMetadata::BundleSlotCount)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException(string.Format("Decoder bundles must contain exactly {0} slots.", BundleMetadata.BundleSlotCount), nameof(bundle));
			throw std::invalid_argument(std::format("Decoder bundles must contain exactly {} slots.", BundleMetadata::BundleSlotCount));
		}

		std::vector<std::shared_ptr<RawSlot>> slots; slots.reserve(BundleMetadata::BundleSlotCount);
		for (int slotIndex = 0; slotIndex < bundle.size(); slotIndex++)
		{
			slots.push_back(RawSlotReader::Read(bundle[slotIndex], slotIndex));
		}

		return std::make_shared<RawBundle>(std::move(slots));
	}

	std::shared_ptr<RawSlot> RawSlotReader::Read(VLIW_Instruction const instruction, int slotIndex)
	{
		if (static_cast<unsigned int>(slotIndex) >= BundleMetadata::BundleSlotCount)
		{
			throw ArgumentOutOfRangeException(L"slotIndex");
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return new RawSlot(slotIndex, instruction, unchecked((ushort)instruction.OpCode), instruction.Word0, instruction.Word1, instruction.Word2, instruction.Word3);
		return std::make_shared<RawSlot>(slotIndex, instruction, static_cast<unsigned short>(instruction.getOpCode()), instruction.getWord0(), instruction.getWord1(), instruction.getWord2(), instruction.getWord3());
	}

	bool OpcodeDescriptorLookup::TryLookup(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> &descriptor, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		if (slot->Opcode == 0)
		{
			descriptor = nullptr;
			failure = std::nullopt;
			return true;
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'ToString':
		if (IsaV4Surface::ProhibitedOpcodes->contains(std::to_wstring(slot->Opcode)))
		{
			descriptor = nullptr;
			failure = DecodeFailure::Create(DecodeFailureCode::ProhibitedOpcode, slot->SlotIndex, L"opcode", RawSlotBytes(slot), std::format(L"Opcode 0x{:04X} belongs to the prohibited ISA v4 decode surface.", slot->Opcode));
			return false;
		}

		std::wstring contour;
		std::unordered_map<unsigned short, std::wstring>::const_iterator UnsupportedOptionalContours_iterator = UnsupportedOptionalContours.find(slot->Opcode);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (UnsupportedOptionalContours.TryGetValue(slot.Opcode, out string? contour))
		if (UnsupportedOptionalContours_iterator != UnsupportedOptionalContours.end())
		{
			contour = UnsupportedOptionalContours_iterator->second;
			descriptor = nullptr;
			failure = DecodeFailure::Create(DecodeFailureCode::UnsupportedOpcode, slot->SlotIndex, L"opcode", RawSlotBytes(slot), std::format(L"Opcode 0x{:04X} belongs to the unsupported optional {} contour.", slot->Opcode, contour));
			return false;
		}

		if (GeneratedIsaCatalog::TryGetDescriptor(slot->Opcode, descriptor))
		{
			failure = std::nullopt;
			return true;
		}

		failure = DecodeFailure::Create(DecodeFailureCode::UnknownOpcode, slot->SlotIndex, L"opcode", RawSlotBytes(slot), std::format(L"Opcode 0x{:04X} has no generated ISA descriptor.", slot->Opcode));
		return false;
	}

	std::vector<unsigned char> OpcodeDescriptorLookup::RawSlotBytes(std::shared_ptr<RawSlot> slot)
	{
		return SerializeRawWordsLittleEndian(slot->Word0, slot->Word1, slot->Word2, slot->Word3);
	}

	bool OperandDecoder::TryDecode(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<DecodedOperandFields> &operands, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		unsigned short opcode = slot->Opcode;
		if (opcode == Processor::CPU_Core::IsaOpcodeValues::DmaStreamCompute)
		{
			operands = std::make_shared<DecodedOperandFields>(VLIW_Instruction::NoArchReg, VLIW_Instruction::NoArchReg, VLIW_Instruction::NoArchReg, static_cast<short>(slot->Instruction.getImmediate()));
			failure = std::nullopt;
			return true;
		}

		if (opcode == Processor::CPU_Core::IsaOpcodeValues::VPOPC)
		{
			operands = std::make_shared<DecodedOperandFields>(static_cast<unsigned char>((slot->Instruction.getImmediate() >> 8) & 0x0F), VLIW_Instruction::NoArchReg, VLIW_Instruction::NoArchReg, static_cast<short>(slot->Instruction.getImmediate()));
			failure = std::nullopt;
			return true;
		}

		if (OpcodeRegistry::IsVectorOp(static_cast<unsigned short>(slot->Instruction.getOpCode())) && (descriptor->StaticClass == InstructionClass::ScalarAlu || descriptor->StaticClass == InstructionClass::Memory))
		{
			operands = std::make_shared<DecodedOperandFields>(VLIW_Instruction::NoArchReg, VLIW_Instruction::NoArchReg, VLIW_Instruction::NoArchReg, static_cast<short>(slot->Instruction.getImmediate()));
			failure = std::nullopt;
			return true;
		}

		if (OpcodeRegistry::UsesPackedArchRegisterWord1(opcode))
		{
			unsigned char rd;
			unsigned char rs1;
			unsigned char rs2;
			if (!VLIW_Instruction::TryUnpackArchRegs(slot->Instruction.getWord1(), rd, rs1, rs2))
			{
				operands = nullptr;
				failure = DecodeFailure::Create(DecodeFailureCode::OperandEncoding, slot->SlotIndex, L"Word1", RawSlotBytes(slot), std::format(L"Opcode '{}' uses a non-canonical packed architectural-register encoding.", descriptor->Mnemonic));
				return false;
			}

			if (!TryValidateRequiredRf06RegisterOperands(slot, descriptor, rd, rs1, rs2, failure))
			{
				operands = nullptr;
				return false;
			}

			operands = std::make_shared<DecodedOperandFields>(rd, rs1, rs2, static_cast<short>(slot->Instruction.getImmediate()));
			failure = std::nullopt;
			return true;
		}

		operands = std::make_shared<DecodedOperandFields>(static_cast<unsigned char>(slot->Instruction.getDestSrc1Pointer() != 0 ? slot->Instruction.getDestSrc1Pointer() & 0x1F : slot->Instruction.getReg1ID()), static_cast<unsigned char>(slot->Instruction.getSrc2Pointer() != 0 ? slot->Instruction.getSrc2Pointer() & 0x1F : slot->Instruction.getReg2ID()), static_cast<unsigned char>(slot->Instruction.getReg3ID()), static_cast<short>(slot->Instruction.getImmediate()));
		failure = std::nullopt;
		return true;
	}

	bool OperandDecoder::TryValidateRequiredRf06RegisterOperands(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, unsigned char rd, unsigned char rs1, unsigned char rs2, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		bool isRf06RegisterRegisterOpcode = slot->Opcode == Processor::CPU_Core::IsaOpcodeValues::ADD or Processor::CPU_Core::IsaOpcodeValues::SUB or Processor::CPU_Core::IsaOpcodeValues::AND or Processor::CPU_Core::IsaOpcodeValues::OR or Processor::CPU_Core::IsaOpcodeValues::XOR;
		if (!isRf06RegisterRegisterOpcode || (rd != VLIW_Instruction::NoArchReg && rs1 != VLIW_Instruction::NoArchReg && rs2 != VLIW_Instruction::NoArchReg))
		{
			failure = std::nullopt;
			return true;
		}

		std::vector<std::wstring> absentRoles;
		absentRoles.reserve(3);
		if (rd == VLIW_Instruction::NoArchReg)
		{
			absentRoles.push_back(L"rd");
		}

		if (rs1 == VLIW_Instruction::NoArchReg)
		{
			absentRoles.push_back(L"rs1");
		}

		if (rs2 == VLIW_Instruction::NoArchReg)
		{
			absentRoles.push_back(L"rs2");
		}

		std::wstring joinedRoles;
		for (const auto& role : absentRoles) { if (!joinedRoles.empty()) joinedRoles += L", "; joinedRoles += role; }
		failure = DecodeFailure::Create(DecodeFailureCode::OperandEncoding, slot->SlotIndex, L"Word1", RawSlotBytes(slot), std::format(L"Opcode '{}' requires present rd, rs1 and rs2 architectural-register operands; NoArchReg is not legal in required role(s): {}.", descriptor->Mnemonic, joinedRoles));
		return false;
	}

	std::vector<unsigned char> OperandDecoder::RawSlotBytes(std::shared_ptr<RawSlot> slot)
	{
return SerializeRawWordsLittleEndian(slot->Word0, slot->Word1, slot->Word2, slot->Word3);
	}

	EncodingConstraintValidator::StaticConstructor::StaticConstructor()
	{
		std::unordered_set<std::wstring> ids;
		for (auto constraint : Constraints)
		{
			if (!ids.insert(constraint->getId()).second)
			{
				throw InvalidOperationException(std::format(L"Duplicate RF-05 encoding constraint identifier '{}'.", constraint->getId()));
			}
		}
	}

	const std::vector<std::wstring> EncodingConstraintValidator::getRegisteredConstraintIds()
	{
		std::vector<std::wstring> ids; ids.reserve(Constraints.size());
		for (const auto& constraint : Constraints) ids.push_back(constraint->getId());
		return ids;
	}

	const std::vector<std::wstring> EncodingConstraintValidator::getRegisteredRawFormConstraintIds()
	{
		std::vector<std::wstring> ids; ids.reserve(RawFormConstraints.size());
		for (const auto& constraint : RawFormConstraints) ids.push_back(constraint->Id);
		return ids;
	}

	bool EncodingConstraintValidator::TryValidateRawForm(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		for (auto constraint : RawFormConstraints)
		{
			failure = constraint->Validate(slot, descriptor);
			if (failure)
			{
				return false;
			}
		}

		failure = std::nullopt;
		return true;
	}

	bool EncodingConstraintValidator::TryValidate(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<DecodedOperandFields> operands, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		for (auto constraint : Constraints)
		{
			if (!constraint->AppliesTo(slot->Opcode) || constraint->TryValidate(slot, descriptor, operands, failure))
			{
				continue;
			}

			return false;
		}

		failure = std::nullopt;
		return true;
	}

	std::optional<std::shared_ptr<DecodeFailure>> EncodingConstraintValidator::ValidateReservedWord0(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor)
	{
		if (slot->Instruction.getReserved() == 0) return std::nullopt;
		return CreateFailure(slot, descriptor->Mnemonic, L"Reserved", L"reserved Word0 bits [47:40] must be zero.");
	}

	std::optional<std::shared_ptr<DecodeFailure>> EncodingConstraintValidator::ValidateRetiredPolicyGap(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor)
	{
		if ((slot->Word3 & (1ULL << 50)) == 0) return std::nullopt;
		return CreateFailure(slot, descriptor->Mnemonic, L"Word3[50]", L"the retired legacy policy-gap bit must be zero.");
	}

	std::optional<std::shared_ptr<DecodeFailure>> EncodingConstraintValidator::ValidateOpcodeFlags(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor)
	{
		std::optional<OpcodeInfo> info = OpcodeRegistry::GetInfo(slot->Opcode);
		bool isAtomic = info.has_value() && (info->InstructionClass == InstructionClass::Atomic || (static_cast<int>(info->Flags) & static_cast<int>(InstructionFlags::Atomic)) != 0);
		bool hasVectorPayload = OpcodeRegistry::RequiresVectorPayloadProjection(slot->Opcode);
		VLIW_Instruction instruction = slot->Instruction;

		if ((instruction.getAcquire() || instruction.getRelease()) && !isAtomic)
		{
			return CreateFailure(slot, descriptor->Mnemonic, L"AcquireRelease", L"Acquire/Release flags are legal only for atomic opcodes.");
		}

		if (instruction.getSaturating() && !OpcodeRegistry::SupportsSaturatingAddPolicy(slot->Opcode))
		{
			return CreateFailure(slot, descriptor->Mnemonic, L"Saturating", L"the Saturating flag is outside the scoped VADD contour.");
		}

		if (instruction.getReduction() && !OpcodeRegistry::IsReductionOp(slot->Opcode))
		{
			return CreateFailure(slot, descriptor->Mnemonic, L"Reduction", L"the Reduction flag requires a reduction opcode.");
		}

		if (instruction.getIndexed() && !hasVectorPayload)
		{
			return CreateFailure(slot, descriptor->Mnemonic, L"Indexed", L"the Indexed flag requires a vector payload opcode.");
		}

		if (instruction.getIs2D() && !hasVectorPayload)
		{
			return CreateFailure(slot, descriptor->Mnemonic, L"Is2D", L"the Is2D flag requires a vector payload opcode.");
		}

		if ((instruction.getTailAgnostic() || instruction.getMaskAgnostic()) && !hasVectorPayload)
		{
			return CreateFailure(slot, descriptor->Mnemonic, L"TailMaskAgnostic", L"tail/mask agnostic flags require a vector payload opcode.");
		}

		return std::nullopt;
	}

	std::optional<std::shared_ptr<DecodeFailure>> EncodingConstraintValidator::ValidateFencePayload(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor)
	{
		if (slot->Opcode != Processor::CPU_Core::IsaOpcodeValues::FENCE && slot->Opcode != Processor::CPU_Core::IsaOpcodeValues::FENCE_I)
		{
			return std::nullopt;
		}

		unsigned long long flags = (slot->Word0 >> 16) & 0xFFULL;
		unsigned long long word3Payload = slot->Word3 & ~(0x3ULL << 48);
		if (slot->Instruction.getImmediate() == 0 && slot->Instruction.getPredicateMask() == 0 && flags == 0 && slot->Word1 == 0 && slot->Word2 == 0 && word3Payload == 0)
		{
			return std::nullopt;
		}

		return CreateFailure(slot, descriptor->Mnemonic, L"FencePayload", L"FENCE/FENCE.I accepts only the canonical zero-payload form.");
	}

	EncodingConstraintValidator::RegisterConstraint::RegisterConstraint(const std::wstring &id, bool requireRs1Zero, bool requireRs2Zero, bool requireImmediateZero, std::optional<unsigned short> maximumImmediate, const std::vector<unsigned short> &opcodes) : Id(id), _opcodes(opcodes.begin(), opcodes.end()), _requireRs1Zero(requireRs1Zero), _requireRs2Zero(requireRs2Zero), _requireImmediateZero(requireImmediateZero), _maximumImmediate(maximumImmediate)
	{
	}

	std::wstring EncodingConstraintValidator::RegisterConstraint::getId() const
	{
		return Id;
	}

	std::shared_ptr<EncodingConstraintValidator::RegisterConstraint> EncodingConstraintValidator::RegisterConstraint::RequireRs2Zero(const std::wstring &id, const std::vector<unsigned short> &opcodes)
	{
		return std::shared_ptr<EncodingConstraintValidator::RegisterConstraint>(new EncodingConstraintValidator::RegisterConstraint(id, false, true, false, std::nullopt, opcodes));
	}

	std::shared_ptr<EncodingConstraintValidator::RegisterConstraint> EncodingConstraintValidator::RegisterConstraint::RequireRs2AndImmediateZero(const std::wstring &id, const std::vector<unsigned short> &opcodes)
	{
		return std::shared_ptr<EncodingConstraintValidator::RegisterConstraint>(new EncodingConstraintValidator::RegisterConstraint(id, false, true, true, std::nullopt, opcodes));
	}

	std::shared_ptr<EncodingConstraintValidator::RegisterConstraint> EncodingConstraintValidator::RegisterConstraint::RequireImmediateZero(const std::wstring &id, const std::vector<unsigned short> &opcodes)
	{
		return std::shared_ptr<EncodingConstraintValidator::RegisterConstraint>(new EncodingConstraintValidator::RegisterConstraint(id, false, false, true, std::nullopt, opcodes));
	}

	std::shared_ptr<EncodingConstraintValidator::RegisterConstraint> EncodingConstraintValidator::RegisterConstraint::RequireRs2ZeroAndImmediateAtMost(const std::wstring &id, unsigned short maximumImmediate, const std::vector<unsigned short> &opcodes)
	{
		return std::shared_ptr<EncodingConstraintValidator::RegisterConstraint>(new EncodingConstraintValidator::RegisterConstraint(id, false, true, false, maximumImmediate, opcodes));
	}

	std::shared_ptr<EncodingConstraintValidator::RegisterConstraint> EncodingConstraintValidator::RegisterConstraint::RequireRs1Rs2AndImmediateZero(const std::wstring &id, const std::vector<unsigned short> &opcodes)
	{
		return std::shared_ptr<EncodingConstraintValidator::RegisterConstraint>(new EncodingConstraintValidator::RegisterConstraint(id, true, true, true, std::nullopt, opcodes));
	}

	bool EncodingConstraintValidator::RegisterConstraint::AppliesTo(unsigned short opcode)
	{
		return std::find(_opcodes.begin(), _opcodes.end(), opcode) != _opcodes.end();
	}

	bool EncodingConstraintValidator::RegisterConstraint::TryValidate(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<DecodedOperandFields> operands, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		if (_requireRs1Zero && operands->Rs1 != 0)
		{
			failure = CreateFailure(slot, descriptor->Mnemonic, L"rs1", L"requires rs1=x0.");
			return false;
		}

		if (_requireRs2Zero && operands->Rs2 != 0)
		{
			failure = CreateFailure(slot, descriptor->Mnemonic, L"rs2", L"requires rs2=x0.");
			return false;
		}

		unsigned short immediate = slot->Instruction.getImmediate();
		if (_requireImmediateZero && immediate != 0)
		{
			failure = CreateFailure(slot, descriptor->Mnemonic, L"Immediate", L"requires Immediate=0.");
			return false;
		}

		if (_maximumImmediate && immediate > _maximumImmediate.value())
		{
			failure = CreateFailure(slot, descriptor->Mnemonic, L"Immediate", std::format(L"requires Immediate in [0, {}].", _maximumImmediate.value()));
			return false;
		}

		failure = std::nullopt;
		return true;
	}

	std::wstring EncodingConstraintValidator::ControlFlowTargetTransportConstraint::getId() const
	{
		return L"control-flow-no-legacy-src2-target";
	}

	bool EncodingConstraintValidator::ControlFlowTargetTransportConstraint::AppliesTo(unsigned short opcode)
	{
		return OpcodeRegistry::IsControlFlowOp(opcode);
	}

	bool EncodingConstraintValidator::ControlFlowTargetTransportConstraint::TryValidate(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<DecodedOperandFields> operands, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		if (slot->Instruction.getSrc2Pointer() == 0)
		{
			failure = std::nullopt;
			return true;
		}

		failure = CreateFailure(slot, descriptor->Mnemonic, L"Src2Pointer", L"uses the retired legacy control-flow target transport; target displacement is encoded in Immediate.");
		return false;
	}

	std::wstring EncodingConstraintValidator::DscQueueControlConstraint::getId() const
	{
		return L"dsc-queue-control-clean-register-abi";
	}

	bool EncodingConstraintValidator::DscQueueControlConstraint::AppliesTo(unsigned short opcode)
	{
		return (opcode == Processor::CPU_Core::IsaOpcodeValues::DSC_STATUS || opcode == Processor::CPU_Core::IsaOpcodeValues::DSC_QUERY_CAPS);
	}

	bool EncodingConstraintValidator::DscQueueControlConstraint::TryValidate(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<DecodedOperandFields> operands, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		if (slot->Opcode == Processor::CPU_Core::IsaOpcodeValues::DSC_STATUS && operands->Rs2 != 0)
		{
			failure = CreateFailure(slot, descriptor->Mnemonic, L"rs2", L"DSC_STATUS requires rs2=x0.");
			return false;
		}

		if (slot->Opcode == Processor::CPU_Core::IsaOpcodeValues::DSC_QUERY_CAPS && (operands->Rs1 != 0 || operands->Rs2 != 0))
		{
			failure = CreateFailure(slot, descriptor->Mnemonic, L"rs1/rs2", L"DSC_QUERY_CAPS requires rs1=x0 and rs2=x0.");
			return false;
		}

		VLIW_Instruction instruction = slot->Instruction;
		if (instruction.getVirtualThreadId() != 0 || instruction.getImmediate() != 0 || instruction.getPredicateMask() != 0 || instruction.getAcquire() || instruction.getRelease() || instruction.getSaturating() || instruction.getMaskAgnostic() || instruction.getTailAgnostic() || instruction.getIndexed() || instruction.getIs2D() || instruction.getReduction())
		{
			failure = CreateFailure(slot, descriptor->Mnemonic, L"QueueControlPayload", L"DSC queue-control opcodes require a clean packed-register carrier.");
			return false;
		}

		failure = std::nullopt;
		return true;
	}

	std::shared_ptr<DecodeFailure> EncodingConstraintValidator::CreateFailure(std::shared_ptr<RawSlot> slot, const std::wstring &mnemonic, const std::wstring &field, const std::wstring &requirement)
	{
		return DecodeFailure::Create(DecodeFailureCode::ReservedEncoding, slot->SlotIndex, field, RawSlotBytes(slot), std::format(L"Opcode '{}' (slot {}) violates '{}' encoding constraint: {}", mnemonic, slot->SlotIndex, field, requirement));
	}

	std::vector<unsigned char> EncodingConstraintValidator::RawSlotBytes(std::shared_ptr<RawSlot> slot)
	{
return SerializeRawWordsLittleEndian(slot->Word0, slot->Word1, slot->Word2, slot->Word3);
	}

	bool ExtensionPayloadDecoder::TryDecode(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<InstructionSlotMetadata> slotMetadata, std::optional<std::shared_ptr<VectorInstructionPayload>> &vectorPayload, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		if (!OpcodeRegistry::RequiresVectorPayloadProjection(slot->Opcode))
		{
			vectorPayload = std::nullopt;
			failure = std::nullopt;
			return true;
		}

		auto payload = std::make_shared<VectorInstructionPayload>();
		VLIW_Instruction instruction = slot->Instruction;
		payload->PrimaryPointer = instruction.getDestSrc1Pointer();
		payload->SecondaryPointer = instruction.getSrc2Pointer();
		payload->StreamLength = instruction.getStreamLength();
		payload->Stride = instruction.getStride();
		payload->RowStride = instruction.getRowStride();
		payload->Indexed = instruction.getIndexed();
		payload->Is2D = instruction.getIs2D();
		payload->TailAgnostic = instruction.getTailAgnostic();
		payload->MaskAgnostic = instruction.getMaskAgnostic();
		payload->Saturating = instruction.getSaturating();
		payload->PredicateMask = instruction.getPredicateMask();
		payload->DataType = instruction.getDataType();
		payload->MatrixTileNumericPolicy = slotMetadata->MatrixTileNumericPolicy;
		payload->MatrixTileLayoutPolicy = slotMetadata->MatrixTileLayoutPolicy;
		vectorPayload = std::move(payload);
		failure = std::nullopt;
		return true;
	}

	const std::vector<std::wstring> SidebandValidator::getRegisteredStaticRuleIds()
	{
		return RegisteredStaticRuleIds;
	}

	bool SidebandValidator::TryValidate(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<InstructionSlotMetadata> slotMetadata, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		bool hasDmaDescriptor = slotMetadata->DmaStreamComputeDescriptor != nullptr;
		bool hasAcceleratorDescriptor = slotMetadata->AcceleratorCommandDescriptor != nullptr;
		unsigned short opcode = slot->Opcode;
		VLIW_Instruction instruction = slot->Instruction;

		if (hasDmaDescriptor && opcode != Processor::CPU_Core::IsaOpcodeValues::DmaStreamCompute)
		{
			failure = CreateFailure(slot, descriptor->Mnemonic, L"DmaStreamComputeDescriptor", L"the descriptor sideband may accompany only the native DmaStreamCompute opcode.");
			return false;
		}

		if (hasAcceleratorDescriptor && opcode != Processor::CPU_Core::IsaOpcodeValues::ACCEL_SUBMIT)
		{
			failure = CreateFailure(slot, descriptor->Mnemonic, L"AcceleratorCommandDescriptor", L"the descriptor sideband may accompany only ACCEL_SUBMIT.");
			return false;
		}

		if (opcode == Processor::CPU_Core::IsaOpcodeValues::DmaStreamCompute)
		{
			if (slot->SlotIndex != 6)
			{
				failure = CreateFailure(slot, descriptor->Mnemonic, L"slot", L"DmaStreamCompute is fixed to lane 6.");
				return false;
			}

			if (!hasDmaDescriptor)
			{
				failure = CreateFailure(slot, descriptor->Mnemonic, L"DmaStreamComputeDescriptor", L"the native carrier requires a typed descriptor sideband.");
				return false;
			}

			std::optional<std::shared_ptr<DmaStreamComputeValidationResult>> dmaFailure;
			if (!DmaStreamComputeDescriptorParser::TryValidateNativeVliwCarrier(instruction, slot->SlotIndex, hasDmaDescriptor, dmaFailure))
			{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: failure = CreateFailure( in slot, descriptor.Mnemonic, "DmaStreamComputeCarrier", dmaFailure!.Message);
				failure = CreateFailure(slot, descriptor->Mnemonic, L"DmaStreamComputeCarrier", dmaFailure.value()->Message);
				return false;
			}
		}

		if ((opcode == Processor::CPU_Core::IsaOpcodeValues::DSC_STATUS || opcode == Processor::CPU_Core::IsaOpcodeValues::DSC_QUERY_CAPS))
		{
			if (slot->SlotIndex != 6)
			{
				failure = CreateFailure(slot, descriptor->Mnemonic, L"slot", L"the DSC queue-control carrier is fixed to lane 6.");
				return false;
			}

			if (hasDmaDescriptor)
			{
				failure = CreateFailure(slot, descriptor->Mnemonic, L"DmaStreamComputeDescriptor", L"the DSC queue-control carrier cannot carry a DmaStreamCompute descriptor.");
				return false;
			}
		}

		if (opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_SUBMIT && !hasAcceleratorDescriptor)
		{
			failure = CreateFailure(slot, descriptor->Mnemonic, L"AcceleratorCommandDescriptor", L"ACCEL_SUBMIT requires a typed descriptor sideband.");
			return false;
		}

		std::optional<std::shared_ptr<AcceleratorCarrierValidationResult>> acceleratorFailure;
		if (OpcodeRegistry::IsSystemDeviceCommandOpcode(opcode) && !AcceleratorDescriptorParser::TryValidateNativeVliwCarrier(instruction, opcode, slot->SlotIndex, hasAcceleratorDescriptor, acceleratorFailure))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: failure = CreateFailure( in slot, descriptor.Mnemonic, "AcceleratorCarrier", acceleratorFailure!.Message);
			failure = CreateFailure(slot, descriptor->Mnemonic, L"AcceleratorCarrier", acceleratorFailure.value()->Message);
			return false;
		}

		failure = std::nullopt;
		return true;
	}

	std::shared_ptr<DecodeFailure> SidebandValidator::CreateFailure(std::shared_ptr<RawSlot> slot, const std::wstring &mnemonic, const std::wstring &field, const std::wstring &message)
	{
		return DecodeFailure::Create(DecodeFailureCode::Sideband, slot->SlotIndex, field, RawSlotBytes(slot), std::format(L"Opcode '{}' (slot {}) has illegal sideband placement: {}", mnemonic, slot->SlotIndex, message));
	}

	std::vector<unsigned char> SidebandValidator::RawSlotBytes(std::shared_ptr<RawSlot> slot)
	{
return SerializeRawWordsLittleEndian(slot->Word0, slot->Word1, slot->Word2, slot->Word3);
	}

	bool CanonicalInstructionIrBuilder::TryBuild(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<DecodedOperandFields> operands, std::optional<std::shared_ptr<VectorInstructionPayload>> &vectorPayload, std::shared_ptr<InstructionSlotMetadata> slotMetadata, std::shared_ptr<CanonicalDecodedInstruction> &canonicalInstruction, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		if (static_cast<unsigned int>(slot->SlotIndex) >= BundleMetadata::BundleSlotCount || slot->Opcode != slot->Instruction.getOpCode() || slot->Word0 != slot->Instruction.getWord0() || slot->Word1 != slot->Instruction.getWord1() || slot->Word2 != slot->Instruction.getWord2() || slot->Word3 != slot->Instruction.getWord3())
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: canonicalInstruction = default!;
			canonicalInstruction = nullptr;
			failure = DecodeFailure::Create(DecodeFailureCode::BundleShape, slot->SlotIndex, L"rawIdentity", RawSlotBytes(slot), L"Canonical raw slot must match its exact instruction bytes and physical source slot.");
			return false;
		}

		if (slot->Opcode == 0)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: canonicalInstruction = default!;
			canonicalInstruction = nullptr;
			failure = DecodeFailure::Create(DecodeFailureCode::BundleShape, slot->SlotIndex, L"opcode", RawSlotBytes(slot), L"CanonicalInstructionIrBuilder accepts occupied slots only.");
			return false;
		}

		std::shared_ptr<GeneratedIsaDescriptor> registeredDescriptor;
		if (descriptor->Opcode != slot->Opcode || !GeneratedIsaCatalog::TryGetDescriptor(slot->Opcode, registeredDescriptor) || descriptor != registeredDescriptor)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: canonicalInstruction = default!;
			canonicalInstruction = nullptr;
			failure = DecodeFailure::Create(DecodeFailureCode::UnsupportedOpcode, slot->SlotIndex, L"securityOperationIdentity", RawSlotBytes(slot), L"Canonical operation requires an exact supported generated registry binding.");
			return false;
		}

		std::shared_ptr<GeneratedStaticBinding> suppliedBinding = GeneratedStaticBinding::FromDescriptor(registeredDescriptor);
		if (CanonicalSecurityOperationIdentity::FromGeneratedBinding(suppliedBinding) != CanonicalSecurityOperationKind::Ordinary)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: canonicalInstruction = default!;
			canonicalInstruction = nullptr;
			failure = DecodeFailure::Create(DecodeFailureCode::UnsupportedOpcode, slot->SlotIndex, L"securityOperationIdentity", RawSlotBytes(slot), L"Generated catalog has no admitted security operation classification.");
			return false;
		}

		std::shared_ptr<DecodedOperandFields> derivedOperands;
		if (!OperandDecoder::TryDecode(slot, registeredDescriptor, derivedOperands, failure))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: canonicalInstruction = default!;
			canonicalInstruction = nullptr;
			return false;
		}
		if (operands != derivedOperands)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: canonicalInstruction = default!;
			canonicalInstruction = nullptr;
			failure = DecodeFailure::Create(DecodeFailureCode::OperandEncoding, slot->SlotIndex, L"operandIdentity", RawSlotBytes(slot), L"Canonical operands must be derived from the exact raw instruction.");
			return false;
		}

// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
		InstructionClass instructionClass = InstructionClassifier::GetClass(slot->Opcode);
		SerializationClass serializationClass = InstructionClassifier::GetSerializationClass(slot->Opcode);
		std::shared_ptr<DeclarativeInstructionSemantics> semantics = std::make_shared<DeclarativeInstructionSemantics>(slot->Opcode, instructionClass, serializationClass, operands->Rd, operands->Rs1, operands->Rs2, operands->Immediate, vectorPayload);
		std::vector<unsigned char> rawBytes = RawSlotBytes(slot);
		canonicalInstruction = std::make_shared<CanonicalDecodedInstruction>(
			slot->SlotIndex, true, slot->Opcode, instructionClass, serializationClass,
			operands->Rd, operands->Rs1, operands->Rs2, operands->Immediate,
			ResolveCsrAddress(instructionClass, slot->Opcode, operands->Immediate),
			slot->Instruction.getAcquire(), slot->Instruction.getRelease(),
			CanonicalPayloadSnapshot::FromBytes(L"VLIW_Instruction", rawBytes),
			CanonicalPayloadSnapshot::FromObject(L"DeclarativeInstructionSemantics", std::any(semantics)),
			CanonicalPayloadSnapshot::FromObject(L"InstructionSlotMetadata", std::any(slotMetadata)));
		canonicalInstruction->StaticBinding = suppliedBinding;
		failure = std::nullopt;
		return true;
	}

	std::vector<unsigned char> CanonicalInstructionIrBuilder::RawSlotBytes(std::shared_ptr<RawSlot> slot)
	{
return SerializeRawWordsLittleEndian(slot->Word0, slot->Word1, slot->Word2, slot->Word3);
	}

	std::optional<unsigned short> CanonicalInstructionIrBuilder::ResolveCsrAddress(InstructionClass instructionClass, unsigned short opcode, long long immediate)
	{
		if (opcode == Processor::CPU_Core::IsaOpcodeValues::RDCYCLE) return CsrAddresses::Cycle;
		if (instructionClass == InstructionClass::Csr) return static_cast<unsigned short>(immediate & 0xFFF);
		return std::nullopt;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TryDecodeBundle(std::span<const VLIW_Instruction> bundle, VliwBundleAnnotations? bundleAnnotations, ulong bundleAddress, ulong bundleSerial, out System.Nullable<DeclarativeDecodedBundle> decodedBundle, out System.Nullable<DecodeFailure> failure)
	bool DeclarativeDecoderPipeline::TryDecodeBundle(std::span<const VLIW_Instruction> bundle, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations, unsigned long long bundleAddress, unsigned long long bundleSerial, std::optional<std::shared_ptr<DeclarativeDecodedBundle>> &decodedBundle, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		std::optional<std::shared_ptr<CanonicalDecodeContext>> noContext;
		return TryDecodeBundle(bundle, bundleAnnotations, bundleAddress, bundleSerial, noContext, decodedBundle, failure);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TryDecodeBundle(std::span<const VLIW_Instruction> bundle, VliwBundleAnnotations? bundleAnnotations, ulong bundleAddress, ulong bundleSerial, System.Nullable<CanonicalDecodeContext> decodeContext, out System.Nullable<DeclarativeDecodedBundle> decodedBundle, out System.Nullable<DecodeFailure> failure)
	bool DeclarativeDecoderPipeline::TryDecodeBundle(std::span<const VLIW_Instruction> bundle, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations, unsigned long long bundleAddress, unsigned long long bundleSerial, std::optional<std::shared_ptr<CanonicalDecodeContext>> &decodeContext, std::optional<std::shared_ptr<DeclarativeDecodedBundle>> &decodedBundle, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		if (bundle.size() != BundleMetadata::BundleSlotCount)
		{
			decodedBundle = std::nullopt;
			failure = DecodeFailure::Create(DecodeFailureCode::BundleShape, -1, L"bundle", {}, std::format(L"Decoder bundles must contain exactly {} slots.", BundleMetadata::BundleSlotCount));
			return false;
		}

		std::shared_ptr<RawBundle> rawBundle = RawBundleReader::Read(bundle);
		std::vector<std::optional<std::shared_ptr<DeclarativeDecodedSlot>>> decodedSlots; decodedSlots.reserve(BundleMetadata::BundleSlotCount);
		std::vector<std::shared_ptr<CanonicalDecodedInstruction>> canonicalSlots; canonicalSlots.reserve(BundleMetadata::BundleSlotCount);

		for (auto slot : rawBundle->getSlots())
		{
			std::shared_ptr<InstructionSlotMetadata> slotMetadata = ResolveSlotMetadata(bundleAnnotations, slot->SlotIndex);
			if (slot->Opcode == 0)
			{
				std::shared_ptr<CanonicalDecodedInstruction> emptySlot;
				if (!TryDecodeEmptySlot(slot, slotMetadata, emptySlot, failure))
				{
					decodedBundle = std::nullopt;
					return false;
				}

				decodedSlots.push_back(std::nullopt);
				canonicalSlots.push_back(emptySlot);
				continue;
			}

			std::optional<std::shared_ptr<DeclarativeDecodedSlot>> decodedSlot;
			if (!TryDecodeOccupiedSlot(slot, slotMetadata, decodedSlot, failure))
			{
				decodedBundle = std::nullopt;
				return false;
			}

			decodedSlots.push_back(decodedSlot);
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: canonicalSlots.push_back(decodedSlot!.CanonicalInstruction);
			canonicalSlots.push_back(decodedSlot.value()->CanonicalInstruction);
		}

		std::shared_ptr<BundleMetadata> bundleMetadata = ((bundleAnnotations == nullptr ? nullptr : bundleAnnotations->getBundleMetadata()) != nullptr) ? (bundleAnnotations == nullptr ? nullptr : bundleAnnotations->getBundleMetadata()) : BundleMetadata::GetDefault();
		decodedBundle = std::make_shared<DeclarativeDecodedBundle>(std::move(decodedSlots), CanonicalBundle::CreateFromCanonicalSlots(bundle, std::move(canonicalSlots), bundleMetadata, bundleAddress, bundleSerial, decodeContext && decodeContext.value() ? decodeContext.value() : nullptr));
		failure = std::nullopt;
		return true;
	}

	bool DeclarativeDecoderPipeline::TryDecodeOccupiedSlot(std::shared_ptr<RawSlot> slot, std::shared_ptr<InstructionSlotMetadata> slotMetadata, std::optional<std::shared_ptr<DeclarativeDecodedSlot>> &decodedSlot, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		if (slot->Opcode == 0)
		{
			decodedSlot = std::nullopt;
			failure = DecodeFailure::Create(DecodeFailureCode::BundleShape, slot->SlotIndex, L"opcode", RawSlotBytes(slot), L"Declarative occupied-slot decode does not accept the NOP sentinel.");
			return false;
		}

		std::shared_ptr<GeneratedIsaDescriptor> descriptor;
		std::shared_ptr<DecodedOperandFields> operands;
		std::optional<std::shared_ptr<VectorInstructionPayload>> vectorPayload;
		std::shared_ptr<CanonicalDecodedInstruction> canonicalInstruction;
		if (!OpcodeDescriptorLookup::TryLookup(slot, descriptor, failure) || !EncodingConstraintValidator::TryValidateRawForm(slot, descriptor, failure) || !OperandDecoder::TryDecode(slot, descriptor, operands, failure) || !EncodingConstraintValidator::TryValidate(slot, descriptor, operands, failure) || !ExtensionPayloadDecoder::TryDecode(slot, descriptor, slotMetadata, vectorPayload, failure) || !SidebandValidator::TryValidate(slot, descriptor, slotMetadata, failure) || !CanonicalInstructionIrBuilder::TryBuild(slot, descriptor, operands, vectorPayload, slotMetadata, canonicalInstruction, failure))
		{
			decodedSlot = std::nullopt;
			return false;
		}

		decodedSlot = std::make_shared<DeclarativeDecodedSlot>(descriptor, operands, vectorPayload, canonicalInstruction);
		failure = std::nullopt;
		return true;
	}

	std::vector<unsigned char> DeclarativeDecoderPipeline::RawSlotBytes(std::shared_ptr<RawSlot> slot)
	{
return SerializeRawWordsLittleEndian(slot->Word0, slot->Word1, slot->Word2, slot->Word3);
	}

	bool DeclarativeDecoderPipeline::TryDecodeEmptySlot(std::shared_ptr<RawSlot> slot, std::shared_ptr<InstructionSlotMetadata> slotMetadata, std::shared_ptr<CanonicalDecodedInstruction> &canonicalInstruction, std::optional<std::shared_ptr<DecodeFailure>> &failure)
	{
		if (slot->Word0 != 0 || slot->Word1 != 0 || slot->Word2 != 0 || slot->Word3 != 0)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: canonicalInstruction = default!;
			canonicalInstruction = nullptr;
			failure = DecodeFailure::Create(DecodeFailureCode::BundleShape, slot->SlotIndex, L"empty-slot", RawSlotBytes(slot), L"Empty/NOP VLIW slots must use the canonical all-zero carrier.");
			return false;
		}

		if (slotMetadata->DmaStreamComputeDescriptor != nullptr || slotMetadata->AcceleratorCommandDescriptor != nullptr)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: canonicalInstruction = default!;
			canonicalInstruction = nullptr;
			failure = DecodeFailure::Create(DecodeFailureCode::Sideband, slot->SlotIndex, L"DescriptorSideband", RawSlotBytes(slot), L"Descriptor sideband cannot accompany an empty/NOP VLIW slot.");
			return false;
		}

		canonicalInstruction = std::make_shared<CanonicalDecodedInstruction>(slot->SlotIndex, false, 0, std::nullopt, std::nullopt, 0, 0, 0, 0, std::nullopt, false, false, CanonicalPayloadSnapshot::FromBytes(L"VLIW_Instruction", RawSlotBytes(slot)), CanonicalPayloadSnapshot::FromObject(L"DeclarativeInstructionSemantics", std::any()), CanonicalPayloadSnapshot::FromObject(L"InstructionSlotMetadata", std::any(slotMetadata)));
		failure = std::nullopt;
		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static InstructionSlotMetadata ResolveSlotMetadata(VliwBundleAnnotations? bundleAnnotations, int slotIndex)
	std::shared_ptr<InstructionSlotMetadata> DeclarativeDecoderPipeline::ResolveSlotMetadata(std::shared_ptr<VliwBundleAnnotations> bundleAnnotations, int slotIndex)
	{
		std::shared_ptr<InstructionSlotMetadata> metadata;
		if (bundleAnnotations != nullptr && bundleAnnotations->TryGetInstructionSlotMetadata(slotIndex, metadata))
		{
			return metadata;
		}

		return InstructionSlotMetadata::getDefault();
	}
}



