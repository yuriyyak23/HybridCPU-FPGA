#include "BundleLegalityAnalyzer.h"
#include "../Decoder/DecodedInstruction.h"
#include "../Pipeline/MicroOps/Control/MicroOp.Control.h"
#include "../Pipeline/MicroOps/Types/MicroOp.h"
#include "../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileResourceContour.h"
#include "../Architecture/Registers/Architectural/ArchRegisterTripletEncoding.h"
#include "../Architecture/Common/StreamEngineId.h"
#include "../Diagnostics/YAKSys_Hybrid_CPU.Core.InstructionRegistry.h"
#include "../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include "../../../NonRTL/Arch/OpcodeInfo.Types.h"

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
using namespace ::YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Legality
{
	using BundleFlags = YAKSys_Hybrid_CPU::Core::DecodedBundleFlags;

	std::shared_ptr<BundleLegalityDescriptor> BundleLegalityAnalyzer::Analyze(std::shared_ptr<DecodedInstructionBundle> bundle)
	{
		ArgumentNullException::ThrowIfNull(bundle);

		unsigned char occupiedSlotMask = 0;
		std::vector<DecodedBundleSlotDescriptor> slotDescriptors(bundle->getSlotCount());
		unsigned long long readRegisterMask = 0;
		unsigned long long writeRegisterMask = 0;
		ResourceBitset aggregateResourceMask = ResourceBitset::Zero;
		unsigned char aluClassMask = 0;
		unsigned char lsuClassMask = 0;
		unsigned char dmaStreamClassMask = 0;
		unsigned char matrixTileStreamClassMask = 0;
		unsigned char branchControlMask = 0;
		unsigned char systemSingletonMask = 0;
		unsigned char unclassifiedMask = 0;
		unsigned char pinnedSlotMask = 0;
		unsigned char flexibleSlotMask = 0;
		BundleFlags flags = BundleFlags::None;
		int minVirtualThreadId = std::numeric_limits<int>::max();
		int maxVirtualThreadId = std::numeric_limits<int>::min();
		bool hasNonEmptySlot = false;

		for (int slotIndex = 0; slotIndex < bundle->getSlotCount(); slotIndex++)
		{
			std::shared_ptr<DecodedInstruction> slot = bundle->GetDecodedSlot(slotIndex);
			unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
			if (!slot->IsOccupied)
			{
				slotDescriptors[slotIndex] = CreateEmptySlotDescriptor(slotIndex);
				flags = static_cast<BundleFlags>(static_cast<int>(flags) | static_cast<int>(BundleFlags::HasEmptyOrNopSlots));
				continue;
			}

			occupiedSlotMask |= slotBit;
			flags = static_cast<BundleFlags>(static_cast<int>(flags) | static_cast<int>(BundleFlags::HasValidSlots));
			hasNonEmptySlot = true;

			std::shared_ptr<InstructionIR> instruction = slot->RequireInstruction();
			SlotClass slotClass = ClassifySlotClass(instruction);
			DecodedBundleSlotDescriptor slotDescriptor = CreateCanonicalSlotDescriptor(slotIndex, slot, instruction, slotClass);
			slotDescriptors[slotIndex] = slotDescriptor;
			AccumulateDependencyInputs(slotDescriptor, readRegisterMask, writeRegisterMask, aggregateResourceMask);
			minVirtualThreadId = std::min(minVirtualThreadId, slotDescriptor.getVirtualThreadId());
			maxVirtualThreadId = std::max(maxVirtualThreadId, slotDescriptor.getVirtualThreadId());
			switch (slotClass)
			{
				case SlotClass::AluClass:
					aluClassMask |= slotBit;
					break;
				case SlotClass::LsuClass:
					lsuClassMask |= slotBit;
					break;
				case SlotClass::DmaStreamClass:
					dmaStreamClassMask |= slotBit;
					break;
				case SlotClass::MatrixTileStreamClass:
					matrixTileStreamClassMask |= slotBit;
					break;
				case SlotClass::BranchControl:
					branchControlMask |= slotBit;
					break;
				case SlotClass::SystemSingleton:
					systemSingletonMask |= slotBit;
					break;
				default:
					unclassifiedMask |= slotBit;
					break;
			}

			if (IsPinnedSlotClass(slotClass))
			{
				pinnedSlotMask |= slotBit;
				flags = static_cast<BundleFlags>(static_cast<int>(flags) | static_cast<int>(BundleFlags::HasPinnedOps));
			}
			else
			{
				flexibleSlotMask |= slotBit;
			}

			if (instruction->Class == InstructionClass::ControlFlow)
			{
				flags = static_cast<BundleFlags>(static_cast<int>(flags) | static_cast<int>(BundleFlags::HasControlFlow));
			}

			if (slotDescriptor.isMemoryOp())
			{
				flags = static_cast<BundleFlags>(static_cast<int>(flags) | static_cast<int>(BundleFlags::HasMemoryOps));
			}

			if (MayWriteArchitecturalRegister(instruction))
			{
				flags = static_cast<BundleFlags>(static_cast<int>(flags) | static_cast<int>(BundleFlags::HasRegisterWrites));
			}
		}

		DecodedBundleTypedSlotFacts typedSlotFacts = DecodedBundleTypedSlotFacts(aluClassMask, lsuClassMask, dmaStreamClassMask, matrixTileStreamClassMask, branchControlMask, systemSingletonMask, unclassifiedMask, pinnedSlotMask, flexibleSlotMask);

		std::optional<DecodedBundleDependencySummary> dependencySummary = occupiedSlotMask != 0 ? std::optional<DecodedBundleDependencySummary>(BuildDependencySummary(slotDescriptors, readRegisterMask, writeRegisterMask, aggregateResourceMask)) : std::nullopt;
		std::vector<DecodedSlotLegality> slotLegalities = BuildSlotLegalities(slotDescriptors, dependencySummary);
		unsigned char maxVirtualThreadSpan = 0;
		if (hasNonEmptySlot)
		{
			maxVirtualThreadSpan = static_cast<unsigned char>(maxVirtualThreadId - minVirtualThreadId + 1);
			if (maxVirtualThreadSpan > 1)
			{
				flags = static_cast<BundleFlags>(static_cast<int>(flags) | static_cast<int>(BundleFlags::HasCrossThreadSpan));
			}
		}

		return std::make_shared<BundleLegalityDescriptor>(bundle->getBundleAddress(), bundle->getBundleSerial(), occupiedSlotMask, typedSlotFacts, flags, maxVirtualThreadSpan, slotLegalities, dependencySummary);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static DecodedBundleDependencySummary BuildDependencySummary(IReadOnlyList<DecodedBundleSlotDescriptor> slots, ulong readRegisterMask, ulong writeRegisterMask, ResourceBitset aggregateResourceMask)
	YAKSys_Hybrid_CPU::Core::DecodedBundleDependencySummary BundleLegalityAnalyzer::BuildDependencySummary(const std::vector<DecodedBundleSlotDescriptor> &slots, unsigned long long readRegisterMask, unsigned long long writeRegisterMask, ResourceBitset aggregateResourceMask)
	{
		unsigned long long rawDependencyMask = 0;
		unsigned long long wawDependencyMask = 0;
		unsigned long long warDependencyMask = 0;
		unsigned long long controlConflictMask = 0;
		unsigned long long memoryConflictMask = 0;
		unsigned long long systemBarrierConflictMask = 0;
		unsigned long long pinnedLaneConflictMask = 0;
		unsigned char scalarClusterEligibleMask = 0;

		for (int slotIndex = 0; slotIndex < BundleWidth; slotIndex++)
		{
			DecodedBundleSlotDescriptor slot = slots[slotIndex];
			if (IsScalarClusterEligible(slot))
			{
				scalarClusterEligibleMask |= static_cast<unsigned char>(1 << slotIndex);
			}

			if (slot.isEmptyOrNop())
			{
				continue;
			}

			for (int dependentSlotIndex = slotIndex + 1; dependentSlotIndex < BundleWidth; dependentSlotIndex++)
			{
				DecodedBundleSlotDescriptor dependentSlot = slots[dependentSlotIndex];
				if (dependentSlot.isEmptyOrNop())
				{
					continue;
				}

				if (DecodedBundleSlotDescriptor::HasRegisterIntersection(slot, dependentSlot, slot.getWriteRegisters(), dependentSlot.getReadRegisters()))
				{
					rawDependencyMask |= EncodeSlotPair(slotIndex, dependentSlotIndex);
				}

				if (DecodedBundleSlotDescriptor::HasRegisterIntersection(slot, dependentSlot, slot.getWriteRegisters(), dependentSlot.getWriteRegisters()))
				{
					wawDependencyMask |= EncodeSlotPair(slotIndex, dependentSlotIndex);
				}

				if (DecodedBundleSlotDescriptor::HasRegisterIntersection(slot, dependentSlot, slot.getReadRegisters(), dependentSlot.getWriteRegisters()))
				{
					warDependencyMask |= EncodeSlotPair(slotIndex, dependentSlotIndex);
				}

				if (HasControlConflict(slot, dependentSlot))
				{
					controlConflictMask |= EncodeSlotPair(slotIndex, dependentSlotIndex);

					if (HasSystemBarrierConflict(slot, dependentSlot))
					{
						systemBarrierConflictMask |= EncodeSlotPair(slotIndex, dependentSlotIndex);
					}
				}

				if (HasCoarseMemoryConflict(slot, dependentSlot))
				{
					memoryConflictMask |= EncodeSlotPair(slotIndex, dependentSlotIndex);
				}

				if (HasPinnedLaneConflict(slot, dependentSlot))
				{
					pinnedLaneConflictMask |= EncodeSlotPair(slotIndex, dependentSlotIndex);
				}
			}
		}

		return DecodedBundleDependencySummary(readRegisterMask, writeRegisterMask, aggregateResourceMask, rawDependencyMask, wawDependencyMask, warDependencyMask, controlConflictMask, memoryConflictMask, scalarClusterEligibleMask, systemBarrierConflictMask, pinnedLaneConflictMask);
	}

		YAKSys_Hybrid_CPU::Core::SlotClass BundleLegalityAnalyzer::ClassifySlotClass(InstructionClass instructionClass)
	{
		switch (instructionClass)
		{
		case InstructionClass::ScalarAlu: return SlotClass::AluClass;
		case InstructionClass::Memory: return SlotClass::LsuClass;
		case InstructionClass::Atomic: return SlotClass::LsuClass;
		case InstructionClass::ControlFlow: return SlotClass::BranchControl;
		case InstructionClass::System: return SlotClass::SystemSingleton;
		case InstructionClass::Csr: return SlotClass::SystemSingleton;
		case InstructionClass::SmtVt: return SlotClass::SystemSingleton;
		case InstructionClass::Vmx: return SlotClass::SystemSingleton;
		default: return SlotClass::Unclassified;
		}
	}

YAKSys_Hybrid_CPU::Core::SlotPlacementMetadata BundleLegalityAnalyzer::BuildCanonicalPlacement(InstructionClass instructionClass, unsigned long long domainTag)
	{
		SlotClass slotClass = ClassifySlotClass(instructionClass);
		return BuildCanonicalPlacement(slotClass, domainTag);
	}

	YAKSys_Hybrid_CPU::Core::SlotPlacementMetadata BundleLegalityAnalyzer::BuildCanonicalPlacement(SlotClass slotClass, unsigned long long domainTag)
	{
		SlotPinningKind pinningKind = IsPinnedSlotClass(slotClass) ? SlotPinningKind::HardPinned : SlotPinningKind::ClassFlexible;

		SlotPlacementMetadata temp_var = SlotPlacementMetadata();
		temp_var.RequiredSlotClass = slotClass;
		temp_var.PinningKind = pinningKind;
		temp_var.PinnedLaneId = ResolvePinnedLaneId(slotClass, pinningKind);
		temp_var.DomainTag = domainTag;
		return temp_var;
	}

	YAKSys_Hybrid_CPU::Core::SlotClass BundleLegalityAnalyzer::ClassifySlotClass(std::shared_ptr<InstructionIR> instruction)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (MatrixTileResourceContour.IsMatrixTileMemoryOpcode(unchecked((uint)(ushort)instruction.CanonicalOpcode)))
		if (MatrixTileResourceContour::IsMatrixTileMemoryOpcode(static_cast<unsigned int>(instruction->CanonicalOpcode->Value)))
		{
			return SlotClass::MatrixTileStreamClass;
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (MatrixTileResourceContour.IsMatrixTileComputeOpcode(unchecked((uint)(ushort)instruction.CanonicalOpcode)))
		if (MatrixTileResourceContour::IsMatrixTileComputeOpcode(static_cast<unsigned int>(instruction->CanonicalOpcode->Value)))
		{
			return SlotClass::AluClass;
		}

		if (IsCurrentLane6DmaStreamContour(instruction))
		{
			return SlotClass::DmaStreamClass;
		}

		return ClassifySlotClass(instruction->Class);
	}

	YAKSys_Hybrid_CPU::Core::DecodedBundleSlotDescriptor BundleLegalityAnalyzer::CreateEmptySlotDescriptor(int slotIndex)
	{
		SlotPlacementMetadata placement = {.RequiredSlotClass = SlotClass::Unclassified, .PinningKind = SlotPinningKind::HardPinned, .PinnedLaneId = 0, .DomainTag = 0};

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return new DecodedBundleSlotDescriptor( microOp: null!, slotIndex: (byte)slotIndex, virtualThreadId: 0, ownerThreadId: 0, opCode: 0, readRegisters: Array.Empty<int>(), writeRegisters: Array.Empty<int>(), writesRegister: false, isMemoryOp: false, isControlFlow: false, placement: placement, memoryBankIntent: -1, isFspInjected: false, isEmptyOrNop: true, isVectorOp: false);
		return DecodedBundleSlotDescriptor(nullptr, static_cast<unsigned char>(slotIndex), 0, 0, 0, std::vector<int>{}, std::vector<int>{}, false, false, false, placement, -1, false, true, false);
	}

	YAKSys_Hybrid_CPU::Core::DecodedBundleSlotDescriptor BundleLegalityAnalyzer::CreateCanonicalSlotDescriptor(int slotIndex, std::shared_ptr<DecodedInstruction> slot, std::shared_ptr<InstructionIR> instruction, SlotClass slotClass)
	{
		bool writesRegister = MayWriteArchitecturalRegister(instruction);
		bool isMemoryOp = IsMemoryLikeClass(instruction->Class) && !IsCurrentLane6DmaStreamRegisterResultContour(instruction);
		bool isControlFlow = instruction->Class == InstructionClass::ControlFlow;
		SlotPlacementMetadata placement = BuildCanonicalPlacement(slotClass, 0);

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return new DecodedBundleSlotDescriptor( microOp: null!, slotIndex: (byte)slotIndex, virtualThreadId: slot.SlotMetadata.VirtualThreadId, ownerThreadId: 0, opCode: (uint)instruction.CanonicalOpcode, readRegisters: GetCanonicalReadRegisters(instruction), writeRegisters: GetCanonicalWriteRegisters(instruction, writesRegister), writesRegister: writesRegister, isMemoryOp: isMemoryOp, isControlFlow: isControlFlow, placement: placement, memoryBankIntent: isMemoryOp ? -1 : -1, isFspInjected: false, isEmptyOrNop: false, isVectorOp: Arch.OpcodeRegistry.IsVectorOp((uint)instruction.CanonicalOpcode));
		return DecodedBundleSlotDescriptor(nullptr, static_cast<unsigned char>(slotIndex), slot->SlotMetadata->VirtualThreadId, 0, static_cast<unsigned int>(instruction->CanonicalOpcode->Value), GetCanonicalReadRegisters(instruction), GetCanonicalWriteRegisters(instruction, writesRegister), writesRegister, isMemoryOp, isControlFlow, placement, isMemoryOp ? -1: -1, false, false, Arch::OpcodeRegistry::IsVectorOp(static_cast<unsigned int>(instruction->CanonicalOpcode->Value)));
	}

	void BundleLegalityAnalyzer::AccumulateDependencyInputs(DecodedBundleSlotDescriptor const slot, unsigned long long &readRegisterMask, unsigned long long &writeRegisterMask, ResourceBitset &aggregateResourceMask)
	{
		for (int i = 0; i < slot.getReadRegisters().size(); i++)
		{
			int registerId = slot.getReadRegisters()[i];
			if (static_cast<unsigned int>(registerId) < 64)
			{
				readRegisterMask |= 1ULL << registerId;
			}

			ArchRegId architecturalRegisterId;
			aggregateResourceMask = aggregateResourceMask | (ArchRegId::TryCreate(registerId, architecturalRegisterId) ? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(architecturalRegisterId)) : ResourceMaskBuilder::ForRegisterRead(registerId));
		}

		for (int i = 0; i < slot.getWriteRegisters().size(); i++)
		{
			int registerId = slot.getWriteRegisters()[i];
			if (static_cast<unsigned int>(registerId) < 64)
			{
				writeRegisterMask |= 1ULL << registerId;
			}

			ArchRegId architecturalRegisterId;
			aggregateResourceMask = aggregateResourceMask | (ArchRegId::TryCreate(registerId, architecturalRegisterId) ? ResourceMaskBuilder::ForArchitecturalRegisterWrite(std::make_shared<ArchRegId>(architecturalRegisterId)) : ResourceMaskBuilder::ForRegisterWrite(registerId));
		}

		aggregateResourceMask = aggregateResourceMask | BuildStructuralResourceMask(slot);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static DecodedSlotLegality[] BuildSlotLegalities(IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, System.Nullable<DecodedBundleDependencySummary> dependencySummary)
		std::vector<DecodedSlotLegality> BundleLegalityAnalyzer::BuildSlotLegalities(const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, std::optional<DecodedBundleDependencySummary> &dependencySummary)
	{
		std::vector<DecodedSlotLegality> slotLegalities;
		slotLegalities.reserve(BundleWidth);
		const unsigned char scalarGroupMask = dependencySummary ? dependencySummary->getScalarClusterEligibleMask() : 0;

		for (int slotIndex = 0; slotIndex < BundleWidth; slotIndex++)
		{
			const DecodedBundleSlotDescriptor &slot = slotDescriptors[slotIndex];
			if (slot.isEmptyOrNop())
			{
				slotLegalities.push_back(DecodedSlotLegality::CreateEmpty(slotIndex));
				continue;
			}

			if (dependencySummary)
			{
				SlotHazardQueryResult query = dependencySummary->QuerySlotHazards(static_cast<unsigned char>(slotIndex), scalarGroupMask);
				slotLegalities.push_back(DecodedSlotLegality::CreateOccupied(slotIndex, slot.getPlacement().RequiredSlotClass, query.getHardRejectPeers(), query.getNeedsCheckPeers(), query.getDominantEffectKind()));
			}
			else
			{
				slotLegalities.push_back(DecodedSlotLegality::CreateOccupied(slotIndex, slot.getPlacement().RequiredSlotClass));
			}
		}

		return slotLegalities;
	}
	bool BundleLegalityAnalyzer::IsPinnedSlotClass(SlotClass slotClass)
	{
		return slotClass == SlotClass::BranchControl || slotClass == SlotClass::SystemSingleton || slotClass == SlotClass::DmaStreamClass;
	}

	bool BundleLegalityAnalyzer::IsMemoryLikeClass(InstructionClass instructionClass)
	{
		return instructionClass == InstructionClass::Memory || instructionClass == InstructionClass::Atomic;
	}

	bool BundleLegalityAnalyzer::MayWriteArchitecturalRegister(std::shared_ptr<InstructionIR> instruction)
	{
		if (IsScalarRegisterFreeVectorAluContour(instruction) || instruction->Rd == ArchRegisterTripletEncoding::NoArchReg || instruction->Rd == 0)
		{
			return false;
		}

		std::shared_ptr<BranchMicroOp> branchMicroOp;
		if (instruction->Class == InstructionClass::ControlFlow && InstructionRegistry::TryCreatePublishedControlFlowMicroOp(instruction, branchMicroOp) && branchMicroOp)
		{
			return branchMicroOp->getWritesRegister();
		}

		VmxOperationKind vmxOperationKind;
		if (instruction->Class == InstructionClass::Vmx && InstructionRegistry::TryResolvePublishedVmxOperationKind(instruction, vmxOperationKind))
		{
			return vmxOperationKind == VmxOperationKind::VmRead;
		}

		if (IsCurrentLane6DmaStreamRegisterResultContour(instruction))
		{
			return true;
		}

		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(static_cast<unsigned int>(instruction->CanonicalOpcode->Value));
		if (opcodeInfo)
		{
			switch (instruction->Class)
			{
			case InstructionClass::ScalarAlu:
			case InstructionClass::Atomic:
			case InstructionClass::Csr:
				return true;
			case InstructionClass::System:
				return IsCurrentL7SdcRegisterResultContour(instruction) || opcodeInfo->isVector();
			case InstructionClass::Vmx:
				return false;
			case InstructionClass::Memory:
				return opcodeInfo->SerializationClass != SerializationClass::MemoryOrdered;
			case InstructionClass::ControlFlow:
			default:
				return false;
			}
		}

		switch (instruction->Class)
		{
		case InstructionClass::ScalarAlu:
		case InstructionClass::Atomic:
		case InstructionClass::Csr:
			return true;
		case InstructionClass::System:
			return IsCurrentL7SdcRegisterResultContour(instruction);
		case InstructionClass::Vmx:
			return false;
		case InstructionClass::Memory:
			return instruction->SerializationClass != SerializationClass::MemoryOrdered;
		case InstructionClass::ControlFlow:
		default:
			return false;
		}
	}

	const std::vector<int> BundleLegalityAnalyzer::GetCanonicalReadRegisters(std::shared_ptr<InstructionIR> instruction)
	{
		if (IsScalarRegisterFreeVectorAluContour(instruction))
		{
			return std::vector<int>{};
		}

		std::vector<int> csrReadRegisters;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: if (instruction.Class == InstructionClass.Csr && TryResolvePublishedCsrReadRegisters(in instruction, out IReadOnlyList<int>? csrReadRegisters))
		if (instruction->Class == InstructionClass::Csr && TryResolvePublishedCsrReadRegisters(instruction, csrReadRegisters))
		{
			return csrReadRegisters;
		}

		std::vector<int> readRegisters;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: if (instruction.Class == InstructionClass.System && TryResolvePublishedSystemReadRegisters(in instruction, out IReadOnlyList<int>? readRegisters))
		if (instruction->Class == InstructionClass::System && TryResolvePublishedSystemReadRegisters(instruction, readRegisters))
		{
			return readRegisters;
		}

		bool readsRs1 = ReadsRs1(instruction);
		bool readsRs2 = ReadsRs2(instruction);
		return BuildRegisterList(readsRs1 ? instruction->Rs1 : static_cast<unsigned char>(0), readsRs2 ? instruction->Rs2 : static_cast<unsigned char>(0));
	}

	const std::vector<int> BundleLegalityAnalyzer::GetCanonicalWriteRegisters(std::shared_ptr<InstructionIR> instruction, bool writesRegister)
	{
		if (IsScalarRegisterFreeVectorAluContour(instruction))
		{
			return std::vector<int>{};
		}

		return writesRegister ? BuildRegisterList(instruction->Rd) : std::vector<int>{};
	}

	const std::vector<int> BundleLegalityAnalyzer::BuildRegisterList(unsigned char first, unsigned char second)
	{
		bool hasFirst = HasArchitecturalRegister(first);
		bool hasSecond = HasArchitecturalRegister(second);

		if (hasFirst && hasSecond)
		{
			return std::vector<int> {static_cast<int>(first), static_cast<int>(second)};
		}
		if (hasFirst)
		{
			return std::vector<int> {static_cast<int>(first)};
		}
		if (hasSecond)
		{
			return std::vector<int> {static_cast<int>(second)};
		}

		return std::vector<int>{};
	}

	bool BundleLegalityAnalyzer::TryResolvePublishedSystemReadRegisters(std::shared_ptr<InstructionIR> instruction, std::vector<int> &readRegisters)
	{
		return InstructionRegistry::TryResolvePublishedSystemReadRegisters(instruction, readRegisters);
	}

	bool BundleLegalityAnalyzer::TryResolvePublishedCsrReadRegisters(std::shared_ptr<InstructionIR> instruction, std::vector<int> &readRegisters)
	{
		readRegisters = std::vector<int>{};

		if (instruction->Class != InstructionClass::Csr)
		{
			return false;
		}

		std::shared_ptr<CSRMicroOp> csrMicroOp;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!InstructionRegistry.TryCreatePublishedCsrMicroOp(in instruction, out CSRMicroOp? csrMicroOp) || csrMicroOp == null)
		if (!InstructionRegistry::TryCreatePublishedCsrMicroOp(instruction, csrMicroOp) || csrMicroOp == nullptr)
		{
			return false;
		}

		readRegisters = csrMicroOp->getReadRegisters();
		return true;
	}

	bool BundleLegalityAnalyzer::HasArchitecturalRegister(unsigned char registerId)
	{
		return registerId != 0 && registerId != ArchRegisterTripletEncoding::NoArchReg;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool ContainsArchitecturalRegister(IReadOnlyList<int> registers, byte registerId)
	bool BundleLegalityAnalyzer::ContainsArchitecturalRegister(const std::vector<int> &registers, unsigned char registerId)
	{
		if (!HasArchitecturalRegister(registerId))
		{
			return false;
		}

		for (int i = 0; i < registers.size(); i++)
		{
			if (registers[i] == registerId)
			{
				return true;
			}
		}

		return false;
	}

	bool BundleLegalityAnalyzer::ReadsRs1(std::shared_ptr<InstructionIR> instruction)
	{
		if (IsScalarRegisterFreeVectorAluContour(instruction))
		{
			return false;
		}

		std::shared_ptr<BranchMicroOp> branchMicroOp;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (instruction.Class == InstructionClass.ControlFlow && InstructionRegistry.TryCreatePublishedControlFlowMicroOp(in instruction, out BranchMicroOp? branchMicroOp) && branchMicroOp != null)
		if (instruction->Class == InstructionClass::ControlFlow && InstructionRegistry::TryCreatePublishedControlFlowMicroOp(instruction, branchMicroOp) && branchMicroOp != nullptr)
		{
			return ContainsArchitecturalRegister(branchMicroOp->getReadRegisters(), instruction->Rs1);
		}

		VmxOperationKind vmxOperationKind;
		if (instruction->Class == InstructionClass::Vmx && InstructionRegistry::TryResolvePublishedVmxOperationKind(instruction, vmxOperationKind))
		{
			return HasArchitecturalRegister(instruction->Rs1) && (vmxOperationKind == VmxOperationKind::VmRead || vmxOperationKind == VmxOperationKind::VmWrite || vmxOperationKind == VmxOperationKind::VmClear || vmxOperationKind == VmxOperationKind::VmPtrLd);
		}

		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(static_cast<unsigned int>(instruction->CanonicalOpcode->Value));
		if (opcodeInfo)
		{
			bool temp_var82;
			switch (instruction->Class)
			{
				case InstructionClass::Memory:
					temp_var82 = true;
					break;
				case InstructionClass::Atomic:
					temp_var82 = opcodeInfo.value().OperandCount != 0;
					break;
				case InstructionClass::ControlFlow:
					temp_var82 = HasArchitecturalRegister(instruction->Rs1);
					break;
				case InstructionClass::Csr:
					temp_var82 = opcodeInfo.value().OperandCount == 2;
					break;
				case InstructionClass::ScalarAlu:
					temp_var82 = opcodeInfo.value().OperandCount > 1;
					break;
				default:
					temp_var82 = HasArchitecturalRegister(instruction->Rs1);
					break;
			}
			return temp_var82;
		}

		bool temp_var83;
		switch (instruction->Class)
		{
			case InstructionClass::Memory:
				temp_var83 = HasArchitecturalRegister(instruction->Rs1);
				break;
			case InstructionClass::Atomic:
				temp_var83 = true;
				break;
			case InstructionClass::ControlFlow:
				temp_var83 = true;
				break;
			case InstructionClass::Csr:
				temp_var83 = false;
				break;
			case InstructionClass::System:
				temp_var83 = false;
				break;
			case InstructionClass::SmtVt:
				temp_var83 = false;
				break;
			case InstructionClass::ScalarAlu:
				temp_var83 = HasArchitecturalRegister(instruction->Rs1);
				break;
			default:
				temp_var83 = HasArchitecturalRegister(instruction->Rs1);
				break;
		}
		return temp_var83;
	}

	bool BundleLegalityAnalyzer::ReadsRs2(std::shared_ptr<InstructionIR> instruction)
	{
		if (IsScalarRegisterFreeVectorAluContour(instruction))
		{
			return false;
		}

		std::shared_ptr<BranchMicroOp> branchMicroOp;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (instruction.Class == InstructionClass.ControlFlow && InstructionRegistry.TryCreatePublishedControlFlowMicroOp(in instruction, out BranchMicroOp? branchMicroOp) && branchMicroOp != null)
		if (instruction->Class == InstructionClass::ControlFlow && InstructionRegistry::TryCreatePublishedControlFlowMicroOp(instruction, branchMicroOp) && branchMicroOp != nullptr)
		{
			return ContainsArchitecturalRegister(branchMicroOp->getReadRegisters(), instruction->Rs2);
		}

		VmxOperationKind vmxOperationKind;
		if (instruction->Class == InstructionClass::Vmx && InstructionRegistry::TryResolvePublishedVmxOperationKind(instruction, vmxOperationKind))
		{
			return vmxOperationKind == VmxOperationKind::VmWrite && HasArchitecturalRegister(instruction->Rs2);
		}

		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(static_cast<unsigned int>(instruction->CanonicalOpcode->Value));
		if (opcodeInfo)
		{
			bool temp_var84;
			switch (instruction->Class)
			{
				case InstructionClass::Memory:
					temp_var84 = opcodeInfo.value().SerializationClass == SerializationClass::MemoryOrdered;
					break;
				case InstructionClass::Atomic:
					temp_var84 = opcodeInfo.value().OperandCount > 1;
					break;
				case InstructionClass::ControlFlow:
					temp_var84 = HasArchitecturalRegister(instruction->Rs2);
					break;
				case InstructionClass::Csr:
					temp_var84 = false;
					break;
				case InstructionClass::System:
					temp_var84 = false;
					break;
				case InstructionClass::SmtVt:
					temp_var84 = false;
					break;
				case InstructionClass::ScalarAlu:
					temp_var84 = (static_cast<int>(opcodeInfo.value().Flags) & static_cast<int>(YAKSys_Hybrid_CPU::Arch::InstructionFlags::UsesImmediate)) == 0;
					break;
				default:
					temp_var84 = HasArchitecturalRegister(instruction->Rs2);
					break;
			}
			return temp_var84;
		}

		bool temp_var85;
		switch (instruction->Class)
		{
			case InstructionClass::Memory:
				temp_var85 = HasArchitecturalRegister(instruction->Rs2);
				break;
			case InstructionClass::Atomic:
				temp_var85 = true;
				break;
			case InstructionClass::ControlFlow:
				temp_var85 = true;
				break;
			case InstructionClass::Csr:
				temp_var85 = false;
				break;
			case InstructionClass::System:
				temp_var85 = false;
				break;
			case InstructionClass::SmtVt:
				temp_var85 = false;
				break;
			case InstructionClass::ScalarAlu:
				temp_var85 = HasArchitecturalRegister(instruction->Rs2);
				break;
			default:
				temp_var85 = HasArchitecturalRegister(instruction->Rs2);
				break;
		}
		return temp_var85;
	}

	bool BundleLegalityAnalyzer::IsScalarRegisterFreeVectorAluContour(std::shared_ptr<InstructionIR> instruction)
	{
		if (instruction->Class != InstructionClass::ScalarAlu)
		{
			return false;
		}

		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(static_cast<unsigned int>(instruction->CanonicalOpcode->Value));
		if (opcodeInfo)
		{
			const auto flags = opcodeInfo.value().Flags;
			bool isScalarResultMaskPopContour = (static_cast<int>(flags) & static_cast<int>(YAKSys_Hybrid_CPU::Arch::InstructionFlags::MaskManipulation)) != 0 && (static_cast<int>(flags) & static_cast<int>(YAKSys_Hybrid_CPU::Arch::InstructionFlags::Reduction)) != 0;

			return opcodeInfo.value().isVector() && !isScalarResultMaskPopContour;
		}

		return OpcodeRegistry::IsVectorOp(static_cast<unsigned int>(instruction->CanonicalOpcode->Value));
	}

	bool BundleLegalityAnalyzer::IsCurrentLane6DmaStreamContour(std::shared_ptr<InstructionIR> instruction)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ushort)instruction.CanonicalOpcode) is Processor.CPU_Core.IsaOpcodeValues.DmaStreamCompute or Processor.CPU_Core.IsaOpcodeValues.DSC_STATUS or Processor.CPU_Core.IsaOpcodeValues.DSC_QUERY_CAPS;
		const unsigned short opcode = instruction->CanonicalOpcode->Value;
		return opcode == Processor::CPU_Core::IsaOpcodeValues::DmaStreamCompute || opcode == Processor::CPU_Core::IsaOpcodeValues::DSC_STATUS || opcode == Processor::CPU_Core::IsaOpcodeValues::DSC_QUERY_CAPS;
	}

	bool BundleLegalityAnalyzer::IsCurrentLane6DmaStreamRegisterResultContour(std::shared_ptr<InstructionIR> instruction)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ushort)instruction.CanonicalOpcode) is Processor.CPU_Core.IsaOpcodeValues.DSC_STATUS or Processor.CPU_Core.IsaOpcodeValues.DSC_QUERY_CAPS;
		const unsigned short opcode = instruction->CanonicalOpcode->Value;
		return opcode == Processor::CPU_Core::IsaOpcodeValues::DSC_STATUS || opcode == Processor::CPU_Core::IsaOpcodeValues::DSC_QUERY_CAPS;
	}

	bool BundleLegalityAnalyzer::IsCurrentL7SdcRegisterResultContour(std::shared_ptr<InstructionIR> instruction)
	{
		if (instruction->Class != InstructionClass::System)
		{
			return false;
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ushort)instruction.CanonicalOpcode) is Processor.CPU_Core.IsaOpcodeValues.ACCEL_QUERY_CAPS or Processor.CPU_Core.IsaOpcodeValues.ACCEL_SUBMIT or Processor.CPU_Core.IsaOpcodeValues.ACCEL_POLL or Processor.CPU_Core.IsaOpcodeValues.ACCEL_WAIT or Processor.CPU_Core.IsaOpcodeValues.ACCEL_CANCEL or Processor.CPU_Core.IsaOpcodeValues.ACCEL_FENCE or Processor.CPU_Core.IsaOpcodeValues.ACCEL_STATUS;
		const unsigned short opcode = instruction->CanonicalOpcode->Value;
		return opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_QUERY_CAPS || opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_SUBMIT || opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_POLL || opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_WAIT || opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_CANCEL || opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_FENCE || opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_STATUS;
	}

	YAKSys_Hybrid_CPU::Core::ResourceBitset BundleLegalityAnalyzer::BuildStructuralResourceMask(DecodedBundleSlotDescriptor const slot)
	{
		if (!slot.isMemoryOp())
		{
			return ResourceBitset::Zero;
		}

		if (MatrixTileResourceContour::IsMatrixTileMemoryOpcode(slot.getOpCode()))
		{
			ResourceBitset mask = ResourceMaskBuilder::ForMemoryDomain(slot.getOwnerThreadId()) |
				ResourceMaskBuilder::ForStreamEngine(std::make_shared<StreamEngineId>(StreamEngineId::Zero)) |
				ResourceMaskBuilder::ForMatrixTileStreamWindow();
			return slot.getOpCode() == static_cast<unsigned int>(InstructionsEnum::MTILE_STORE)
				? mask | ResourceMaskBuilder::ForStore() | ResourceMaskBuilder::ForMatrixTileEgress() | ResourceMaskBuilder::ForMatrixTileStateRead()
				: mask | ResourceMaskBuilder::ForLoad() | ResourceMaskBuilder::ForMatrixTileIngress() | ResourceMaskBuilder::ForMatrixTileStateWrite();
		}

		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(slot.getOpCode());
		if (opcodeInfo)
		{
			if (opcodeInfo->InstructionClass == InstructionClass::Atomic)
			{
				return ResourceMaskBuilder::ForAtomic();
			}
			if (opcodeInfo->InstructionClass == InstructionClass::Memory && opcodeInfo->SerializationClass == SerializationClass::MemoryOrdered)
			{
				return ResourceMaskBuilder::ForStore();
			}
			return ResourceMaskBuilder::ForLoad();
		}

		return slot.getOpCode() == static_cast<unsigned int>(InstructionsEnum::MTILE_STORE)
			? ResourceMaskBuilder::ForStore()
			: ResourceMaskBuilder::ForLoad();
	}

	unsigned char BundleLegalityAnalyzer::ResolvePinnedLaneId(SlotClass slotClass, SlotPinningKind pinningKind)
	{
		if (pinningKind != SlotPinningKind::HardPinned)
		{
			return 0;
		}

		int temp_var86;
		switch (slotClass)
		{
			case SlotClass::BranchControl:
				temp_var86 = 7;
				break;
			case SlotClass::SystemSingleton:
				temp_var86 = 7;
				break;
			case SlotClass::DmaStreamClass:
				temp_var86 = 6;
				break;
			case SlotClass::MatrixTileStreamClass:
				temp_var86 = 6;
				break;
			default:
				temp_var86 = 0;
				break;
		}
		return temp_var86;
	}

	unsigned long long BundleLegalityAnalyzer::EncodeSlotPair(int sourceSlotIndex, int targetSlotIndex)
	{
		return 1ULL << ((sourceSlotIndex * BundleWidth) + targetSlotIndex);
	}

	bool BundleLegalityAnalyzer::HasControlConflict(DecodedBundleSlotDescriptor left, DecodedBundleSlotDescriptor right)
	{
		return RequiresSingletonLane(left.getPlacement().RequiredSlotClass) && RequiresSingletonLane(right.getPlacement().RequiredSlotClass);
	}

	bool BundleLegalityAnalyzer::HasSystemBarrierConflict(DecodedBundleSlotDescriptor left, DecodedBundleSlotDescriptor right)
	{
		return HasControlConflict(left, right) && (left.getPlacement().RequiredSlotClass == SlotClass::SystemSingleton || right.getPlacement().RequiredSlotClass == SlotClass::SystemSingleton);
	}

	bool BundleLegalityAnalyzer::HasCoarseMemoryConflict(DecodedBundleSlotDescriptor left, DecodedBundleSlotDescriptor right)
	{
		if (!left.isMemoryOp() || !right.isMemoryOp())
		{
			return false;
		}

		if (left.getMemoryBankIntent() < 0 || right.getMemoryBankIntent() < 0)
		{
			return true;
		}

		return left.getMemoryBankIntent() == right.getMemoryBankIntent();
	}

	bool BundleLegalityAnalyzer::HasPinnedLaneConflict(DecodedBundleSlotDescriptor left, DecodedBundleSlotDescriptor right)
	{
		return left.getPlacement().PinningKind == SlotPinningKind::HardPinned && right.getPlacement().PinningKind == SlotPinningKind::HardPinned && left.getPlacement().PinnedLaneId == right.getPlacement().PinnedLaneId;
	}

	bool BundleLegalityAnalyzer::IsScalarClusterEligible(DecodedBundleSlotDescriptor slot)
	{
		return !slot.isEmptyOrNop() && !slot.isMemoryOp() && !slot.isControlFlow() && !slot.isVectorOp() && slot.getPlacement().RequiredSlotClass == SlotClass::AluClass;
	}

	bool BundleLegalityAnalyzer::RequiresSingletonLane(SlotClass slotClass)
	{
		return (slotClass == SlotClass::BranchControl || slotClass == SlotClass::SystemSingleton);
	}
}
