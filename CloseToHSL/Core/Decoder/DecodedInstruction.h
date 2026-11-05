#pragma once

#include "../Pipeline/AbstractBundle.h"
#include "../Pipeline/MicroOps/IR/InstructionIR.h"
#include "../Contracts/CompilerTransport/InstructionSlotMetadata.h"
#include "../Pipeline/Metadata/BundleWidth.h"
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>
#include <format>
#include "../../../tangible_exception_helper.h"

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	// Compatibility view of one canonical decoded bundle slot. The semantic IR
	// remains shared with execution, while per-slot transport metadata is retained
	// for scheduler and replay consumers.
	class DecodedInstruction final : public Pipeline::IAbstractBundleSlot
	{
	public:
		const int SlotIndex;
		const std::shared_ptr<Pipeline::MicroOps::InstructionIR> Instruction;
		const std::shared_ptr<HybridCPU_ISE::Arch::InstructionSlotMetadata> SlotMetadata;
		const bool IsOccupied;

		DecodedInstruction(int slotIndex,
			std::shared_ptr<Pipeline::MicroOps::InstructionIR> instruction,
			std::shared_ptr<HybridCPU_ISE::Arch::InstructionSlotMetadata> slotMetadata = nullptr)
			: SlotIndex(slotIndex), Instruction(std::move(instruction)), SlotMetadata(std::move(slotMetadata)), IsOccupied(Instruction != nullptr)
		{
			if (static_cast<unsigned int>(slotIndex) >= static_cast<unsigned int>(BundleSlotCount))
				throw std::out_of_range("slotIndex");
		}

		int getSlotIndex() const override { return SlotIndex; }
		bool isOccupied() const override { return IsOccupied; }
		std::optional<std::shared_ptr<Pipeline::MicroOps::InstructionIR>> getInstruction() const override
		{
			return IsOccupied ? std::optional<std::shared_ptr<Pipeline::MicroOps::InstructionIR>>(Instruction) : std::nullopt;
		}

		std::shared_ptr<Pipeline::MicroOps::InstructionIR> RequireInstruction() const
		{
			if (!Instruction)
			{
				throw InvalidOperationException(std::format(L"Bundle slot {0} does not contain a decoded instruction.", SlotIndex));
			}
			return Instruction;
		}

		static std::shared_ptr<DecodedInstruction> CreateEmpty(int slotIndex,
			std::shared_ptr<HybridCPU_ISE::Arch::InstructionSlotMetadata> slotMetadata = nullptr)
		{
			return std::make_shared<DecodedInstruction>(slotIndex, nullptr, std::move(slotMetadata));
		}

		static std::shared_ptr<DecodedInstruction> CreateOccupied(int slotIndex,
			std::shared_ptr<Pipeline::MicroOps::InstructionIR> instruction,
			std::shared_ptr<HybridCPU_ISE::Arch::InstructionSlotMetadata> slotMetadata = nullptr)
		{
			if (!instruction) throw std::invalid_argument("instruction");
			return std::make_shared<DecodedInstruction>(slotIndex, std::move(instruction), std::move(slotMetadata));
		}
	};
}
