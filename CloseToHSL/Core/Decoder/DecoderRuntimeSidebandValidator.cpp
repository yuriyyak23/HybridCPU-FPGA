#include "DecoderRuntimeSidebandValidator.h"
#include "../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../Execution/ExternalAccelerators/Auth/AcceleratorOwnerDomainGuard.h"
#include "../Pipeline/Metadata/MicroOpAdmissionMetadata.h"
#include "../Pipeline/Metadata/SlotMetadata.h"
#include "../Pipeline/Scheduling/SlotPlacementMetadata.h"
#include "../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	std::shared_ptr<RuntimeSidebandProjection> DecoderRuntimeSidebandValidator::ValidateAndResolve(VLIW_Instruction const instruction, int slotIndex, std::shared_ptr<InstructionSlotMetadata> slotMetadata)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ushort opcode = checked((ushort)instruction.OpCode);
		unsigned short opcode = static_cast<unsigned short>(instruction.getOpCode());
		std::wstring opcodeName = OpcodeRegistry::GetMnemonicOrHex(opcode);
		std::optional<std::shared_ptr<DmaStreamComputeDescriptor>> dmaDescriptor = ValidateDmaDescriptor(instruction, opcode, opcodeName, slotIndex, slotMetadata);
		std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> acceleratorDescriptor = ValidateAcceleratorDescriptor(instruction, opcode, opcodeName, slotIndex, slotMetadata);
		return std::make_shared<RuntimeSidebandProjection>(dmaDescriptor, acceleratorDescriptor);
	}

	std::optional<std::shared_ptr<DmaStreamComputeDescriptor>> DecoderRuntimeSidebandValidator::ValidateDmaDescriptor(VLIW_Instruction const instruction, unsigned short opcode, const std::wstring &opcodeName, int slotIndex, std::shared_ptr<InstructionSlotMetadata> slotMetadata)
	{
		if (opcode != Processor::CPU_Core::IsaOpcodeValues::DmaStreamCompute)
		{
			if (slotMetadata->DmaStreamComputeDescriptor != nullptr || slotMetadata->GetDmaStreamComputeDescriptorReference().has_value())
			{
				throw Failure(std::format(L"Slot {}: DmaStreamCompute descriptor sideband can only accompany the native DmaStreamCompute opcode.", slotIndex), opcodeName, slotIndex);
			}

			return std::nullopt;
		}

		std::shared_ptr<DmaStreamComputeDescriptor> descriptor = slotMetadata->DmaStreamComputeDescriptor;
		if (!descriptor)
		{
			// Static carrier association is expected to have rejected this before
			// this continuation. Keep the guard fail-closed for direct callers.
			throw Failure(std::format(L"Slot {}: DmaStreamCompute requires a typed descriptor sideband.", slotIndex), opcodeName, slotIndex);
		}

		if (!descriptor->OwnerGuardDecision || !descriptor->OwnerGuardDecision->IsAllowed)
		{
			throw Failure(std::format(L"Slot {}: DmaStreamCompute descriptor sideband lacks an accepted owner/domain guard decision.", slotIndex), opcodeName, slotIndex);
		}

		const auto reference = slotMetadata->GetDmaStreamComputeDescriptorReference();
		if (reference.has_value() && reference.value() != descriptor->DescriptorReference)
		{
			throw Failure(std::format(L"Slot {}: DmaStreamCompute descriptor reference sideband does not match the accepted descriptor payload.", slotIndex), opcodeName, slotIndex);
		}

		return descriptor;
	}

	std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> DecoderRuntimeSidebandValidator::ValidateAcceleratorDescriptor(VLIW_Instruction const instruction, unsigned short opcode, const std::wstring &opcodeName, int slotIndex, std::shared_ptr<InstructionSlotMetadata> slotMetadata)
	{
		std::shared_ptr<AcceleratorCommandDescriptor> descriptor = slotMetadata->AcceleratorCommandDescriptor;
		if (!OpcodeRegistry::IsSystemDeviceCommandOpcode(opcode))
		{
			if (descriptor)
			{
				throw Failure(std::format(L"Slot {}: AcceleratorCommandDescriptor sideband can only accompany the native ACCEL_SUBMIT L7-SDC opcode.", slotIndex), opcodeName, slotIndex);
			}

			return std::nullopt;
		}

		if (opcode != Processor::CPU_Core::IsaOpcodeValues::ACCEL_SUBMIT)
		{
			return std::nullopt;
		}

		if (!descriptor)
		{
			// The static pipeline owns this rejection. Retain a fail-closed
			// continuation for callers that bypass it.
			throw Failure(L"ACCEL_SUBMIT requires typed AcceleratorCommandDescriptor sideband.", opcodeName, slotIndex);
		}

		std::wstring guardMessage;
		if (!AcceleratorOwnerDomainGuard::getDefault()->IsDescriptorGuardBacked(descriptor, guardMessage))
		{
			throw Failure(std::format(L"Slot {}: ACCEL_SUBMIT descriptor sideband lacks guard-backed owner/domain acceptance. {}", slotIndex, guardMessage), opcodeName, slotIndex);
		}

		if (!descriptor->OwnerGuardDecision)
		{
			throw Failure(std::format(L"Slot {}: ACCEL_SUBMIT descriptor sideband lacks an owner/domain guard decision.", slotIndex), opcodeName, slotIndex);
		}
		std::optional<std::shared_ptr<AcceleratorGuardEvidence>> submitGuardEvidence = descriptor->OwnerGuardDecision->Evidence;
		std::shared_ptr<AcceleratorGuardDecision> submitGuard = AcceleratorOwnerDomainGuard::getDefault()->EnsureBeforeSubmit(descriptor, submitGuardEvidence);
		if (!submitGuard->IsAllowed)
		{
			throw Failure(std::format(L"Slot {}: ACCEL_SUBMIT admission guard rejected. {}", slotIndex, submitGuard->Message), opcodeName, slotIndex);
		}

		const std::shared_ptr<SlotMetadata> slotPolicy = slotMetadata->SlotMetadata != nullptr ? slotMetadata->SlotMetadata : SlotMetadata::getDefault();
		const std::shared_ptr<MicroOpAdmissionMetadata> admissionMetadata = slotPolicy->AdmissionMetadata;
		// Native SlotMetadata uses a null admission snapshot for the C# value-type default.
		if (admissionMetadata == nullptr)
		{
			throw Failure(std::format(L"Slot {}: ACCEL_SUBMIT typed sideband requires explicit slot metadata; default admission metadata is not accepted.", slotIndex), opcodeName, slotIndex);
		}

		SlotPlacementMetadata placement = admissionMetadata->Placement;
		if (placement.RequiredSlotClass != SlotClass::SystemSingleton || placement.PinningKind != SlotPinningKind::HardPinned || placement.PinnedLaneId != 7)
		{
			throw Failure(std::format(L"Slot {}: ACCEL_SUBMIT typed sideband requires SystemSingleton hard-pinned lane7 slot metadata.", slotIndex), opcodeName, slotIndex);
		}

		return descriptor;
	}

	std::shared_ptr<InvalidOpcodeException> DecoderRuntimeSidebandValidator::Failure(const std::wstring &message, const std::wstring &opcodeName, int slotIndex)
	{
		return std::make_shared<InvalidOpcodeException>(message, opcodeName, slotIndex, false);
	}
}
