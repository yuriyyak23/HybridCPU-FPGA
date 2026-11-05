#include "DecodedBundleDescriptor.h"
#include "DecodedInstruction.h"
#include <bit>
#include <algorithm>
#include "../Pipeline/MicroOps/Types/MicroOp.IO.h"
#include "../Legality/BundleLegalityDescriptor.h"
#include "../Pipeline/Metadata/MicroOpAdmissionMetadata.h"
#include "../Architecture/Registers/Architectural/ArchRegisterTripletEncoding.h"
#include "../Pipeline/MicroOps/Types/MicroOp.Misc.h"
#include "../Pipeline/MicroOps/Vector/VectorMicroOps.h"
#include "../Pipeline/MicroOps/Memory/MicroOp.LoadStore.h"
#include "../../../NonRTL/Arch/InstructionClass.h"
#include "../Legality/BundleLegalityAnalyzer.h"
#include "../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"

using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DecodedBundleSlotDescriptor Create(byte slotIndex, MicroOp? microOp, System.Nullable<bool> isVectorOpOverride = null)
	static std::string StateOriginName(DecodedBundleStateOrigin origin)
	{
		switch (origin)
		{
		case DecodedBundleStateOrigin::None: return "None";
		case DecodedBundleStateOrigin::Reset: return "Reset";
		case DecodedBundleStateOrigin::CanonicalDecode: return "CanonicalDecode";
		case DecodedBundleStateOrigin::DecodeFallbackTrap: return "DecodeFallbackTrap";
		case DecodedBundleStateOrigin::ReplayBundleLoad: return "ReplayBundleLoad";
		case DecodedBundleStateOrigin::ForegroundBundlePublication: return "ForegroundBundlePublication";
		case DecodedBundleStateOrigin::SingleSlotMutation: return "SingleSlotMutation";
		case DecodedBundleStateOrigin::ClearMaskMutation: return "ClearMaskMutation";
		case DecodedBundleStateOrigin::FspPacking: return "FspPacking";
		default: return std::to_string(static_cast<unsigned int>(origin));
		}
	}

	DecodedBundleSlotDescriptor DecodedBundleSlotDescriptor::Create(unsigned char slotIndex, std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> microOp, const std::optional<bool> &isVectorOpOverride)
	{
		const bool isEmptyOrNop = microOp == nullptr || std::dynamic_pointer_cast<NopMicroOp>(microOp) != nullptr;
		std::shared_ptr<MicroOpAdmissionMetadata> admission = microOp != nullptr ? microOp->getAdmissionMetadata() : nullptr;
		if (admission == nullptr) admission = std::make_shared<MicroOpAdmissionMetadata>();
		const bool isMemoryOp = microOp != nullptr && admission->IsMemoryOp;
		const bool isControlFlow = microOp != nullptr && admission->IsControlFlow;
		const bool writesRegister = microOp != nullptr && admission->WritesRegister;
		const bool isVectorOp = isVectorOpOverride.value_or(
			microOp != nullptr && (std::dynamic_pointer_cast<VectorMicroOp>(microOp) != nullptr || microOp->getClass() == MicroOpClass::Vector));
		const SlotPlacementMetadata placement = microOp != nullptr ? admission->Placement : EmptySlotPlacement;
		const auto loadStoreMicroOp = std::dynamic_pointer_cast<LoadStoreMicroOp>(microOp);
		const auto trapMicroOp = std::dynamic_pointer_cast<TrapMicroOp>(microOp);
		const int memoryBankIntent = trapMicroOp != nullptr && isMemoryOp
			? trapMicroOp->getProjectedMemoryBankIntent()
			: loadStoreMicroOp != nullptr ? loadStoreMicroOp->getMemoryBankId() : -1;

		return DecodedBundleSlotDescriptor(
			microOp, slotIndex,
			microOp != nullptr ? microOp->getVirtualThreadId() : 0,
			microOp != nullptr ? microOp->getOwnerThreadId() : 0,
			microOp != nullptr ? microOp->getOpCode() : 0,
			microOp != nullptr ? admission->ReadRegisters : std::vector<int>{},
			NormalizeWriteRegisters(microOp, admission), writesRegister, isMemoryOp, isControlFlow, placement,
			memoryBankIntent, microOp != nullptr && microOp->isFspInjected(), isEmptyOrNop, isVectorOp);
	}

	const std::vector<int> DecodedBundleSlotDescriptor::NormalizeWriteRegisters(std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> microOp, std::shared_ptr<MicroOpAdmissionMetadata> admission)
	{
		const std::vector<int>& writeRegisters = admission->WriteRegisters;
		if (admission->WritesRegister)
		{
			if (writeRegisters.empty())
				throw InvalidOperationException(std::format(L"DecodedBundleSlotDescriptor.Create observed opcode 0x{:X} with WritesRegister=true but an empty canonical WriteRegisters list. DestRegID is convenience only; decode/registration must publish authoritative write-register facts before runtime transport.", microOp->getOpCode()));

			const unsigned short destination = microOp->getDestRegID();
			if (destination != 0 && destination != VLIW_Instruction::NoReg && destination != ArchRegisterTripletEncoding::NoArchReg && std::find(writeRegisters.begin(), writeRegisters.end(), static_cast<int>(destination)) == writeRegisters.end())
			{
				std::wstring registers;
				for (int reg : writeRegisters) { if (!registers.empty()) registers += L", "; registers += std::to_wstring(reg); }
				throw InvalidOperationException(std::format(L"DecodedBundleSlotDescriptor.Create observed opcode 0x{:X} with DestRegID={} missing from canonical WriteRegisters [{}]. DestRegID is convenience only; canonical dependency truth must live in WriteRegisters.", microOp->getOpCode(), destination, registers));
			}
			return writeRegisters;
		}

		if (!writeRegisters.empty())
		{
			std::wstring registers;
			for (int reg : writeRegisters) { if (!registers.empty()) registers += L", "; registers += std::to_wstring(reg); }
			throw InvalidOperationException(std::format(L"DecodedBundleSlotDescriptor.Create observed opcode 0x{:X} with WritesRegister=false but canonical WriteRegisters [{}].", microOp != nullptr ? microOp->getOpCode() : 0U, registers));
		}
		return {};
	}

	DecodedBundleSlotDescriptor::DecodedBundleSlotDescriptor(std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> microOp, unsigned char slotIndex, int virtualThreadId, int ownerThreadId, unsigned int opCode, const std::vector<int> &readRegisters, const std::vector<int> &writeRegisters, bool writesRegister, bool isMemoryOp, bool isControlFlow, SlotPlacementMetadata placement, int memoryBankIntent, bool isFspInjected, bool isEmptyOrNop, bool isVectorOp)
	{
		MicroOp = microOp;
		SlotIndex = slotIndex;
		VirtualThreadId = virtualThreadId;
		OwnerThreadId = ownerThreadId;
		OpCode = opCode;
		ReadRegisters = readRegisters;
		WriteRegisters = writeRegisters;
		WritesRegister = writesRegister;
		IsMemoryOp = isMemoryOp;
		IsControlFlow = isControlFlow;
		Placement = placement;
		MemoryBankIntent = memoryBankIntent;
		IsFspInjected = isFspInjected;
		IsEmptyOrNop = isEmptyOrNop;
		IsVectorOp = isVectorOp;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> DecodedBundleSlotDescriptor::getMicroOp() const
	{
		return MicroOp;
	}

	unsigned char DecodedBundleSlotDescriptor::getSlotIndex() const
	{
		return SlotIndex;
	}

	int DecodedBundleSlotDescriptor::getVirtualThreadId() const
	{
		return VirtualThreadId;
	}

	int DecodedBundleSlotDescriptor::getOwnerThreadId() const
	{
		return OwnerThreadId;
	}

	unsigned int DecodedBundleSlotDescriptor::getOpCode() const
	{
		return OpCode;
	}

	const std::vector<int> DecodedBundleSlotDescriptor::getReadRegisters() const
	{
		return ReadRegisters;
	}

	const std::vector<int> DecodedBundleSlotDescriptor::getWriteRegisters() const
	{
		return WriteRegisters;
	}

	bool DecodedBundleSlotDescriptor::getWritesRegister() const
	{
		return WritesRegister;
	}

	bool DecodedBundleSlotDescriptor::isMemoryOp() const
	{
		return IsMemoryOp;
	}

	bool DecodedBundleSlotDescriptor::isControlFlow() const
	{
		return IsControlFlow;
	}

	SlotPlacementMetadata DecodedBundleSlotDescriptor::getPlacement() const
	{
		return Placement;
	}

	int DecodedBundleSlotDescriptor::getMemoryBankIntent() const
	{
		return MemoryBankIntent;
	}

	bool DecodedBundleSlotDescriptor::isFspInjected() const
	{
		return IsFspInjected;
	}

	bool DecodedBundleSlotDescriptor::isEmptyOrNop() const
	{
		return IsEmptyOrNop;
	}

	bool DecodedBundleSlotDescriptor::isVectorOp() const
	{
		return IsVectorOp;
	}

	bool DecodedBundleSlotDescriptor::isValid() const
	{
		return getMicroOp() != nullptr;
	}

	SlotPlacementMetadata DecodedBundleSlotDescriptor::GetRuntimeAdmissionPlacement() const
	{
		if (getMicroOp() == nullptr || std::dynamic_pointer_cast<TrapMicroOp>(getMicroOp()) != nullptr)
		{
			return getPlacement();
		}

		return getMicroOp()->getAdmissionMetadata()->Placement;
	}

	bool DecodedBundleSlotDescriptor::GetRuntimeAdmissionIsMemoryOp() const
	{
		if (getMicroOp() == nullptr || std::dynamic_pointer_cast<TrapMicroOp>(getMicroOp()) != nullptr)
		{
			return isMemoryOp();
		}

		return getMicroOp()->getAdmissionMetadata()->IsMemoryOp;
	}

	bool DecodedBundleSlotDescriptor::GetRuntimeAdmissionIsControlFlow() const
	{
		if (getMicroOp() == nullptr || std::dynamic_pointer_cast<TrapMicroOp>(getMicroOp()) != nullptr)
		{
			return isControlFlow();
		}

		return getMicroOp()->getAdmissionMetadata()->IsControlFlow;
	}

	bool DecodedBundleSlotDescriptor::GetRuntimeAdmissionWritesRegister() const
	{
		if (getMicroOp() == nullptr || std::dynamic_pointer_cast<TrapMicroOp>(getMicroOp()) != nullptr)
		{
			return getWritesRegister();
		}

		return getMicroOp()->getAdmissionMetadata()->WritesRegister;
	}

	const std::vector<int> DecodedBundleSlotDescriptor::GetRuntimeAdmissionReadRegisters() const
	{
		if (getMicroOp() == nullptr || std::dynamic_pointer_cast<TrapMicroOp>(getMicroOp()) != nullptr)
		{
			return getReadRegisters();
		}

		return getMicroOp()->getAdmissionMetadata()->ReadRegisters;
	}

	const std::vector<int> DecodedBundleSlotDescriptor::GetRuntimeAdmissionWriteRegisters() const
	{
		if (getMicroOp() == nullptr || std::dynamic_pointer_cast<TrapMicroOp>(getMicroOp()) != nullptr)
		{
			return getWriteRegisters();
		}

		return getMicroOp()->getAdmissionMetadata()->WriteRegisters;
	}

	int DecodedBundleSlotDescriptor::GetRuntimeExecutionVirtualThreadId() const
	{
		if (getMicroOp() != nullptr)
		{
			return getMicroOp()->getVirtualThreadId();
		}

		return getVirtualThreadId();
	}

	int DecodedBundleSlotDescriptor::GetRuntimeExecutionOwnerThreadId() const
	{
		if (getMicroOp() != nullptr)
		{
			return getMicroOp()->getOwnerThreadId();
		}

		return getOwnerThreadId();
	}

	unsigned int DecodedBundleSlotDescriptor::GetRuntimeExecutionOpCode() const
	{
		if (getMicroOp() != nullptr)
		{
			return getMicroOp()->getOpCode();
		}

		return getOpCode();
	}

	unsigned long long DecodedBundleSlotDescriptor::GetRuntimeAdmissionDomainTag() const
	{
		if (getMicroOp() == nullptr || std::dynamic_pointer_cast<TrapMicroOp>(getMicroOp()) != nullptr)
		{
			return getPlacement().DomainTag;
		}

		return getMicroOp()->getAdmissionMetadata()->DomainTag;
	}

	bool DecodedBundleSlotDescriptor::GetRuntimeExecutionIsFspInjected() const
	{
		if (getMicroOp() == nullptr)
		{
			return isFspInjected();
		}

		return getMicroOp()->isFspInjected();
	}

	bool DecodedBundleSlotDescriptor::GetRuntimeExecutionIsEmptyOrNop() const
	{
		if (getMicroOp() == nullptr)
		{
			return true;
		}

		return std::dynamic_pointer_cast<NopMicroOp>(getMicroOp()) != nullptr;
	}

	int DecodedBundleSlotDescriptor::GetRuntimeExecutionMemoryBankIntent() const
	{
		std::shared_ptr<LoadStoreMicroOp> loadStoreMicroOp = std::dynamic_pointer_cast<LoadStoreMicroOp>(getMicroOp());
		if (loadStoreMicroOp != nullptr)
		{
			return loadStoreMicroOp->getMemoryBankId();
		}

		return getMemoryBankIntent();
	}

	bool DecodedBundleSlotDescriptor::getCanTargetScalarCluster() const
	{
		return !isEmptyOrNop() && !isMemoryOp() && !isControlFlow() && !isVectorOp() && getPlacement().RequiredSlotClass == SlotClass::AluClass;
	}

	ScalarLaneTargetClass DecodedBundleSlotDescriptor::ClassifyScalarLaneTarget(unsigned char preparedMask) const
	{
		if (!getCanTargetScalarCluster())
		{
			return ScalarLaneTargetClass::NotEligible;
		}

		unsigned char slotBit = static_cast<unsigned char>(1 << getSlotIndex());
		return (preparedMask & slotBit) != 0 ? ScalarLaneTargetClass::Prepared : ScalarLaneTargetClass::Eligible;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static bool HasRegisterIntersection(in DecodedBundleSlotDescriptor left, in DecodedBundleSlotDescriptor right, IReadOnlyList<int> leftRegs, IReadOnlyList<int> rightRegs)
	bool DecodedBundleSlotDescriptor::HasRegisterIntersection(DecodedBundleSlotDescriptor const left, DecodedBundleSlotDescriptor const right, const std::vector<int> &leftRegs, const std::vector<int> &rightRegs)
	{
		if (left.getVirtualThreadId() != right.getVirtualThreadId())
		{
			return false;
		}

		if (leftRegs.empty() || rightRegs.empty() || leftRegs.empty() || rightRegs.empty())
		{
			return false;
		}

		for (int i = 0; i < leftRegs.size(); i++)
		{
			int registerId = leftRegs[i];
			for (int j = 0; j < rightRegs.size(); j++)
			{
				if (registerId == rightRegs[j])
				{
					return true;
				}
			}
		}

		return false;
	}

	DecodedBundleTypedSlotFacts::DecodedBundleTypedSlotFacts(unsigned char aluClassMask, unsigned char lsuClassMask, unsigned char dmaStreamClassMask, unsigned char matrixTileStreamClassMask, unsigned char branchControlMask, unsigned char systemSingletonMask, unsigned char unclassifiedMask, unsigned char pinnedSlotMask, unsigned char flexibleSlotMask)
		: AluClassMask(aluClassMask), LsuClassMask(lsuClassMask), DmaStreamClassMask(dmaStreamClassMask),
		  MatrixTileStreamClassMask(matrixTileStreamClassMask), BranchControlMask(branchControlMask),
		  SystemSingletonMask(systemSingletonMask), UnclassifiedMask(unclassifiedMask),
		  PinnedSlotMask(pinnedSlotMask), FlexibleSlotMask(flexibleSlotMask)
	{
	}

	unsigned char DecodedBundleTypedSlotFacts::getAluClassMask() const
	{
		return AluClassMask;
	}

	unsigned char DecodedBundleTypedSlotFacts::getLsuClassMask() const
	{
		return LsuClassMask;
	}

	unsigned char DecodedBundleTypedSlotFacts::getDmaStreamClassMask() const
	{
		return DmaStreamClassMask;
	}

	unsigned char DecodedBundleTypedSlotFacts::getMatrixTileStreamClassMask() const
	{
		return MatrixTileStreamClassMask;
	}

	unsigned char DecodedBundleTypedSlotFacts::getBranchControlMask() const
	{
		return BranchControlMask;
	}

	unsigned char DecodedBundleTypedSlotFacts::getSystemSingletonMask() const
	{
		return SystemSingletonMask;
	}

	unsigned char DecodedBundleTypedSlotFacts::getUnclassifiedMask() const
	{
		return UnclassifiedMask;
	}

	unsigned char DecodedBundleTypedSlotFacts::getPinnedSlotMask() const
	{
		return PinnedSlotMask;
	}

	unsigned char DecodedBundleTypedSlotFacts::getFlexibleSlotMask() const
	{
		return FlexibleSlotMask;
	}

	int DecodedBundleTypedSlotFacts::getAluClassCount() const
	{
		return std::popcount(static_cast<unsigned int>(getAluClassMask()));
	}

	int DecodedBundleTypedSlotFacts::getLsuClassCount() const
	{
		return std::popcount(static_cast<unsigned int>(getLsuClassMask()));
	}

	int DecodedBundleTypedSlotFacts::getDmaStreamClassCount() const
	{
		return std::popcount(static_cast<unsigned int>(getDmaStreamClassMask()));
	}

	int DecodedBundleTypedSlotFacts::getMatrixTileStreamClassCount() const
	{
		return std::popcount(static_cast<unsigned int>(getMatrixTileStreamClassMask()));
	}

	int DecodedBundleTypedSlotFacts::getBranchControlCount() const
	{
		return std::popcount(static_cast<unsigned int>(getBranchControlMask()));
	}

	int DecodedBundleTypedSlotFacts::getSystemSingletonCount() const
	{
		return std::popcount(static_cast<unsigned int>(getSystemSingletonMask()));
	}

	int DecodedBundleTypedSlotFacts::getUnclassifiedCount() const
	{
		return std::popcount(static_cast<unsigned int>(getUnclassifiedMask()));
	}

	DecodedBundleDependencySummary::DecodedBundleDependencySummary(unsigned long long readRegisterMask, unsigned long long writeRegisterMask, ResourceBitset aggregateResourceMask, unsigned long long rawDependencyMask, unsigned long long wawDependencyMask, unsigned long long warDependencyMask, unsigned long long controlConflictMask, unsigned long long memoryConflictMask, unsigned char scalarClusterEligibleMask, unsigned long long systemBarrierConflictMask, unsigned long long pinnedLaneConflictMask) : _systemBarrierConflictMask(systemBarrierConflictMask), _pinnedLaneConflictMask(pinnedLaneConflictMask)
	{
		ReadRegisterMask = readRegisterMask;
		WriteRegisterMask = writeRegisterMask;
		AggregateResourceMask = aggregateResourceMask;
		RawDependencyMask = rawDependencyMask;
		WawDependencyMask = wawDependencyMask;
		WarDependencyMask = warDependencyMask;
		ControlConflictMask = controlConflictMask;
		MemoryConflictMask = memoryConflictMask;
		ScalarClusterEligibleMask = scalarClusterEligibleMask;
	}

	unsigned long long DecodedBundleDependencySummary::getReadRegisterMask() const
	{
		return ReadRegisterMask;
	}

	unsigned long long DecodedBundleDependencySummary::getWriteRegisterMask() const
	{
		return WriteRegisterMask;
	}

	ResourceBitset DecodedBundleDependencySummary::getAggregateResourceMask() const
	{
		return AggregateResourceMask;
	}

	unsigned long long DecodedBundleDependencySummary::getRawDependencyMask() const
	{
		return RawDependencyMask;
	}

	unsigned long long DecodedBundleDependencySummary::getWawDependencyMask() const
	{
		return WawDependencyMask;
	}

	unsigned long long DecodedBundleDependencySummary::getWarDependencyMask() const
	{
		return WarDependencyMask;
	}

	unsigned long long DecodedBundleDependencySummary::getControlConflictMask() const
	{
		return ControlConflictMask;
	}

	unsigned long long DecodedBundleDependencySummary::getMemoryConflictMask() const
	{
		return MemoryConflictMask;
	}

	unsigned char DecodedBundleDependencySummary::getScalarClusterEligibleMask() const
	{
		return ScalarClusterEligibleMask;
	}

	SlotHazardQueryResult DecodedBundleDependencySummary::QuerySlotHazards(unsigned char slotIndex, unsigned char scalarGroupMask) const
	{
		if (slotIndex >= 8)
		{
			return YAKSys_Hybrid_CPU::Core::SlotHazardQueryResult();
		}

		unsigned char hardRejectPeers = 0;
		unsigned char needsCheckPeers = 0;
		HazardEffectKind dominantEffectKind = HazardEffectKind::RegisterData;
		bool dominantEffectAssigned = false;

		for (int peer = 0; peer < 8; peer++)
		{
			if (peer == slotIndex)
			{
				continue;
			}

			unsigned char peerBit = static_cast<unsigned char>(1 << peer);
			HazardEffectKind effectKind;
			HazardTriageClass triage = QueryPairHazard(slotIndex, static_cast<unsigned char>(peer), scalarGroupMask, effectKind);

			switch (triage)
			{
				case HazardTriageClass::HardReject:
					hardRejectPeers |= peerBit;
					break;
				case HazardTriageClass::NeedsRuntimeCheck:
					needsCheckPeers |= peerBit;
					break;
				case HazardTriageClass::Safe:
					break;
			}

			if (!dominantEffectAssigned && triage != HazardTriageClass::Safe)
			{
				dominantEffectKind = effectKind;
				dominantEffectAssigned = true;
			}
		}

		return SlotHazardQueryResult(hardRejectPeers, needsCheckPeers, dominantEffectKind);
	}

	HazardTriageClass DecodedBundleDependencySummary::QueryPairHazard(unsigned char slotIndex, unsigned char peerIndex, unsigned char scalarGroupMask, HazardEffectKind &dominantEffectKind) const
	{
		dominantEffectKind = HazardEffectKind::RegisterData;

		if (slotIndex >= 8 || peerIndex >= 8 || slotIndex == peerIndex)
		{
			return HazardTriageClass::Safe;
		}

		unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
		unsigned char peerBit = static_cast<unsigned char>(1 << peerIndex);
		bool slotIsScalar = (scalarGroupMask & slotBit) != 0;
		bool peerIsScalar = (scalarGroupMask & peerBit) != 0;

		unsigned long long forwardBit = 1ULL << (slotIndex * 8 + peerIndex);
		unsigned long long reverseBit = 1ULL << (peerIndex * 8 + slotIndex);
		unsigned long long pairBits = forwardBit | reverseBit;

		bool hasRaw = (getRawDependencyMask() & pairBits) != 0;
		bool hasWaw = (getWawDependencyMask() & pairBits) != 0;
		bool hasWar = (getWarDependencyMask() & pairBits) != 0;
		bool hasSystemBarrier = (_systemBarrierConflictMask & pairBits) != 0;
		bool hasControlFlow = (getControlConflictMask() & pairBits) != 0 && !hasSystemBarrier;
		bool hasMemory = (getMemoryConflictMask() & pairBits) != 0;
		bool hasPinnedLane = (_pinnedLaneConflictMask & pairBits) != 0;

		return ClassifyPairHazard(slotIsScalar, peerIsScalar, hasRaw, hasWaw, hasWar, hasMemory, hasControlFlow, hasSystemBarrier, hasPinnedLane, dominantEffectKind);
	}

	unsigned char DecodedBundleDependencySummary::ComputeRefinedWideReadyScalarMask(unsigned char scalarCandidateMask) const
	{
		unsigned char refinedMask = 0;

		for (int slotIndex = 0; slotIndex < 8; slotIndex++)
		{
			unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
			if ((scalarCandidateMask & slotBit) == 0)
			{
				continue;
			}

			SlotHazardQueryResult query = QuerySlotHazards(static_cast<unsigned char>(slotIndex), scalarCandidateMask);

			// Slot is wide-ready only if no HardReject peer exists within the scalar group
			unsigned char scalarHardReject = static_cast<unsigned char>(query.getHardRejectPeers() & scalarCandidateMask);
			if (scalarHardReject == 0)
			{
				refinedMask |= slotBit;
			}
		}

		return refinedMask;
	}

	HazardTriageClass DecodedBundleDependencySummary::ClassifyPairHazard(bool slotIsScalar, bool peerIsScalar, bool hasRaw, bool hasWaw, bool hasWar, bool hasMemory, bool hasControlFlow, bool hasSystemBarrier, bool hasPinnedLane, HazardEffectKind &dominantEffectKind)
	{
		dominantEffectKind = HazardEffectKind::RegisterData;
		bool bothScalar = slotIsScalar && peerIsScalar;

		if (hasRaw || hasWaw || hasWar)
		{
			dominantEffectKind = HazardEffectKind::RegisterData;

			if (hasRaw || hasWaw)
			{
				if (bothScalar)
				{
					return HazardTriageClass::HardReject;
				}

				if (slotIsScalar || peerIsScalar)
				{
					return HazardTriageClass::NeedsRuntimeCheck;
				}

				return HazardTriageClass::HardReject;
			}

			return HazardTriageClass::NeedsRuntimeCheck;
		}

		if (hasMemory)
		{
			dominantEffectKind = HazardEffectKind::MemoryBank;
			return HazardTriageClass::HardReject;
		}

		if (hasControlFlow)
		{
			dominantEffectKind = HazardEffectKind::ControlFlow;
			return HazardTriageClass::HardReject;
		}

		if (hasSystemBarrier)
		{
			dominantEffectKind = HazardEffectKind::SystemBarrier;
			return HazardTriageClass::HardReject;
		}

		if (hasPinnedLane)
		{
			dominantEffectKind = HazardEffectKind::PinnedLane;
			return HazardTriageClass::NeedsRuntimeCheck;
		}

		return HazardTriageClass::Safe;
	}

	SlotHazardQueryResult::SlotHazardQueryResult(unsigned char hardRejectPeers, unsigned char needsCheckPeers, HazardEffectKind dominantEffectKind)
		: HardRejectPeers(hardRejectPeers), NeedsCheckPeers(needsCheckPeers), DominantEffectKind(dominantEffectKind)
	{
	}

	unsigned char SlotHazardQueryResult::getHardRejectPeers() const
	{
		return HardRejectPeers;
	}

	unsigned char SlotHazardQueryResult::getNeedsCheckPeers() const
	{
		return NeedsCheckPeers;
	}

	HazardEffectKind SlotHazardQueryResult::getDominantEffectKind() const
	{
		return DominantEffectKind;
	}

	bool SlotHazardQueryResult::isWideCandidate() const
	{
		return getHardRejectPeers() == 0;
	}

	DecodedBundleAdmissionPrep::DecodedBundleAdmissionPrep(unsigned char scalarCandidateMask, unsigned char wideReadyScalarMask, unsigned char auxiliaryOpMask, unsigned char narrowOnlyMask, DecodedBundleAdmissionFlags flags)
	{
		ScalarCandidateMask = scalarCandidateMask;
		WideReadyScalarMask = wideReadyScalarMask;
		AuxiliaryOpMask = auxiliaryOpMask;
		NarrowOnlyMask = narrowOnlyMask;
		Flags = flags;
	}

	unsigned char DecodedBundleAdmissionPrep::getScalarCandidateMask() const
	{
		return ScalarCandidateMask;
	}

	unsigned char DecodedBundleAdmissionPrep::getWideReadyScalarMask() const
	{
		return WideReadyScalarMask;
	}

	unsigned char DecodedBundleAdmissionPrep::getAuxiliaryOpMask() const
	{
		return AuxiliaryOpMask;
	}

	unsigned char DecodedBundleAdmissionPrep::getNarrowOnlyMask() const
	{
		return NarrowOnlyMask;
	}

	DecodedBundleAdmissionFlags DecodedBundleAdmissionPrep::getFlags() const
	{
		return Flags;
	}

	int DecodedBundleAdmissionPrep::getScalarCandidateCount() const
	{
		return std::popcount(static_cast<unsigned int>(getScalarCandidateMask()));
	}

	int DecodedBundleAdmissionPrep::getWideReadyScalarCount() const
	{
		return std::popcount(static_cast<unsigned int>(getWideReadyScalarMask()));
	}

	bool DecodedBundleAdmissionPrep::getSuggestNarrowFallback() const
	{
		return (static_cast<int>(getFlags()) & static_cast<int>(DecodedBundleAdmissionFlags::SuggestNarrowFallback)) != 0;
	}

	DecodedBundleTransportFacts::DecodedBundleTransportFacts(unsigned long long pc, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char validMask, unsigned char nopMask, std::optional<DecodedBundleDependencySummary> &dependencySummary, DecodedBundleAdmissionPrep admissionPrep, DecodedBundleStateKind stateKind, DecodedBundleStateOrigin stateOrigin)
	{
		// A reference to std::vector cannot be null; the C# null guard is
		// represented by the native reference type at this boundary.

		PC = pc;
		Slots = slots;
		ValidMask = validMask;
		NopMask = nopMask;
		DependencySummary = dependencySummary;
		AdmissionPrep = admissionPrep;
		StateKind = stateKind;
		StateOrigin = stateOrigin;
	}

	unsigned long long DecodedBundleTransportFacts::getPC() const
	{
		return PC;
	}

	std::vector<DecodedBundleSlotDescriptor> DecodedBundleTransportFacts::getSlots() const
	{
		return Slots;
	}

	unsigned char DecodedBundleTransportFacts::getValidMask() const
	{
		return ValidMask;
	}

	unsigned char DecodedBundleTransportFacts::getNopMask() const
	{
		return NopMask;
	}

	unsigned char DecodedBundleTransportFacts::getValidNonEmptyMask() const
	{
		return static_cast<unsigned char>(getValidMask() & ~getNopMask());
	}

	std::optional<DecodedBundleDependencySummary> DecodedBundleTransportFacts::getDependencySummary() const
	{
		return DependencySummary;
	}

	DecodedBundleAdmissionPrep DecodedBundleTransportFacts::getAdmissionPrep() const
	{
		return AdmissionPrep;
	}

	DecodedBundleStateKind DecodedBundleTransportFacts::getStateKind() const
	{
		return StateKind;
	}

	DecodedBundleStateOrigin DecodedBundleTransportFacts::getStateOrigin() const
	{
		return StateOrigin;
	}

	BundleProgressState::BundleProgressState(unsigned long long bundlePc, unsigned char remainingMask, unsigned char nextSlotIndex, unsigned char consumedMask)
	{
		BundlePc = bundlePc;
		RemainingMask = remainingMask;
		NextSlotIndex = nextSlotIndex;
		ConsumedMask = consumedMask;
	}

	unsigned long long BundleProgressState::getBundlePc() const
	{
		return BundlePc;
	}

	unsigned char BundleProgressState::getRemainingMask() const
	{
		return RemainingMask;
	}

	unsigned char BundleProgressState::getNextSlotIndex() const
	{
		return NextSlotIndex;
	}

	unsigned char BundleProgressState::getConsumedMask() const
	{
		return ConsumedMask;
	}

	bool BundleProgressState::getHasRemaining() const
	{
		return getRemainingMask() != 0 && getNextSlotIndex() < TerminalSlotIndex;
	}

	bool BundleProgressState::Contains(unsigned char slotIndex) const
	{
		return slotIndex < TerminalSlotIndex && (getRemainingMask() & (1 << slotIndex)) != 0;
	}

	BundleProgressState BundleProgressState::CreateEmpty(unsigned long long bundlePc)
	{
		return BundleProgressState(bundlePc, 0, TerminalSlotIndex, 0);
	}

	BundleProgressState BundleProgressState::CreateForCursor(unsigned long long bundlePc, unsigned char remainingMask, unsigned char currentSlotIndex, unsigned char consumedMask)
	{
		unsigned char nextSlotIndex = ResolveNextSlotIndexForCursor(remainingMask, currentSlotIndex);
		return BundleProgressState(bundlePc, remainingMask, nextSlotIndex, consumedMask);
	}

	BundleProgressState BundleProgressState::ConsumeSlot(unsigned char slotIndex) const
	{
		if (slotIndex >= TerminalSlotIndex)
		{
			return *this;
		}

		unsigned char remainingMask = static_cast<unsigned char>(getRemainingMask() & ~(1 << slotIndex));
		unsigned char next = ResolveNextSlotIndexForCursor(remainingMask, slotIndex);
		unsigned char passed = next < TerminalSlotIndex ? static_cast<unsigned char>((1 << next) - 1) : std::numeric_limits<unsigned char>::max();
		return CreateForCursor(getBundlePc(), remainingMask, slotIndex, static_cast<unsigned char>(getConsumedMask() | (1 << slotIndex) | (passed & ~remainingMask)));
	}

	BundleProgressState BundleProgressState::ConsumeMask(unsigned char consumedSlotMask, unsigned char currentSlotIndex) const
	{
		unsigned char remainingMask = static_cast<unsigned char>(getRemainingMask() & ~consumedSlotMask);
		unsigned char next = ResolveNextSlotIndexForCursor(remainingMask, currentSlotIndex);
		unsigned char passed = next < TerminalSlotIndex ? static_cast<unsigned char>((1 << next) - 1) : std::numeric_limits<unsigned char>::max();
		return CreateForCursor(getBundlePc(), remainingMask, currentSlotIndex, static_cast<unsigned char>(getConsumedMask() | consumedSlotMask | (passed & ~remainingMask)));
	}

	BundleProgressState BundleProgressState::RebaseAfterSourceRefresh(unsigned char currentValidMask) const
	{
		return CreateForCursor(getBundlePc(), static_cast<unsigned char>(currentValidMask & ~getConsumedMask()), getNextSlotIndex(), getConsumedMask());
	}

	unsigned char BundleProgressState::ResolveNextSlotIndexForCursor(unsigned char remainingMask, unsigned char currentSlotIndex)
	{
		if (remainingMask == 0)
		{
			return TerminalSlotIndex;
		}

		if (currentSlotIndex < TerminalSlotIndex && (remainingMask & (1 << currentSlotIndex)) != 0)
		{
			return currentSlotIndex;
		}

		if (currentSlotIndex < TerminalSlotIndex)
		{
			for (unsigned char slotIndex = static_cast<unsigned char>(currentSlotIndex + 1); slotIndex < TerminalSlotIndex; slotIndex++)
			{
				if ((remainingMask & (1 << slotIndex)) != 0)
				{
					return slotIndex;
				}
			}
		}

		for (unsigned char slotIndex = 0; slotIndex < TerminalSlotIndex; slotIndex++)
		{
			if ((remainingMask & (1 << slotIndex)) != 0)
			{
				return slotIndex;
			}
		}

		return TerminalSlotIndex;
	}

	DecodedBundleTransformHistory::DecodedBundleTransformHistory(DecodedBundleTransformKind appliedTransforms, unsigned char mutationDepth, DecodedBundleStateOrigin latestOrigin)
		: AppliedTransforms(appliedTransforms), MutationDepth(mutationDepth), LatestOrigin(latestOrigin)
	{
	}

	DecodedBundleTransformKind DecodedBundleTransformHistory::getAppliedTransforms() const
	{
		return AppliedTransforms;
	}

	unsigned char DecodedBundleTransformHistory::getMutationDepth() const
	{
		return MutationDepth;
	}

	DecodedBundleStateOrigin DecodedBundleTransformHistory::getLatestOrigin() const
	{
		return LatestOrigin;
	}

	bool DecodedBundleTransformHistory::Contains(DecodedBundleTransformKind transformKind) const
	{
		const int applied = static_cast<int>(getAppliedTransforms());
		const int requested = static_cast<int>(transformKind);
		return (applied & requested) == requested;
	}

	DecodedBundleTransformHistory DecodedBundleTransformHistory::CreateBase(DecodedBundleStateOrigin origin)
	{
		return DecodedBundleTransformHistory(ResolveTransformKind(origin), CountsAsMutation(origin) ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0), origin);
	}

	DecodedBundleTransformHistory DecodedBundleTransformHistory::Append(DecodedBundleStateOrigin origin) const
	{
		unsigned char mutationDepth = getMutationDepth();
		if (CountsAsMutation(origin) && mutationDepth < std::numeric_limits<unsigned char>::max())
		{
			mutationDepth++;
		}

		return DecodedBundleTransformHistory(static_cast<DecodedBundleTransformKind>(static_cast<int>(getAppliedTransforms()) | static_cast<int>(ResolveTransformKind(origin))), mutationDepth, origin);
	}

	bool DecodedBundleTransformHistory::CountsAsMutation(DecodedBundleStateOrigin origin)
	{
		return origin == DecodedBundleStateOrigin::ReplayBundleLoad || origin == DecodedBundleStateOrigin::ForegroundBundlePublication || origin == DecodedBundleStateOrigin::SingleSlotMutation || origin == DecodedBundleStateOrigin::ClearMaskMutation || origin == DecodedBundleStateOrigin::FspPacking;
	}

	DecodedBundleTransformKind DecodedBundleTransformHistory::ResolveTransformKind(DecodedBundleStateOrigin origin)
	{
		switch (origin)
		{
		case DecodedBundleStateOrigin::None: return DecodedBundleTransformKind::None;
		case DecodedBundleStateOrigin::Reset: return DecodedBundleTransformKind::Reset;
		case DecodedBundleStateOrigin::CanonicalDecode: return DecodedBundleTransformKind::CanonicalDecode;
		case DecodedBundleStateOrigin::DecodeFallbackTrap: return DecodedBundleTransformKind::DecodeFallbackTrap;
		case DecodedBundleStateOrigin::ReplayBundleLoad: return DecodedBundleTransformKind::ReplayBundleLoad;
		case DecodedBundleStateOrigin::ForegroundBundlePublication: return DecodedBundleTransformKind::ForegroundBundlePublication;
		case DecodedBundleStateOrigin::SingleSlotMutation: return DecodedBundleTransformKind::SingleSlotMutation;
		case DecodedBundleStateOrigin::ClearMaskMutation: return DecodedBundleTransformKind::ClearMaskMutation;
		case DecodedBundleStateOrigin::FspPacking: return DecodedBundleTransformKind::FspPacking;
		default: return DecodedBundleTransformKind::None;
		}
	}

	DecodedBundleRuntimeState::DecodedBundleRuntimeState(DecodedBundleTransportFacts transportFacts, std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalDecode, std::shared_ptr<Legality::BundleLegalityDescriptor> legalityDescriptor, DecodedBundleTransformHistory transformHistory, DecodedBundleStateOwnerKind stateOwnerKind, unsigned long long stateEpoch, unsigned long long stateVersion, unsigned long long lineageStateVersion)
	{
		ArgumentNullException::ThrowIfNull(canonicalDecode);
		ArgumentNullException::ThrowIfNull(legalityDescriptor);

		ValidateBundleIdentity(transportFacts.getPC(), canonicalDecode, legalityDescriptor);

		TransportFacts = transportFacts;
		CanonicalDecode = canonicalDecode;
		LegalityDescriptor = legalityDescriptor;
		CanonicalValidity = ResolveCanonicalValidity(canonicalDecode, legalityDescriptor);
		TransformHistory = transformHistory;
		ValidatePublicationIdentity(stateOwnerKind, stateEpoch, stateVersion, lineageStateVersion);
		StateOwnerKind = stateOwnerKind;
		StateEpoch = stateEpoch;
		StateVersion = stateVersion;
		LineageStateVersion = lineageStateVersion;
	}

	DecodedBundleTransportFacts DecodedBundleRuntimeState::getTransportFacts() const
	{
		return TransportFacts;
	}

	std::shared_ptr<Decoder::DecodedInstructionBundle> DecodedBundleRuntimeState::getCanonicalDecode() const
	{
		return CanonicalDecode;
	}

	std::shared_ptr<Legality::BundleLegalityDescriptor> DecodedBundleRuntimeState::getLegalityDescriptor() const
	{
		return LegalityDescriptor;
	}

	DecodedBundleCanonicalValidity DecodedBundleRuntimeState::getCanonicalValidity() const
	{
		return CanonicalValidity;
	}

	DecodedBundleTransformHistory DecodedBundleRuntimeState::getTransformHistory() const
	{
		return TransformHistory;
	}

	DecodedBundleStateOwnerKind DecodedBundleRuntimeState::getStateOwnerKind() const
	{
		return StateOwnerKind;
	}

	unsigned long long DecodedBundleRuntimeState::getStateEpoch() const
	{
		return StateEpoch;
	}

	unsigned long long DecodedBundleRuntimeState::getStateVersion() const
	{
		return StateVersion;
	}

	unsigned long long DecodedBundleRuntimeState::getLineageStateVersion() const
	{
		return LineageStateVersion;
	}

	unsigned long long DecodedBundleRuntimeState::getBundlePc() const
	{
		return getTransportFacts().getPC();
	}

	unsigned long long DecodedBundleRuntimeState::getBundleSerial() const
	{
		return getCanonicalDecode()->getBundleSerial();
	}

	DecodedBundleStateKind DecodedBundleRuntimeState::getStateKind() const
	{
		return getTransportFacts().getStateKind();
	}

	DecodedBundleStateOrigin DecodedBundleRuntimeState::getStateOrigin() const
	{
		return getTransportFacts().getStateOrigin();
	}

	bool DecodedBundleRuntimeState::getHasCanonicalDecode() const
	{
		return getCanonicalValidity() == DecodedBundleCanonicalValidity::Canonical;
	}

	bool DecodedBundleRuntimeState::getHasCanonicalLegality() const
	{
		return getCanonicalValidity() == DecodedBundleCanonicalValidity::Canonical;
	}

	bool DecodedBundleRuntimeState::getHasDecodeFault() const
	{
		return getStateKind() == DecodedBundleStateKind::DecodeFault || getCanonicalValidity() == DecodedBundleCanonicalValidity::DecodeFault;
	}

	bool DecodedBundleRuntimeState::CanPreserveCanonicalForPc(unsigned long long pc) const
	{
		return getCanonicalValidity() == DecodedBundleCanonicalValidity::Canonical && getCanonicalDecode()->getBundleAddress() == pc && getLegalityDescriptor()->getBundleAddress() == pc;
	}

	DecodedBundleRuntimeState DecodedBundleRuntimeState::RepublishTransport(DecodedBundleTransportFacts const transportFacts) const
	{
		if (CanPreserveCanonicalForPc(transportFacts.getPC()))
		{
			DecodedBundleTransformHistory history = getTransformHistory();
			if (transportFacts.getStateKind() != DecodedBundleStateKind::Canonical || transportFacts.getStateOrigin() != DecodedBundleStateOrigin::CanonicalDecode)
			{
				history = history.Append(transportFacts.getStateOrigin());
			}

			return DecodedBundleRuntimeState(transportFacts, getCanonicalDecode(), getLegalityDescriptor(), history, getStateOwnerKind(), getStateEpoch(), getStateVersion(), getLineageStateVersion());
		}

		return CreateTransportOnly(transportFacts, 0, std::nullopt);
	}

	DecodedBundleRuntimeState DecodedBundleRuntimeState::ApplyFspPacking(DecodedBundleTransportFacts const transportFacts) const
	{
		ValidateExpectedOrigin(transportFacts, DecodedBundleStateOrigin::FspPacking);
		return ApplyTransform(transportFacts, DecodedBundleStateOrigin::FspPacking);
	}

	DecodedBundleRuntimeState DecodedBundleRuntimeState::ApplySingleSlotMutation(DecodedBundleTransportFacts const transportFacts) const
	{
		ValidateExpectedOrigin(transportFacts, DecodedBundleStateOrigin::SingleSlotMutation);
		return ApplyTransform(transportFacts, DecodedBundleStateOrigin::SingleSlotMutation);
	}

	DecodedBundleRuntimeState DecodedBundleRuntimeState::ApplyProgressProjection(DecodedBundleTransportFacts const transportFacts) const
	{
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
		const DecodedBundleTransportFacts currentFacts = getTransportFacts();
		const auto currentSlots = currentFacts.getSlots();
		const auto projectedSlots = transportFacts.getSlots();
		const bool sameSlotFacts = currentSlots.size() == projectedSlots.size() && std::equal(currentSlots.begin(), currentSlots.end(), projectedSlots.begin(), [](const auto& left, const auto& right) {
			const auto leftPlacement = left.getPlacement();
			const auto rightPlacement = right.getPlacement();
			return left.getMicroOp() == right.getMicroOp() && left.getSlotIndex() == right.getSlotIndex() && left.getVirtualThreadId() == right.getVirtualThreadId() && left.getOwnerThreadId() == right.getOwnerThreadId() && left.getOpCode() == right.getOpCode() && left.getReadRegisters() == right.getReadRegisters() && left.getWriteRegisters() == right.getWriteRegisters() && left.getWritesRegister() == right.getWritesRegister() && left.isMemoryOp() == right.isMemoryOp() && left.isControlFlow() == right.isControlFlow() && leftPlacement.RequiredSlotClass == rightPlacement.RequiredSlotClass && leftPlacement.PinningKind == rightPlacement.PinningKind && leftPlacement.PinnedLaneId == rightPlacement.PinnedLaneId && leftPlacement.DomainTag == rightPlacement.DomainTag && left.getMemoryBankIntent() == right.getMemoryBankIntent() && left.isFspInjected() == right.isFspInjected() && left.isEmptyOrNop() == right.isEmptyOrNop() && left.isVectorOp() == right.isVectorOp();
		});
		if (currentFacts.getValidMask() == transportFacts.getValidMask() && currentFacts.getNopMask() == transportFacts.getNopMask() && currentFacts.getStateKind() == transportFacts.getStateKind() && currentFacts.getStateOrigin() == transportFacts.getStateOrigin() && sameSlotFacts)
		{
			return *this;
		}

		return ApplyTransform(transportFacts, DecodedBundleStateOrigin::ClearMaskMutation);
	}

	DecodedBundleRuntimeState DecodedBundleRuntimeState::WithPublicationIdentity(DecodedBundleStateOwnerKind stateOwnerKind, unsigned long long stateEpoch, unsigned long long stateVersion, unsigned long long lineageStateVersion) const
	{
		if (getStateOwnerKind() == stateOwnerKind && getStateEpoch() == stateEpoch && getStateVersion() == stateVersion && getLineageStateVersion() == lineageStateVersion)
		{
			return *this;
		}

		return DecodedBundleRuntimeState(getTransportFacts(), getCanonicalDecode(), getLegalityDescriptor(), getTransformHistory(), stateOwnerKind, stateEpoch, stateVersion, lineageStateVersion);
	}

	DecodedBundleRuntimeState DecodedBundleRuntimeState::CreateCanonical(std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalDecode, std::shared_ptr<Legality::BundleLegalityDescriptor> legalityDescriptor, DecodedBundleTransportFacts const transportFacts)
	{
		return DecodedBundleRuntimeState(transportFacts, canonicalDecode, legalityDescriptor, DecodedBundleTransformHistory::CreateBase(DecodedBundleStateOrigin::CanonicalDecode));
	}

	DecodedBundleRuntimeState DecodedBundleRuntimeState::CreateFallback(unsigned long long pc, DecodedBundleTransportFacts const transportFacts, unsigned long long bundleSerial)
	{
		if (transportFacts.getPC() != pc)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException(string.Format("Fallback transport PC 0x{0:X} must match requested bundle PC 0x{1:X}.", transportFacts.PC, pc), nameof(transportFacts));
			throw std::invalid_argument(std::format("Fallback transport PC 0x{:X} must match requested bundle PC 0x{:X}.", transportFacts.getPC(), pc));
		}

		return DecodedBundleRuntimeState(transportFacts, Decoder::DecodedInstructionBundle::CreateDecodeFault(pc, bundleSerial), Legality::BundleLegalityDescriptor::CreateDecodeFault(pc, bundleSerial), DecodedBundleTransformHistory::CreateBase(DecodedBundleStateOrigin::DecodeFallbackTrap));
	}

	DecodedBundleRuntimeState DecodedBundleRuntimeState::CreateReplay(DecodedBundleTransportFacts const transportFacts, unsigned long long bundleSerial)
	{
		ValidateExpectedOrigin(transportFacts, DecodedBundleStateOrigin::ReplayBundleLoad);
		return CreateTransportOnly(transportFacts, bundleSerial, DecodedBundleTransformHistory::CreateBase(DecodedBundleStateOrigin::ReplayBundleLoad));
	}

	DecodedBundleRuntimeState DecodedBundleRuntimeState::CreateTransportOnly(DecodedBundleTransportFacts const transportFacts, unsigned long long bundleSerial, const std::optional<DecodedBundleTransformHistory> &transformHistory)
	{
		return DecodedBundleRuntimeState(transportFacts, Decoder::DecodedInstructionBundle::CreateEmpty(transportFacts.getPC(), bundleSerial), Legality::BundleLegalityDescriptor::CreateEmpty(transportFacts.getPC(), bundleSerial), transformHistory.value_or(DecodedBundleTransformHistory::CreateBase(transportFacts.getStateOrigin())));
	}

	DecodedBundleRuntimeState DecodedBundleRuntimeState::CreateEmpty(unsigned long long pc, unsigned long long bundleSerial)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: DecodedBundleTransportFacts transportFacts = DecodedBundleSlotCarrierBuilder.BuildTransportFacts(pc, Array.Empty<MicroOp?>(), DecodedBundleStateKind.Empty, DecodedBundleStateOrigin.Reset);
		DecodedBundleTransportFacts transportFacts = DecodedBundleSlotCarrierBuilder::BuildTransportFacts(pc, std::vector<std::shared_ptr<MicroOp>>{}, DecodedBundleStateKind::Empty, DecodedBundleStateOrigin::Reset);

		return CreateTransportOnly(transportFacts, bundleSerial);
	}

	DecodedBundleCanonicalValidity DecodedBundleRuntimeState::ResolveCanonicalValidity(std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalDecode, std::shared_ptr<Legality::BundleLegalityDescriptor> legalityDescriptor)
	{
		bool hasDecodeFault = canonicalDecode->getHasDecodeFault() || legalityDescriptor->getHasDecodeFault();
		if (hasDecodeFault)
		{
			return DecodedBundleCanonicalValidity::DecodeFault;
		}

		return canonicalDecode->isEmpty() ? DecodedBundleCanonicalValidity::None : DecodedBundleCanonicalValidity::Canonical;
	}

	void DecodedBundleRuntimeState::ValidateBundleIdentity(unsigned long long transportPc, std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalDecode, std::shared_ptr<Legality::BundleLegalityDescriptor> legalityDescriptor)
	{
		if (canonicalDecode->getBundleAddress() != legalityDescriptor->getBundleAddress())
		{
			throw std::invalid_argument(std::format("Canonical decode PC 0x{:X} must match legality PC 0x{:X}.", canonicalDecode->getBundleAddress(), legalityDescriptor->getBundleAddress()));
		}

		if (canonicalDecode->getBundleSerial() != legalityDescriptor->getBundleSerial())
		{
			throw std::invalid_argument(std::format("Canonical decode serial {} must match legality serial {}.", canonicalDecode->getBundleSerial(), legalityDescriptor->getBundleSerial()));
		}

		if (canonicalDecode->getBundleAddress() != transportPc)
		{
			throw std::invalid_argument(std::format("Transport PC 0x{:X} must match canonical decode PC 0x{:X}.", transportPc, canonicalDecode->getBundleAddress()));
		}

		bool canonicalHasPayload = !canonicalDecode->isEmpty();
		bool legalityHasPayload = !legalityDescriptor->isEmpty();
		if (canonicalHasPayload != legalityHasPayload)
		{
			throw std::invalid_argument("Canonical decode and legality descriptors must either both carry payload or both be empty.");
		}

		if (canonicalDecode->getHasDecodeFault() != legalityDescriptor->getHasDecodeFault())
		{
			throw std::invalid_argument("Canonical decode and legality descriptors must agree on decode-fault publication.");
		}
	}

	DecodedBundleRuntimeState DecodedBundleRuntimeState::ApplyTransform(DecodedBundleTransportFacts const transportFacts, DecodedBundleStateOrigin transformOrigin) const
	{
		if (transportFacts.getPC() != getBundlePc())
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException(string.Format("Transport PC 0x{0:X} must stay aligned with runtime-state PC 0x{1:X}.", transportFacts.PC, BundlePc), nameof(transportFacts));
			throw std::invalid_argument(std::format("Transport PC 0x{:X} must stay aligned with runtime-state PC 0x{:X}.", transportFacts.getPC(), getBundlePc()));
		}

		return DecodedBundleRuntimeState(transportFacts, getCanonicalDecode(), getLegalityDescriptor(), getTransformHistory().Append(transformOrigin), getStateOwnerKind(), getStateEpoch(), getStateVersion(), getLineageStateVersion());
	}

	void DecodedBundleRuntimeState::ValidatePublicationIdentity(DecodedBundleStateOwnerKind stateOwnerKind, unsigned long long stateEpoch, unsigned long long stateVersion, unsigned long long lineageStateVersion)
	{
		if (stateOwnerKind == DecodedBundleStateOwnerKind::None)
		{
			if (stateEpoch != 0 || stateVersion != 0 || lineageStateVersion != 0)
			{
				throw std::invalid_argument("Unpublished decoded-bundle runtime state must not carry owner/epoch/version identity.");
			}

			return;
		}

		if (stateEpoch == 0)
		{
			throw std::invalid_argument("Published decoded-bundle runtime state must carry a non-zero StateEpoch.");
		}

		if (stateVersion == 0)
		{
			throw std::invalid_argument("Published decoded-bundle runtime state must carry a non-zero StateVersion.");
		}

		if (lineageStateVersion == 0)
		{
			throw std::invalid_argument("Published decoded-bundle runtime state must carry a non-zero lineage state version.");
		}
	}

	void DecodedBundleRuntimeState::ValidateExpectedOrigin(DecodedBundleTransportFacts const transportFacts, DecodedBundleStateOrigin expectedOrigin)
	{
		if (transportFacts.getStateOrigin() != expectedOrigin)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException(string.Format("Transport origin {0} must match expected runtime transform {1}.", transportFacts.StateOrigin, expectedOrigin), nameof(transportFacts));
			throw std::invalid_argument(std::format("Transport origin {} must match expected runtime transform {}.", StateOriginName(transportFacts.getStateOrigin()), StateOriginName(expectedOrigin)));
		}
	}

	DecodedBundleDerivedIssuePlanState::DecodedBundleDerivedIssuePlanState(bool isActive, DecodedBundleRuntimeState runtimeState)
	{
		IsActive = isActive;
		RuntimeState = runtimeState;
	}

	bool DecodedBundleDerivedIssuePlanState::isActive() const
	{
		return IsActive;
	}

	DecodedBundleRuntimeState DecodedBundleDerivedIssuePlanState::getRuntimeState() const
	{
		return RuntimeState;
	}

	DecodedBundleTransportFacts DecodedBundleDerivedIssuePlanState::getTransportFacts() const
	{
		return getRuntimeState().getTransportFacts();
	}

	unsigned long long DecodedBundleDerivedIssuePlanState::getBundlePc() const
	{
		return getRuntimeState().getBundlePc();
	}

	unsigned long long DecodedBundleDerivedIssuePlanState::getBundleSerial() const
	{
		return getRuntimeState().getBundleSerial();
	}

	unsigned long long DecodedBundleDerivedIssuePlanState::getStateEpoch() const
	{
		return getRuntimeState().getStateEpoch();
	}

	unsigned long long DecodedBundleDerivedIssuePlanState::getStateVersion() const
	{
		return getRuntimeState().getStateVersion();
	}

	unsigned long long DecodedBundleDerivedIssuePlanState::getLineageStateVersion() const
	{
		return getRuntimeState().getLineageStateVersion();
	}

	bool DecodedBundleDerivedIssuePlanState::MatchesBundlePc(unsigned long long pc) const
	{
		return isActive() && getRuntimeState().getBundlePc() == pc;
	}

	void DecodedBundleDerivedIssuePlanState::ValidateAgainstBaseRuntimeState(DecodedBundleRuntimeState const baseRuntimeState, const std::wstring &caller) const
	{
		if (!isActive())
		{
			return;
		}

		if (getRuntimeState().getStateOwnerKind() != DecodedBundleStateOwnerKind::DerivedIssuePlanPublication)
		{
			throw InvalidOperationException(std::format(L"{} observed an active derived issue-plan state without DerivedIssuePlanPublication ownership.", caller));
		}

		if (getRuntimeState().getBundlePc() != baseRuntimeState.getBundlePc())
		{
			throw InvalidOperationException(std::format(L"{} observed derived bundle PC 0x{:X} that diverges from base bundle PC 0x{:X}.", caller, getRuntimeState().getBundlePc(), baseRuntimeState.getBundlePc()));
		}

		if (getRuntimeState().getBundleSerial() != baseRuntimeState.getBundleSerial())
		{
			throw InvalidOperationException(std::format(L"{} observed derived bundle serial {} that diverges from base bundle serial {}.", caller, getRuntimeState().getBundleSerial(), baseRuntimeState.getBundleSerial()));
		}

		if (getRuntimeState().getStateEpoch() != baseRuntimeState.getStateEpoch())
		{
			throw InvalidOperationException(std::format(L"{} observed derived state epoch {} that diverges from base state epoch {}.", caller, getRuntimeState().getStateEpoch(), baseRuntimeState.getStateEpoch()));
		}

		if (getRuntimeState().getLineageStateVersion() != baseRuntimeState.getStateVersion())
		{
			throw InvalidOperationException(std::format(L"{} observed derived lineage state version {} that does not match base state version {}.", caller, getRuntimeState().getLineageStateVersion(), baseRuntimeState.getStateVersion()));
		}
	}

	DecodedBundleDerivedIssuePlanState DecodedBundleDerivedIssuePlanState::CreateEmpty(unsigned long long bundlePc)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: DecodedBundleTransportFacts transportFacts = DecodedBundleSlotCarrierBuilder.BuildTransportFacts(bundlePc, Array.Empty<MicroOp?>(), DecodedBundleStateKind.Empty, DecodedBundleStateOrigin.None);
		std::vector<std::shared_ptr<MicroOp>> emptyMicroOps;
		DecodedBundleTransportFacts transportFacts = DecodedBundleSlotCarrierBuilder::BuildTransportFacts(bundlePc, emptyMicroOps, DecodedBundleStateKind::Empty, DecodedBundleStateOrigin::None);
		return DecodedBundleDerivedIssuePlanState(false, DecodedBundleRuntimeState::CreateTransportOnly(transportFacts));
	}

	DecodedBundleDerivedIssuePlanState DecodedBundleDerivedIssuePlanState::CreateActive(DecodedBundleRuntimeState const runtimeState)
	{
		if (runtimeState.getStateOwnerKind() != DecodedBundleStateOwnerKind::DerivedIssuePlanPublication)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Derived issue-plan publication must stamp runtime state ownership before activation.", nameof(runtimeState));
			throw std::invalid_argument("Derived issue-plan publication must stamp runtime state ownership before activation.");
		}

		return DecodedBundleDerivedIssuePlanState(true, runtimeState);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildTransportFacts(ulong pc, IReadOnlyList<MicroOp?> carrierBundle, DecodedBundleStateKind stateKind = DecodedBundleStateKind.ForegroundMutated, DecodedBundleStateOrigin stateOrigin = DecodedBundleStateOrigin.ForegroundBundlePublication)
	DecodedBundleTransportFacts DecodedBundleSlotCarrierBuilder::BuildTransportFacts(unsigned long long pc, const std::vector<std::shared_ptr<MicroOp>> &carrierBundle, DecodedBundleStateKind stateKind, DecodedBundleStateOrigin stateOrigin)
	{
		// A reference to std::vector cannot be null; the C# null guard is
		// represented by the native reference type at this boundary.

		std::vector<DecodedBundleSlotDescriptor> slots;
		unsigned char validMask;
		unsigned char nopMask;
		std::optional<DecodedBundleDependencySummary> dependencySummary;
		DecodedBundleAdmissionPrep admissionPrep;
		ResolveTransportFacts(carrierBundle, slots, validMask, nopMask, dependencySummary, admissionPrep);

		return DecodedBundleTransportFacts(pc, slots, validMask, nopMask, dependencySummary, admissionPrep, stateKind, stateOrigin);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildTransportFacts(ulong pc, IReadOnlyList<MicroOp?> carrierBundle, System.Nullable<DecodedBundleDependencySummary> dependencySummary, DecodedBundleStateKind stateKind = DecodedBundleStateKind.ForegroundMutated, DecodedBundleStateOrigin stateOrigin = DecodedBundleStateOrigin.ForegroundBundlePublication)
	DecodedBundleTransportFacts DecodedBundleSlotCarrierBuilder::BuildTransportFacts(unsigned long long pc, const std::vector<std::shared_ptr<MicroOp>> &carrierBundle, std::optional<DecodedBundleDependencySummary> &dependencySummary, DecodedBundleStateKind stateKind, DecodedBundleStateOrigin stateOrigin)
	{
		// carrierBundle is a native reference and cannot be null.

		std::vector<DecodedBundleSlotDescriptor> slots;
		unsigned char validMask;
		unsigned char nopMask;
		ResolveCarrierTransportFactsShape(carrierBundle, slots, validMask, nopMask);

		DecodedBundleAdmissionPrep admissionPrep = dependencySummary ? BuildAdmissionPrep(slots, validMask, nopMask, dependencySummary.value()) : YAKSys_Hybrid_CPU::Core::DecodedBundleAdmissionPrep();

		return DecodedBundleTransportFacts(pc, slots, validMask, nopMask, dependencySummary, admissionPrep, stateKind, stateOrigin);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildTransportFacts(ulong pc, IReadOnlyList<MicroOp?> carrierBundle, Decoder.DecodedInstructionBundle canonicalBundle, System.Nullable<DecodedBundleDependencySummary> dependencySummary)
	DecodedBundleTransportFacts DecodedBundleSlotCarrierBuilder::BuildTransportFacts(unsigned long long pc, const std::vector<std::shared_ptr<MicroOp>> &carrierBundle, std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalBundle, std::optional<DecodedBundleDependencySummary> &dependencySummary)
	{
		// carrierBundle is a native reference and cannot be null.
		ArgumentNullException::ThrowIfNull(canonicalBundle);

		std::vector<DecodedBundleSlotDescriptor> slots;
		unsigned char validMask;
		unsigned char nopMask;
		ResolveCanonicalCarrierTransportFactsShape(carrierBundle, canonicalBundle, slots, validMask, nopMask);

		DecodedBundleAdmissionPrep admissionPrep = dependencySummary ? BuildAdmissionPrep(slots, validMask, nopMask, dependencySummary.value()) : YAKSys_Hybrid_CPU::Core::DecodedBundleAdmissionPrep();

		return DecodedBundleTransportFacts(pc, slots, validMask, nopMask, dependencySummary, admissionPrep, DecodedBundleStateKind::Canonical, DecodedBundleStateOrigin::CanonicalDecode);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildTransportFactsFromSingleSlotMutationContour(ulong pc, IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, byte slotIndex, MicroOp? microOp)
	DecodedBundleTransportFacts DecodedBundleSlotCarrierBuilder::BuildTransportFactsFromSingleSlotMutationContour(unsigned long long pc, const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, unsigned char slotIndex, std::shared_ptr<MicroOp> microOp)
	{
		// slotDescriptors is a native reference and cannot be null.

		std::vector<DecodedBundleSlotDescriptor> slots;
		unsigned char validMask;
		unsigned char nopMask;
		ResolveTransportFactsShapeWithSingleSlotMutation(slotDescriptors, slotIndex, DecodedBundleSlotDescriptor::Create(slotIndex, microOp), slots, validMask, nopMask);

		std::optional<DecodedBundleDependencySummary> dependencySummary;
		DecodedBundleAdmissionPrep admissionPrep;
		ResolveTransportFactSummary(slots, validMask, nopMask, dependencySummary, admissionPrep);

		return DecodedBundleTransportFacts(pc, slots, validMask, nopMask, dependencySummary, admissionPrep, DecodedBundleStateKind::ForegroundMutated, DecodedBundleStateOrigin::SingleSlotMutation);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildTransportFactsFromClearMaskMutationContour(ulong pc, IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, byte clearSlotMask)
	DecodedBundleTransportFacts DecodedBundleSlotCarrierBuilder::BuildTransportFactsFromClearMaskMutationContour(unsigned long long pc, const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, unsigned char clearSlotMask)
	{
		// slotDescriptors is a native reference and cannot be null.

		std::vector<DecodedBundleSlotDescriptor> slots;
		unsigned char validMask;
		unsigned char nopMask;
		ResolveTransportFactsShapeWithClearMaskMutation(slotDescriptors, clearSlotMask, slots, validMask, nopMask);

		std::optional<DecodedBundleDependencySummary> dependencySummary;
		DecodedBundleAdmissionPrep admissionPrep;
		ResolveTransportFactSummary(slots, validMask, nopMask, dependencySummary, admissionPrep);

		return DecodedBundleTransportFacts(pc, slots, validMask, nopMask, dependencySummary, admissionPrep, DecodedBundleStateKind::ForegroundMutated, DecodedBundleStateOrigin::ClearMaskMutation);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildTransportFactsFromSlotDescriptorProjection(ulong pc, IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, DecodedBundleStateKind stateKind = DecodedBundleStateKind.ForegroundMutated, DecodedBundleStateOrigin stateOrigin = DecodedBundleStateOrigin.ForegroundBundlePublication)
	DecodedBundleTransportFacts DecodedBundleSlotCarrierBuilder::BuildTransportFactsFromSlotDescriptorProjection(unsigned long long pc, const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, DecodedBundleStateKind stateKind, DecodedBundleStateOrigin stateOrigin)
	{
		// slotDescriptors is a native reference and cannot be null.

		std::vector<DecodedBundleSlotDescriptor> slots;
		unsigned char validMask;
		unsigned char nopMask;
		std::optional<DecodedBundleDependencySummary> dependencySummary;
		DecodedBundleAdmissionPrep admissionPrep;
		ResolveTransportFacts(slotDescriptors, slots, validMask, nopMask, dependencySummary, admissionPrep);

		return DecodedBundleTransportFacts(pc, slots, validMask, nopMask, dependencySummary, admissionPrep, stateKind, stateOrigin);
	}

	DecodedBundleTransportFacts DecodedBundleSlotCarrierBuilder::BuildTransportFactsFromProgressProjection(DecodedBundleTransportFacts const baseTransportFacts, unsigned char remainingMask)
	{
		unsigned char effectiveRemainingMask = static_cast<unsigned char>(remainingMask & baseTransportFacts.getValidNonEmptyMask());
		unsigned char clearSlotMask = static_cast<unsigned char>(baseTransportFacts.getValidNonEmptyMask() & ~effectiveRemainingMask);

		if (clearSlotMask == 0)
		{
			return baseTransportFacts;
		}

		std::vector<DecodedBundleSlotDescriptor> slots;
		unsigned char validMask;
		unsigned char nopMask;
		ResolveTransportFactsShapeWithClearMaskMutation(baseTransportFacts.getSlots(), clearSlotMask, slots, validMask, nopMask);

		std::optional<DecodedBundleDependencySummary> dependencySummary;
		DecodedBundleAdmissionPrep admissionPrep;
		ResolveTransportFactSummary(slots, validMask, nopMask, dependencySummary, admissionPrep);

		return DecodedBundleTransportFacts(baseTransportFacts.getPC(), slots, validMask, nopMask, dependencySummary, admissionPrep, baseTransportFacts.getStateKind(), baseTransportFacts.getStateOrigin());
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveTransportFacts(IReadOnlyList<MicroOp?> carrierBundle, out DecodedBundleSlotDescriptor[] slots, out byte validMask, out byte nopMask, out System.Nullable<DecodedBundleDependencySummary> dependencySummary, out DecodedBundleAdmissionPrep admissionPrep)
	void DecodedBundleSlotCarrierBuilder::ResolveTransportFacts(const std::vector<std::shared_ptr<MicroOp>> &carrierBundle, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char &validMask, unsigned char &nopMask, std::optional<DecodedBundleDependencySummary> &dependencySummary, DecodedBundleAdmissionPrep &admissionPrep)
	{
		ResolveCarrierTransportFactsShape(carrierBundle, slots, validMask, nopMask);

		ResolveTransportFactSummary(slots, validMask, nopMask, dependencySummary, admissionPrep);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveTransportFacts(IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, out DecodedBundleSlotDescriptor[] slots, out byte validMask, out byte nopMask, out System.Nullable<DecodedBundleDependencySummary> dependencySummary, out DecodedBundleAdmissionPrep admissionPrep)
	void DecodedBundleSlotCarrierBuilder::ResolveTransportFacts(const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char &validMask, unsigned char &nopMask, std::optional<DecodedBundleDependencySummary> &dependencySummary, DecodedBundleAdmissionPrep &admissionPrep)
	{
		ResolveTransportFactsShape(slotDescriptors, slots, validMask, nopMask);

		ResolveTransportFactSummary(slots, validMask, nopMask, dependencySummary, admissionPrep);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveTransportFactSummary(IReadOnlyList<DecodedBundleSlotDescriptor> slots, byte validMask, byte nopMask, out System.Nullable<DecodedBundleDependencySummary> dependencySummary, out DecodedBundleAdmissionPrep admissionPrep)
	void DecodedBundleSlotCarrierBuilder::ResolveTransportFactSummary(const std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char validMask, unsigned char nopMask, std::optional<DecodedBundleDependencySummary> &dependencySummary, DecodedBundleAdmissionPrep &admissionPrep)
	{
		unsigned long long readRegisterMask = 0;
		unsigned long long writeRegisterMask = 0;
		ResourceBitset aggregateResourceMask = ResourceBitset::Zero;

		for (unsigned char slotIndex = 0; slotIndex < BundleWidth; slotIndex++)
		{
			DecodedBundleSlotDescriptor slot = slots[slotIndex];
			if (slot.isEmptyOrNop())
			{
				continue;
			}

			if (!slot.getReadRegisters().empty())
			{
				for (int i = 0; i < slot.getReadRegisters().size(); i++)
				{
					int registerId = slot.getReadRegisters()[i];
					readRegisterMask |= EncodeDependencyRegisterMaskBit(registerId, slotIndex, L"read");
				}
			}

			if (!slot.getWriteRegisters().empty())
			{
				for (int i = 0; i < slot.getWriteRegisters().size(); i++)
				{
					int registerId = slot.getWriteRegisters()[i];
					writeRegisterMask |= EncodeDependencyRegisterMaskBit(registerId, slotIndex, L"write");
				}
			}

			if (slot.getMicroOp() != nullptr)
			{
				aggregateResourceMask = aggregateResourceMask | slot.getMicroOp()->getResourceMask();
			}
		}

		if (validMask != 0)
		{
			dependencySummary = Legality::BundleLegalityAnalyzer::BuildDependencySummary(slots, readRegisterMask, writeRegisterMask, aggregateResourceMask);
		}
		else
		{
			dependencySummary = std::nullopt;
		}
		admissionPrep = dependencySummary ? BuildAdmissionPrep(slots, validMask, nopMask, dependencySummary.value()) : YAKSys_Hybrid_CPU::Core::DecodedBundleAdmissionPrep();
	}

	unsigned long long DecodedBundleSlotCarrierBuilder::EncodeDependencyRegisterMaskBit(int registerId, unsigned char slotIndex, const std::wstring &accessKind)
	{
		if (static_cast<unsigned int>(registerId) >= ArchRegId::DependencyMaskBitCount)
		{
			throw InvalidOperationException(std::format(L"DecodedBundleSlotCarrierBuilder.ResolveTransportFactSummary observed slot {} {} register id {}, but dependency masks are limited to architectural register ids in [0, {}]. Migrate DecodedBundleDependencySummary to a wider bitset before publishing larger register ids.", slotIndex, accessKind, registerId, ArchRegId::DependencyMaskBitCount - 1));
		}

		return 1ULL << registerId;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveCarrierTransportFactsShape(IReadOnlyList<MicroOp?> carrierBundle, out DecodedBundleSlotDescriptor[] slots, out byte validMask, out byte nopMask)
	void DecodedBundleSlotCarrierBuilder::ResolveCarrierTransportFactsShape(const std::vector<std::shared_ptr<MicroOp>> &carrierBundle, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char &validMask, unsigned char &nopMask)
	{
		slots = std::vector<DecodedBundleSlotDescriptor>(BundleWidth);
		validMask = 0;
		nopMask = 0;

		for (unsigned char slotIndex = 0; slotIndex < BundleWidth; slotIndex++)
		{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp? microOp = slotIndex < carrierBundle.Count ? carrierBundle[slotIndex] : null;
			std::shared_ptr<MicroOp> microOp = slotIndex < carrierBundle.size() ? carrierBundle[slotIndex] : nullptr;
			DecodedBundleSlotDescriptor slot = DecodedBundleSlotDescriptor::Create(slotIndex, microOp);
			slots[slotIndex] = slot;

			unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
			if (slot.isValid())
			{
				validMask |= slotBit;
			}

			if (slot.isEmptyOrNop())
			{
				nopMask |= slotBit;
			}
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveCanonicalCarrierTransportFactsShape(IReadOnlyList<MicroOp?> carrierBundle, Decoder.DecodedInstructionBundle canonicalBundle, out DecodedBundleSlotDescriptor[] slots, out byte validMask, out byte nopMask)
	void DecodedBundleSlotCarrierBuilder::ResolveCanonicalCarrierTransportFactsShape(const std::vector<std::shared_ptr<MicroOp>> &carrierBundle, std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalBundle, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char &validMask, unsigned char &nopMask)
	{
		slots = std::vector<DecodedBundleSlotDescriptor>(BundleWidth);
		validMask = 0;
		nopMask = 0;

		for (unsigned char slotIndex = 0; slotIndex < BundleWidth; slotIndex++)
		{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp? microOp = slotIndex < carrierBundle.Count ? carrierBundle[slotIndex] : null;
			std::shared_ptr<MicroOp> microOp = slotIndex < carrierBundle.size() ? carrierBundle[slotIndex] : nullptr;
			DecodedBundleSlotDescriptor slot = ResolveCanonicalCarrierTransportSlotDescriptor(slotIndex, microOp, canonicalBundle);
			slots[slotIndex] = slot;

			unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
			if (slot.isValid())
			{
				validMask |= slotBit;
			}

			if (slot.isEmptyOrNop())
			{
				nopMask |= slotBit;
			}
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static DecodedBundleSlotDescriptor ResolveCanonicalCarrierTransportSlotDescriptor(byte slotIndex, MicroOp? microOp, Decoder.DecodedInstructionBundle canonicalBundle)
	DecodedBundleSlotDescriptor DecodedBundleSlotCarrierBuilder::ResolveCanonicalCarrierTransportSlotDescriptor(unsigned char slotIndex, std::shared_ptr<MicroOp> microOp, std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalBundle)
	{
		bool isVectorOp = ResolveCanonicalExecutionVectorClassification(slotIndex, microOp, canonicalBundle);

		std::shared_ptr<TrapMicroOp> trapMicroOp = std::dynamic_pointer_cast<TrapMicroOp>(microOp);
		if (trapMicroOp != nullptr && slotIndex < canonicalBundle->getSlotCount())
		{
			std::shared_ptr<Decoder::DecodedInstruction> decodedSlot = canonicalBundle->GetDecodedSlot(slotIndex);
			if (decodedSlot != nullptr && decodedSlot->IsOccupied)
			{
				return BuildCanonicalTrapTransportSlotDescriptor(slotIndex, trapMicroOp, decodedSlot, isVectorOp);
			}
		}

		return DecodedBundleSlotDescriptor::Create(slotIndex, microOp, isVectorOp);
	}

	DecodedBundleSlotDescriptor DecodedBundleSlotCarrierBuilder::BuildCanonicalTrapTransportSlotDescriptor(unsigned char slotIndex, std::shared_ptr<TrapMicroOp> trapMicroOp, std::shared_ptr<Decoder::DecodedInstruction> decodedSlot, bool isVectorOp)
	{
		if (decodedSlot == nullptr || !decodedSlot->IsOccupied || decodedSlot->Instruction == nullptr)
		{
			throw std::invalid_argument("Canonical trap transport requires an occupied decoded slot with an instruction.");
		}
		std::shared_ptr<Pipeline::MicroOps::InstructionIR> instruction = decodedSlot->Instruction;
		bool writesRegister = Legality::BundleLegalityAnalyzer::MayWriteArchitecturalRegister(instruction);
		bool isMemoryOp = Legality::BundleLegalityAnalyzer::IsMemoryLikeClass(instruction->Class);
		bool isControlFlow = instruction->Class == Arch::InstructionClass::ControlFlow;
		if (decodedSlot->SlotMetadata == nullptr)
		{
			throw std::invalid_argument("Canonical trap transport requires decoded slot metadata.");
		}
		int virtualThreadId = trapMicroOp->getVirtualThreadId() != 0 ? trapMicroOp->getVirtualThreadId() : static_cast<int>(decodedSlot->SlotMetadata->VirtualThreadId.getValue());
		int ownerThreadId = trapMicroOp->getOwnerThreadId() != 0 ? trapMicroOp->getOwnerThreadId() : virtualThreadId;
		SlotPlacementMetadata placement = Legality::BundleLegalityAnalyzer::BuildCanonicalPlacement(instruction->Class, ResolveCanonicalTransportDomainTag(trapMicroOp, decodedSlot));

		if (instruction->CanonicalOpcode == nullptr)
		{
			throw std::invalid_argument("Canonical trap transport requires a canonical opcode.");
		}
		return DecodedBundleSlotDescriptor(trapMicroOp, slotIndex, virtualThreadId, ownerThreadId, instruction->CanonicalOpcode->Value, Legality::BundleLegalityAnalyzer::GetCanonicalReadRegisters(instruction), Legality::BundleLegalityAnalyzer::GetCanonicalWriteRegisters(instruction, writesRegister), writesRegister, isMemoryOp, isControlFlow, placement, isMemoryOp ? trapMicroOp->getProjectedMemoryBankIntent(): -1, trapMicroOp->isFspInjected(), false, isVectorOp);
	}

	unsigned long long DecodedBundleSlotCarrierBuilder::ResolveCanonicalTransportDomainTag(std::shared_ptr<MicroOp> microOp, std::shared_ptr<Decoder::DecodedInstruction> decodedSlot)
	{
		unsigned long long domainTag = microOp->getAdmissionMetadata()->DomainTag;
		if (domainTag != 0)
		{
			return domainTag;
		}

		domainTag = microOp->getPlacement().DomainTag;
		if (domainTag != 0)
		{
			return domainTag;
		}

		if (decodedSlot == nullptr || decodedSlot->SlotMetadata == nullptr || decodedSlot->SlotMetadata->SlotMetadata == nullptr)
		{
			throw std::invalid_argument("Canonical transport slot is missing admission metadata.");
		}
		std::shared_ptr<MicroOpAdmissionMetadata> canonicalAdmissionMetadata = decodedSlot->SlotMetadata->SlotMetadata->AdmissionMetadata;
		if (canonicalAdmissionMetadata == nullptr)
		{
			throw std::invalid_argument("Canonical transport slot is missing admission metadata.");
		}
		if (canonicalAdmissionMetadata->DomainTag != 0)
		{
			return canonicalAdmissionMetadata->DomainTag;
		}

		return canonicalAdmissionMetadata->Placement.DomainTag;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool ResolveCanonicalExecutionVectorClassification(byte slotIndex, MicroOp? microOp, Decoder.DecodedInstructionBundle canonicalBundle)
	bool DecodedBundleSlotCarrierBuilder::ResolveCanonicalExecutionVectorClassification(unsigned char slotIndex, std::shared_ptr<MicroOp> microOp, std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalBundle)
	{
		if (slotIndex < canonicalBundle->getSlotCount())
		{
			std::shared_ptr<Decoder::DecodedInstruction> decodedSlot = canonicalBundle->GetDecodedSlot(slotIndex);
			if (decodedSlot != nullptr && decodedSlot->IsOccupied)
			{
				if (decodedSlot->Instruction == nullptr || decodedSlot->Instruction->CanonicalOpcode == nullptr)
				{
					throw std::invalid_argument("Occupied canonical slot is missing its opcode.");
				}
				return Arch::OpcodeRegistry::IsVectorOp(decodedSlot->Instruction->CanonicalOpcode->Value);
			}
		}

		return std::dynamic_pointer_cast<VectorMicroOp>(microOp) != nullptr || (microOp != nullptr && microOp->getClass() == MicroOpClass::Vector);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveTransportFactsShape(IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, out DecodedBundleSlotDescriptor[] slots, out byte validMask, out byte nopMask)
	void DecodedBundleSlotCarrierBuilder::ResolveTransportFactsShape(const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char &validMask, unsigned char &nopMask)
	{
		slots = std::vector<DecodedBundleSlotDescriptor>(BundleWidth);
		validMask = 0;
		nopMask = 0;

		for (unsigned char slotIndex = 0; slotIndex < BundleWidth; slotIndex++)
		{
			DecodedBundleSlotDescriptor slot = NormalizeSlotDescriptor(slotIndex, slotDescriptors);
			slots[slotIndex] = slot;

			unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
			if (slot.isValid())
			{
				validMask |= slotBit;
			}

			if (slot.isEmptyOrNop())
			{
				nopMask |= slotBit;
			}
		}
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveTransportFactsShapeWithSingleSlotMutation(IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, byte replacementSlotIndex, in DecodedBundleSlotDescriptor replacementSlot, out DecodedBundleSlotDescriptor[] slots, out byte validMask, out byte nopMask)
	void DecodedBundleSlotCarrierBuilder::ResolveTransportFactsShapeWithSingleSlotMutation(const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, unsigned char replacementSlotIndex, DecodedBundleSlotDescriptor const replacementSlot, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char &validMask, unsigned char &nopMask)
	{
		slots = std::vector<DecodedBundleSlotDescriptor>(BundleWidth);
		validMask = 0;
		nopMask = 0;

		for (unsigned char slotIndex = 0; slotIndex < BundleWidth; slotIndex++)
		{
			DecodedBundleSlotDescriptor slot = slotIndex == replacementSlotIndex ? replacementSlot : NormalizeSlotDescriptor(slotIndex, slotDescriptors);
			slots[slotIndex] = slot;

			unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
			if (slot.isValid())
			{
				validMask |= slotBit;
			}

			if (slot.isEmptyOrNop())
			{
				nopMask |= slotBit;
			}
		}
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveTransportFactsShapeWithClearMaskMutation(IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, byte clearSlotMask, out DecodedBundleSlotDescriptor[] slots, out byte validMask, out byte nopMask)
	void DecodedBundleSlotCarrierBuilder::ResolveTransportFactsShapeWithClearMaskMutation(const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, unsigned char clearSlotMask, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char &validMask, unsigned char &nopMask)
	{
		slots = std::vector<DecodedBundleSlotDescriptor>(BundleWidth);
		validMask = 0;
		nopMask = 0;

		for (unsigned char slotIndex = 0; slotIndex < BundleWidth; slotIndex++)
		{
			DecodedBundleSlotDescriptor slot = (clearSlotMask & (1 << slotIndex)) != 0 ? DecodedBundleSlotDescriptor::Create(slotIndex, nullptr) : NormalizeSlotDescriptor(slotIndex, slotDescriptors);
			slots[slotIndex] = slot;

			unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
			if (slot.isValid())
			{
				validMask |= slotBit;
			}

			if (slot.isEmptyOrNop())
			{
				nopMask |= slotBit;
			}
		}
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static DecodedBundleSlotDescriptor NormalizeSlotDescriptor(byte slotIndex, IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors)
	DecodedBundleSlotDescriptor DecodedBundleSlotCarrierBuilder::NormalizeSlotDescriptor(unsigned char slotIndex, const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors)
	{
		if (slotIndex >= slotDescriptors.size())
		{
			return DecodedBundleSlotDescriptor::Create(slotIndex, nullptr);
		}

		DecodedBundleSlotDescriptor slot = slotDescriptors[slotIndex];
		if (slot.getMicroOp() != nullptr)
		{
			return DecodedBundleSlotDescriptor::Create(slotIndex, slot.getMicroOp());
		}

		if (!slot.isValid() && !slot.isEmptyOrNop())
		{
			return DecodedBundleSlotDescriptor::Create(slotIndex, nullptr);
		}

		return slot;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static DecodedBundleAdmissionPrep BuildAdmissionPrep(IReadOnlyList<DecodedBundleSlotDescriptor> slots, byte validMask, byte nopMask, DecodedBundleDependencySummary dependencySummary)
	DecodedBundleAdmissionPrep DecodedBundleSlotCarrierBuilder::BuildAdmissionPrep(const std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char validMask, unsigned char nopMask, DecodedBundleDependencySummary dependencySummary)
	{
		unsigned char nonEmptyMask = static_cast<unsigned char>(validMask & ~nopMask);
		unsigned char scalarCandidateMask = ResolveAuthoritativeScalarCandidateMask(slots, dependencySummary.getScalarClusterEligibleMask());
		unsigned long long blockerPairMask = dependencySummary.getRawDependencyMask() | dependencySummary.getWawDependencyMask() | dependencySummary.getWarDependencyMask() | dependencySummary.getControlConflictMask() | dependencySummary.getMemoryConflictMask();
		unsigned char blockedSlotMask = ExtractBlockedTargetSlotMask(blockerPairMask);
		unsigned char wideReadyScalarMask = static_cast<unsigned char>(scalarCandidateMask & ~blockedSlotMask);
		unsigned char auxiliaryOpMask = static_cast<unsigned char>(nonEmptyMask & ~scalarCandidateMask);
		unsigned char narrowOnlyMask = static_cast<unsigned char>(nonEmptyMask & ~wideReadyScalarMask);

		DecodedBundleAdmissionFlags flags = DecodedBundleAdmissionFlags::None;
		if (scalarCandidateMask != 0)
		{
			flags = static_cast<DecodedBundleAdmissionFlags>(static_cast<int>(flags) | static_cast<int>(DecodedBundleAdmissionFlags::HasScalarClusterCandidates));
		}
		if (wideReadyScalarMask != 0)
		{
			flags = static_cast<DecodedBundleAdmissionFlags>(static_cast<int>(flags) | static_cast<int>(DecodedBundleAdmissionFlags::HasWideReadyScalarCandidates));
		}
		if ((dependencySummary.getRawDependencyMask() | dependencySummary.getWawDependencyMask() | dependencySummary.getWarDependencyMask()) != 0)
		{
			flags = static_cast<DecodedBundleAdmissionFlags>(static_cast<int>(flags) | static_cast<int>(DecodedBundleAdmissionFlags::HasBundleLocalDependencies));
		}
		if (dependencySummary.getControlConflictMask() != 0)
		{
			flags = static_cast<DecodedBundleAdmissionFlags>(static_cast<int>(flags) | static_cast<int>(DecodedBundleAdmissionFlags::HasControlConflicts));
		}
		if (dependencySummary.getMemoryConflictMask() != 0)
		{
			flags = static_cast<DecodedBundleAdmissionFlags>(static_cast<int>(flags) | static_cast<int>(DecodedBundleAdmissionFlags::HasMemoryOrderingConstraints));
		}
		if (auxiliaryOpMask != 0)
		{
			flags = static_cast<DecodedBundleAdmissionFlags>(static_cast<int>(flags) | static_cast<int>(DecodedBundleAdmissionFlags::HasAuxiliaryClusterOps));
		}

		int partialWideReadyCount = std::popcount(static_cast<unsigned int>(wideReadyScalarMask));
		if (nonEmptyMask != 0 && partialWideReadyCount <= 1)
		{
			flags = static_cast<DecodedBundleAdmissionFlags>(static_cast<int>(flags) | static_cast<int>(DecodedBundleAdmissionFlags::SuggestNarrowFallback));
		}

		return DecodedBundleAdmissionPrep(scalarCandidateMask, wideReadyScalarMask, auxiliaryOpMask, narrowOnlyMask, flags);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static byte ResolveAuthoritativeScalarCandidateMask(IReadOnlyList<DecodedBundleSlotDescriptor> slots, byte scalarClusterEligibleMask)
	unsigned char DecodedBundleSlotCarrierBuilder::ResolveAuthoritativeScalarCandidateMask(const std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char scalarClusterEligibleMask)
	{
		unsigned char sanitizedMask = 0;
		int slotCount = std::min(BundleWidth, static_cast<int>(slots.size()));

		for (int slotIndex = 0; slotIndex < slotCount; slotIndex++)
		{
			unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
			if ((scalarClusterEligibleMask & slotBit) == 0)
			{
				continue;
			}

			if (IsAuthoritativeScalarAdmissionCandidate(slots[slotIndex]))
			{
				sanitizedMask |= slotBit;
			}
		}

		return sanitizedMask;
	}

	bool DecodedBundleSlotCarrierBuilder::IsAuthoritativeScalarAdmissionCandidate(DecodedBundleSlotDescriptor const slot)
	{
		if (slot.GetRuntimeExecutionIsEmptyOrNop() || slot.GetRuntimeAdmissionIsMemoryOp() || slot.GetRuntimeAdmissionIsControlFlow() || slot.isVectorOp())
		{
			return false;
		}

		SlotPlacementMetadata placement = slot.GetRuntimeAdmissionPlacement();
		return placement.RequiredSlotClass == SlotClass::AluClass && placement.PinningKind == SlotPinningKind::ClassFlexible;
	}

	unsigned long long DecodedBundleSlotCarrierBuilder::EncodeSlotPair(int sourceSlotIndex, int targetSlotIndex)
	{
		return 1ULL << ((sourceSlotIndex * BundleWidth) + targetSlotIndex);
	}

	unsigned char DecodedBundleSlotCarrierBuilder::ExtractBlockedTargetSlotMask(unsigned long long pairMask)
	{
		unsigned char slotMask = 0;

		for (int sourceSlotIndex = 0; sourceSlotIndex < BundleWidth; sourceSlotIndex++)
		{
			for (int targetSlotIndex = sourceSlotIndex + 1; targetSlotIndex < BundleWidth; targetSlotIndex++)
			{
				if ((pairMask & EncodeSlotPair(sourceSlotIndex, targetSlotIndex)) == 0)
				{
					continue;
				}

				slotMask |= static_cast<unsigned char>(1 << targetSlotIndex);
			}
		}

		return slotMask;
	}
}
