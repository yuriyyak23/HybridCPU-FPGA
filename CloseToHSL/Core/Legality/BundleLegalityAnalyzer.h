#pragma once

#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../Decoder/DecodedInstructionBundle.h"
#include "../Pipeline/Metadata/BundleWidth.h"
#include "BundleLegalityDescriptor.h"
#include "../Runtime/Domains/Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.BundleLegalityDescriptor.h"
#include "../Decoder/DecodedBundleDescriptor.h"
#include "../Pipeline/Safety/ResourceBitset.h"
#include "../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../../NonRTL/Arch/InstructionClass.h"
#include "../Pipeline/Scheduling/SlotPlacementMetadata.h"
#include "DecodedSlotLegality.h"
#include <vector>
#include <cmath>
#include <limits>
#include <optional>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
using namespace ::YAKSys_Hybrid_CPU::Core;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::Core::Legality
{
	/// <summary>
	/// Canonical Phase 03 legality analyzer over decoded IR bundles.
	/// Classifies slot occupancy, typed-slot facts, and bundle-local dependency
	/// summaries from <see cref="DecodedInstructionBundle"/> without reintroducing
	/// legacy <see cref="MicroOp"/> materialization.
	/// </summary>
	class BundleLegalityAnalyzer final : public std::enable_shared_from_this<BundleLegalityAnalyzer>
	{
	private:
		static constexpr int BundleWidth = BundleSlotCount;

	public:
		std::shared_ptr<BundleLegalityDescriptor> Analyze(std::shared_ptr<DecodedInstructionBundle> bundle);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static DecodedBundleDependencySummary BuildDependencySummary(IReadOnlyList<DecodedBundleSlotDescriptor> slots, ulong readRegisterMask, ulong writeRegisterMask, ResourceBitset aggregateResourceMask)
		static DecodedBundleDependencySummary BuildDependencySummary(const std::vector<DecodedBundleSlotDescriptor> &slots, unsigned long long readRegisterMask, unsigned long long writeRegisterMask, ResourceBitset aggregateResourceMask);

		static SlotClass ClassifySlotClass(InstructionClass instructionClass);

		static SlotPlacementMetadata BuildCanonicalPlacement(InstructionClass instructionClass, unsigned long long domainTag = 0);

		static SlotPlacementMetadata BuildCanonicalPlacement(SlotClass slotClass, unsigned long long domainTag = 0);

	private:
		static SlotClass ClassifySlotClass(std::shared_ptr<InstructionIR> instruction);

		static DecodedBundleSlotDescriptor CreateEmptySlotDescriptor(int slotIndex);

		static DecodedBundleSlotDescriptor CreateCanonicalSlotDescriptor(int slotIndex, std::shared_ptr<DecodedInstruction> slot, std::shared_ptr<InstructionIR> instruction, SlotClass slotClass);

		static void AccumulateDependencyInputs(DecodedBundleSlotDescriptor const slot, unsigned long long &readRegisterMask, unsigned long long &writeRegisterMask, ResourceBitset &aggregateResourceMask);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static DecodedSlotLegality[] BuildSlotLegalities(IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, System.Nullable<DecodedBundleDependencySummary> dependencySummary)
		static std::vector<DecodedSlotLegality> BuildSlotLegalities(const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, std::optional<DecodedBundleDependencySummary> &dependencySummary);

		static bool IsPinnedSlotClass(SlotClass slotClass);

	public:
		static bool IsMemoryLikeClass(InstructionClass instructionClass);

		static bool MayWriteArchitecturalRegister(std::shared_ptr<InstructionIR> instruction);

		static const std::vector<int> GetCanonicalReadRegisters(std::shared_ptr<InstructionIR> instruction);

		static const std::vector<int> GetCanonicalWriteRegisters(std::shared_ptr<InstructionIR> instruction, bool writesRegister);

	private:
		static const std::vector<int> BuildRegisterList(unsigned char first, unsigned char second = 0);

		static bool TryResolvePublishedSystemReadRegisters(std::shared_ptr<InstructionIR> instruction, std::vector<int> &readRegisters);

		static bool TryResolvePublishedCsrReadRegisters(std::shared_ptr<InstructionIR> instruction, std::vector<int> &readRegisters);

		static bool HasArchitecturalRegister(unsigned char registerId);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool ContainsArchitecturalRegister(IReadOnlyList<int> registers, byte registerId)
		static bool ContainsArchitecturalRegister(const std::vector<int> &registers, unsigned char registerId);

		static bool ReadsRs1(std::shared_ptr<InstructionIR> instruction);

		static bool ReadsRs2(std::shared_ptr<InstructionIR> instruction);

		static bool IsScalarRegisterFreeVectorAluContour(std::shared_ptr<InstructionIR> instruction);

		static bool IsCurrentLane6DmaStreamContour(std::shared_ptr<InstructionIR> instruction);

		static bool IsCurrentLane6DmaStreamRegisterResultContour(std::shared_ptr<InstructionIR> instruction);

		static bool IsCurrentL7SdcRegisterResultContour(std::shared_ptr<InstructionIR> instruction);

		static ResourceBitset BuildStructuralResourceMask(DecodedBundleSlotDescriptor const slot);

		static unsigned char ResolvePinnedLaneId(SlotClass slotClass, SlotPinningKind pinningKind);

		static unsigned long long EncodeSlotPair(int sourceSlotIndex, int targetSlotIndex);

		static bool HasControlConflict(DecodedBundleSlotDescriptor left, DecodedBundleSlotDescriptor right);

		static bool HasSystemBarrierConflict(DecodedBundleSlotDescriptor left, DecodedBundleSlotDescriptor right);

		static bool HasCoarseMemoryConflict(DecodedBundleSlotDescriptor left, DecodedBundleSlotDescriptor right);

		static bool HasPinnedLaneConflict(DecodedBundleSlotDescriptor left, DecodedBundleSlotDescriptor right);

		static bool IsScalarClusterEligible(DecodedBundleSlotDescriptor slot);

		static bool RequiresSingletonLane(SlotClass slotClass);
	};
}
