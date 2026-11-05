#pragma once

#include "../../Decoder/ClusterIssuePreparation.h"
#include "RuntimeClusterAdmissionPreparation.ExecutionMode.h"
#include "RuntimeClusterAdmissionPreparation.IssuePacketLane.h"
#include "RuntimeClusterAdmissionPreparation.BundleIssueFallbackInfo.h"
#include "../../Decoder/DecodedBundleDescriptor.h"
#include "RuntimeClusterAdmissionPreparation.CandidateView.h"
#include "RuntimeClusterAdmissionPreparation.DecisionDraft.h"
#include <vector>
#include <limits>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct BundleIssuePacket
	class BundleIssuePacket
	{
	private:
		unsigned long long PC;
		YAKSys_Hybrid_CPU::Core::DecodeMode DecodeMode;
		unsigned char ValidNonEmptyMask;
		unsigned char ScalarCandidateMask;
		unsigned char ScalarIssueMask;
		unsigned char SelectedSlotMask;
		unsigned char UnmappedSelectedSlotMask;
		unsigned char PreparedScalarMask;
		unsigned char RefinedPreparedScalarMask;
		int AdvisoryScalarIssueWidth;
		int RefinedAdvisoryScalarIssueWidth;
		RuntimeClusterAdmissionExecutionMode ExecutionMode;
		bool ShouldProbeClusterPath;
		bool UsesIssuePacketAsExecutionSource;
		bool RetainsReferenceSequentialPath;
		IssuePacketLane Lane0;
		IssuePacketLane Lane1;
		IssuePacketLane Lane2;
		IssuePacketLane Lane3;
		IssuePacketLane Lane4;
		IssuePacketLane Lane5;
		IssuePacketLane Lane6;
		IssuePacketLane Lane7;
		BundleIssueFallbackInfo FallbackInfo;

	public:
		BundleIssuePacket(unsigned long long pc, YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode, unsigned char validNonEmptyMask, unsigned char scalarCandidateMask, unsigned char scalarIssueMask, unsigned char selectedSlotMask, unsigned char unmappedSelectedSlotMask, unsigned char preparedScalarMask, unsigned char refinedPreparedScalarMask, int advisoryScalarIssueWidth, int refinedAdvisoryScalarIssueWidth, RuntimeClusterAdmissionExecutionMode executionMode, bool shouldProbeClusterPath, bool usesIssuePacketAsExecutionSource, bool retainsReferenceSequentialPath, IssuePacketLane lane0, IssuePacketLane lane1, IssuePacketLane lane2, IssuePacketLane lane3, IssuePacketLane lane4, IssuePacketLane lane5, IssuePacketLane lane6, IssuePacketLane lane7, BundleIssueFallbackInfo fallbackInfo);

		unsigned long long getPC() const;
		YAKSys_Hybrid_CPU::Core::DecodeMode getDecodeMode() const;
		unsigned char getValidNonEmptyMask() const;
		unsigned char getScalarCandidateMask() const;
		unsigned char getScalarIssueMask() const;
		unsigned char getSelectedSlotMask() const;
		unsigned char getUnmappedSelectedSlotMask() const;
		unsigned char getPreparedScalarMask() const;
		unsigned char getRefinedPreparedScalarMask() const;
		int getAdvisoryScalarIssueWidth() const;
		int getRefinedAdvisoryScalarIssueWidth() const;
		RuntimeClusterAdmissionExecutionMode getExecutionMode() const;

		/// <summary>
		/// Diagnostic flag indicating whether this bundle stays interesting for cluster-path probing.
		/// This does not, by itself, select the packet as the live execution source.
		/// </summary>
		bool getShouldProbeClusterPath() const;

		/// <summary>
		/// True when execute-stage materialization should consume the packet lanes as the live source.
		/// False means the packet remains metadata while execution stays on the retained reference path.
		/// </summary>
		bool getUsesIssuePacketAsExecutionSource() const;

		/// <summary>
		/// True when the decode/execute handoff still carries the reference sequential path alongside
		/// any packetized admission metadata. This may be false in cluster-prepared execution modes.
		/// </summary>
		bool getRetainsReferenceSequentialPath() const;
		IssuePacketLane getLane0() const;
		IssuePacketLane getLane1() const;
		IssuePacketLane getLane2() const;
		IssuePacketLane getLane3() const;
		IssuePacketLane getLane4() const;
		IssuePacketLane getLane5() const;
		IssuePacketLane getLane6() const;
		IssuePacketLane getLane7() const;
		BundleIssueFallbackInfo getFallbackInfo() const;
		unsigned char getSelectedNonScalarSlotMask() const;
		bool getHasUnmappedSelectedSlots() const;
		int getPreparedScalarLaneCount() const;
		int getPreparedPhysicalLaneCount() const;

		IssuePacketLane GetPhysicalLane(unsigned char laneIndex) const;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static BundleIssuePacket Create(ClusterIssuePreparation clusterPreparation, IReadOnlyList<DecodedBundleSlotDescriptor> slots, RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionDraft decisionDraft)
		static BundleIssuePacket Create(std::shared_ptr<ClusterIssuePreparation> clusterPreparation, const std::vector<DecodedBundleSlotDescriptor> &slots, RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionDraft decisionDraft);

		static BundleIssuePacket CreateEmpty(unsigned long long pc = 0);

	private:
		static bool TryAssignToPacket(ScalarClusterIssueEntry entry, unsigned char &occupiedPhysicalLaneMask, IssuePacketLane &lane0, IssuePacketLane &lane1, IssuePacketLane &lane2, IssuePacketLane &lane3, IssuePacketLane &lane4, IssuePacketLane &lane5, IssuePacketLane &lane6, IssuePacketLane &lane7);

		static bool TryAssignToPacket(DecodedBundleSlotDescriptor slot, bool countsTowardScalarProjection, unsigned char &occupiedPhysicalLaneMask, IssuePacketLane &lane0, IssuePacketLane &lane1, IssuePacketLane &lane2, IssuePacketLane &lane3, IssuePacketLane &lane4, IssuePacketLane &lane5, IssuePacketLane &lane6, IssuePacketLane &lane7);

		static bool TryResolvePacketLane(ScalarClusterIssueEntry entry, unsigned char occupiedPhysicalLaneMask, unsigned char &physicalLaneIndex);

		static bool TryResolvePacketLane(DecodedBundleSlotDescriptor slot, unsigned char occupiedPhysicalLaneMask, unsigned char &physicalLaneIndex);

		static void SetLane(unsigned char laneIndex, IssuePacketLane lane, IssuePacketLane &lane0, IssuePacketLane &lane1, IssuePacketLane &lane2, IssuePacketLane &lane3, IssuePacketLane &lane4, IssuePacketLane &lane5, IssuePacketLane &lane6, IssuePacketLane &lane7);

		int GetPreparedScalarLaneCount() const;

		int GetPreparedPhysicalLaneCount() const;

		BundleIssuePacket() = default;
	};

}
