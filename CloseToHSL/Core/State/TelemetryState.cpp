#include "TelemetryState.h"

namespace YAKSys_Hybrid_CPU::Core
{

	void TelemetryState::ClearPipeline()
	{
		CycleCount = 0;
		StallCycles = 0;
		InstructionsRetired = 0;
		Lane7ConditionalBranchExecuteCompletionCount = 0;
		Lane7ConditionalBranchRedirectCount = 0;
		BranchMispredicts = 0;
		DataHazards = 0;
		MemoryStalls = 0;
		ForwardingEvents = 0;
		ControlHazards = 0;
		WAWHazards = 0;
		LoadUseBubbles = 0;
		FrontendStalls = 0;
		DomainSquashCount = 0;
		EarlyDomainSquashCount = 0;
		MshrScoreboardStalls = 0;
		BankConflictStallCycles = 0;
		ExceptionYoungerSuppressCount = 0;
		MultiSlotDecodeAdvanceCount = 0;
		MultiLaneExecuteCount = 0;
		MemoryFaultCarrierCount = 0;
		AuxiliaryCoexistenceConflictCount = 0;
		ScalarIssueWidthHistogram = std::vector<unsigned long long>(5);
		ClusterProbeCount = 0;
		ClusterProbeRefinedWidthSum = 0;
		ClusterProbeNarrowFallbackCount = 0;
		ClusterPreparedExecutionChoiceCount = 0;
		DifferentialTraceCompareCount = 0;
		DifferentialTraceDiscrepancyCount = 0;
		ClusterModeFallbackCount = 0;
		DecoderPreparedScalarGroupCount = 0;
		DecoderPreparedFallbackCount = 0;
		DecodeFallbackCount = 0;
		DecodeFaultBundleCount = 0;
		CrossSlotRejectCount = 0;
		HazardRegisterDataCount = 0;
		HazardMemoryBankCount = 0;
		HazardControlFlowCount = 0;
		HazardSystemBarrierCount = 0;
		HazardPinnedLaneCount = 0;
		ScalarClusterEligibleButBlockedCount = 0;
		ReferenceFallbackDueToControlConflictCount = 0;
		ReferenceFallbackDueToMemoryConflictCount = 0;
		VTSpreadPerBundle = 0;
		BurstReadCycles = 0;
		BurstWriteCycles = 0;
		ComputeCycles = 0;
		OverlappedCycles = 0;
		WidePathGate3_ReferenceSequentialCount = 0;
		WidePathGate4_NarrowFallbackCount = 0;
		WidePathGate5_NotClusterCandidateCount = 0;
		WidePathGate6_PreparedMaskZeroCount = 0;
		WidePathSuccessCount = 0;
		RefinedMaskPromotionCount = 0;
		PartialWidthIssueCount = 0;
		IssuePacketPreparedLaneCountSum = 0;
		IssuePacketMaterializedLaneCountSum = 0;
		IssuePacketPreparedPhysicalLaneCountSum = 0;
		IssuePacketMaterializedPhysicalLaneCountSum = 0;
		IssuePacketWidthDropCount = 0;
		ReferenceSequentialFallbackCount = 0;
		FallbackSofteningPromotionCount = 0;
		NopElisionSkipCount = 0;
		ScalarLanesRetired = 0;
		NonScalarLanesRetired = 0;
		RetireCycleCount = 0;
		InvariantViolationCount = 0;
	}

	void TelemetryState::ClearAssist()
	{
		AssistLaunchCount = 0;
		AssistCompletedCount = 0;
		AssistKilledCount = 0;
		AssistInvalidationCount = 0;
	}

	std::shared_ptr<TelemetryState> TelemetryState::CreateSnapshot()
	{
		return std::make_shared<TelemetryState>(*this);
	}
}
