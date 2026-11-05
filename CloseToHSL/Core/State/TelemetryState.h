#pragma once

#include "../Diagnostics/DifferentialTraceCapture.h"
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Reference-owned per-core diagnostic telemetry. This state observes existing
	/// owner events; it is not an execution, legality, timing, commit, rollback or
	/// publication authority.
	/// </summary>
	class TelemetryState final : public std::enable_shared_from_this<TelemetryState>
	{
	public:
		unsigned long long CycleCount = 0;
		unsigned long long StallCycles = 0;
		unsigned long long InstructionsRetired = 0;
		unsigned long long Lane7ConditionalBranchExecuteCompletionCount = 0;
		unsigned long long Lane7ConditionalBranchRedirectCount = 0;
		unsigned long long BranchMispredicts = 0;
		unsigned long long DataHazards = 0;
		unsigned long long MemoryStalls = 0;
		unsigned long long ForwardingEvents = 0;
		unsigned long long ControlHazards = 0;
		unsigned long long WAWHazards = 0;
		unsigned long long LoadUseBubbles = 0;
		unsigned long long FrontendStalls = 0;
		unsigned long long DomainSquashCount = 0;
		unsigned long long EarlyDomainSquashCount = 0;
		unsigned long long MshrScoreboardStalls = 0;
		unsigned long long BankConflictStallCycles = 0;
		unsigned long long ExceptionYoungerSuppressCount = 0;
		unsigned long long MultiSlotDecodeAdvanceCount = 0;
		unsigned long long MultiLaneExecuteCount = 0;
		unsigned long long MemoryFaultCarrierCount = 0;
		unsigned long long AuxiliaryCoexistenceConflictCount = 0;
		std::vector<unsigned long long> ScalarIssueWidthHistogram = std::vector<unsigned long long>(5);
		unsigned long long ClusterProbeCount = 0;
		unsigned long long ClusterProbeRefinedWidthSum = 0;
		unsigned long long ClusterProbeNarrowFallbackCount = 0;
		unsigned long long ClusterPreparedExecutionChoiceCount = 0;
		unsigned long long DifferentialTraceCompareCount = 0;
		unsigned long long DifferentialTraceDiscrepancyCount = 0;
		unsigned long long ClusterModeFallbackCount = 0;
		unsigned long long DecoderPreparedScalarGroupCount = 0;
		unsigned long long DecoderPreparedFallbackCount = 0;
		unsigned long long DecodeFallbackCount = 0;
		unsigned long long DecodeFaultBundleCount = 0;
		unsigned long long CrossSlotRejectCount = 0;
		unsigned long long HazardRegisterDataCount = 0;
		unsigned long long HazardMemoryBankCount = 0;
		unsigned long long HazardControlFlowCount = 0;
		unsigned long long HazardSystemBarrierCount = 0;
		unsigned long long HazardPinnedLaneCount = 0;
		unsigned long long ScalarClusterEligibleButBlockedCount = 0;
		unsigned long long ReferenceFallbackDueToControlConflictCount = 0;
		unsigned long long ReferenceFallbackDueToMemoryConflictCount = 0;
		unsigned long long VTSpreadPerBundle = 0;
		unsigned long long BurstReadCycles = 0;
		unsigned long long BurstWriteCycles = 0;
		unsigned long long ComputeCycles = 0;
		unsigned long long OverlappedCycles = 0;
		unsigned long long WidePathGate3_ReferenceSequentialCount = 0;
		unsigned long long WidePathGate4_NarrowFallbackCount = 0;
		unsigned long long WidePathGate5_NotClusterCandidateCount = 0;
		unsigned long long WidePathGate6_PreparedMaskZeroCount = 0;
		unsigned long long WidePathSuccessCount = 0;
		unsigned long long RefinedMaskPromotionCount = 0;
		unsigned long long PartialWidthIssueCount = 0;
		unsigned long long IssuePacketPreparedLaneCountSum = 0;
		unsigned long long IssuePacketMaterializedLaneCountSum = 0;
		unsigned long long IssuePacketPreparedPhysicalLaneCountSum = 0;
		unsigned long long IssuePacketMaterializedPhysicalLaneCountSum = 0;
		unsigned long long IssuePacketWidthDropCount = 0;
		unsigned long long ReferenceSequentialFallbackCount = 0;
		unsigned long long FallbackSofteningPromotionCount = 0;
		unsigned long long NopElisionSkipCount = 0;
		unsigned long long ScalarLanesRetired = 0;
		unsigned long long NonScalarLanesRetired = 0;
		unsigned long long RetireCycleCount = 0;
		unsigned long long InvariantViolationCount = 0;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DifferentialTraceCapture? DifferentialTraceCapture;
		std::shared_ptr<DifferentialTraceCapture> DifferentialTraceCapture;

		long long AssistLaunchCount = 0;
		long long AssistCompletedCount = 0;
		long long AssistKilledCount = 0;
		long long AssistInvalidationCount = 0;
		unsigned long long TestReferenceRawFallbackCount = 0;

		void ClearPipeline();

		void ClearAssist();

		std::shared_ptr<TelemetryState> CreateSnapshot();
	};

}
