#include "RuntimeClusterAdmissionPreparation.BundleIssuePacket.h"
#include "../MicroOps/Types/MicroOp.h"
#include "../Scheduling/SlotPlacementMetadata.h"
#include "../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../Scheduling/PostStageBIssuedAttempt.h"
#include "../../Decoder/Rf06ExecutionContracts.h"
#include "../Metadata/BundleMetadata.h"
#include "../Metadata/MicroOpAdmissionMetadata.h"

namespace YAKSys_Hybrid_CPU::Core
{

	BundleIssuePacket::BundleIssuePacket(unsigned long long pc, YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode, unsigned char validNonEmptyMask, unsigned char scalarCandidateMask, unsigned char scalarIssueMask, unsigned char selectedSlotMask, unsigned char unmappedSelectedSlotMask, unsigned char preparedScalarMask, unsigned char refinedPreparedScalarMask, int advisoryScalarIssueWidth, int refinedAdvisoryScalarIssueWidth, RuntimeClusterAdmissionExecutionMode executionMode, bool shouldProbeClusterPath, bool usesIssuePacketAsExecutionSource, bool retainsReferenceSequentialPath, IssuePacketLane lane0, IssuePacketLane lane1, IssuePacketLane lane2, IssuePacketLane lane3, IssuePacketLane lane4, IssuePacketLane lane5, IssuePacketLane lane6, IssuePacketLane lane7, BundleIssueFallbackInfo fallbackInfo)
	{
		PC = pc;
		DecodeMode = decodeMode;
		ValidNonEmptyMask = validNonEmptyMask;
		ScalarCandidateMask = scalarCandidateMask;
		ScalarIssueMask = scalarIssueMask;
		SelectedSlotMask = selectedSlotMask;
		UnmappedSelectedSlotMask = unmappedSelectedSlotMask;
		PreparedScalarMask = preparedScalarMask;
		RefinedPreparedScalarMask = refinedPreparedScalarMask;
		AdvisoryScalarIssueWidth = advisoryScalarIssueWidth;
		RefinedAdvisoryScalarIssueWidth = refinedAdvisoryScalarIssueWidth;
		ExecutionMode = executionMode;
		ShouldProbeClusterPath = shouldProbeClusterPath;
		UsesIssuePacketAsExecutionSource = usesIssuePacketAsExecutionSource;
		RetainsReferenceSequentialPath = retainsReferenceSequentialPath;
		Lane0 = lane0;
		Lane1 = lane1;
		Lane2 = lane2;
		Lane3 = lane3;
		Lane4 = lane4;
		Lane5 = lane5;
		Lane6 = lane6;
		Lane7 = lane7;
		FallbackInfo = fallbackInfo;
	}

	unsigned long long BundleIssuePacket::getPC() const
	{
		return PC;
	}

	YAKSys_Hybrid_CPU::Core::DecodeMode BundleIssuePacket::getDecodeMode() const
	{
		return DecodeMode;
	}

	unsigned char BundleIssuePacket::getValidNonEmptyMask() const
	{
		return ValidNonEmptyMask;
	}

	unsigned char BundleIssuePacket::getScalarCandidateMask() const
	{
		return ScalarCandidateMask;
	}

	unsigned char BundleIssuePacket::getScalarIssueMask() const
	{
		return ScalarIssueMask;
	}

	unsigned char BundleIssuePacket::getSelectedSlotMask() const
	{
		return SelectedSlotMask;
	}

	unsigned char BundleIssuePacket::getUnmappedSelectedSlotMask() const
	{
		return UnmappedSelectedSlotMask;
	}

	unsigned char BundleIssuePacket::getPreparedScalarMask() const
	{
		return PreparedScalarMask;
	}

	unsigned char BundleIssuePacket::getRefinedPreparedScalarMask() const
	{
		return RefinedPreparedScalarMask;
	}

	int BundleIssuePacket::getAdvisoryScalarIssueWidth() const
	{
		return AdvisoryScalarIssueWidth;
	}

	int BundleIssuePacket::getRefinedAdvisoryScalarIssueWidth() const
	{
		return RefinedAdvisoryScalarIssueWidth;
	}

	RuntimeClusterAdmissionExecutionMode BundleIssuePacket::getExecutionMode() const
	{
		return ExecutionMode;
	}

	bool BundleIssuePacket::getShouldProbeClusterPath() const
	{
		return ShouldProbeClusterPath;
	}

	bool BundleIssuePacket::getUsesIssuePacketAsExecutionSource() const
	{
		return UsesIssuePacketAsExecutionSource;
	}

	bool BundleIssuePacket::getRetainsReferenceSequentialPath() const
	{
		return RetainsReferenceSequentialPath;
	}

	IssuePacketLane BundleIssuePacket::getLane0() const
	{
		return Lane0;
	}

	IssuePacketLane BundleIssuePacket::getLane1() const
	{
		return Lane1;
	}

	IssuePacketLane BundleIssuePacket::getLane2() const
	{
		return Lane2;
	}

	IssuePacketLane BundleIssuePacket::getLane3() const
	{
		return Lane3;
	}

	IssuePacketLane BundleIssuePacket::getLane4() const
	{
		return Lane4;
	}

	IssuePacketLane BundleIssuePacket::getLane5() const
	{
		return Lane5;
	}

	IssuePacketLane BundleIssuePacket::getLane6() const
	{
		return Lane6;
	}

	IssuePacketLane BundleIssuePacket::getLane7() const
	{
		return Lane7;
	}

	BundleIssueFallbackInfo BundleIssuePacket::getFallbackInfo() const
	{
		return FallbackInfo;
	}

	unsigned char BundleIssuePacket::getSelectedNonScalarSlotMask() const
	{
		return static_cast<unsigned char>(getSelectedSlotMask() & ~getScalarIssueMask());
	}

	bool BundleIssuePacket::getHasUnmappedSelectedSlots() const
	{
		return getUnmappedSelectedSlotMask() != 0;
	}

	int BundleIssuePacket::getPreparedScalarLaneCount() const
	{
		return GetPreparedScalarLaneCount();
	}

	int BundleIssuePacket::getPreparedPhysicalLaneCount() const
	{
		return GetPreparedPhysicalLaneCount();
	}

	IssuePacketLane BundleIssuePacket::GetPhysicalLane(unsigned char laneIndex) const
	{
		switch (laneIndex)
		{
		case 0: return Lane0;
		case 1: return Lane1;
		case 2: return Lane2;
		case 3: return Lane3;
		case 4: return Lane4;
		case 5: return Lane5;
		case 6: return Lane6;
		case 7: return Lane7;
		default: return IssuePacketLane::CreateEmpty(laneIndex);
		}

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return laneIndex switch
	//		{
	//			0 => Lane0,
	//			1 => Lane1,
	//			2 => Lane2,
	//			3 => Lane3,
	//			4 => Lane4,
	//			5 => Lane5,
	//			6 => Lane6,
	//			7 => Lane7,
	//			_ => IssuePacketLane.CreateEmpty(laneIndex)
	//		};
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static BundleIssuePacket Create(ClusterIssuePreparation clusterPreparation, IReadOnlyList<DecodedBundleSlotDescriptor> slots, RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionDraft decisionDraft)
	BundleIssuePacket BundleIssuePacket::Create(std::shared_ptr<ClusterIssuePreparation> clusterPreparation, const std::vector<DecodedBundleSlotDescriptor> &slots, RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionDraft decisionDraft)
	{
		ArgumentNullException::ThrowIfNull(slots);

		IssuePacketLane lane0 = IssuePacketLane::CreateEmpty(0);
		IssuePacketLane lane1 = IssuePacketLane::CreateEmpty(1);
		IssuePacketLane lane2 = IssuePacketLane::CreateEmpty(2);
		IssuePacketLane lane3 = IssuePacketLane::CreateEmpty(3);
		IssuePacketLane lane4 = IssuePacketLane::CreateEmpty(4);
		IssuePacketLane lane5 = IssuePacketLane::CreateEmpty(5);
		IssuePacketLane lane6 = IssuePacketLane::CreateEmpty(6);
		IssuePacketLane lane7 = IssuePacketLane::CreateEmpty(7);

		std::vector<ScalarClusterIssueEntry> entries = clusterPreparation->getScalarClusterGroup()->getEntries();
		unsigned char scalarIssueMask = decisionDraft.getScalarIssueMask();
		unsigned char auxiliarySlotMask = decisionDraft.getAuxiliaryReservationMask();
		unsigned char selectedSlotMask = static_cast<unsigned char>(scalarIssueMask | auxiliarySlotMask);
		unsigned char occupiedPhysicalLaneMask = 0;
		unsigned char unmappedSelectedSlotMask = 0;

		// Original scheduler grants already own their physical lanes. Reserve
		// those handoffs before proposing placements for unissued owners.
		for (auto issuedEntry : entries)
		{
			if ((scalarIssueMask & (1 << issuedEntry.getSlotIndex())) == 0 || (issuedEntry.getMicroOp() == nullptr ? nullptr : issuedEntry.getMicroOp()->getPostStageBIssuedAttempt()) == nullptr)
			{
				continue;
			}
			if (!TryAssignToPacket(issuedEntry, occupiedPhysicalLaneMask, lane0, lane1, lane2, lane3, lane4, lane5, lane6, lane7))
			{
				unmappedSelectedSlotMask |= static_cast<unsigned char>(1 << issuedEntry.getSlotIndex());
			}
		}

		for (int entryIndex = 0; entryIndex < entries.size(); entryIndex++)
		{
			ScalarClusterIssueEntry entry = entries[entryIndex];
			unsigned char slotBit = static_cast<unsigned char>(1 << entry.getSlotIndex());
			if ((scalarIssueMask & slotBit) == 0)
			{
				continue;
			}
			if ((entry.getMicroOp() == nullptr ? nullptr : entry.getMicroOp()->getPostStageBIssuedAttempt()) != nullptr)
			{
				continue;
			}

			if (!TryAssignToPacket(entry, occupiedPhysicalLaneMask, lane0, lane1, lane2, lane3, lane4, lane5, lane6, lane7))
			{
				unmappedSelectedSlotMask |= slotBit;
			}
		}

		for (unsigned char slotIndex = 0; slotIndex < 8; slotIndex++)
		{
			unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
			if ((auxiliarySlotMask & slotBit) == 0)
			{
				continue;
			}

			if (slotIndex >= slots.size())
			{
				unmappedSelectedSlotMask |= slotBit;
				continue;
			}

			DecodedBundleSlotDescriptor slot = slots[slotIndex];
			if (!slot.isValid())
			{
				unmappedSelectedSlotMask |= slotBit;
				continue;
			}

			if (!TryAssignToPacket(slot, false, occupiedPhysicalLaneMask, lane0, lane1, lane2, lane3, lane4, lane5, lane6, lane7))
			{
				unmappedSelectedSlotMask |= slotBit;
			}
		}

		return BundleIssuePacket(candidateView.getPC(), candidateView.getDecodeMode(), candidateView.getValidNonEmptyMask(), candidateView.getScalarCandidateMask(), scalarIssueMask, selectedSlotMask, unmappedSelectedSlotMask, candidateView.getPreparedScalarMask(), candidateView.getRefinedPreparedScalarMask(), candidateView.getAdvisoryScalarIssueWidth(), candidateView.getRefinedAdvisoryScalarIssueWidth(), decisionDraft.getExecutionMode(), candidateView.getShouldConsiderClusterAdmission() && decisionDraft.getShouldProbeClusterPath(), decisionDraft.getUsesIssuePacketAsExecutionSource(), decisionDraft.getRetainsReferenceSequentialPath(), lane0, lane1, lane2, lane3, lane4, lane5, lane6, lane7, BundleIssueFallbackInfo::Create(candidateView, decisionDraft));
	}

	BundleIssuePacket BundleIssuePacket::CreateEmpty(unsigned long long pc)
	{
		return BundleIssuePacket(pc, DecodeMode::ReferenceSequentialMode, 0, 0, 0, 0, 0, 0, 0, 0, 0, RuntimeClusterAdmissionExecutionMode::Empty, false, false, true, IssuePacketLane::CreateEmpty(0), IssuePacketLane::CreateEmpty(1), IssuePacketLane::CreateEmpty(2), IssuePacketLane::CreateEmpty(3), IssuePacketLane::CreateEmpty(4), IssuePacketLane::CreateEmpty(5), IssuePacketLane::CreateEmpty(6), IssuePacketLane::CreateEmpty(7), BundleIssueFallbackInfo::CreateEmpty());
	}

	bool BundleIssuePacket::TryAssignToPacket(ScalarClusterIssueEntry entry, unsigned char &occupiedPhysicalLaneMask, IssuePacketLane &lane0, IssuePacketLane &lane1, IssuePacketLane &lane2, IssuePacketLane &lane3, IssuePacketLane &lane4, IssuePacketLane &lane5, IssuePacketLane &lane6, IssuePacketLane &lane7)
	{
		unsigned char physicalLaneIndex;
		if (!TryResolvePacketLane(entry, occupiedPhysicalLaneMask, physicalLaneIndex))
		{
			return false;
		}

		IssuePacketLane lane = IssuePacketLane::Create(physicalLaneIndex, entry);
		SetLane(physicalLaneIndex, lane, lane0, lane1, lane2, lane3, lane4, lane5, lane6, lane7);
		occupiedPhysicalLaneMask |= static_cast<unsigned char>(1 << physicalLaneIndex);
		return true;
	}

	bool BundleIssuePacket::TryAssignToPacket(DecodedBundleSlotDescriptor slot, bool countsTowardScalarProjection, unsigned char &occupiedPhysicalLaneMask, IssuePacketLane &lane0, IssuePacketLane &lane1, IssuePacketLane &lane2, IssuePacketLane &lane3, IssuePacketLane &lane4, IssuePacketLane &lane5, IssuePacketLane &lane6, IssuePacketLane &lane7)
	{
		unsigned char physicalLaneIndex;
		if (!TryResolvePacketLane(slot, occupiedPhysicalLaneMask, physicalLaneIndex))
		{
			return false;
		}

		IssuePacketLane lane = IssuePacketLane::Create(physicalLaneIndex, slot, countsTowardScalarProjection);
		SetLane(physicalLaneIndex, lane, lane0, lane1, lane2, lane3, lane4, lane5, lane6, lane7);
		occupiedPhysicalLaneMask |= static_cast<unsigned char>(1 << physicalLaneIndex);
		return true;
	}

	bool BundleIssuePacket::TryResolvePacketLane(ScalarClusterIssueEntry entry, unsigned char occupiedPhysicalLaneMask, unsigned char &physicalLaneIndex)
	{
		physicalLaneIndex = std::numeric_limits<unsigned char>::max();
		std::shared_ptr<MicroOp> microOp = entry.getMicroOp();
		if (microOp == nullptr)
		{
			throw InvalidOperationException(L"Prepared scalar-cluster entry must carry a live MicroOp.");
		}
		SlotPlacementMetadata placement = microOp->getAdmissionMetadata()->Placement;

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		std::shared_ptr<PostStageBIssuedAttempt> issued = microOp->getPostStageBIssuedAttempt();
		if (issued != nullptr)
		{
			std::shared_ptr<Decoder::ScheduledOperation> scheduledOperation = issued->getScheduledOperation();
			if (scheduledOperation == nullptr || !issued->isCurrent())
			{
				return false;
			}

			int lane = scheduledOperation->getPhysicalLane();
			if (lane < 0 || lane >= BundleMetadata::BundleSlotCount || (occupiedPhysicalLaneMask & (1 << lane)) != 0)
			{
				return false;
			}
			physicalLaneIndex = static_cast<unsigned char>(lane);
			return true;
		}

		if (placement.PinningKind == SlotPinningKind::HardPinned)
		{
			unsigned char pinnedLaneId = placement.PinnedLaneId;
			if (pinnedLaneId >= 8)
			{
				return false;
			}

			unsigned char pinnedLaneBit = static_cast<unsigned char>(1 << pinnedLaneId);
			if ((occupiedPhysicalLaneMask & pinnedLaneBit) != 0)
			{
				return false;
			}

			physicalLaneIndex = pinnedLaneId;
			return true;
		}

		if (placement.RequiredSlotClass == SlotClass::Unclassified)
		{
			return false;
		}

		unsigned char eligibleLaneMask = SlotClassLaneMap::GetLaneMask(placement.RequiredSlotClass);
		if (eligibleLaneMask == 0)
		{
			return false;
		}

		for (unsigned char laneIndex = 0; laneIndex < 8; laneIndex++)
		{
			unsigned char laneBit = static_cast<unsigned char>(1 << laneIndex);
			if ((eligibleLaneMask & laneBit) == 0 || (occupiedPhysicalLaneMask & laneBit) != 0)
			{
				continue;
			}

			physicalLaneIndex = laneIndex;
			return true;
		}

		return false;
	}

	bool BundleIssuePacket::TryResolvePacketLane(DecodedBundleSlotDescriptor slot, unsigned char occupiedPhysicalLaneMask, unsigned char &physicalLaneIndex)
	{
		physicalLaneIndex = std::numeric_limits<unsigned char>::max();
		SlotPlacementMetadata placement = slot.GetRuntimeAdmissionPlacement();

		if (placement.PinningKind == SlotPinningKind::HardPinned)
		{
			unsigned char pinnedLaneId = placement.PinnedLaneId;
			if (pinnedLaneId >= 8)
			{
				return false;
			}

			unsigned char pinnedLaneBit = static_cast<unsigned char>(1 << pinnedLaneId);
			if ((occupiedPhysicalLaneMask & pinnedLaneBit) != 0)
			{
				return false;
			}

			physicalLaneIndex = pinnedLaneId;
			return true;
		}

		if (placement.RequiredSlotClass == SlotClass::Unclassified)
		{
			return false;
		}

		unsigned char eligibleLaneMask = SlotClassLaneMap::GetLaneMask(placement.RequiredSlotClass);
		if (eligibleLaneMask == 0)
		{
			return false;
		}

		for (unsigned char laneIndex = 0; laneIndex < 8; laneIndex++)
		{
			unsigned char laneBit = static_cast<unsigned char>(1 << laneIndex);
			if ((eligibleLaneMask & laneBit) == 0 || (occupiedPhysicalLaneMask & laneBit) != 0)
			{
				continue;
			}

			physicalLaneIndex = laneIndex;
			return true;
		}

		return false;
	}

	void BundleIssuePacket::SetLane(unsigned char laneIndex, IssuePacketLane lane, IssuePacketLane &lane0, IssuePacketLane &lane1, IssuePacketLane &lane2, IssuePacketLane &lane3, IssuePacketLane &lane4, IssuePacketLane &lane5, IssuePacketLane &lane6, IssuePacketLane &lane7)
	{
		switch (laneIndex)
		{
			case 0:
				lane0 = lane;
				break;
			case 1:
				lane1 = lane;
				break;
			case 2:
				lane2 = lane;
				break;
			case 3:
				lane3 = lane;
				break;
			case 4:
				lane4 = lane;
				break;
			case 5:
				lane5 = lane;
				break;
			case 6:
				lane6 = lane;
				break;
			case 7:
				lane7 = lane;
				break;
		}
	}

	int BundleIssuePacket::GetPreparedScalarLaneCount() const
	{
		int laneCount = 0;
		if (getLane0().isOccupied() && getLane0().getCountsTowardScalarProjection())
		{
			laneCount++;
		}
		if (getLane1().isOccupied() && getLane1().getCountsTowardScalarProjection())
		{
			laneCount++;
		}
		if (getLane2().isOccupied() && getLane2().getCountsTowardScalarProjection())
		{
			laneCount++;
		}
		if (getLane3().isOccupied() && getLane3().getCountsTowardScalarProjection())
		{
			laneCount++;
		}

		return laneCount;
	}

	int BundleIssuePacket::GetPreparedPhysicalLaneCount() const
	{
		int laneCount = 0;
		if (getLane0().isOccupied())
		{
			laneCount++;
		}
		if (getLane1().isOccupied())
		{
			laneCount++;
		}
		if (getLane2().isOccupied())
		{
			laneCount++;
		}
		if (getLane3().isOccupied())
		{
			laneCount++;
		}
		if (getLane4().isOccupied())
		{
			laneCount++;
		}
		if (getLane5().isOccupied())
		{
			laneCount++;
		}
		if (getLane6().isOccupied())
		{
			laneCount++;
		}
		if (getLane7().isOccupied())
		{
			laneCount++;
		}

		return laneCount;
	}
}
