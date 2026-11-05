#include "DecoderFrontendOccupiedInstructionProjection.h"
#include "DecodedInstructionBundle.h"
#include "DecodedInstruction.h"
#include <stdexcept>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	std::vector<std::shared_ptr<InstructionIR>> DecoderFrontendOccupiedInstructionProjection::DecodeOccupiedInstructions(std::shared_ptr<IDecoderFrontend> frontend, std::span<const VLIW_Instruction> bundle)
	{
		ArgumentNullException::ThrowIfNull(frontend);

		std::shared_ptr<DecodedInstructionBundle> decodedBundle = frontend->DecodeInstructionBundle(bundle, 0, 0);
		if (decodedBundle == nullptr)
		{
			throw std::logic_error("Decoder frontend returned a null decoded bundle.");
		}
		std::vector<std::shared_ptr<InstructionIR>> results(decodedBundle->getOccupiedSlotCount());
		int resultIndex = 0;
		for (int slotIndex = 0; slotIndex < decodedBundle->getSlotCount(); slotIndex++)
		{
			std::shared_ptr<DecodedInstruction> slot = decodedBundle->GetDecodedSlot(slotIndex);
			if (slot == nullptr || !slot->IsOccupied)
			{
				continue;
			}

			const auto instruction = slot->getInstruction();
			if (!instruction.has_value() || instruction.value() == nullptr)
			{
				throw std::logic_error("Occupied decoded slot does not contain an instruction.");
			}
			results[resultIndex++] = instruction.value();
		}

		return results;
	}
}
