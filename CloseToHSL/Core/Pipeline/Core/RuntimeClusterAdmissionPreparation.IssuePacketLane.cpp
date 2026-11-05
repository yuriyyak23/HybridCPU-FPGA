#include "RuntimeClusterAdmissionPreparation.IssuePacketLane.h"
#include "../Scheduling/SlotPlacementMetadata.h"
#include "../MicroOps/Types/MicroOp.h"
#include "../Metadata/MicroOpAdmissionMetadata.h"

namespace YAKSys_Hybrid_CPU::Core
{

	IssuePacketLane::IssuePacketLane(unsigned char physicalLaneIndex, bool isOccupied, unsigned char slotIndex, int virtualThreadId, int ownerThreadId, unsigned int opCode, std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> microOp, SlotClass requiredSlotClass, SlotPinningKind pinningKind, bool countsTowardScalarProjection)
	{
		PhysicalLaneIndex = physicalLaneIndex;
		IsOccupied = isOccupied;
		SlotIndex = slotIndex;
		VirtualThreadId = virtualThreadId;
		OwnerThreadId = ownerThreadId;
		OpCode = opCode;
		MicroOp = microOp;
		RequiredSlotClass = requiredSlotClass;
		PinningKind = pinningKind;
		CountsTowardScalarProjection = countsTowardScalarProjection;
	}

	unsigned char IssuePacketLane::getPhysicalLaneIndex() const
	{
		return PhysicalLaneIndex;
	}

	unsigned char IssuePacketLane::getLaneIndex() const
	{
		return getPhysicalLaneIndex();
	}

	bool IssuePacketLane::isOccupied() const
	{
		return IsOccupied;
	}

	unsigned char IssuePacketLane::getSlotIndex() const
	{
		return SlotIndex;
	}

	int IssuePacketLane::getVirtualThreadId() const
	{
		return VirtualThreadId;
	}

	int IssuePacketLane::getOwnerThreadId() const
	{
		return OwnerThreadId;
	}

	unsigned int IssuePacketLane::getOpCode() const
	{
		return OpCode;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> IssuePacketLane::getMicroOp() const
	{
		return MicroOp;
	}

	SlotClass IssuePacketLane::getRequiredSlotClass() const
	{
		return RequiredSlotClass;
	}

	SlotPinningKind IssuePacketLane::getPinningKind() const
	{
		return PinningKind;
	}

	bool IssuePacketLane::getCountsTowardScalarProjection() const
	{
		return CountsTowardScalarProjection;
	}

	bool IssuePacketLane::isNonScalarSelection() const
	{
		return isOccupied() && !getCountsTowardScalarProjection();
	}

	IssuePacketLane IssuePacketLane::Create(unsigned char physicalLaneIndex, ScalarClusterIssueEntry entry)
	{
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> microOp = entry.getMicroOp();
		if (microOp == nullptr)
		{
			throw InvalidOperationException(L"Prepared scalar-cluster entry must carry a live MicroOp.");
		}
		SlotPlacementMetadata placement = microOp->getAdmissionMetadata()->Placement;

		return IssuePacketLane(physicalLaneIndex, true, entry.getSlotIndex(), entry.getVirtualThreadId(), entry.getOwnerThreadId(), entry.getOpCode(), microOp, placement.RequiredSlotClass, placement.PinningKind, true);
	}

	IssuePacketLane IssuePacketLane::Create(unsigned char physicalLaneIndex, DecodedBundleSlotDescriptor slot, bool countsTowardScalarProjection)
	{
		SlotPlacementMetadata placement = slot.GetRuntimeAdmissionPlacement();

		return IssuePacketLane(physicalLaneIndex, true, slot.getSlotIndex(), slot.GetRuntimeExecutionVirtualThreadId(), slot.GetRuntimeExecutionOwnerThreadId(), slot.GetRuntimeExecutionOpCode(), slot.getMicroOp(), placement.RequiredSlotClass, placement.PinningKind, countsTowardScalarProjection);
	}

	IssuePacketLane IssuePacketLane::CreateEmpty(unsigned char physicalLaneIndex)
	{
		return IssuePacketLane(physicalLaneIndex, false, 0, 0, 0, 0, nullptr, SlotClass::Unclassified, SlotPinningKind::ClassFlexible, false);
	}
}
