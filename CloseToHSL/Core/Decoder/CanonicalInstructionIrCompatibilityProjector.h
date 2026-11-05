#pragma once

#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../Contracts/CompilerTransport/VliwBundleAnnotations.h"
#include "DecodedInstructionBundle.h"
#include "CanonicalDecodedContracts.h"
#include "CanonicalDecodedContracts.h"
#include "DeclarativeDecoderStages.h"
#include "DecoderRuntimeSidebandValidator.h"
#include "../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include <string>
#include <vector>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include <span>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	/// <summary>
	/// RF-05 compatibility boundary. It consumes only frozen declarative decode
	/// output and projects the retained public <see cref="InstructionIR"/> adapter.
	/// It never creates executable work, admission identity or replay state.
	/// </summary>
	class CanonicalInstructionIrCompatibilityProjector final
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static DecodedInstructionBundle ProjectBundle(std::span<const VLIW_Instruction> rawSlots, VliwBundleAnnotations? bundleAnnotations, DeclarativeDecodedBundle declarativeBundle, ulong bundleAddress, ulong bundleSerial)
		static std::shared_ptr<DecodedInstructionBundle> ProjectBundle(std::span<const VLIW_Instruction> rawSlots, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations, std::shared_ptr<DeclarativeDecodedBundle> declarativeBundle, unsigned long long bundleAddress, unsigned long long bundleSerial);

		static std::shared_ptr<InstructionIR> ProjectInstruction(std::shared_ptr<CanonicalDecodedInstruction> canonicalSlot, std::shared_ptr<DeclarativeDecodedSlot> declarativeSlot, std::shared_ptr<RuntimeSidebandProjection> runtimeSideband);

		static std::shared_ptr<InstructionIR> ProjectReplayInstruction(VLIW_Instruction const rawSlot, std::shared_ptr<CanonicalDecodedInstruction> canonicalSlot, std::shared_ptr<InstructionSlotMetadata> slotMetadata);

	private:
		static std::shared_ptr<InstructionIR> ProjectInstruction(std::shared_ptr<CanonicalDecodedInstruction> canonicalSlot, const std::optional<std::shared_ptr<VectorInstructionPayload>> &vectorPayload, std::shared_ptr<RuntimeSidebandProjection> runtimeSideband);

		static std::optional<std::shared_ptr<MatrixTileInstructionIrProjection>> CreateMatrixProjection(unsigned short opcode, const std::optional<std::shared_ptr<VectorInstructionPayload>> &vectorPayload, long long immediate);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static InstructionSlotMetadata ResolveSlotMetadata(VliwBundleAnnotations? bundleAnnotations, int slotIndex)
		static std::shared_ptr<InstructionSlotMetadata> ResolveSlotMetadata(std::shared_ptr<VliwBundleAnnotations> bundleAnnotations, int slotIndex);
	};

}
