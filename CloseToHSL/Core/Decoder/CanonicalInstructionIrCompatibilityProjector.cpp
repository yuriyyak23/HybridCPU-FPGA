#include "CanonicalInstructionIrCompatibilityProjector.h"
#include "DecoderRuntimeSidebandValidator.h"
#include "../../../NonRTL/Arch/InstructionClass.h"
#include "../../../NonRTL/Arch/SerializationClass.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileRuntimeOwnedVlmRows.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileIrProjectionAndMaterializer.h"
#include "../Pipeline/Metadata/BundleMetadata.h"
#include "DecodedInstruction.h"
#include "DeclarativeDecoderStages.h"
#include <limits>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static DecodedInstructionBundle ProjectBundle(ReadOnlySpan<VLIW_Instruction> rawSlots, VliwBundleAnnotations? bundleAnnotations, DeclarativeDecodedBundle declarativeBundle, ulong bundleAddress, ulong bundleSerial)
	std::shared_ptr<DecodedInstructionBundle> CanonicalInstructionIrCompatibilityProjector::ProjectBundle(std::span<const VLIW_Instruction> rawSlots, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations, std::shared_ptr<DeclarativeDecodedBundle> declarativeBundle, unsigned long long bundleAddress, unsigned long long bundleSerial)
	{
		ArgumentNullException::ThrowIfNull(declarativeBundle);
		if (!declarativeBundle->CanonicalBundleValue ||
			rawSlots.size() != static_cast<std::size_t>(BundleMetadata::BundleSlotCount) ||
			declarativeBundle->Slots.size() != static_cast<std::size_t>(BundleMetadata::BundleSlotCount) ||
			declarativeBundle->CanonicalBundleValue->getSlots().size() != static_cast<std::size_t>(BundleMetadata::BundleSlotCount))
		{
			throw InvalidOperationException(L"Compatibility projection requires one normalized eight-slot canonical bundle.");
		}

		std::vector<std::shared_ptr<DecodedInstruction>> projected(static_cast<std::size_t>(BundleMetadata::BundleSlotCount));
		for (int slotIndex = 0; slotIndex < BundleMetadata::BundleSlotCount; ++slotIndex)
		{
			std::shared_ptr<InstructionSlotMetadata> slotMetadata = ResolveSlotMetadata(bundleAnnotations, slotIndex);
			std::shared_ptr<CanonicalDecodedInstruction> canonicalSlot = declarativeBundle->CanonicalBundleValue->GetSlot(slotIndex);
			const std::optional<std::shared_ptr<DeclarativeDecodedSlot>>& declarativeSlot = declarativeBundle->Slots[slotIndex];
			if (!canonicalSlot)
				throw InvalidOperationException(L"Canonical bundle contains a null slot.");

			if (!canonicalSlot->IsOccupied)
			{
				if (declarativeSlot && declarativeSlot.value())
					throw InvalidOperationException(std::format("Empty canonical slot {} has declarative semantics.", slotIndex));
				projected[slotIndex] = DecodedInstruction::CreateEmpty(slotIndex, slotMetadata);
				continue;
			}

			if (!declarativeSlot || !declarativeSlot.value())
				throw InvalidOperationException(std::format("Occupied canonical slot {} lacks declarative semantics.", slotIndex));

			std::shared_ptr<RuntimeSidebandProjection> runtimeSideband = DecoderRuntimeSidebandValidator::ValidateAndResolve(rawSlots[slotIndex], slotIndex, slotMetadata);
			projected[slotIndex] = DecodedInstruction::CreateOccupied(slotIndex, ProjectInstruction(canonicalSlot, declarativeSlot.value(), runtimeSideband), slotMetadata);
		}

		std::shared_ptr<BundleMetadata> bundleMetadata = bundleAnnotations ? bundleAnnotations->getBundleMetadata() : nullptr;
		if (!bundleMetadata)
			bundleMetadata = BundleMetadata::GetDefault();
		return std::make_shared<DecodedInstructionBundle>(bundleAddress, bundleSerial, projected, std::optional<std::shared_ptr<BundleMetadata>>(bundleMetadata), false, declarativeBundle->CanonicalBundleValue);
	}
std::shared_ptr<InstructionIR> CanonicalInstructionIrCompatibilityProjector::ProjectInstruction(std::shared_ptr<CanonicalDecodedInstruction> canonicalSlot, std::shared_ptr<DeclarativeDecodedSlot> declarativeSlot, std::shared_ptr<RuntimeSidebandProjection> runtimeSideband)
	{
		ArgumentNullException::ThrowIfNull(canonicalSlot);
		ArgumentNullException::ThrowIfNull(declarativeSlot);
		ArgumentNullException::ThrowIfNull(runtimeSideband);
		if (!canonicalSlot->IsOccupied || canonicalSlot->SlotIndex != declarativeSlot->CanonicalInstruction->SlotIndex || canonicalSlot->Opcode != declarativeSlot->CanonicalInstruction->Opcode)
		{
			throw InvalidOperationException(L"Compatibility projection received inconsistent canonical slot identity.");
		}

		return ProjectInstruction(canonicalSlot, declarativeSlot->VectorPayload, runtimeSideband);
	}

		std::shared_ptr<InstructionIR> CanonicalInstructionIrCompatibilityProjector::ProjectReplayInstruction(VLIW_Instruction const rawSlot, std::shared_ptr<CanonicalDecodedInstruction> canonicalSlot, std::shared_ptr<InstructionSlotMetadata> slotMetadata)
	{
		ArgumentNullException::ThrowIfNull(canonicalSlot);
		if (!canonicalSlot->IsOccupied || !canonicalSlot->InstructionPayload || canonicalSlot->InstructionPayload->getKind() != L"DeclarativeInstructionSemantics")
			throw InvalidOperationException(L"Replay projection requires an occupied declarative canonical slot.");

		auto restored = canonicalSlot->InstructionPayload->Deserialize<std::shared_ptr<DeclarativeInstructionSemantics>>();
		if (!restored || !restored.value())
			throw InvalidOperationException(L"Replay projection could not restore frozen declarative semantics.");
		std::shared_ptr<DeclarativeInstructionSemantics> semantics = restored.value();
		if (semantics->Opcode != canonicalSlot->Opcode || semantics->InstructionClass != canonicalSlot->InstructionClass || semantics->SerializationClass != canonicalSlot->SerializationClass || semantics->Rd != canonicalSlot->Rd || semantics->Rs1 != canonicalSlot->Rs1 || semantics->Rs2 != canonicalSlot->Rs2 || semantics->Immediate != canonicalSlot->Immediate)
			throw InvalidOperationException(L"Frozen declarative semantics do not match the canonical replay slot.");

		std::shared_ptr<RuntimeSidebandProjection> runtimeSideband = DecoderRuntimeSidebandValidator::ValidateAndResolve(rawSlot, canonicalSlot->SlotIndex, slotMetadata);
		return ProjectInstruction(canonicalSlot, semantics->VectorPayload, runtimeSideband);
	}
	std::shared_ptr<InstructionIR> CanonicalInstructionIrCompatibilityProjector::ProjectInstruction(std::shared_ptr<CanonicalDecodedInstruction> canonicalSlot, const std::optional<std::shared_ptr<VectorInstructionPayload>>& vectorPayload, std::shared_ptr<RuntimeSidebandProjection> runtimeSideband)
	{
		ArgumentNullException::ThrowIfNull(canonicalSlot);
		ArgumentNullException::ThrowIfNull(runtimeSideband);
		if (!canonicalSlot->InstructionClass)
			throw InvalidOperationException(L"Occupied canonical slot has no instruction class.");
		const InstructionClass instructionClass = canonicalSlot->InstructionClass.value();
		if (!canonicalSlot->SerializationClass)
			throw InvalidOperationException(L"Occupied canonical slot has no serialization class.");
		const SerializationClass serializationClass = canonicalSlot->SerializationClass.value();
		if (canonicalSlot->Opcode > std::numeric_limits<unsigned short>::max())
			throw std::overflow_error("Canonical opcode does not fit in UInt16.");
		const unsigned short opcode = static_cast<unsigned short>(canonicalSlot->Opcode);
		const std::optional<std::shared_ptr<MatrixTileInstructionIrProjection>> matrixProjection = CreateMatrixProjection(opcode, vectorPayload, canonicalSlot->Immediate);

		auto instruction = std::make_shared<InstructionIR>();
		instruction->CanonicalOpcode = std::make_shared<Processor::CPU_Core::IsaOpcode>(opcode);
		instruction->Class = instructionClass;
		instruction->SerializationClass = serializationClass;
		instruction->Rd = canonicalSlot->Rd;
		instruction->Rs1 = canonicalSlot->Rs1;
		instruction->Rs2 = canonicalSlot->Rs2;
		instruction->Imm = canonicalSlot->Immediate;
		instruction->CsrAddress = canonicalSlot->CsrAddress;
		instruction->HasAbsoluteAddressing = false;
		instruction->AcquireOrdering = canonicalSlot->AcquireOrdering;
		instruction->ReleaseOrdering = canonicalSlot->ReleaseOrdering;
		instruction->VectorPayload = vectorPayload;
		instruction->MatrixTileProjection = matrixProjection;
		if (runtimeSideband->DmaDescriptor && runtimeSideband->DmaDescriptor.value())
		{
			const auto& descriptor = runtimeSideband->DmaDescriptor.value();
			instruction->DmaStreamComputeDescriptorReference = std::make_shared<DmaStreamComputeDescriptorReference>(descriptor->DescriptorReference);
			instruction->DmaStreamComputeDescriptor = descriptor;
		}
		if (runtimeSideband->AcceleratorDescriptor && runtimeSideband->AcceleratorDescriptor.value())
		{
			const auto& descriptor = runtimeSideband->AcceleratorDescriptor.value();
			instruction->AcceleratorCommandDescriptorReference = std::make_shared<AcceleratorDescriptorReference>(descriptor->DescriptorReference);
			instruction->AcceleratorCommandDescriptor = descriptor;
		}
		return instruction;
	}
std::optional<std::shared_ptr<MatrixTileInstructionIrProjection>> CanonicalInstructionIrCompatibilityProjector::CreateMatrixProjection(unsigned short opcode, const std::optional<std::shared_ptr<VectorInstructionPayload>> &vectorPayload, long long immediate)
	{
		if (!MatrixTileRuntimeOwnedVlmRows::IsMatrixTileOpcode(static_cast<InstructionsEnum>(opcode)) || !vectorPayload)
		{
			return std::nullopt;
		}

		return MatrixTileIrProjectionAndMaterializer::ProjectDecodedVectorPayload(static_cast<InstructionsEnum>(opcode), vectorPayload.value(), immediate, true);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static InstructionSlotMetadata ResolveSlotMetadata(VliwBundleAnnotations? bundleAnnotations, int slotIndex)
	std::shared_ptr<InstructionSlotMetadata> CanonicalInstructionIrCompatibilityProjector::ResolveSlotMetadata(std::shared_ptr<VliwBundleAnnotations> bundleAnnotations, int slotIndex)
	{
		std::shared_ptr<InstructionSlotMetadata> metadata;
		return bundleAnnotations != nullptr && bundleAnnotations->TryGetInstructionSlotMetadata(slotIndex, metadata) ? metadata : InstructionSlotMetadata::getDefault();
	}
}
