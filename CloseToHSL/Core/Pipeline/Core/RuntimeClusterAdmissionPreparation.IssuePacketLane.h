#pragma once

namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }
#include "../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../Decoder/ClusterIssuePreparation.h"
#include "../../Decoder/DecodedBundleDescriptor.h"
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct IssuePacketLane
	class IssuePacketLane
	{
	private:
		unsigned char PhysicalLaneIndex;
		bool IsOccupied;
		unsigned char SlotIndex;
		int VirtualThreadId;
		int OwnerThreadId;
		unsigned int OpCode;
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> MicroOp;
		SlotClass RequiredSlotClass;
		SlotPinningKind PinningKind;
		bool CountsTowardScalarProjection;

	public:
		IssuePacketLane(unsigned char physicalLaneIndex, bool isOccupied, unsigned char slotIndex, int virtualThreadId, int ownerThreadId, unsigned int opCode, std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> microOp, SlotClass requiredSlotClass, SlotPinningKind pinningKind, bool countsTowardScalarProjection);

		unsigned char getPhysicalLaneIndex() const;
		unsigned char getLaneIndex() const;
		bool isOccupied() const;
		unsigned char getSlotIndex() const;
		int getVirtualThreadId() const;
		int getOwnerThreadId() const;
		unsigned int getOpCode() const;
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> getMicroOp() const;
		SlotClass getRequiredSlotClass() const;
		SlotPinningKind getPinningKind() const;
		bool getCountsTowardScalarProjection() const;
		bool isNonScalarSelection() const;

		static IssuePacketLane Create(unsigned char physicalLaneIndex, ScalarClusterIssueEntry entry);

		static IssuePacketLane Create(unsigned char physicalLaneIndex, DecodedBundleSlotDescriptor slot, bool countsTowardScalarProjection);

		static IssuePacketLane CreateEmpty(unsigned char physicalLaneIndex);

		IssuePacketLane() = default;
	};
}
