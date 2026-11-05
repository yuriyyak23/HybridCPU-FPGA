#pragma once

#include "DecodedBundleDescriptor.h"
#include "DecodedInstructionBundle.h"
#include "../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../Pipeline/MicroOps/Types/MicroOp.h"
#include "../Contracts/CompilerTransport/VliwBundleAnnotations.h"
#include "../Frontend/Decode/VliwDecoderV4Bridge/VliwDecoderV4.h"
#include "../../../NonRTL/Arch/InvalidOpcodeException.h"
#include "IDecoderFrontend.h"
#include "../Pipeline/MicroOps/Types/MicroOp.IO.h"
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>
#include <optional>
#include <format>
#include <memory>
#include <span>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	/// <summary>
	/// Canonical decode-to-runtime transport projector.
	/// Builds the published decoded-bundle transport facts directly from canonical
	/// decode outputs so the main runtime path no longer depends on a legacy
	/// slot-carrier materializer ABI.
	/// </summary>
	class DecodedBundleTransportProjector final
	{
	public:
		static DecodedBundleTransportFacts BuildCanonicalTransportFacts(std::span<const VLIW_Instruction> rawSlots, std::shared_ptr<DecodedInstructionBundle> canonicalBundle, std::optional<DecodedBundleDependencySummary> &dependencySummary);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static MicroOp?[] BuildCanonicalCarrierBundleForTesting(std::span<const VLIW_Instruction> rawSlots, DecodedInstructionBundle canonicalBundle)
		static std::vector<std::shared_ptr<MicroOp>> BuildCanonicalCarrierBundleForTesting(std::span<const VLIW_Instruction> rawSlots, std::shared_ptr<DecodedInstructionBundle> canonicalBundle);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildFallbackTransportFacts(std::span<const VLIW_Instruction> rawSlots, VliwDecoderV4 slotDecoder, InvalidOpcodeException bundleDecodeException, ulong bundlePc, VliwBundleAnnotations? bundleAnnotations = null)
		static DecodedBundleTransportFacts BuildFallbackTransportFacts(std::span<const VLIW_Instruction> rawSlots, std::shared_ptr<VliwDecoderV4> slotDecoder, std::shared_ptr<InvalidOpcodeException> bundleDecodeException, unsigned long long bundlePc, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static MicroOp?[] BuildFallbackCarrierBundleForTesting(std::span<const VLIW_Instruction> rawSlots, IDecoderFrontend slotDecoder, InvalidOpcodeException bundleDecodeException, ulong bundlePc, System.Nullable<VliwBundleAnnotations> bundleAnnotations = null)
		static std::vector<std::shared_ptr<MicroOp>> BuildFallbackCarrierBundleForTesting(std::span<const VLIW_Instruction> rawSlots, std::shared_ptr<IDecoderFrontend> slotDecoder, std::shared_ptr<InvalidOpcodeException> bundleDecodeException, unsigned long long bundlePc, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations = nullptr);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static MicroOp?[] ProjectCanonicalCarrierBundle(std::span<const VLIW_Instruction> rawSlots, DecodedInstructionBundle canonicalBundle)
		static std::vector<std::shared_ptr<MicroOp>> ProjectCanonicalCarrierBundle(std::span<const VLIW_Instruction> rawSlots, std::shared_ptr<DecodedInstructionBundle> canonicalBundle);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static MicroOp?[] ProjectFallbackCarrierBundle(std::span<const VLIW_Instruction> rawSlots, IDecoderFrontend slotDecoder, InvalidOpcodeException bundleDecodeException, ulong bundlePc, System.Nullable<VliwBundleAnnotations> bundleAnnotations = null)
		static std::vector<std::shared_ptr<MicroOp>> ProjectFallbackCarrierBundle(std::span<const VLIW_Instruction> rawSlots, std::shared_ptr<IDecoderFrontend> slotDecoder, std::shared_ptr<InvalidOpcodeException> bundleDecodeException, unsigned long long bundlePc, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations = nullptr);

		static std::shared_ptr<MicroOp> CreateEmptySlot(VLIW_Instruction const rawInstruction, std::shared_ptr<InstructionSlotMetadata> slotMetadata = nullptr);

		static std::shared_ptr<MicroOp> CreateFromDecodedInstruction(VLIW_Instruction const rawInstruction, std::shared_ptr<DecodedInstruction> decodedInstruction, unsigned long long bundlePc);

		static std::shared_ptr<MicroOp> CreateFromInstruction(VLIW_Instruction const rawInstruction, std::shared_ptr<InstructionIR> instruction, std::shared_ptr<InstructionSlotMetadata> slotMetadata = nullptr, std::optional<unsigned long long> bundlePc = std::nullopt);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static TrapMicroOp CreateCanonicalTrapMicroOp(uint opCode, in InstructionIR instruction, string? trapReason = null, InstructionSlotMetadata slotMetadata = default, int projectedMemoryBankIntent = -1)
		static std::shared_ptr<TrapMicroOp> CreateCanonicalTrapMicroOp(unsigned int opCode, std::shared_ptr<InstructionIR> instruction, const std::wstring &trapReason = L"", std::shared_ptr<InstructionSlotMetadata> slotMetadata = nullptr, int projectedMemoryBankIntent = -1);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static TrapMicroOp CreateTrapMicroOp(uint opCode, string? trapReason = null, InstructionSlotMetadata slotMetadata = default, int projectedMemoryBankIntent = -1)
		static std::shared_ptr<TrapMicroOp> CreateTrapMicroOp(unsigned int opCode, const std::wstring &trapReason = L"", std::shared_ptr<InstructionSlotMetadata> slotMetadata = nullptr, int projectedMemoryBankIntent = -1);

	public:
		static int ResolveProjectedTrapMemoryBankIntent(VLIW_Instruction const rawInstruction, std::shared_ptr<InstructionIR> instruction);

	private:
		static VLIW_Instruction ProjectCanonicalMaterializationInstruction(VLIW_Instruction const rawInstruction, std::shared_ptr<InstructionIR> instruction, std::optional<unsigned long long> &bundlePc);

		static unsigned long long ResolveCanonicalAuxData(std::shared_ptr<InstructionIR> instruction, VLIW_Instruction const contextInstruction, std::optional<unsigned long long> &bundlePc);

		static bool TryResolveCanonicalStaticBranchTarget(std::shared_ptr<InstructionIR> instruction, std::optional<unsigned long long> &bundlePc, unsigned long long &targetAddress);

		static bool RequiresCanonicalDecodeProjection(std::shared_ptr<MicroOp> microOp, std::shared_ptr<InstructionIR> instruction);

		static std::wstring BuildTrapReason(std::runtime_error exception);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static InstructionSlotMetadata ResolveFallbackSlotMetadata(VliwBundleAnnotations? bundleAnnotations, int slotIndex)
		static std::shared_ptr<InstructionSlotMetadata> ResolveFallbackSlotMetadata(std::shared_ptr<VliwBundleAnnotations> bundleAnnotations, int slotIndex);

		static void RefreshOwnerDependentMaterializedResourceFacts(std::shared_ptr<MicroOp> microOp);

		static void ApplyCanonicalExecutionOwnerProjection(std::shared_ptr<MicroOp> microOp, std::shared_ptr<InstructionSlotMetadata> slotMetadata);

		static void ApplyCanonicalCompatibilitySlotMetadataProjection(std::shared_ptr<MicroOp> microOp, std::shared_ptr<InstructionSlotMetadata> slotMetadata);

		static int ResolveProjectedOwnerContextId(std::shared_ptr<InstructionSlotMetadata> slotMetadata, int ownerVirtualThreadId);

		static unsigned long long ResolveProjectedDomainTag(std::shared_ptr<InstructionSlotMetadata> slotMetadata);

		static unsigned short ToLegacyDecoderField(unsigned char regId);
	};
}
