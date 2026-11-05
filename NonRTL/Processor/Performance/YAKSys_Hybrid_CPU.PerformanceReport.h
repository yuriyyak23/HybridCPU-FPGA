#pragma once

#include "../../../CloseToHSL/Core/Pipeline/Safety/SafetyVerifier.Types.h"
#include "../../../CloseToHSL/Core/Diagnostics/ReplayEngine.Types.h"
#include "PerformanceReport.Phase1EvidenceCorrelationReport.h"
#include <string>
#include <vector>
#include <cmath>
#include <optional>
#include <format>
#include <memory>
#include "../../../tangible_stringbuilder.h"

// C# TO C++ CONVERTER NOTE: Forward class declarations:


using namespace HybridCPU_ISE::Core;








namespace YAKSys_Hybrid_CPU
{
	class PerformanceReport : public std::enable_shared_from_this<PerformanceReport>
	{
	public:
		class BurstTrace;
	private:
	private:
		std::vector<std::shared_ptr<BurstTrace>> BurstTraces;
		long long TotalBursts = 0;
		long long TotalBytesTransferred = 0;
		long long BankConflicts = 0;
		long long StallCycles = 0;
		long long DmaTransfers = 0;
		double AverageBurstLength = 0.0;
		long long TotalWaitCycles = 0;
		double AverageWaitCycles = 0.0;
		int MaxQueueDepth = 0;
		int CurrentQueuedRequests = 0;
		long long TotalMemoryStalls = 0;
		long long MemoryQueueFullEvents = 0;
		long long BankSaturationCycles = 0;
		long long IdleMemoryCycles = 0;
		double MemoryUtilization = 0.0;
		double AverageMemoryQueueDepth = 0.0;
		long long QueueOverflowEvents = 0;
		double AverageMemoryLatency = 0.0;
		std::wstring MemoryCycleTelemetrySchemaVersion;
		bool MemoryCycleTelemetryAvailable = false;
		long long MemoryControllerCycles = 0;
		long long MemoryReadServiceCycles = 0;
		long long MemoryStoreReadinessServiceCycles = 0;
		long long MemoryCompletionPublicationCycles = 0;
		long long MemoryAcceptedRequests = 0;
		long long MemoryCompletedRequests = 0;
		long long DataReadAcceptedRequests = 0;
		long long DataReadCompletedRequests = 0;
		long long DataWriteAcceptedRequests = 0;
		long long DataWriteCompletedRequests = 0;
		long long DataReadBytes = 0;
		long long CommittedDataWriteBytes = 0;
		bool InstructionFetchReadBytesTelemetryAvailable = false;
		long long InstructionFetchReadBytes = 0;
		bool InstructionFetchRequestTelemetryAvailable = false;
		long long MemoryQueueFullRejects = 0;
		bool MemoryBankConflictRejectTelemetryAvailable = false;
		long long MemoryBankConflictRejects = 0;
		long long MemoryTelemetryBaselineOutstandingRequests = 0;
		long long MemoryCanceledRequests = 0;
		long long MemoryConsumedCompletions = 0;
		long long MemoryOutstandingRequests = 0;
		long long LoadStalls = 0;
		long long StoreStalls = 0;
		long long BurstStalls = 0;
		long long UnalignedAccessCount = 0;
		long long TotalAlignmentPenalty = 0;
		double BurstEfficiency = 0.0;
		long long EligibilityMaskedCycles = 0;
		long long EligibilityMaskedReadyCandidates = 0;
		unsigned char LastEligibilityRequestedMask = 0;
		unsigned char LastEligibilityNormalizedMask = 0;
		unsigned char LastEligibilityReadyPortMask = 0;
		unsigned char LastEligibilityVisibleReadyMask = 0;
		unsigned char LastEligibilityMaskedReadyMask = 0;
		long long TotalInstructions = 0;
		long long TotalCycles = 0;
		long long PipelineStalls = 0;
		long long BranchMispredictions = 0;
		long long VectorOperations = 0;
		long long VectorElementsProcessed = 0;
		long long VectorExceptions = 0;
		long long TotalSchedulerCycles = 0;
		long long SuccessfulInjections = 0;
		long long RejectedInjections = 0;
		long long RejectedDueToHazards = 0;
		long long RejectedDueToMemConflict = 0;
		long long RollbackEvents = 0;
		double AverageReadyQueueDepth = 0.0;
		int MaxReadyQueueDepth = 0;
		long long ThreadStarvationEvents = 0;
		double SchedulerUtilization = 0.0;
		double FSPContribution = 0.0;
		std::vector<double> PerThreadUtilization;
		long long TotalSchedulerLatency = 0;
		double AverageSchedulerLatency = 0.0;
		long long BankContentionStalls = 0;
		long long FspPipelineLatencyCycles = 0;
		long long ReplayEpochCount = 0;
		double AverageReplayEpochLength = 0.0;
		double StableDonorSlotRatio = 0.0;
		long long ReplayAwareCycles = 0;
		long long PhaseCertificateReadyHits = 0;
		long long PhaseCertificateReadyMisses = 0;
		long long EstimatedPhaseCertificateChecksSaved = 0;
		long long PhaseCertificateInvalidations = 0;
		long long PhaseCertificateMutationInvalidations = 0;
		long long PhaseCertificatePhaseMismatchInvalidations = 0;
		long long DeterministicReplayTransitions = 0;
		std::vector<long long> PerVtInjections;
		long long FairnessStarvationEvents = 0;
		long long BankPressureAvoidanceCount = 0;
		long long SpeculationBudgetExhaustionEvents = 0;
		long long PeakConcurrentSpeculativeOps = 0;
		long long DeterminismReferenceOpportunitySlots = 0;
		long long DeterminismReplayEligibleSlots = 0;
		long long DeterminismMaskedSlots = 0;
		long long DeterminismEstimatedLostSlots = 0;
		long long DeterminismConstrainedCycles = 0;
		long long DomainIsolationProbeAttempts = 0;
		long long DomainIsolationBlockedAttempts = 0;
		long long DomainIsolationCrossDomainBlocks = 0;
		long long DomainIsolationKernelToUserBlocks = 0;
		long long OracleGapTotalOracleSlots = 0;
		long long OracleGapTotalRealSlots = 0;
		long long OracleGapDonorRestriction = 0;
		long long OracleGapFairnessOrdering = 0;
		long long OracleGapLegalityConservatism = 0;
		long long OracleGapDomainIsolation = 0;
		long long OracleGapSpeculationBudget = 0;
		long long OracleGapCyclesWithGap = 0;
		long long OracleGapTotalCyclesAnalyzed = 0;
		int OracleCounterexampleCount = 0;
		long long OracleCounterexampleTotalMissedSlots = 0;
		std::wstring OracleCounterexampleDominantCategory;
		long long L1BypassHits = 0;
		long long ForegroundWarmAttempts = 0;
		long long ForegroundWarmSuccesses = 0;
		long long ForegroundWarmReuseHits = 0;
		long long ForegroundBypassHits = 0;
		long long AssistWarmAttempts = 0;
		long long AssistWarmSuccesses = 0;
		long long AssistWarmReuseHits = 0;
		long long AssistBypassHits = 0;
		long long StreamWarmTranslationRejects = 0;
		long long StreamWarmBackendRejects = 0;
		long long AssistWarmResidentBudgetRejects = 0;
		long long AssistWarmLoadingBudgetRejects = 0;
		long long AssistWarmNoVictimRejects = 0;
		int ActiveDmaTransfers = 0;
		long long CompletedDmaTransfers = 0;
		long long TotalDmaLatency = 0;
		long long DmaErrors = 0;
		long long DmaBytesTransferred = 0;
		long long StaticClassOvercommitRejects = 0;
		long long DynamicClassExhaustionRejects = 0;
		long long PinnedLaneConflicts = 0;
		long long LateBindingConflicts = 0;
		long long TypedSlotDomainRejects = 0;
		long long SmtOwnerContextGuardRejects = 0;
		long long SmtDomainGuardRejects = 0;
		long long SmtBoundaryGuardRejects = 0;
		long long SmtSharedResourceCertificateRejects = 0;
		long long SmtRegisterGroupCertificateRejects = 0;
		Core::RejectKind LastSmtLegalityRejectKind = static_cast<Core::RejectKind>(0);
		Core::LegalityAuthoritySource LastSmtLegalityAuthoritySource = static_cast<Core::LegalityAuthoritySource>(0);
		long long SmtLegalityRejectByAluClass = 0;
		long long SmtLegalityRejectByLsuClass = 0;
		long long SmtLegalityRejectByDmaStreamClass = 0;
		long long SmtLegalityRejectByMatrixTileStreamClass = 0;
		long long SmtLegalityRejectByBranchControl = 0;
		long long SmtLegalityRejectBySystemSingleton = 0;
		long long TypedSlotFastPathAccepts = 0;
		long long TypedSlotStandardPathAccepts = 0;
		long long ClassTemplateReuseHits = 0;
		long long ClassTemplateInvalidations = 0;
		long long ClassTemplateDomainInvalidations = 0;
		long long ClassTemplateCapacityMismatchInvalidations = 0;
		long long LaneReuseHits = 0;
		long long LaneReuseMisses = 0;
		long long TotalLaneBindings = 0;
		long long NopDueToPinnedConstraint = 0;
		long long NopDueToNoClassCapacity = 0;
		long long NopDueToResourceConflict = 0;
		long long NopDueToDynamicState = 0;
		long long NopAvoided = 0;
		long long AluClassInjects = 0;
		long long LsuClassInjects = 0;
		long long DmaStreamClassInjects = 0;
		long long MatrixTileStreamClassInjects = 0;
		long long BranchControlInjects = 0;
		long long HardPinnedInjects = 0;
		long long ClassFlexibleInjects = 0;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public List<BurstTrace>? BurstTraces {get;set;}
	public:
		std::vector<std::shared_ptr<BurstTrace>> getBurstTraces() const;
		void setBurstTraces(const std::vector<std::shared_ptr<BurstTrace>> &value);

	public:
		class BurstTrace : public std::enable_shared_from_this<BurstTrace>
		{
		private:
			long long Timestamp = 0;
			unsigned long long Address = 0;
			int Length = 0;
			bool IsRead = false;
			int BankId = 0;
			long long Duration = 0;

		public:
			long long getTimestamp() const;
			void setTimestamp(long long value);
			unsigned long long getAddress() const;
			void setAddress(unsigned long long value);
			int getLength() const;
			void setLength(int value);
			bool isRead() const;
			void setRead(bool value);
			int getBankId() const;
			void setBankId(int value);
			long long getDuration() const;
			void setDuration(long long value);
		};

	private:
		bool getHasBurstTraces() const;

		void AppendBurstTraceSummary(std::shared_ptr<StringBuilder> sb);

	public:
		std::wstring ExportBurstTracesToCSV();


		/// <summary>
		/// Creates an additive interval for diagnostics that sample shared,
		/// cumulative producers more than once. Per-execution fields and
		/// last-value snapshots remain the current snapshot values.
		/// </summary>
		/// <remarks>
		/// Numeric-only compatibility counters have no availability carrier.
		/// A regression therefore fails closed instead of presenting a reset as
		/// measured zero or silently discarding pre-reset activity.
		/// </remarks>
		std::shared_ptr<PerformanceReport> CreateAdditiveDiagnosticIntervalSince(std::shared_ptr<PerformanceReport> baseline);

	private:
		static long long SubtractMonotonicOrThrow(long long current, long long baseline, const std::wstring &metricName);


//		#region Memory Subsystem Statistics

		/// <summary>
		/// Total burst operations
		/// </summary>
	public:
		long long getTotalBursts() const;
		void setTotalBursts(long long value);

		/// <summary>
		/// Total bytes transferred
		/// </summary>
		long long getTotalBytesTransferred() const;
		void setTotalBytesTransferred(long long value);

		/// <summary>
		/// Bank conflicts detected
		/// </summary>
		long long getBankConflicts() const;
		void setBankConflicts(long long value);

		/// <summary>
		/// Stall cycles due to conflicts
		/// </summary>
		long long getStallCycles() const;
		void setStallCycles(long long value);

		/// <summary>
		/// DMA transfers
		/// </summary>
		long long getDmaTransfers() const;
		void setDmaTransfers(long long value);

		/// <summary>
		/// Average burst length
		/// </summary>
		double getAverageBurstLength() const;
		void setAverageBurstLength(double value);

		/// <summary>
		/// Total wait cycles across all memory requests
		/// </summary>
		long long getTotalWaitCycles() const;
		void setTotalWaitCycles(long long value);

		/// <summary>
		/// Average wait cycles per request
		/// </summary>
		double getAverageWaitCycles() const;
		void setAverageWaitCycles(double value);

		/// <summary>
		/// Maximum queue depth observed
		/// </summary>
		int getMaxQueueDepth() const;
		void setMaxQueueDepth(int value);

		/// <summary>
		/// Current queued requests
		/// </summary>
		int getCurrentQueuedRequests() const;
		void setCurrentQueuedRequests(int value);

		// Phase 3: Memory wall counters
		/// <summary>
		/// Total memory stalls (cycles stalled waiting for memory)
		/// </summary>
		long long getTotalMemoryStalls() const;
		void setTotalMemoryStalls(long long value);

		/// <summary>
		/// Memory queue full events
		/// </summary>
		long long getMemoryQueueFullEvents() const;
		void setMemoryQueueFullEvents(long long value);

		/// <summary>
		/// Bank saturation cycles (all banks busy)
		/// </summary>
		long long getBankSaturationCycles() const;
		void setBankSaturationCycles(long long value);

		/// <summary>
		/// Idle memory cycles (no activity)
		/// </summary>
		long long getIdleMemoryCycles() const;
		void setIdleMemoryCycles(long long value);

		/// <summary>
		/// Memory utilization percentage
		/// </summary>
		double getMemoryUtilization() const;
		void setMemoryUtilization(double value);

		/// <summary>
		/// Average memory queue depth
		/// </summary>
		double getAverageMemoryQueueDepth() const;
		void setAverageMemoryQueueDepth(double value);

		/// <summary>
		/// Queue overflow events
		/// </summary>
		long long getQueueOverflowEvents() const;
		void setQueueOverflowEvents(long long value);

		/// <summary>
		/// Average memory latency per request
		/// </summary>
		double getAverageMemoryLatency() const;
		void setAverageMemoryLatency(double value);

		/// <summary>
		/// Additive RF-10 producer telemetry. Availability flags distinguish
		/// a measured zero from a contour that has no truthful producer.
		/// </summary>
		std::wstring getMemoryCycleTelemetrySchemaVersion() const;
		void setMemoryCycleTelemetrySchemaVersion(const std::wstring &value);
		bool getMemoryCycleTelemetryAvailable() const;
		void setMemoryCycleTelemetryAvailable(bool value);
		long long getMemoryControllerCycles() const;
		void setMemoryControllerCycles(long long value);
		long long getMemoryReadServiceCycles() const;
		void setMemoryReadServiceCycles(long long value);
		long long getMemoryStoreReadinessServiceCycles() const;
		void setMemoryStoreReadinessServiceCycles(long long value);
		long long getMemoryCompletionPublicationCycles() const;
		void setMemoryCompletionPublicationCycles(long long value);
		long long getMemoryAcceptedRequests() const;
		void setMemoryAcceptedRequests(long long value);
		long long getMemoryCompletedRequests() const;
		void setMemoryCompletedRequests(long long value);
		long long getDataReadAcceptedRequests() const;
		void setDataReadAcceptedRequests(long long value);
		long long getDataReadCompletedRequests() const;
		void setDataReadCompletedRequests(long long value);
		long long getDataWriteAcceptedRequests() const;
		void setDataWriteAcceptedRequests(long long value);
		long long getDataWriteCompletedRequests() const;
		void setDataWriteCompletedRequests(long long value);
		long long getDataReadBytes() const;
		void setDataReadBytes(long long value);
		long long getCommittedDataWriteBytes() const;
		void setCommittedDataWriteBytes(long long value);
		bool getInstructionFetchReadBytesTelemetryAvailable() const;
		void setInstructionFetchReadBytesTelemetryAvailable(bool value);
		long long getInstructionFetchReadBytes() const;
		void setInstructionFetchReadBytes(long long value);
		bool getInstructionFetchRequestTelemetryAvailable() const;
		void setInstructionFetchRequestTelemetryAvailable(bool value);
		long long getMemoryQueueFullRejects() const;
		void setMemoryQueueFullRejects(long long value);
		bool getMemoryBankConflictRejectTelemetryAvailable() const;
		void setMemoryBankConflictRejectTelemetryAvailable(bool value);
		long long getMemoryBankConflictRejects() const;
		void setMemoryBankConflictRejects(long long value);
		long long getMemoryTelemetryBaselineOutstandingRequests() const;
		void setMemoryTelemetryBaselineOutstandingRequests(long long value);
		long long getMemoryCanceledRequests() const;
		void setMemoryCanceledRequests(long long value);
		long long getMemoryConsumedCompletions() const;
		void setMemoryConsumedCompletions(long long value);
		long long getMemoryOutstandingRequests() const;
		void setMemoryOutstandingRequests(long long value);

		/// <summary>
		/// Creates an observation-only interval view of the cumulative RF-10
		/// memory-controller counters. The current report is not modified.
		/// A missing baseline, schema change, or counter regression means that
		/// no truthful interval can be formed and is reported as unavailable.
		/// </summary>
		std::shared_ptr<PerformanceReport> CreateMemoryCycleTelemetryIntervalSince(std::shared_ptr<PerformanceReport> baseline);

	private:
		static bool TrySubtract(long long current, long long baseline, long long &difference);

		/// <summary>
		/// Stalls on load operations
		/// </summary>
	public:
		long long getLoadStalls() const;
		void setLoadStalls(long long value);

		/// <summary>
		/// Stalls on store operations
		/// </summary>
		long long getStoreStalls() const;
		void setStoreStalls(long long value);

		/// <summary>
		/// Stalls on burst operations
		/// </summary>
		long long getBurstStalls() const;
		void setBurstStalls(long long value);

		// Phase 3: Burst timing and efficiency
		/// <summary>
		/// Number of unaligned memory accesses (Phase 3)
		/// </summary>
		long long getUnalignedAccessCount() const;
		void setUnalignedAccessCount(long long value);

		/// <summary>
		/// Total alignment penalty cycles (Phase 3)
		/// </summary>
		long long getTotalAlignmentPenalty() const;
		void setTotalAlignmentPenalty(long long value);

		/// <summary>
		/// Burst efficiency (data cycles / total cycles) (Phase 3)
		/// </summary>
		double getBurstEfficiency() const;
		void setBurstEfficiency(double value);

		/// <summary>
		/// Average alignment penalty per unaligned access (Phase 3)
		/// </summary>
		double getAverageAlignmentPenalty() const;

//		#endregion


		/// <summary>
		/// Number of scheduler cycles where ready SMT candidates were masked out by the FSM-owned eligibility mask.
		/// </summary>
		long long getEligibilityMaskedCycles() const;
		void setEligibilityMaskedCycles(long long value);

		/// <summary>
		/// Number of ready SMT candidates hidden by the FSM-owned eligibility mask.
		/// </summary>
		long long getEligibilityMaskedReadyCandidates() const;
		void setEligibilityMaskedReadyCandidates(long long value);

		/// <summary>
		/// Last requested VT eligibility mask observed by the scheduler diagnostics snapshot.
		/// </summary>
		unsigned char getLastEligibilityRequestedMask() const;
		void setLastEligibilityRequestedMask(unsigned char value);

		/// <summary>
		/// Last normalized VT eligibility mask observed by the scheduler diagnostics snapshot.
		/// </summary>
		unsigned char getLastEligibilityNormalizedMask() const;
		void setLastEligibilityNormalizedMask(unsigned char value);

		/// <summary>
		/// Last ready-port mask before eligibility filtering.
		/// </summary>
		unsigned char getLastEligibilityReadyPortMask() const;
		void setLastEligibilityReadyPortMask(unsigned char value);

		/// <summary>
		/// Last ready-port mask visible after eligibility filtering.
		/// </summary>
		unsigned char getLastEligibilityVisibleReadyMask() const;
		void setLastEligibilityVisibleReadyMask(unsigned char value);

		/// <summary>
		/// Last ready-port subset removed by eligibility filtering.
		/// </summary>
		unsigned char getLastEligibilityMaskedReadyMask() const;
		void setLastEligibilityMaskedReadyMask(unsigned char value);

		/// <summary>
		/// Whether explicit eligibility telemetry is present in the report.
		/// Zero-valued fields from older runs remain indistinguishable from no telemetry, so this is a best-effort signal.
		/// </summary>
		bool getHasEligibilityTelemetry() const;

		/// <summary>
		/// Average masked ready candidates among cycles where the eligibility gate hid at least one candidate.
		/// </summary>
		double getEligibilityMaskedReadyCandidatesPerMaskedCycle() const;

	private:
		void AppendEligibilityValidationSummary(std::shared_ptr<StringBuilder> sb);

		void AppendEligibilityExecutionSummary(std::shared_ptr<StringBuilder> sb);


//		#region Pipeline Statistics

		/// <summary>
		/// Total instructions executed
		/// </summary>
	public:
		long long getTotalInstructions() const;
		void setTotalInstructions(long long value);

		/// <summary>
		/// Total cycles elapsed
		/// </summary>
		long long getTotalCycles() const;
		void setTotalCycles(long long value);

		/// <summary>
		/// Instructions Per Cycle (IPC)
		/// </summary>
		double getIPC() const;

		/// <summary>
		/// Pipeline stalls
		/// </summary>
		long long getPipelineStalls() const;
		void setPipelineStalls(long long value);

		/// <summary>
		/// Branch mispredictions
		/// </summary>
		long long getBranchMispredictions() const;
		void setBranchMispredictions(long long value);

//		#endregion

//		#region Vector Engine Statistics

		/// <summary>
		/// Vector operations executed
		/// </summary>
		long long getVectorOperations() const;
		void setVectorOperations(long long value);

		/// <summary>
		/// Elements processed by vector engine
		/// </summary>
		long long getVectorElementsProcessed() const;
		void setVectorElementsProcessed(long long value);

		/// <summary>
		/// Vector exceptions
		/// </summary>
		long long getVectorExceptions() const;
		void setVectorExceptions(long long value);

//		#endregion


//		#region MicroOp Scheduler Statistics (Phase 3)

		/// <summary>
		/// Total scheduler cycles (cycles scheduler was active)
		/// </summary>
		long long getTotalSchedulerCycles() const;
		void setTotalSchedulerCycles(long long value);

		/// <summary>
		/// Successful FSP injections
		/// </summary>
		long long getSuccessfulInjections() const;
		void setSuccessfulInjections(long long value);

		/// <summary>
		/// Rejected FSP injections
		/// </summary>
		long long getRejectedInjections() const;
		void setRejectedInjections(long long value);

		/// <summary>
		/// Rejections due to hazards (RAW/WAW/WAR)
		/// </summary>
		long long getRejectedDueToHazards() const;
		void setRejectedDueToHazards(long long value);

		/// <summary>
		/// Rejections due to memory conflicts
		/// </summary>
		long long getRejectedDueToMemConflict() const;
		void setRejectedDueToMemConflict(long long value);

		/// <summary>
		/// Number of rollback events
		/// </summary>
		long long getRollbackEvents() const;
		void setRollbackEvents(long long value);

		/// <summary>
		/// Average ready queue depth across all threads
		/// </summary>
		double getAverageReadyQueueDepth() const;
		void setAverageReadyQueueDepth(double value);

		/// <summary>
		/// Maximum ready queue depth observed
		/// </summary>
		int getMaxReadyQueueDepth() const;
		void setMaxReadyQueueDepth(int value);

		/// <summary>
		/// Thread starvation events (thread had no ready ops)
		/// </summary>
		long long getThreadStarvationEvents() const;
		void setThreadStarvationEvents(long long value);

		/// <summary>
		/// Scheduler utilization (% of VLIW slots filled)
		/// </summary>
		double getSchedulerUtilization() const;
		void setSchedulerUtilization(double value);

		/// <summary>
		/// FSP contribution (% of slots filled by FSP)
		/// </summary>
		double getFSPContribution() const;
		void setFSPContribution(double value);

		/// <summary>
		/// Utilization per thread (16 threads)
		/// </summary>
		const std::vector<double>& getPerThreadUtilization() const;
		void setPerThreadUtilization(const std::vector<double>& value);

		/// <summary>
		/// Total scheduler latency (cycles spent in scheduler)
		/// </summary>
		long long getTotalSchedulerLatency() const;
		void setTotalSchedulerLatency(long long value);

		/// <summary>
		/// Average scheduler latency per bundle
		/// </summary>
		double getAverageSchedulerLatency() const;
		void setAverageSchedulerLatency(double value);

		/// <summary>
		/// Bank contention stalls detected by MSHR/bank-level scoreboard (Q1 Review §2).
		/// Proves the Universal Scoreboard successfully predicts bank-level hazards.
		/// </summary>
		long long getBankContentionStalls() const;
		void setBankContentionStalls(long long value);

		/// <summary>
		/// FSP pipeline latency cycles from 2-stage pipelined arbitration (Q1 Review §1).
		/// Non-zero when PipelinedFspEnabled is true, indicates stolen ops delayed by 1 cycle.
		/// </summary>
		long long getFspPipelineLatencyCycles() const;
		void setFspPipelineLatencyCycles(long long value);

		/// <summary>
		/// Replay epochs observed by the phase-aware runtime substrate.
		/// </summary>
		long long getReplayEpochCount() const;
		void setReplayEpochCount(long long value);

		/// <summary>
		/// Average replay epoch length.
		/// </summary>
		double getAverageReplayEpochLength() const;
		void setAverageReplayEpochLength(double value);

		/// <summary>
		/// Ratio of slots that remained stable donors across replay hits.
		/// </summary>
		double getStableDonorSlotRatio() const;
		void setStableDonorSlotRatio(double value);

		/// <summary>
		/// Number of scheduler cycles that observed an active replay phase.
		/// </summary>
		long long getReplayAwareCycles() const;
		void setReplayAwareCycles(long long value);

		/// <summary>
		/// Number of legality checks that could reuse a phase-certificate template.
		/// </summary>
		long long getPhaseCertificateReadyHits() const;
		void setPhaseCertificateReadyHits(long long value);

		/// <summary>
		/// Number of legality checks that required a fresh phase-certificate path.
		/// </summary>
		long long getPhaseCertificateReadyMisses() const;
		void setPhaseCertificateReadyMisses(long long value);

		/// <summary>
		/// Estimated number of repeated checks avoided by phase-certificate reuse.
		/// </summary>
		long long getEstimatedPhaseCertificateChecksSaved() const;
		void setEstimatedPhaseCertificateChecksSaved(long long value);

		/// <summary>
		/// Number of explicit phase-certificate invalidations.
		/// </summary>
		long long getPhaseCertificateInvalidations() const;
		void setPhaseCertificateInvalidations(long long value);

		/// <summary>
		/// Invalidation count caused by bundle mutation after a successful injection.
		/// </summary>
		long long getPhaseCertificateMutationInvalidations() const;
		void setPhaseCertificateMutationInvalidations(long long value);

		/// <summary>
		/// Invalidation count caused by replay-phase mismatch or inactive phases.
		/// </summary>
		long long getPhaseCertificatePhaseMismatchInvalidations() const;
		void setPhaseCertificatePhaseMismatchInvalidations(long long value);

		/// <summary>
		/// Deterministic replay transitions recorded by the loop buffer.
		/// </summary>
		long long getDeterministicReplayTransitions() const;
		void setDeterministicReplayTransitions(long long value);

		// ── Phase 2A: Credit-Based Fairness Telemetry ─────────────────

		/// <summary>
		/// Per-VT successful injection counts for fairness distribution analysis.
		/// Index 0–3 corresponds to VT 0–3.
		/// </summary>
		const std::vector<long long>& getPerVtInjections() const;
		void setPerVtInjections(const std::vector<long long>& value);

		/// <summary>
		/// Number of starvation events (VT credit saturated at cap).
		/// </summary>
		long long getFairnessStarvationEvents() const;
		void setFairnessStarvationEvents(long long value);

		// ── Phase 2B: Bank-Pressure Telemetry ─────────────────────────

		/// <summary>
		/// Number of injection decisions where bank-pressure tie-break reordered candidates.
		/// </summary>
		long long getBankPressureAvoidanceCount() const;
		void setBankPressureAvoidanceCount(long long value);

		// ── Phase 2C: Speculation Budget Telemetry ────────────────────

		/// <summary>
		/// Number of speculative injections blocked due to budget exhaustion.
		/// </summary>
		long long getSpeculationBudgetExhaustionEvents() const;
		void setSpeculationBudgetExhaustionEvents(long long value);

		/// <summary>
		/// Peak concurrent speculative operations observed during execution.
		/// </summary>
		long long getPeakConcurrentSpeculativeOps() const;
		void setPeakConcurrentSpeculativeOps(long long value);

		/// <summary>
		/// Reference empty-slot opportunities before replay-stable masking is applied.
		/// </summary>
		long long getDeterminismReferenceOpportunitySlots() const;
		void setDeterminismReferenceOpportunitySlots(long long value);

		/// <summary>
		/// Empty-slot opportunities that remained eligible after replay-stable masking.
		/// </summary>
		long long getDeterminismReplayEligibleSlots() const;
		void setDeterminismReplayEligibleSlots(long long value);

		/// <summary>
		/// Empty-slot opportunities hidden by deterministic replay constraints.
		/// </summary>
		long long getDeterminismMaskedSlots() const;
		void setDeterminismMaskedSlots(long long value);

		/// <summary>
		/// Bounded estimate of injection opportunities lost to deterministic replay constraints.
		/// </summary>
		long long getDeterminismEstimatedLostSlots() const;
		void setDeterminismEstimatedLostSlots(long long value);

		/// <summary>
		/// Scheduling cycles where deterministic replay constraints removed at least one otherwise empty slot.
		/// </summary>
		long long getDeterminismConstrainedCycles() const;
		void setDeterminismConstrainedCycles(long long value);

		/// <summary>
		/// Domain-isolation probes evaluated while screening injected candidates.
		/// </summary>
		long long getDomainIsolationProbeAttempts() const;
		void setDomainIsolationProbeAttempts(long long value);

		/// <summary>
		/// Domain-isolation probes rejected by policy.
		/// </summary>
		long long getDomainIsolationBlockedAttempts() const;
		void setDomainIsolationBlockedAttempts(long long value);

		/// <summary>
		/// Domain-isolation blocks caused by disjoint non-kernel domains.
		/// </summary>
		long long getDomainIsolationCrossDomainBlocks() const;
		void setDomainIsolationCrossDomainBlocks(long long value);

		/// <summary>
		/// Domain-isolation blocks caused by kernel-to-user enforcement.
		/// </summary>
		long long getDomainIsolationKernelToUserBlocks() const;
		void setDomainIsolationKernelToUserBlocks(long long value);

		// ── Phase 5: Oracle Gap Telemetry ──────────────────────────

		/// <summary>
		/// Total slots the shadow oracle packed across all analyzed cycles.
		/// </summary>
		long long getOracleGapTotalOracleSlots() const;
		void setOracleGapTotalOracleSlots(long long value);

		/// <summary>
		/// Total slots the real scheduler packed across all analyzed cycles.
		/// </summary>
		long long getOracleGapTotalRealSlots() const;
		void setOracleGapTotalRealSlots(long long value);

		/// <summary>
		/// Gap slots attributed to donor/replay mask restrictions.
		/// </summary>
		long long getOracleGapDonorRestriction() const;
		void setOracleGapDonorRestriction(long long value);

		/// <summary>
		/// Gap slots attributed to fairness/TDM ordering.
		/// </summary>
		long long getOracleGapFairnessOrdering() const;
		void setOracleGapFairnessOrdering(long long value);

		/// <summary>
		/// Gap slots attributed to legality-layer conservatism.
		/// </summary>
		long long getOracleGapLegalityConservatism() const;
		void setOracleGapLegalityConservatism(long long value);

		/// <summary>
		/// Gap slots attributed to domain-isolation enforcement.
		/// </summary>
		long long getOracleGapDomainIsolation() const;
		void setOracleGapDomainIsolation(long long value);

		/// <summary>
		/// Gap slots attributed to speculation budget exhaustion.
		/// </summary>
		long long getOracleGapSpeculationBudget() const;
		void setOracleGapSpeculationBudget(long long value);

		/// <summary>
		/// Scheduling cycles where oracle packed more than real.
		/// </summary>
		long long getOracleGapCyclesWithGap() const;
		void setOracleGapCyclesWithGap(long long value);

		/// <summary>
		/// Total scheduling cycles analyzed by the shadow oracle.
		/// </summary>
		long long getOracleGapTotalCyclesAnalyzed() const;
		void setOracleGapTotalCyclesAnalyzed(long long value);

		/// <summary>
		/// Number of counterexample records captured.
		/// </summary>
		int getOracleCounterexampleCount() const;
		void setOracleCounterexampleCount(int value);

		/// <summary>
		/// Total missed slots across all captured counterexamples.
		/// </summary>
		long long getOracleCounterexampleTotalMissedSlots() const;
		void setOracleCounterexampleTotalMissedSlots(long long value);

		/// <summary>
		/// Dominant counterexample category name.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public string? OracleCounterexampleDominantCategory {get;set;}
		std::wstring getOracleCounterexampleDominantCategory() const;
		void setOracleCounterexampleDominantCategory(const std::wstring &value);

//		#endregion


//		#region Stream Register File Statistics (Q1 Review §5)

		/// <summary>
		/// L1 bypass hits: stream data served from SRF (RegisterState.Valid)
		/// without consuming a memory port. Proves memory ports are not saturated
		/// thanks to the internal prefetch buffer.
		/// </summary>
		long long getL1BypassHits() const;
		void setL1BypassHits(long long value);

		/// <summary>
		/// Foreground (non-assist) ingress warm attempts issued through the SRF seam.
		/// </summary>
		long long getForegroundWarmAttempts() const;
		void setForegroundWarmAttempts(long long value);

		/// <summary>
		/// Foreground ingress warm attempts that successfully left reusable SRF state behind.
		/// </summary>
		long long getForegroundWarmSuccesses() const;
		void setForegroundWarmSuccesses(long long value);

		/// <summary>
		/// Foreground warm attempts that reused an already-valid SRF window.
		/// </summary>
		long long getForegroundWarmReuseHits() const;
		void setForegroundWarmReuseHits(long long value);

		/// <summary>
		/// Foreground execution reads that were served from SRF rather than backend memory.
		/// </summary>
		long long getForegroundBypassHits() const;
		void setForegroundBypassHits(long long value);

		/// <summary>
		/// Assist-driven lane6/DMA ingress warm attempts issued through the SRF seam.
		/// </summary>
		long long getAssistWarmAttempts() const;
		void setAssistWarmAttempts(long long value);

		/// <summary>
		/// Assist-driven ingress warm attempts that successfully published reusable SRF state.
		/// </summary>
		long long getAssistWarmSuccesses() const;
		void setAssistWarmSuccesses(long long value);

		/// <summary>
		/// Assist-driven warm attempts that found an already-valid SRF window.
		/// </summary>
		long long getAssistWarmReuseHits() const;
		void setAssistWarmReuseHits(long long value);

		/// <summary>
		/// Assist-originated execution reads that were served from SRF rather than backend memory.
		/// </summary>
		long long getAssistBypassHits() const;
		void setAssistBypassHits(long long value);

		/// <summary>
		/// Warm attempts rejected by translation/domain/permission truth before backend ingress.
		/// </summary>
		long long getStreamWarmTranslationRejects() const;
		void setStreamWarmTranslationRejects(long long value);

		/// <summary>
		/// Warm attempts that passed translation but could not materialize ingress/backend state.
		/// </summary>
		long long getStreamWarmBackendRejects() const;
		void setStreamWarmBackendRejects(long long value);

		/// <summary>
		/// Assist warm attempts rejected because the assist SRF resident budget was exhausted.
		/// </summary>
		long long getAssistWarmResidentBudgetRejects() const;
		void setAssistWarmResidentBudgetRejects(long long value);

		/// <summary>
		/// Assist warm attempts rejected because the assist SRF loading budget was exhausted.
		/// </summary>
		long long getAssistWarmLoadingBudgetRejects() const;
		void setAssistWarmLoadingBudgetRejects(long long value);

		/// <summary>
		/// Assist warm attempts rejected because no assist-owned SRF victim was available.
		/// </summary>
		long long getAssistWarmNoVictimRejects() const;
		void setAssistWarmNoVictimRejects(long long value);

//		#endregion

//		#region DMA Controller Statistics

		/// <summary>
		/// Active DMA transfers
		/// </summary>
		int getActiveDmaTransfers() const;
		void setActiveDmaTransfers(int value);

		/// <summary>
		/// Completed DMA transfers
		/// </summary>
		long long getCompletedDmaTransfers() const;
		void setCompletedDmaTransfers(long long value);

		/// <summary>
		/// Total DMA transfer latency in cycles
		/// </summary>
		long long getTotalDmaLatency() const;
		void setTotalDmaLatency(long long value);

		/// <summary>
		/// DMA errors
		/// </summary>
		long long getDmaErrors() const;
		void setDmaErrors(long long value);

		/// <summary>
		/// DMA bytes transferred
		/// </summary>
		long long getDmaBytesTransferred() const;
		void setDmaBytesTransferred(long long value);

//		#endregion


//		#region Methods

		/// <summary>
		/// Generate a compact Phase 1 validation summary for replay-aware evidence review.
		/// </summary>
		std::wstring GeneratePhase1ValidationSummary();

		/// <summary>
		/// Correlate Phase 1 perf-side counters with trace-side evidence and repeated-run determinism.
		/// </summary>
		Phase1EvidenceCorrelationReport CorrelatePhase1Evidence(ReplayTraceEvidenceSummary traceSummary, ReplayDeterminismReport determinismReport);

		/// <summary>
		/// Generate a compact regression-baseline summary that combines perf-side and trace-side Phase 1 evidence.
		/// </summary>
		std::wstring GeneratePhase1RegressionBaselineSummary(ReplayTraceEvidenceSummary traceSummary, ReplayDeterminismReport determinismReport);

		/// <summary>
		/// Generate a compact Phase 2 policy summary for fairness/pressure/budget evidence review.
		/// </summary>
		std::wstring GeneratePhase2PolicySummary();

		/// <summary>
		/// Generate a compact Phase 3 execution summary for replay-aware reuse review.
		/// </summary>
		std::wstring GeneratePhase3ExecutionSummary();

		/// <summary>
		/// Generate a compact Phase 4 evidence summary for determinism-tax and isolation-stress review.
		/// </summary>
		std::wstring GeneratePhase4EvidenceSummary(const std::optional<ReplayEnvelopeReport> &envelopeReport = std::nullopt);

		/// <summary>
		/// Generate a richer Phase 4 regression snapshot that combines perf, trace, determinism, and envelope evidence.
		/// </summary>
		std::wstring GeneratePhase4RegressionSnapshot(ReplayTraceEvidenceSummary traceSummary, ReplayDeterminismReport determinismReport, ReplayEnvelopeReport envelopeReport);

		/// <summary>
		/// Generate a compact Phase 08 typed-slot telemetry summary.
		/// </summary>
		std::wstring GeneratePhase8TypedSlotSummary();

		/// <summary>
		/// Generate human-readable summary
		/// </summary>
		std::wstring GenerateSummary();

		/// <summary>
		/// Export to CSV format
		/// </summary>
		std::wstring ExportToCSV();

		/// <summary>
		/// Export scheduler metrics (Phase 3)
		/// </summary>
		std::wstring ExportSchedulerMetrics();

		/// <summary>
		/// Generate a compact Phase 5 oracle-gap summary for research-grade evidence review.
		/// </summary>
		std::wstring GeneratePhase5OracleGapSummary();

	private:
		static std::wstring FormatBytes(long long bytes);

		static bool ApproximatelyEqual(double left, double right, double tolerance = 0.0001);

//		#endregion


	public:
		double getPhaseCertificateReuseHitRate() const;

		double getPhaseCertificateInvalidationRate() const;

		bool getHasPhase1ValidationTelemetry() const;

		bool getHasPhase2PolicyTelemetry() const;

		bool getHasPhase3ReuseTelemetry() const;

		bool getHasPhase4EvidenceTelemetry() const;

		double getDeterminismOpportunityRetentionRate() const;

		double getDeterminismTaxRate() const;

		double getDomainIsolationBlockRate() const;

		bool getHasPhase5OracleGapTelemetry() const;

		bool getHasStreamIngressWarmTelemetry() const;

		double getOracleEfficiency() const;

		double getOracleGapRate() const;


//		#region Typed-Slot Statistics (Phase 08)

		/// <summary>Disaggregated: compiler bundle exceeds class capacity.</summary>
		long long getStaticClassOvercommitRejects() const;
		void setStaticClassOvercommitRejects(long long value);

		/// <summary>Disaggregated: intra-cycle typed-slot densification exhaustion.</summary>
		long long getDynamicClassExhaustionRejects() const;
		void setDynamicClassExhaustionRejects(long long value);

		/// <summary>Disaggregated: hard-pinned lane occupied.</summary>
		long long getPinnedLaneConflicts() const;
		void setPinnedLaneConflicts(long long value);

		/// <summary>Disaggregated: all class lanes occupied.</summary>
		long long getLateBindingConflicts() const;
		void setLateBindingConflicts(long long value);

		/// <summary>Domain isolation gating reject.</summary>
		long long getTypedSlotDomainRejects() const;
		void setTypedSlotDomainRejects(long long value);

		/// <summary>SMT owner-context guard-plane rejects hidden behind ResourceConflict.</summary>
		long long getSmtOwnerContextGuardRejects() const;
		void setSmtOwnerContextGuardRejects(long long value);

		/// <summary>SMT domain guard-plane rejects hidden behind ResourceConflict.</summary>
		long long getSmtDomainGuardRejects() const;
		void setSmtDomainGuardRejects(long long value);

		/// <summary>SMT boundary guard-plane rejects.</summary>
		long long getSmtBoundaryGuardRejects() const;
		void setSmtBoundaryGuardRejects(long long value);

		/// <summary>SMT shared-resource certificate rejects hidden behind ResourceConflict.</summary>
		long long getSmtSharedResourceCertificateRejects() const;
		void setSmtSharedResourceCertificateRejects(long long value);

		/// <summary>SMT register-group certificate rejects hidden behind ResourceConflict.</summary>
		long long getSmtRegisterGroupCertificateRejects() const;
		void setSmtRegisterGroupCertificateRejects(long long value);

		/// <summary>Most recent SMT legality reject kind observed by typed-slot admission.</summary>
		Core::RejectKind getLastSmtLegalityRejectKind() const;
		void setLastSmtLegalityRejectKind(Core::RejectKind value);

		/// <summary>Most recent SMT legality authority source observed by typed-slot admission.</summary>
		Core::LegalityAuthoritySource getLastSmtLegalityAuthoritySource() const;
		void setLastSmtLegalityAuthoritySource(Core::LegalityAuthoritySource value);

		/// <summary>SMT legality rejections attributed to ALU-class candidates.</summary>
		long long getSmtLegalityRejectByAluClass() const;
		void setSmtLegalityRejectByAluClass(long long value);

		/// <summary>SMT legality rejections attributed to LSU-class candidates.</summary>
		long long getSmtLegalityRejectByLsuClass() const;
		void setSmtLegalityRejectByLsuClass(long long value);

		/// <summary>SMT legality rejections attributed to DMA/Stream-class candidates.</summary>
		long long getSmtLegalityRejectByDmaStreamClass() const;
		void setSmtLegalityRejectByDmaStreamClass(long long value);
		long long getSmtLegalityRejectByMatrixTileStreamClass() const;
		void setSmtLegalityRejectByMatrixTileStreamClass(long long value);

		/// <summary>SMT legality rejections attributed to Branch/Control candidates.</summary>
		long long getSmtLegalityRejectByBranchControl() const;
		void setSmtLegalityRejectByBranchControl(long long value);

		/// <summary>SMT legality rejections attributed to SystemSingleton candidates.</summary>
		long long getSmtLegalityRejectBySystemSingleton() const;
		void setSmtLegalityRejectBySystemSingleton(long long value);

		/// <summary>Fast-path accepts via template budget.</summary>
		long long getTypedSlotFastPathAccepts() const;
		void setTypedSlotFastPathAccepts(long long value);

		/// <summary>Admission via full pipeline (complement to fast-path).</summary>
		long long getTypedSlotStandardPathAccepts() const;
		void setTypedSlotStandardPathAccepts(long long value);

		/// <summary>Level 1 class-template reuse hits.</summary>
		long long getClassTemplateReuseHits() const;
		void setClassTemplateReuseHits(long long value);

		/// <summary>Class-template invalidations.</summary>
		long long getClassTemplateInvalidations() const;
		void setClassTemplateInvalidations(long long value);

		/// <summary>Template invalidated by domain boundary.</summary>
		long long getClassTemplateDomainInvalidations() const;
		void setClassTemplateDomainInvalidations(long long value);

		/// <summary>Template invalidated by capacity mismatch.</summary>
		long long getClassTemplateCapacityMismatchInvalidations() const;
		void setClassTemplateCapacityMismatchInvalidations(long long value);

		/// <summary>Tier 2 lane reuse hits.</summary>
		long long getLaneReuseHits() const;
		void setLaneReuseHits(long long value);

		/// <summary>Tier 2 lane reuse misses (fallback to Tier 1).</summary>
		long long getLaneReuseMisses() const;
		void setLaneReuseMisses(long long value);

		/// <summary>Total successful late lane bindings.</summary>
		long long getTotalLaneBindings() const;
		void setTotalLaneBindings(long long value);

		/// <summary>NOPs caused by hard-pinned lane unavailability.</summary>
		long long getNopDueToPinnedConstraint() const;
		void setNopDueToPinnedConstraint(long long value);

		/// <summary>NOPs caused by class-capacity exhaustion.</summary>
		long long getNopDueToNoClassCapacity() const;
		void setNopDueToNoClassCapacity(long long value);

		/// <summary>NOPs caused by SafetyMask conflict.</summary>
		long long getNopDueToResourceConflict() const;
		void setNopDueToResourceConflict(long long value);

		/// <summary>NOPs caused by dynamic runtime state.</summary>
		long long getNopDueToDynamicState() const;
		void setNopDueToDynamicState(long long value);

		/// <summary>NOPs avoided thanks to class-flexible placement.</summary>
		long long getNopAvoided() const;
		void setNopAvoided(long long value);

		/// <summary>Successful injections into ALU-class lanes.</summary>
		long long getAluClassInjects() const;
		void setAluClassInjects(long long value);

		/// <summary>Successful injections into LSU-class lanes.</summary>
		long long getLsuClassInjects() const;
		void setLsuClassInjects(long long value);

		/// <summary>Successful injections into DMA/Stream-class lanes.</summary>
		long long getDmaStreamClassInjects() const;
		void setDmaStreamClassInjects(long long value);
		long long getMatrixTileStreamClassInjects() const;
		void setMatrixTileStreamClassInjects(long long value);

		/// <summary>Injections into Branch/Control lanes.</summary>
		long long getBranchControlInjects() const;
		void setBranchControlInjects(long long value);

		/// <summary>Injections of hard-pinned ops.</summary>
		long long getHardPinnedInjects() const;
		void setHardPinnedInjects(long long value);

		/// <summary>Injections of class-flexible ops.</summary>
		long long getClassFlexibleInjects() const;
		void setClassFlexibleInjects(long long value);

		/// <summary>
		/// Fraction of rejections that were class-capacity-related.
		/// </summary>
		double getClassCapacityRejectRate() const;

		/// <summary>
		/// Fraction of lane bindings that reused replay hint.
		/// </summary>
		double getLaneReuseRate() const;

		/// <summary>
		/// Ratio of class-flexible to hard-pinned injects.
		/// </summary>
		double getClassFlexibleRatio() const;

		/// <summary>
		/// Estimated NOP reduction from typed-slot flexibility.
		/// </summary>
		double getNopReductionRate() const;

		/// <summary>
		/// Whether Phase 08 typed-slot telemetry is present.
		/// </summary>
		bool getHasPhase8TypedSlotTelemetry() const;

//		#endregion
	};
}
