#include "DecodedBundleTransportProjector.h"
#include "../../../../CloseToHSL/Core/Pipeline/MicroOps/Types/MicroOp.Misc.h"
#include "../../../../CloseToHSL/Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../../CloseToHSL/Core/Pipeline/MicroOps/Lane6DmaStream/DmaStreamComputeMicroOp.h"
#include "../../../../CloseToHSL/Core/Execution/ExternalAccelerators/Auth/AcceleratorOwnerDomainGuard.h"
#include "../../../../CloseToHSL/Core/Pipeline/MicroOps/Lane7Accelerator/SystemDeviceCommandMicroOp.h"
#include "../../../../CloseToHSL/Core/Diagnostics/YAKSys_Hybrid_CPU.Core.InstructionRegistry.h"
#include "../../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../../CloseToHSL/Core/Diagnostics/InstructionRegistry.Types.h"
#include "../../../../CloseToHSL/Core/Legality/BundleLegalityAnalyzer.h"
#include "../../../../NonRTL/Arch/InstructionClass.h"
#include "../../../../CloseToHSL/Core/Diagnostics/DecodeProjectionFaultException.h"
#include "../../../../CloseToHSL/Core/Pipeline/MicroOps/Memory/MicroOp.LoadStore.h"
#include "../../../../CloseToHSL/Core/Pipeline/MicroOps/Lane6DmaStream/DmaStreamComputeStatusMicroOp.h"
#include "../../../../CloseToHSL/Core/Pipeline/MicroOps/Lane6DmaStream/DmaStreamComputeQueryCapsMicroOp.h"
#include "../../../../CloseToHSL/Core/Pipeline/MicroOps/MatrixTile/MatrixTileMicroOps.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	YAKSys_Hybrid_CPU::Core::DecodedBundleTransportFacts DecodedBundleTransportProjector::BuildCanonicalTransportFacts(ReadOnlySpan<VLIW_Instruction> rawSlots, std::shared_ptr<DecodedInstructionBundle> canonicalBundle, std::optional<DecodedBundleDependencySummary> &dependencySummary)
	{
		ArgumentNullException::ThrowIfNull(canonicalBundle);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp?[] carrierBundle = ProjectCanonicalCarrierBundle(rawSlots, canonicalBundle);
		std::vector<std::shared_ptr<MicroOp>> carrierBundle = ProjectCanonicalCarrierBundle(rawSlots, canonicalBundle);
		return DecodedBundleSlotCarrierBuilder::BuildTransportFacts(canonicalBundle->getBundleAddress(), carrierBundle, canonicalBundle, dependencySummary);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static MicroOp?[] BuildCanonicalCarrierBundleForTesting(ReadOnlySpan<VLIW_Instruction> rawSlots, DecodedInstructionBundle canonicalBundle)
	std::vector<std::shared_ptr<MicroOp>> DecodedBundleTransportProjector::BuildCanonicalCarrierBundleForTesting(ReadOnlySpan<VLIW_Instruction> rawSlots, std::shared_ptr<DecodedInstructionBundle> canonicalBundle)
	{
		ArgumentNullException::ThrowIfNull(canonicalBundle);
		return ProjectCanonicalCarrierBundle(rawSlots, canonicalBundle);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildFallbackTransportFacts(ReadOnlySpan<VLIW_Instruction> rawSlots, VliwDecoderV4 slotDecoder, InvalidOpcodeException bundleDecodeException, ulong bundlePc, VliwBundleAnnotations? bundleAnnotations = null)
	YAKSys_Hybrid_CPU::Core::DecodedBundleTransportFacts DecodedBundleTransportProjector::BuildFallbackTransportFacts(ReadOnlySpan<VLIW_Instruction> rawSlots, std::shared_ptr<VliwDecoderV4> slotDecoder, std::shared_ptr<InvalidOpcodeException> bundleDecodeException, unsigned long long bundlePc, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp?[] carrierBundle = ProjectFallbackCarrierBundle(rawSlots, slotDecoder, bundleDecodeException, bundlePc, bundleAnnotations);
		std::vector<std::shared_ptr<MicroOp>> carrierBundle = ProjectFallbackCarrierBundle(rawSlots, slotDecoder, bundleDecodeException, bundlePc, bundleAnnotations);
		return DecodedBundleSlotCarrierBuilder::BuildTransportFacts(bundlePc, carrierBundle, Core::DecodedBundleStateKind::DecodeFault, Core::DecodedBundleStateOrigin::DecodeFallbackTrap);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static MicroOp?[] BuildFallbackCarrierBundleForTesting(ReadOnlySpan<VLIW_Instruction> rawSlots, IDecoderFrontend slotDecoder, InvalidOpcodeException bundleDecodeException, ulong bundlePc, System.Nullable<VliwBundleAnnotations> bundleAnnotations = null)
	std::vector<std::shared_ptr<MicroOp>> DecodedBundleTransportProjector::BuildFallbackCarrierBundleForTesting(ReadOnlySpan<VLIW_Instruction> rawSlots, std::shared_ptr<IDecoderFrontend> slotDecoder, std::shared_ptr<InvalidOpcodeException> bundleDecodeException, unsigned long long bundlePc, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations)
	{
		return ProjectFallbackCarrierBundle(rawSlots, slotDecoder, bundleDecodeException, bundlePc, bundleAnnotations);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static MicroOp?[] ProjectCanonicalCarrierBundle(ReadOnlySpan<VLIW_Instruction> rawSlots, DecodedInstructionBundle canonicalBundle)
	std::vector<std::shared_ptr<MicroOp>> DecodedBundleTransportProjector::ProjectCanonicalCarrierBundle(ReadOnlySpan<VLIW_Instruction> rawSlots, std::shared_ptr<DecodedInstructionBundle> canonicalBundle)
	{
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
// ORIGINAL LINE: private static MicroOp?[] ProjectFallbackCarrierBundle(ReadOnlySpan<VLIW_Instruction> rawSlots, IDecoderFrontend slotDecoder, InvalidOpcodeException bundleDecodeException, ulong bundlePc, System.Nullable<VliwBundleAnnotations> bundleAnnotations = null)
	std::vector<std::shared_ptr<MicroOp>> DecodedBundleTransportProjector::ProjectFallbackCarrierBundle(ReadOnlySpan<VLIW_Instruction> rawSlots, std::shared_ptr<IDecoderFrontend> slotDecoder, std::shared_ptr<InvalidOpcodeException> bundleDecodeException, unsigned long long bundlePc, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations)
	{
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
					std::runtime_error effectiveException = bundleDecodeException->getSlotIndex() == slotIndex ? bundleDecodeException : slotDecodeException;

					slotCarrierMicroOp = CreateTrapMicroOp(rawInstruction.getOpCode(), BuildTrapReason(effectiveException), slotMetadata);
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
		std::shared_ptr<NopMicroOp> nopMicroOp = {.OpCode = rawInstruction.getOpCode()};

		ApplyCanonicalExecutionOwnerProjection(nopMicroOp, slotMetadata);
		ApplyCanonicalCompatibilitySlotMetadataProjection(nopMicroOp, slotMetadata);
		return nopMicroOp;
	}

	std::shared_ptr<MicroOp> DecodedBundleTransportProjector::CreateFromDecodedInstruction(VLIW_Instruction const rawInstruction, std::shared_ptr<DecodedInstruction> decodedInstruction, unsigned long long bundlePc)
	{
		std::shared_ptr<InstructionIR> instruction = decodedInstruction->RequireInstruction();
		return CreateFromInstruction(rawInstruction, instruction, decodedInstruction->SlotMetadata, bundlePc);
	}

	std::shared_ptr<MicroOp> DecodedBundleTransportProjector::CreateFromInstruction(VLIW_Instruction const rawInstruction, std::shared_ptr<InstructionIR> instruction, std::shared_ptr<InstructionSlotMetadata> slotMetadata, std::optional<unsigned long long> &bundlePc)
	{
		unsigned int opCode = static_cast<unsigned int>(instruction->CanonicalOpcode);
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

		if (instruction->DmaStreamComputeDescriptorReference.HasValue)
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

			std::shared_ptr<AcceleratorGuardDecision> submitGuard = AcceleratorOwnerDomainGuard::getDefault()->EnsureBeforeSubmit(instruction->AcceleratorCommandDescriptor, instruction->AcceleratorCommandDescriptor.OwnerGuardDecision.Evidence);
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

		if (instruction->AcceleratorCommandDescriptorReference.HasValue)
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
			std::optional<std::shared_ptr<VectorInstructionPayload>> vectorPayload = instruction->VectorPayload;
			bool requiresVectorPayload = OpcodeRegistry::RequiresVectorPayloadProjection(opCode);
			unsigned char contextDataType = ((!vectorPayload ? nullptr : vectorPayload.DataType) != nullptr) ? (!vectorPayload ? nullptr : vectorPayload.DataType) : contextInstruction.getDataType();
			bool indexedAddressing = ((!vectorPayload ? nullptr : vectorPayload.Indexed) != nullptr) ? (!vectorPayload ? nullptr : vectorPayload.Indexed) : contextInstruction.getIndexed();
			bool is2DAddressing = ((!vectorPayload ? nullptr : vectorPayload.Is2D) != nullptr) ? (!vectorPayload ? nullptr : vectorPayload.Is2D) : contextInstruction.getIs2D();
			unsigned long long vectorPrimaryPointer = ((!vectorPayload ? nullptr : vectorPayload.PrimaryPointer) != nullptr) ? (!vectorPayload ? nullptr : vectorPayload.PrimaryPointer) : contextInstruction.getDestSrc1Pointer();
			unsigned long long vectorSecondaryPointer = ((!vectorPayload ? nullptr : vectorPayload.SecondaryPointer) != nullptr) ? (!vectorPayload ? nullptr : vectorPayload.SecondaryPointer) : contextInstruction.getSrc2Pointer();
			unsigned int vectorStreamLength = ((!vectorPayload ? nullptr : vectorPayload.StreamLength) != nullptr) ? (!vectorPayload ? nullptr : vectorPayload.StreamLength) : contextInstruction.getStreamLength();
			unsigned short vectorStride = ((!vectorPayload ? nullptr : vectorPayload.Stride) != nullptr) ? (!vectorPayload ? nullptr : vectorPayload.Stride) : contextInstruction.getStride();
			unsigned short vectorRowStride = ((!vectorPayload ? nullptr : vectorPayload.RowStride) != nullptr) ? (!vectorPayload ? nullptr : vectorPayload.RowStride) : contextInstruction.getRowStride();
			bool tailAgnostic = ((!vectorPayload ? nullptr : vectorPayload.TailAgnostic) != nullptr) ? (!vectorPayload ? nullptr : vectorPayload.TailAgnostic) : contextInstruction.getTailAgnostic();
			bool maskAgnostic = ((!vectorPayload ? nullptr : vectorPayload.MaskAgnostic) != nullptr) ? (!vectorPayload ? nullptr : vectorPayload.MaskAgnostic) : contextInstruction.getMaskAgnostic();
			bool saturating = ((!vectorPayload ? nullptr : vectorPayload.Saturating) != nullptr) ? (!vectorPayload ? nullptr : vectorPayload.Saturating) : contextInstruction.getSaturating();
			unsigned char predicateMask = ((!vectorPayload ? nullptr : vectorPayload.PredicateMask) != nullptr) ? (!vectorPayload ? nullptr : vectorPayload.PredicateMask) : rawInstruction.getPredicateMask();
			DecoderContext context = {.OpCode = opCode, .Immediate = contextInstruction.getImmediate(), .HasImmediate = true, .DataType = contextDataType, .HasDataType = true, .IndexedAddressing = indexedAddressing, .Is2DAddressing = is2DAddressing, .HasVectorAddressingContour = true, .VectorPrimaryPointer = vectorPrimaryPointer, .VectorSecondaryPointer = vectorSecondaryPointer, .VectorStreamLength = vectorStreamLength, .VectorStride = vectorStride, .VectorRowStride = vectorRowStride, .TailAgnostic = tailAgnostic, .MaskAgnostic = maskAgnostic, .Saturating = saturating, .MatrixTileNumericPolicy = !vectorPayload ? nullptr : vectorPayload.MatrixTileNumericPolicy, .MatrixTileLayoutPolicy = !vectorPayload ? nullptr : vectorPayload.MatrixTileLayoutPolicy, .HasVectorPayload = requiresVectorPayload, .MemoryAddress = instruction->HasAbsoluteAddressing ? static_cast<unsigned long long>(instruction->Imm) : contextInstruction.getSrc2Pointer(), .HasMemoryAddress = instruction->HasAbsoluteAddressing, .PackedRegisterTriplet = contextInstruction.getDestSrc1Pointer(), .HasPackedRegisterTriplet = true, .Reg1ID = ToLegacyDecoderField(instruction->Rd), .Reg2ID = ToLegacyDecoderField(instruction->Rs1), .Reg3ID = ToLegacyDecoderField(instruction->Rs2), .AuxData = ResolveCanonicalAuxData(instruction, contextInstruction, bundlePc), .PredicateMask = predicateMask, .AcquireOrdering = instruction->AcquireOrdering, .ReleaseOrdering = instruction->ReleaseOrdering};

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
			return CreateCanonicalTrapMicroOp(opCode, instruction, BuildTrapReason(projectionException), slotMetadata, projectedTrapMemoryBankIntent);
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
		std::shared_ptr<TrapMicroOp> trapMicroOp = {.OpCode = opCode, .UndecodedOpCode = opCode, .TrapReason = trapReason, .ProjectedMemoryBankIntent = projectedMemoryBankIntent};

		ApplyCanonicalExecutionOwnerProjection(trapMicroOp, slotMetadata);
		ApplyCanonicalCompatibilitySlotMetadataProjection(trapMicroOp, slotMetadata);
		return trapMicroOp;
	}

	int DecodedBundleTransportProjector::ResolveProjectedTrapMemoryBankIntent(VLIW_Instruction const rawInstruction, std::shared_ptr<InstructionIR> instruction)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			ulong memoryAddress = instruction.Class switch
	//			{
	//				InstructionClass.Memory => instruction.HasAbsoluteAddressing
	//					? (ulong)instruction.Imm
	//					: (rawInstruction.Src2Pointer != 0
	//						? rawInstruction.Src2Pointer
	//						: (ulong)(short)rawInstruction.Immediate), InstructionClass.Atomic => rawInstruction.Src2Pointer,
	//				_ => ulong.MaxValue
	//			};

		return memoryAddress == std::numeric_limits<unsigned long long>::max() ? -1 : static_cast<int>((memoryAddress / 4096ULL) % 16ULL);
	}

	VLIW_Instruction DecodedBundleTransportProjector::ProjectCanonicalMaterializationInstruction(VLIW_Instruction const rawInstruction, std::shared_ptr<InstructionIR> instruction, std::optional<unsigned long long> &bundlePc)
	{
		unsigned int opCode = static_cast<unsigned int>(instruction->CanonicalOpcode);
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

		if (OpcodeRegistry::HasStaticRelativeControlFlowTarget(static_cast<unsigned int>(instruction->CanonicalOpcode)))
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: targetAddress = unchecked((ulong)((long)bundlePc.Value + instruction.Imm));
			targetAddress = static_cast<unsigned long long>(static_cast<long long>(bundlePc.value()) + instruction->Imm);
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
				throw InvalidOperationException(; break; std::format("Opcode {0:s} ({1:s}) materialized {2:s} ", instruction->CanonicalOpcode, instruction->Class, microOp->GetType()->Name) + L"without an explicit canonical decode publication policy. " + L"Declare whether the MicroOp self-publishes canonical facts or requires projector publication.")
		}
		return temp_var187;
	}

	std::wstring DecodedBundleTransportProjector::BuildTrapReason(std::runtime_error exception)
	{
		std::shared_ptr<InvalidOpcodeException> invalidOpcodeException = std::dynamic_pointer_cast<InvalidOpcodeException>(exception);
		if (invalidOpcodeException != nullptr)
		{
			std::wstring opcodeKind = invalidOpcodeException->isProhibited() ? L"Prohibited opcode" : L"Invalid opcode";
			return invalidOpcodeException->getSlotIndex() >= 0 ? std::format("{0:s} at slot {1:s}: {2:s}", opcodeKind, invalidOpcodeException->getSlotIndex(), invalidOpcodeException->what()) : std::format("{0:s}: {1:s}", opcodeKind, invalidOpcodeException->what());
		}

		std::shared_ptr<DecodeProjectionFaultException> projectionException = std::dynamic_pointer_cast<DecodeProjectionFaultException>(exception);
		if (projectionException != nullptr)
		{
			return std::format("Decode projection fault: {0:s}", projectionException->Message);
		}

		return std::format("Decode exception: {0:s}", exception.what());
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
		switch (microOp)
		{
// C# TO C++ CONVERTER TASK: The following 'case' pattern variable is not converted by C# to C++ Converter:
			case LoadMicroOp loadMicroOp:
				loadMicroOp::InitializeMetadata();
				break;

// C# TO C++ CONVERTER TASK: The following 'case' pattern variable is not converted by C# to C++ Converter:
			case StoreMicroOp storeMicroOp:
				storeMicroOp::InitializeMetadata();
				break;

// C# TO C++ CONVERTER TASK: The following 'case' pattern variable is not converted by C# to C++ Converter:
			case AtomicMicroOp atomicMicroOp:
				atomicMicroOp::InitializeMetadata();
				break;

// C# TO C++ CONVERTER TASK: The following 'case' pattern variable is not converted by C# to C++ Converter:
			case DmaStreamComputeStatusMicroOp statusMicroOp:
				statusMicroOp::RefreshOwnerDependentMetadata();
				break;

// C# TO C++ CONVERTER TASK: The following 'case' pattern variable is not converted by C# to C++ Converter:
			case DmaStreamComputeQueryCapsMicroOp queryCapsMicroOp:
				queryCapsMicroOp::RefreshOwnerDependentMetadata();
				break;

// C# TO C++ CONVERTER TASK: The following 'case' pattern variable is not converted by C# to C++ Converter:
			case SystemDeviceCommandMicroOp systemDeviceCommandMicroOp:
				systemDeviceCommandMicroOp::InitializeMetadata();
				break;

// C# TO C++ CONVERTER TASK: The following 'case' pattern variable is not converted by C# to C++ Converter:
			case MatrixTileMicroOp matrixTileMicroOp:
				matrixTileMicroOp::InitializeMetadata();
				break;
		}
	}

	void DecodedBundleTransportProjector::ApplyCanonicalExecutionOwnerProjection(std::shared_ptr<MicroOp> microOp, std::shared_ptr<InstructionSlotMetadata> slotMetadata)
	{
		int ownerVirtualThreadId = slotMetadata->VirtualThreadId->Value;
		microOp->setVirtualThreadId(ownerVirtualThreadId);
		microOp->setOwnerThreadId(ownerVirtualThreadId);
		microOp->setOwnerContextId(ResolveProjectedOwnerContextId(slotMetadata, ownerVirtualThreadId));

		unsigned long long projectedDomainTag = ResolveProjectedDomainTag(slotMetadata);
		if (projectedDomainTag != microOp->getPlacement().DomainTag)
		{
// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
//			microOp.Placement = microOp.Placement with { DomainTag = projectedDomainTag };
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
		std::shared_ptr<MicroOpAdmissionMetadata> canonicalAdmissionMetadata = ((slotMetadata->SlotMetadata != nullptr) ? slotMetadata->SlotMetadata : Core::SlotMetadata::getDefault()).AdmissionMetadata;

		return canonicalAdmissionMetadata->OwnerContextId != UnsetOwnerContextSentinel ? canonicalAdmissionMetadata->OwnerContextId : ownerVirtualThreadId;
	}

	unsigned long long DecodedBundleTransportProjector::ResolveProjectedDomainTag(std::shared_ptr<InstructionSlotMetadata> slotMetadata)
	{
		std::shared_ptr<MicroOpAdmissionMetadata> canonicalAdmissionMetadata = ((slotMetadata->SlotMetadata != nullptr) ? slotMetadata->SlotMetadata : Core::SlotMetadata::getDefault()).AdmissionMetadata;

		return canonicalAdmissionMetadata->DomainTag != 0 ? canonicalAdmissionMetadata->DomainTag : canonicalAdmissionMetadata->Placement.DomainTag;
	}

	unsigned short DecodedBundleTransportProjector::ToLegacyDecoderField(unsigned char regId)
	{
		return regId == VLIW_Instruction::NoArchReg ? VLIW_Instruction::NoReg : regId;
	}
}
