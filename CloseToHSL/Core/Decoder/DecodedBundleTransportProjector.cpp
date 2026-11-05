#include "DecodedBundleTransportProjector.h"
#include "DecodedInstruction.h"
#include "../Pipeline/Metadata/BundleMetadata.h"
#include "../Pipeline/Metadata/MicroOpAdmissionMetadata.h"
#include "../Contracts/CompilerTransport/InstructionSlotMetadata.h"
#include "../Pipeline/MicroOps/Types/MicroOp.Misc.h"
#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../Pipeline/MicroOps/Lane6DmaStream/DmaStreamComputeMicroOp.h"
#include "../Execution/ExternalAccelerators/Auth/AcceleratorOwnerDomainGuard.h"
#include "../Pipeline/MicroOps/Lane7Accelerator/SystemDeviceCommandMicroOp.h"
#include "../Diagnostics/YAKSys_Hybrid_CPU.Core.InstructionRegistry.h"
#include "../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../Diagnostics/InstructionRegistry.Types.h"
#include "../Legality/BundleLegalityAnalyzer.h"
#include "../../../NonRTL/Arch/InstructionClass.h"
#include "../Diagnostics/DecodeProjectionFaultException.h"
#include "../Pipeline/MicroOps/Memory/MicroOp.LoadStore.h"
#include "../Pipeline/MicroOps/Lane6DmaStream/DmaStreamComputeStatusMicroOp.h"
#include "../Pipeline/MicroOps/Lane6DmaStream/DmaStreamComputeQueryCapsMicroOp.h"
#include "../Pipeline/MicroOps/MatrixTile/MatrixTileMicroOps.h"
#include <codecvt>
#include <limits>
#include <locale>
#include <typeinfo>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace
{
	std::wstring Utf8ToWide(const char *value)
	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(value != nullptr ? value : "");
	}

	std::wstring FormatDecodeTrapReason(const std::exception &exception)
	{
		if (const auto *invalidOpcodeException = dynamic_cast<const InvalidOpcodeException *>(&exception))
		{
			const std::wstring opcodeKind = invalidOpcodeException->isProhibited() ? L"Prohibited opcode" : L"Invalid opcode";
			const std::wstring message = Utf8ToWide(invalidOpcodeException->what());
			return invalidOpcodeException->getSlotIndex() >= 0
				? opcodeKind + L" at slot " + std::to_wstring(invalidOpcodeException->getSlotIndex()) + L": " + message
				: opcodeKind + L": " + message;
		}

		if (const auto *projectionException = dynamic_cast<const DecodeProjectionFaultException *>(&exception))
		{
			return L"Decode projection fault: " + Utf8ToWide(projectionException->what());
		}

		return L"Decode exception: " + Utf8ToWide(exception.what());
	}
}

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	YAKSys_Hybrid_CPU::Core::DecodedBundleTransportFacts DecodedBundleTransportProjector::BuildCanonicalTransportFacts(std::span<const VLIW_Instruction> rawSlots, std::shared_ptr<DecodedInstructionBundle> canonicalBundle, std::optional<DecodedBundleDependencySummary> &dependencySummary)
	{
		if (!canonicalBundle) throw std::invalid_argument("canonicalBundle must not be null.");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp?[] carrierBundle = ProjectCanonicalCarrierBundle(rawSlots, canonicalBundle);
		std::vector<std::shared_ptr<MicroOp>> carrierBundle = ProjectCanonicalCarrierBundle(rawSlots, canonicalBundle);
		return DecodedBundleSlotCarrierBuilder::BuildTransportFacts(canonicalBundle->getBundleAddress(), carrierBundle, canonicalBundle, dependencySummary);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static MicroOp?[] BuildCanonicalCarrierBundleForTesting(std::span<const VLIW_Instruction> rawSlots, DecodedInstructionBundle canonicalBundle)
	std::vector<std::shared_ptr<MicroOp>> DecodedBundleTransportProjector::BuildCanonicalCarrierBundleForTesting(std::span<const VLIW_Instruction> rawSlots, std::shared_ptr<DecodedInstructionBundle> canonicalBundle)
	{
		if (!canonicalBundle) throw std::invalid_argument("canonicalBundle must not be null.");
		return ProjectCanonicalCarrierBundle(rawSlots, canonicalBundle);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildFallbackTransportFacts(std::span<const VLIW_Instruction> rawSlots, VliwDecoderV4 slotDecoder, InvalidOpcodeException bundleDecodeException, ulong bundlePc, VliwBundleAnnotations? bundleAnnotations = null)
	YAKSys_Hybrid_CPU::Core::DecodedBundleTransportFacts DecodedBundleTransportProjector::BuildFallbackTransportFacts(std::span<const VLIW_Instruction> rawSlots, std::shared_ptr<VliwDecoderV4> slotDecoder, std::shared_ptr<InvalidOpcodeException> bundleDecodeException, unsigned long long bundlePc, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp?[] carrierBundle = ProjectFallbackCarrierBundle(rawSlots, slotDecoder, bundleDecodeException, bundlePc, bundleAnnotations);
		std::vector<std::shared_ptr<MicroOp>> carrierBundle = ProjectFallbackCarrierBundle(rawSlots, slotDecoder, bundleDecodeException, bundlePc, bundleAnnotations);
		return DecodedBundleSlotCarrierBuilder::BuildTransportFacts(bundlePc, carrierBundle, Core::DecodedBundleStateKind::DecodeFault, Core::DecodedBundleStateOrigin::DecodeFallbackTrap);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static MicroOp?[] BuildFallbackCarrierBundleForTesting(std::span<const VLIW_Instruction> rawSlots, IDecoderFrontend slotDecoder, InvalidOpcodeException bundleDecodeException, ulong bundlePc, System.Nullable<VliwBundleAnnotations> bundleAnnotations = null)
	std::vector<std::shared_ptr<MicroOp>> DecodedBundleTransportProjector::BuildFallbackCarrierBundleForTesting(std::span<const VLIW_Instruction> rawSlots, std::shared_ptr<IDecoderFrontend> slotDecoder, std::shared_ptr<InvalidOpcodeException> bundleDecodeException, unsigned long long bundlePc, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations)
	{
		return ProjectFallbackCarrierBundle(rawSlots, slotDecoder, bundleDecodeException, bundlePc, bundleAnnotations);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static MicroOp?[] ProjectCanonicalCarrierBundle(std::span<const VLIW_Instruction> rawSlots, DecodedInstructionBundle canonicalBundle)
	std::vector<std::shared_ptr<MicroOp>> DecodedBundleTransportProjector::ProjectCanonicalCarrierBundle(std::span<const VLIW_Instruction> rawSlots, std::shared_ptr<DecodedInstructionBundle> canonicalBundle)
	{
		if (rawSlots.size() < Core::BundleMetadata::BundleSlotCount)
		{
			throw std::out_of_range("rawSlots does not contain every bundle slot.");
		}
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp?[] carrierBundle = new MicroOp?[Core.BundleMetadata.BundleSlotCount];
		std::vector<std::shared_ptr<MicroOp>> carrierBundle(Core::BundleMetadata::BundleSlotCount);
		for (int slotIndex = 0; slotIndex < Core::BundleMetadata::BundleSlotCount; slotIndex++)
		{
			std::shared_ptr<DecodedInstruction> decodedSlot = canonicalBundle->GetDecodedSlot(slotIndex);
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref readonly VLIW_Instruction rawInstruction = ref rawSlots[slotIndex];
			VLIW_Instruction rawInstruction = rawSlots[slotIndex];

			carrierBundle[slotIndex] = decodedSlot->IsOccupied ? CreateFromDecodedInstruction(rawInstruction, decodedSlot, canonicalBundle->getBundleAddress()) : CreateEmptySlot(rawInstruction, decodedSlot->SlotMetadata);
		}

		return carrierBundle;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static MicroOp?[] ProjectFallbackCarrierBundle(std::span<const VLIW_Instruction> rawSlots, IDecoderFrontend slotDecoder, InvalidOpcodeException bundleDecodeException, ulong bundlePc, System.Nullable<VliwBundleAnnotations> bundleAnnotations = null)
	std::vector<std::shared_ptr<MicroOp>> DecodedBundleTransportProjector::ProjectFallbackCarrierBundle(std::span<const VLIW_Instruction> rawSlots, std::shared_ptr<IDecoderFrontend> slotDecoder, std::shared_ptr<InvalidOpcodeException> bundleDecodeException, unsigned long long bundlePc, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations)
	{
		if (rawSlots.size() < Core::BundleMetadata::BundleSlotCount)
		{
			throw std::out_of_range("rawSlots does not contain every bundle slot.");
		}
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp?[] carrierBundle = new MicroOp?[Core.BundleMetadata.BundleSlotCount];
		std::vector<std::shared_ptr<MicroOp>> carrierBundle(Core::BundleMetadata::BundleSlotCount);
		for (unsigned char slotIndex = 0; slotIndex < Core::BundleMetadata::BundleSlotCount; slotIndex++)
		{
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref readonly VLIW_Instruction rawInstruction = ref rawSlots[slotIndex];
			VLIW_Instruction rawInstruction = rawSlots[slotIndex];
			std::shared_ptr<InstructionSlotMetadata> slotMetadata = ResolveFallbackSlotMetadata(bundleAnnotations, slotIndex);
			std::shared_ptr<MicroOp> slotCarrierMicroOp;
			if (rawInstruction.getOpCode() == 0)
			{
				slotCarrierMicroOp = CreateEmptySlot(rawInstruction, slotMetadata);
			}
			else
			{
				std::shared_ptr<InstructionIR> instruction;
				try
				{
					instruction = slotDecoder->Decode(rawInstruction, slotIndex);
				}
				catch (const InvalidOpcodeException &slotDecodeException)
				{
					const std::exception &effectiveException = bundleDecodeException->getSlotIndex() == slotIndex
						? static_cast<const std::exception &>(*bundleDecodeException)
						: static_cast<const std::exception &>(slotDecodeException);

						slotCarrierMicroOp = CreateTrapMicroOp(rawInstruction.getOpCode(), FormatDecodeTrapReason(effectiveException), slotMetadata);
					carrierBundle[slotIndex] = slotCarrierMicroOp;
					continue;
				}

				slotCarrierMicroOp = CreateFromInstruction(rawInstruction, instruction, slotMetadata, bundlePc);
			}

			carrierBundle[slotIndex] = slotCarrierMicroOp;
		}

		return carrierBundle;
	}

	std::shared_ptr<MicroOp> DecodedBundleTransportProjector::CreateEmptySlot(VLIW_Instruction const rawInstruction, std::shared_ptr<InstructionSlotMetadata> slotMetadata)
	{
		std::shared_ptr<NopMicroOp> nopMicroOp = std::make_shared<NopMicroOp>();
		nopMicroOp->setOpCode(rawInstruction.getOpCode());

		ApplyCanonicalExecutionOwnerProjection(nopMicroOp, slotMetadata);
		ApplyCanonicalCompatibilitySlotMetadataProjection(nopMicroOp, slotMetadata);
		return nopMicroOp;
	}

	std::shared_ptr<MicroOp> DecodedBundleTransportProjector::CreateFromDecodedInstruction(VLIW_Instruction const rawInstruction, std::shared_ptr<DecodedInstruction> decodedInstruction, unsigned long long bundlePc)
	{
		std::shared_ptr<InstructionIR> instruction = decodedInstruction->Instruction;
		if (!instruction) throw std::logic_error("Bundle slot " + std::to_string(decodedInstruction->SlotIndex) + " does not contain a decoded instruction.");
		return CreateFromInstruction(rawInstruction, instruction, decodedInstruction->SlotMetadata, bundlePc);
	}

	std::shared_ptr<MicroOp> DecodedBundleTransportProjector::CreateFromInstruction(VLIW_Instruction const rawInstruction, std::shared_ptr<InstructionIR> instruction, std::shared_ptr<InstructionSlotMetadata> slotMetadata, std::optional<unsigned long long> bundlePc)
	{
		unsigned int opCode = instruction->CanonicalOpcode->Value;
		int projectedTrapMemoryBankIntent = ResolveProjectedTrapMemoryBankIntent(rawInstruction, instruction);
		if (instruction->DmaStreamComputeDescriptor != nullptr)
		{
			if (opCode != Processor::CPU_Core::IsaOpcodeValues::DmaStreamCompute)
			{
				return CreateCanonicalTrapMicroOp(opCode, instruction, L"DmaStreamCompute descriptor sideband reached projector on a non-DmaStreamCompute opcode.", slotMetadata, projectedTrapMemoryBankIntent);
			}

			std::shared_ptr<DmaStreamComputeMicroOp> dmaStreamComputeMicroOp = std::make_shared<DmaStreamComputeMicroOp>(instruction->DmaStreamComputeDescriptor);
			ApplyCanonicalCompatibilitySlotMetadataProjection(dmaStreamComputeMicroOp, slotMetadata);
			return dmaStreamComputeMicroOp;
		}

		if (instruction->DmaStreamComputeDescriptorReference.has_value())
		{
			return CreateCanonicalTrapMicroOp(opCode, instruction, L"DmaStreamCompute descriptor reference reached projector without the guard-accepted descriptor payload.", slotMetadata, projectedTrapMemoryBankIntent);
		}

		if (instruction->AcceleratorCommandDescriptor != nullptr)
		{
			if (opCode != Processor::CPU_Core::IsaOpcodeValues::ACCEL_SUBMIT)
			{
				return CreateCanonicalTrapMicroOp(opCode, instruction, L"AcceleratorCommandDescriptor sideband reached projector on a non-ACCEL_SUBMIT opcode.", slotMetadata, projectedTrapMemoryBankIntent);
			}

			std::wstring guardMessage;
			if (!AcceleratorOwnerDomainGuard::getDefault()->IsDescriptorGuardBacked(instruction->AcceleratorCommandDescriptor, guardMessage))
			{
				return CreateCanonicalTrapMicroOp(opCode, instruction, L"AcceleratorCommandDescriptor reached projector without guard-backed owner/domain acceptance. " + guardMessage, slotMetadata, projectedTrapMemoryBankIntent);
			}

			std::optional<std::shared_ptr<AcceleratorGuardEvidence>> guardEvidence = instruction->AcceleratorCommandDescriptor->OwnerGuardDecision->Evidence;
			std::shared_ptr<AcceleratorGuardDecision> submitGuard = AcceleratorOwnerDomainGuard::getDefault()->EnsureBeforeSubmit(instruction->AcceleratorCommandDescriptor, guardEvidence);
			if (!submitGuard->IsAllowed)
			{
				return CreateCanonicalTrapMicroOp(opCode, instruction, L"ACCEL_SUBMIT admission guard rejected before carrier materialization. " + submitGuard->Message, slotMetadata, projectedTrapMemoryBankIntent);
			}

			std::shared_ptr<AcceleratorSubmitMicroOp> submitMicroOp = std::make_shared<AcceleratorSubmitMicroOp>(ToLegacyDecoderField(instruction->Rd), instruction->AcceleratorCommandDescriptor);
			ApplyCanonicalExecutionOwnerProjection(submitMicroOp, slotMetadata);
			submitMicroOp->InitializeMetadata();
			ApplyCanonicalCompatibilitySlotMetadataProjection(submitMicroOp, slotMetadata);
			return submitMicroOp;
		}

		if (instruction->AcceleratorCommandDescriptorReference.has_value())
		{
			return CreateCanonicalTrapMicroOp(opCode, instruction, L"AcceleratorCommandDescriptor reference reached projector without the validated descriptor payload.", slotMetadata, projectedTrapMemoryBankIntent);
		}

		if (!InstructionRegistry::IsRegistered(opCode))
		{
			return CreateCanonicalTrapMicroOp(opCode, instruction, L"", slotMetadata, projectedTrapMemoryBankIntent);
		}

		try
		{
			VLIW_Instruction contextInstruction = ProjectCanonicalMaterializationInstruction(rawInstruction, instruction, bundlePc);
			const std::shared_ptr<VectorInstructionPayload> vectorPayload = instruction->VectorPayload.value_or(nullptr);
			bool requiresVectorPayload = OpcodeRegistry::RequiresVectorPayloadProjection(opCode);
			unsigned char contextDataType = vectorPayload ? vectorPayload->DataType : contextInstruction.getDataType();
			bool indexedAddressing = vectorPayload ? vectorPayload->Indexed : contextInstruction.getIndexed();
			bool is2DAddressing = vectorPayload ? vectorPayload->Is2D : contextInstruction.getIs2D();
			unsigned long long vectorPrimaryPointer = vectorPayload ? vectorPayload->PrimaryPointer : contextInstruction.getDestSrc1Pointer();
			unsigned long long vectorSecondaryPointer = vectorPayload ? vectorPayload->SecondaryPointer : contextInstruction.getSrc2Pointer();
			unsigned int vectorStreamLength = vectorPayload ? vectorPayload->StreamLength : contextInstruction.getStreamLength();
			unsigned short vectorStride = vectorPayload ? vectorPayload->Stride : contextInstruction.getStride();
			unsigned short vectorRowStride = vectorPayload ? vectorPayload->RowStride : contextInstruction.getRowStride();
			bool tailAgnostic = vectorPayload ? vectorPayload->TailAgnostic : contextInstruction.getTailAgnostic();
			bool maskAgnostic = vectorPayload ? vectorPayload->MaskAgnostic : contextInstruction.getMaskAgnostic();
			bool saturating = vectorPayload ? vectorPayload->Saturating : contextInstruction.getSaturating();
			unsigned char predicateMask = vectorPayload ? vectorPayload->PredicateMask : rawInstruction.getPredicateMask();
			DecoderContext context = {.OpCode = opCode, .Immediate = contextInstruction.getImmediate(), .HasImmediate = true, .DataType = contextDataType, .HasDataType = true, .IndexedAddressing = indexedAddressing, .Is2DAddressing = is2DAddressing, .HasVectorAddressingContour = true, .VectorPrimaryPointer = vectorPrimaryPointer, .VectorSecondaryPointer = vectorSecondaryPointer, .VectorStreamLength = vectorStreamLength, .VectorStride = vectorStride, .VectorRowStride = vectorRowStride, .TailAgnostic = tailAgnostic, .MaskAgnostic = maskAgnostic, .Saturating = saturating, .MatrixTileNumericPolicy = vectorPayload ? vectorPayload->MatrixTileNumericPolicy : nullptr, .MatrixTileLayoutPolicy = vectorPayload ? vectorPayload->MatrixTileLayoutPolicy : nullptr, .HasVectorPayload = requiresVectorPayload, .MemoryAddress = instruction->HasAbsoluteAddressing ? static_cast<unsigned long long>(instruction->Imm) : contextInstruction.getSrc2Pointer(), .HasMemoryAddress = instruction->HasAbsoluteAddressing, .PackedRegisterTriplet = contextInstruction.getDestSrc1Pointer(), .HasPackedRegisterTriplet = true, .Reg1ID = ToLegacyDecoderField(instruction->Rd), .Reg2ID = ToLegacyDecoderField(instruction->Rs1), .Reg3ID = ToLegacyDecoderField(instruction->Rs2), .AuxData = ResolveCanonicalAuxData(instruction, contextInstruction, bundlePc), .PredicateMask = predicateMask, .AcquireOrdering = instruction->AcquireOrdering, .ReleaseOrdering = instruction->ReleaseOrdering};

			std::shared_ptr<MicroOp> microOp = InstructionRegistry::CreateMicroOp(opCode, context);
			ApplyCanonicalExecutionOwnerProjection(microOp, slotMetadata);
			RefreshOwnerDependentMaterializedResourceFacts(microOp);
			if (RequiresCanonicalDecodeProjection(microOp, instruction))
			{
				bool writesRegister = Core::Legality::BundleLegalityAnalyzer::MayWriteArchitecturalRegister(instruction);
				microOp->ApplyCanonicalDecodeProjection(instruction->Class, instruction->SerializationClass, Core::Legality::BundleLegalityAnalyzer::BuildCanonicalPlacement(instruction->Class, microOp->getPlacement().DomainTag), Core::Legality::BundleLegalityAnalyzer::IsMemoryLikeClass(instruction->Class), instruction->Class == InstructionClass::ControlFlow, writesRegister, Core::Legality::BundleLegalityAnalyzer::GetCanonicalReadRegisters(instruction), Core::Legality::BundleLegalityAnalyzer::GetCanonicalWriteRegisters(instruction, writesRegister));
			}
			ApplyCanonicalCompatibilitySlotMetadataProjection(microOp, slotMetadata);

			return microOp;
		}
		catch (const DecodeProjectionFaultException &projectionException)
		{
			return CreateCanonicalTrapMicroOp(opCode, instruction, FormatDecodeTrapReason(projectionException), slotMetadata, projectedTrapMemoryBankIntent);
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static TrapMicroOp CreateCanonicalTrapMicroOp(uint opCode, in InstructionIR instruction, string? trapReason = null, InstructionSlotMetadata slotMetadata = default, int projectedMemoryBankIntent = -1)
	std::shared_ptr<TrapMicroOp> DecodedBundleTransportProjector::CreateCanonicalTrapMicroOp(unsigned int opCode, std::shared_ptr<InstructionIR> instruction, const std::wstring &trapReason, std::shared_ptr<InstructionSlotMetadata> slotMetadata, int projectedMemoryBankIntent)
	{
		std::shared_ptr<TrapMicroOp> trapMicroOp = CreateTrapMicroOp(opCode, trapReason, slotMetadata, projectedMemoryBankIntent);
		bool writesRegister = Core::Legality::BundleLegalityAnalyzer::MayWriteArchitecturalRegister(instruction);
		trapMicroOp->ApplyCanonicalDecodeProjection(instruction->Class, instruction->SerializationClass, Core::Legality::BundleLegalityAnalyzer::BuildCanonicalPlacement(instruction->Class, trapMicroOp->getPlacement().DomainTag), Core::Legality::BundleLegalityAnalyzer::IsMemoryLikeClass(instruction->Class), instruction->Class == InstructionClass::ControlFlow, writesRegister, Core::Legality::BundleLegalityAnalyzer::GetCanonicalReadRegisters(instruction), Core::Legality::BundleLegalityAnalyzer::GetCanonicalWriteRegisters(instruction, writesRegister));
		return trapMicroOp;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static TrapMicroOp CreateTrapMicroOp(uint opCode, string? trapReason = null, InstructionSlotMetadata slotMetadata = default, int projectedMemoryBankIntent = -1)
	std::shared_ptr<TrapMicroOp> DecodedBundleTransportProjector::CreateTrapMicroOp(unsigned int opCode, const std::wstring &trapReason, std::shared_ptr<InstructionSlotMetadata> slotMetadata, int projectedMemoryBankIntent)
	{
		std::shared_ptr<TrapMicroOp> trapMicroOp = std::make_shared<TrapMicroOp>();
		trapMicroOp->setOpCode(opCode);
		trapMicroOp->setUndecodedOpCode(opCode);
		trapMicroOp->setTrapReason(trapReason);
		trapMicroOp->setProjectedMemoryBankIntent(projectedMemoryBankIntent);

		ApplyCanonicalExecutionOwnerProjection(trapMicroOp, slotMetadata);
		ApplyCanonicalCompatibilitySlotMetadataProjection(trapMicroOp, slotMetadata);
		return trapMicroOp;
	}

	int DecodedBundleTransportProjector::ResolveProjectedTrapMemoryBankIntent(VLIW_Instruction const rawInstruction, std::shared_ptr<InstructionIR> instruction)
	{
		unsigned long long memoryAddress = (std::numeric_limits<unsigned long long>::max)();
		switch (instruction->Class)
		{
			case InstructionClass::Memory:
				memoryAddress = instruction->HasAbsoluteAddressing
					? static_cast<unsigned long long>(instruction->Imm)
					: (rawInstruction.getSrc2Pointer() != 0
						? rawInstruction.getSrc2Pointer()
						: static_cast<unsigned long long>(static_cast<short>(rawInstruction.getImmediate())));
				break;
			case InstructionClass::Atomic:
				memoryAddress = rawInstruction.getSrc2Pointer();
				break;
			default:
				break;
		}

		return memoryAddress == (std::numeric_limits<unsigned long long>::max)() ? -1 : static_cast<int>((memoryAddress / 4096ULL) % 16ULL);
	}

	VLIW_Instruction DecodedBundleTransportProjector::ProjectCanonicalMaterializationInstruction(VLIW_Instruction const rawInstruction, std::shared_ptr<InstructionIR> instruction, std::optional<unsigned long long> &bundlePc)
	{
		unsigned int opCode = instruction->CanonicalOpcode->Value;
		if (instruction->Class == InstructionClass::ControlFlow)
		{
			VLIW_Instruction projectedControlFlowInstruction = rawInstruction;
			projectedControlFlowInstruction.setOpCode(opCode);
			projectedControlFlowInstruction.setDestSrc1Pointer(VLIW_Instruction::PackArchRegs(instruction->Rd, instruction->Rs1, instruction->Rs2));
			projectedControlFlowInstruction.setSrc2Pointer(0);

			return projectedControlFlowInstruction;
		}

		// Absolute-memory truth now flows through DecoderContext.MemoryAddress/AuxData.
		// Non-control-flow materialization no longer needs a second raw-instruction
		// rewrite branch for retained absolute Load/Store compatibility contours.
		return rawInstruction;
	}

	unsigned long long DecodedBundleTransportProjector::ResolveCanonicalAuxData(std::shared_ptr<InstructionIR> instruction, VLIW_Instruction const contextInstruction, std::optional<unsigned long long> &bundlePc)
	{
		unsigned long long targetAddress;
		if (instruction->Class == InstructionClass::ControlFlow && TryResolveCanonicalStaticBranchTarget(instruction, bundlePc, targetAddress))
		{
			return targetAddress;
		}

		return instruction->HasAbsoluteAddressing ? static_cast<unsigned long long>(instruction->Imm) : contextInstruction.getSrc2Pointer();
	}

	bool DecodedBundleTransportProjector::TryResolveCanonicalStaticBranchTarget(std::shared_ptr<InstructionIR> instruction, std::optional<unsigned long long> &bundlePc, unsigned long long &targetAddress)
	{
		if (instruction->HasAbsoluteAddressing)
		{
			targetAddress = static_cast<unsigned long long>(instruction->Imm);
			return true;
		}

		if (!bundlePc)
		{
			targetAddress = 0;
			return false;
		}

		if (OpcodeRegistry::HasStaticRelativeControlFlowTarget(instruction->CanonicalOpcode->Value))
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: targetAddress = unchecked((ulong)((long)bundlePc.Value + instruction.Imm));
			targetAddress = static_cast<unsigned long long>(bundlePc.value()) + static_cast<unsigned long long>(instruction->Imm);
			return true;
		}

		targetAddress = 0;
		return false;
	}

	bool DecodedBundleTransportProjector::RequiresCanonicalDecodeProjection(std::shared_ptr<MicroOp> microOp, std::shared_ptr<InstructionIR> instruction)
	{
		bool temp_var187;
		switch (microOp->getCanonicalDecodePublication())
		{
			case CanonicalDecodePublicationMode::ProjectorPublishes:
				temp_var187 = true;
				break;
			case CanonicalDecodePublicationMode::SelfPublishes:
				temp_var187 = false;
				break;
			default:
				throw InvalidOperationException(
					L"Opcode " + std::to_wstring(instruction->CanonicalOpcode->Value) + L" (class " + std::to_wstring(static_cast<unsigned int>(instruction->Class)) +
					L") materialized " + Utf8ToWide(typeid(*microOp).name()) +
					L" without an explicit canonical decode publication policy. Declare whether the MicroOp self-publishes canonical facts or requires projector publication.");
		}
		return temp_var187;
	}

	std::wstring DecodedBundleTransportProjector::BuildTrapReason(std::runtime_error exception)
	{
		return FormatDecodeTrapReason(exception);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static InstructionSlotMetadata ResolveFallbackSlotMetadata(VliwBundleAnnotations? bundleAnnotations, int slotIndex)
	std::shared_ptr<InstructionSlotMetadata> DecodedBundleTransportProjector::ResolveFallbackSlotMetadata(std::shared_ptr<VliwBundleAnnotations> bundleAnnotations, int slotIndex)
	{
		std::shared_ptr<InstructionSlotMetadata> metadata;
		if (bundleAnnotations != nullptr && bundleAnnotations->TryGetInstructionSlotMetadata(slotIndex, metadata))
		{
			return metadata;
		}

		return InstructionSlotMetadata::getDefault();
	}

	void DecodedBundleTransportProjector::RefreshOwnerDependentMaterializedResourceFacts(std::shared_ptr<MicroOp> microOp)
	{
		if (const auto operation = std::dynamic_pointer_cast<LoadMicroOp>(microOp))
		{
			operation->InitializeMetadata();
			return;
		}
		if (const auto operation = std::dynamic_pointer_cast<StoreMicroOp>(microOp))
		{
			operation->InitializeMetadata();
			return;
		}
		if (const auto operation = std::dynamic_pointer_cast<AtomicMicroOp>(microOp))
		{
			operation->InitializeMetadata();
			return;
		}
		if (const auto operation = std::dynamic_pointer_cast<DmaStreamComputeStatusMicroOp>(microOp))
		{
			operation->RefreshOwnerDependentMetadata();
			return;
		}
		if (const auto operation = std::dynamic_pointer_cast<DmaStreamComputeQueryCapsMicroOp>(microOp))
		{
			operation->RefreshOwnerDependentMetadata();
			return;
		}
		if (const auto operation = std::dynamic_pointer_cast<SystemDeviceCommandMicroOp>(microOp))
		{
			operation->InitializeMetadata();
			return;
		}
		if (const auto operation = std::dynamic_pointer_cast<MatrixTileMicroOp>(microOp))
		{
			operation->InitializeMetadata();
			return;
		}
	}

	void DecodedBundleTransportProjector::ApplyCanonicalExecutionOwnerProjection(std::shared_ptr<MicroOp> microOp, std::shared_ptr<InstructionSlotMetadata> slotMetadata)
	{
		int ownerVirtualThreadId = slotMetadata->VirtualThreadId.getValue();
		microOp->setVirtualThreadId(ownerVirtualThreadId);
		microOp->setOwnerThreadId(ownerVirtualThreadId);
		microOp->setOwnerContextId(ResolveProjectedOwnerContextId(slotMetadata, ownerVirtualThreadId));

		unsigned long long projectedDomainTag = ResolveProjectedDomainTag(slotMetadata);
		if (projectedDomainTag != microOp->getPlacement().DomainTag)
		{
			SlotPlacementMetadata placement = microOp->getPlacement();
			placement.DomainTag = projectedDomainTag;
			microOp->setPlacement(placement);
		}
	}

	void DecodedBundleTransportProjector::ApplyCanonicalCompatibilitySlotMetadataProjection(std::shared_ptr<MicroOp> microOp, std::shared_ptr<InstructionSlotMetadata> slotMetadata)
	{
		std::shared_ptr<Core::SlotMetadata> canonicalSlotMetadata = (slotMetadata->SlotMetadata != nullptr) ? slotMetadata->SlotMetadata : Core::SlotMetadata::getDefault();
		microOp->setStealable(microOp->isStealable() & canonicalSlotMetadata->StealabilityPolicy == Core::StealabilityPolicy::Stealable);
		microOp->setMemoryLocalityHint(canonicalSlotMetadata->LocalityHint);
		microOp->RefreshAdmissionMetadata();
	}

	int DecodedBundleTransportProjector::ResolveProjectedOwnerContextId(std::shared_ptr<InstructionSlotMetadata> slotMetadata, int ownerVirtualThreadId)
	{
		constexpr int UnsetOwnerContextSentinel = -1;
		std::shared_ptr<MicroOpAdmissionMetadata> canonicalAdmissionMetadata = ((slotMetadata->SlotMetadata != nullptr) ? slotMetadata->SlotMetadata : Core::SlotMetadata::getDefault())->AdmissionMetadata;

		if (!canonicalAdmissionMetadata) canonicalAdmissionMetadata = std::make_shared<MicroOpAdmissionMetadata>();

		return canonicalAdmissionMetadata->OwnerContextId != UnsetOwnerContextSentinel ? canonicalAdmissionMetadata->OwnerContextId : ownerVirtualThreadId;
	}

	unsigned long long DecodedBundleTransportProjector::ResolveProjectedDomainTag(std::shared_ptr<InstructionSlotMetadata> slotMetadata)
	{
		std::shared_ptr<MicroOpAdmissionMetadata> canonicalAdmissionMetadata = ((slotMetadata->SlotMetadata != nullptr) ? slotMetadata->SlotMetadata : Core::SlotMetadata::getDefault())->AdmissionMetadata;

		if (!canonicalAdmissionMetadata) canonicalAdmissionMetadata = std::make_shared<MicroOpAdmissionMetadata>();

		return canonicalAdmissionMetadata->DomainTag != 0 ? canonicalAdmissionMetadata->DomainTag : canonicalAdmissionMetadata->Placement.DomainTag;
	}

	unsigned short DecodedBundleTransportProjector::ToLegacyDecoderField(unsigned char regId)
	{
		return regId == VLIW_Instruction::NoArchReg ? VLIW_Instruction::NoReg : regId;
	}
}
