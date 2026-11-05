#pragma once

#include "../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../Decoder/DecodedBundleDescriptor.h"
#include "../Pipeline/Certificates/ReplayPhaseSubstrate.h"
#include "../Pipeline/Assist/Runtime/AssistRuntime.h"
#include "../Architecture/State/Pipeline/PipelineState.h"
#include "../Architecture/State/Pipeline/PipelineTransitionTrigger.h"
#include "IV4TraceEventSink.h"
#include "V4TraceEvent.h"
#include "../Pipeline/Certificates/ReplayPhaseSubstrate.Implementations.h"
#include "TraceEventKind.h"
#include "../Architecture/Common/BinaryIOCompat.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <any>
#include <optional>
#include <format>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core;

namespace HybridCPU_ISE::Core
{
	using System::IO::BinaryWriter;
	// Phase 11 partial — v4 trace recording lives below, in the TraceSink class.
	/// <summary>
	/// Trace format for execution events
	/// </summary>
	enum class TraceFormat
	{
		CSV,
		JSON,
		Binary // Phase 5: Binary format for efficient storage
	};

	/// <summary>
	/// Trace level for controlling trace detail
	/// </summary>
	enum class TraceLevel
	{
		None, // No tracing
		Summary, // PC, opcode, exception code only
		Full // Complete trace with all operands and results (Phase 5: includes full architectural state)
	};

	/// <summary>
	/// Vector exception flags for tracing
	/// </summary>
	class VectorExceptionFlags
	{
	public:
		bool Overflow = false;
		bool Underflow = false;
		bool DivByZero = false;
		bool InvalidOp = false;
		bool Inexact = false;

		bool getHasException() const;

		std::wstring ToString();
	};

	/// <summary>
	/// Single execution event for tracing
	/// </summary>
	class TraceEvent
	{
	public:
		long long PC = 0;
		int BundleId = 0;
		int OpIndex = 0;
		unsigned int Opcode = 0;
		std::vector<std::any> Operands;
		unsigned long long PredicateMask = 0;
		VectorExceptionFlags Flags;
		std::any Result;
		unsigned int ExceptionCount = 0;

		TraceEvent(long long pc, int bundleId, int opIndex, unsigned int opcode);

		TraceEvent() = default;
	};

	/// <summary>
	/// Memory delta for incremental state tracking (Phase 5)
	/// </summary>
	class MemoryDelta
	{
	public:
		unsigned long long Address = 0;
		std::vector<unsigned char> OldData;
		std::vector<unsigned char> NewData;

		MemoryDelta(unsigned long long address, std::vector<unsigned char> &oldData, std::vector<unsigned char> &newData);

		MemoryDelta() = default;
	};

	/// <summary>
	/// FP exception context per thread (Phase 5)
	/// </summary>
	class FPExceptionContext
	{
	public:
		VectorExceptionFlags Flags;
		int RoundingMode = 0; // 0=RNE, 1=RTZ, 2=RDN, 3=RUP, 4=RMM
		bool FlushToZero = false;
		bool DefaultNaN = false;

		FPExceptionContext();
	};

	/// <summary>
	/// Optional typed-slot annotation for trace events (Phase 08).
	/// Emitted at Full trace level when FSP injection occurs.
	/// </summary>
	class TypedSlotTraceAnnotation
	{
	public:
		SlotClass CandidateClass = static_cast<SlotClass>(0);
		SlotPinningKind PinningKind = static_cast<SlotPinningKind>(0);
		int SelectedLane = 0;
		bool UsedFastPath = false;
		bool UsedReplayHint = false;
		TypedSlotRejectReason RejectReason = static_cast<TypedSlotRejectReason>(0);
	};

	/// <summary>
	/// Comprehensive trace event for deterministic replay (Phase 5)
	/// </summary>
	class FullStateTraceEvent
	{
		// Existing fields from TraceEvent
	public:
		long long PC = 0;
		int BundleId = 0;
		int OpIndex = 0;
		unsigned int Opcode = 0;

		// Extended fields for full state
		int ThreadId = 0; // Which thread executed this
		long long CycleNumber = 0; // Global cycle counter
		std::shared_ptr<std::vector<unsigned long long>> RegisterFile; // All 32 int registers
		std::shared_ptr<std::vector<unsigned long long>> VectorRegisters; // VLMAX vector regs (if used)
		std::shared_ptr<std::vector<unsigned short>> PredicateRegisters; // 16x 64-bit predicate regs
		FPExceptionContext FPContext; // FP exception state
		std::shared_ptr<std::vector<MemoryDelta>> MemoryWrites; // Memory changes this cycle
		bool WasStolenSlot = false; // Was this FSP-injected?
		int OriginalThreadId = 0; // If stolen, original owner

		// Pipeline state
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public string? PipelineStage;
		std::wstring PipelineStage; // IF/ID/EX/MEM/WB
		bool Stalled = false; // Was pipeline stalled?
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public string? StallReason;
		std::wstring StallReason; // Why stalled
		YAKSys_Hybrid_CPU::Core::DecodedBundleStateOwnerKind DecodedBundleStateOwnerKind = static_cast<YAKSys_Hybrid_CPU::Core::DecodedBundleStateOwnerKind>(0);
		unsigned long long DecodedBundleStateEpoch = 0;
		unsigned long long DecodedBundleStateVersion = 0;
		YAKSys_Hybrid_CPU::Core::DecodedBundleStateKind DecodedBundleStateKind = static_cast<YAKSys_Hybrid_CPU::Core::DecodedBundleStateKind>(0);
		YAKSys_Hybrid_CPU::Core::DecodedBundleStateOrigin DecodedBundleStateOrigin = static_cast<YAKSys_Hybrid_CPU::Core::DecodedBundleStateOrigin>(0);
		unsigned long long DecodedBundlePc = 0;
		unsigned char DecodedBundleValidMask = 0;
		unsigned char DecodedBundleNopMask = 0;
		bool DecodedBundleHasCanonicalDecode = false;
		bool DecodedBundleHasCanonicalLegality = false;
		bool DecodedBundleHasDecodeFault = false;

		// Memory subsystem state
		std::shared_ptr<std::vector<int>> BankQueueDepths; // Per-bank queue depths
		int ActiveMemoryRequests = 0; // In-flight requests
		long long MemorySubsystemCycle = 0; // Memory subsystem clock

		// Scheduler compatibility state. CurrentFSPPolicy is a legacy trace field
		// interpreted as the replay-aware bundle densification policy label.
		std::shared_ptr<std::vector<int>> ThreadReadyQueueDepths; // Per-thread ready queue
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public string? CurrentFSPPolicy;
		std::wstring CurrentFSPPolicy; // Legacy trace field retained for binary compatibility

		// Phase-aware replay state
		unsigned long long ReplayEpochId = 0;
		unsigned long long ReplayPhaseCachedPc = 0;
		unsigned long long ReplayEpochLength = 0;
		int ReplayPhaseValidSlotCount = 0;
		unsigned char StableDonorMask = 0;
		ReplayPhaseInvalidationReason ReplayInvalidationReason = static_cast<ReplayPhaseInvalidationReason>(0);
		bool PhaseCertificateTemplateReusable = false;
		long long PhaseCertificateReadyHits = 0;
		long long PhaseCertificateReadyMisses = 0;
		long long EstimatedPhaseCertificateChecksSaved = 0;
		long long PhaseCertificateInvalidations = 0;
		ReplayPhaseInvalidationReason PhaseCertificateInvalidationReason = static_cast<ReplayPhaseInvalidationReason>(0);
		long long DeterminismReferenceOpportunitySlots = 0;
		long long DeterminismReplayEligibleSlots = 0;
		long long DeterminismMaskedSlots = 0;
		long long DeterminismEstimatedLostSlots = 0;
		long long DeterminismConstrainedCycles = 0;
		long long DomainIsolationProbeAttempts = 0;
		long long DomainIsolationBlockedAttempts = 0;
		long long DomainIsolationCrossDomainBlocks = 0;
		long long DomainIsolationKernelToUserBlocks = 0;

		// Phase 06: explicit scheduler/FSM eligibility diagnostics
		long long EligibilityMaskedCycles = 0;
		long long EligibilityMaskedReadyCandidates = 0;
		unsigned char LastEligibilityRequestedMask = 0;
		unsigned char LastEligibilityNormalizedMask = 0;
		unsigned char LastEligibilityReadyPortMask = 0;
		unsigned char LastEligibilityVisibleReadyMask = 0;
		unsigned char LastEligibilityMaskedReadyMask = 0;

		// Post-phase-08 assist runtime telemetry
		long long AssistNominations = 0;
		long long AssistInjections = 0;
		long long AssistRejects = 0;
		long long AssistBoundaryRejects = 0;
		long long AssistInvalidations = 0;
		long long AssistInterCoreNominations = 0;
		long long AssistInterCoreInjections = 0;
		long long AssistInterCoreRejects = 0;
		long long AssistInterCoreDomainRejects = 0;
		long long AssistInterCorePodLocalInjections = 0;
		long long AssistInterCoreCrossPodInjections = 0;
		long long AssistInterCorePodLocalRejects = 0;
		long long AssistInterCoreCrossPodRejects = 0;
		long long AssistInterCorePodLocalDomainRejects = 0;
		long long AssistInterCoreCrossPodDomainRejects = 0;
		long long AssistInterCoreSameVtVectorInjects = 0;
		long long AssistInterCoreDonorVtVectorInjects = 0;
		long long AssistInterCoreSameVtVectorWritebackInjects = 0;
		long long AssistInterCoreDonorVtVectorWritebackInjects = 0;
		long long AssistInterCoreLane6DefaultStoreDonorPrefetchInjects = 0;
		long long AssistInterCoreLane6HotLoadDonorPrefetchInjects = 0;
		long long AssistInterCoreLane6HotStoreDonorPrefetchInjects = 0;
		long long AssistInterCoreLane6DonorPrefetchInjects = 0;
		long long AssistInterCoreLane6ColdStoreLdsaInjects = 0;
		long long AssistInterCoreLane6LdsaInjects = 0;
		long long AssistQuotaRejects = 0;
		long long AssistQuotaIssueRejects = 0;
		long long AssistQuotaLineRejects = 0;
		long long AssistQuotaLinesReserved = 0;
		long long AssistBackpressureRejects = 0;
		long long AssistBackpressureOuterCapRejects = 0;
		long long AssistBackpressureMshrRejects = 0;
		long long AssistBackpressureDmaSrfRejects = 0;
		long long AssistDonorPrefetchInjects = 0;
		long long AssistLdsaInjects = 0;
		long long AssistVdsaInjects = 0;
		long long AssistSameVtInjects = 0;
		long long AssistDonorVtInjects = 0;
		YAKSys_Hybrid_CPU::Core::AssistInvalidationReason AssistInvalidationReason = static_cast<YAKSys_Hybrid_CPU::Core::AssistInvalidationReason>(0);
		unsigned long long AssistOwnershipSignature = 0;

		// Phase 08: typed-slot annotation (memory trace only until a later binary-trace revision)
		std::optional<TypedSlotTraceAnnotation> TypedSlotAnnotation;

		// Phase 05: pipeline FSM state at time of event
		PipelineState CurrentPipelineState = static_cast<PipelineState>(0);
	};

	/// <summary>
	/// State checkpoint for incremental replay (Phase 5)
	/// </summary>
	class StateCheckpoint
	{
	public:
		long long CycleNumber = 0;
		std::vector<unsigned char> ProcessorState; // Serialized CPU state
		std::vector<unsigned char> MemorySnapshot; // Compressed memory dump
		unsigned long long RandomState = 0; // RNG state for determinism
	};

	/// <summary>
	/// Pipeline FSM transition event (Phase 05).
	/// Recorded whenever the pipeline FSM changes state.
	/// </summary>
	class FsmTransitionEvent
	{
		/// <summary>Global cycle counter at the time of the transition.</summary>
	public:
		long long CycleNumber = 0;

		/// <summary>Virtual thread (VT) ID that underwent the transition.</summary>
		int VtId = 0;

		/// <summary>Pipeline state before the transition.</summary>
		PipelineState FromState = static_cast<PipelineState>(0);

		/// <summary>Trigger that caused the transition.</summary>
		PipelineTransitionTrigger Trigger = static_cast<PipelineTransitionTrigger>(0);

		/// <summary>Pipeline state after the transition.</summary>
		PipelineState ToState = static_cast<PipelineState>(0);
	};

	/// <summary>
	/// TraceSink records execution events for verification and debugging.
	/// Phase 5: Enhanced with full state capture and checkpointing.
	/// Phase 11: Extended with v4 typed trace event recording (<see cref="IV4TraceEventSink"/>).
	/// </summary>
	class TraceSink : public IV4TraceEventSink
	{
	private:
		static constexpr unsigned int TraceMagic = 0x54524143; // 'TRAC' in ASCII for binary trace header
	public:
		static constexpr unsigned short BinaryTraceVersion = 23;

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<TraceEvent> events;
		std::vector<TraceEvent> events;
		const TraceFormat format;
		const std::wstring filePath;
		bool enabled = false;
		TraceLevel level = static_cast<TraceLevel>(0);

		// Phase 5: Per-thread full state traces
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<int, List<FullStateTraceEvent>> _perThreadTraces;
		std::unordered_map<int, std::vector<FullStateTraceEvent>> _perThreadTraces;
		long long _checkpointInterval = 10000; // Checkpoint every 10K cycles
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<StateCheckpoint> _checkpoints;
		std::vector<StateCheckpoint> _checkpoints;
		long long _currentCycle = 0;

		// Phase 05: FSM transition log
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<FsmTransitionEvent> _fsmTransitions;
		std::vector<FsmTransitionEvent> _fsmTransitions;

		// Phase 11: v4 typed trace event log (append-only)
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<V4TraceEvent> _v4Events;
		std::vector<V4TraceEvent> _v4Events;

	public:
		TraceSink(TraceFormat format = TraceFormat::CSV, const std::wstring &filePath = L"trace.log");

		/// <summary>
		/// Enable or disable tracing
		/// </summary>
		void SetEnabled(bool enabled);

		/// <summary>
		/// Set trace level
		/// </summary>
		void SetLevel(TraceLevel level);

		/// <summary>
		/// Returns true only when full-state trace payloads should be materialized and recorded.
		/// Callers can use this to avoid building register/predicate snapshots on summary or disabled paths.
		/// </summary>
		bool getShouldCaptureFullState() const;

		/// <summary>
		/// Record a trace event
		/// </summary>
		void Record(TraceEvent evt);

		/// <summary>
		/// Clear all recorded events
		/// </summary>
		void Clear();

		/// <summary>
		/// Get count of recorded events
		/// </summary>
		int getEventCount() const;

		/// <summary>
		/// Set checkpoint interval (Phase 5)
		/// </summary>
		void SetCheckpointInterval(long long interval);

		/// <summary>
		/// Record full architectural state (Phase 5)
		/// </summary>
		void RecordFullState(FullStateTraceEvent evt);

		/// <summary>
		/// Record a pipeline FSM state transition (Phase 05).
		/// Always recorded regardless of trace level — FSM transitions are
		/// architecturally significant and required for deterministic replay.
		/// </summary>
		void RecordFsmTransition(long long cycleNumber, int vtId, PipelineState fromState, PipelineTransitionTrigger trigger, PipelineState toState);

		/// <summary>
		/// Returns all recorded FSM transition events for inspection and replay.
		/// </summary>
		const std::vector<FsmTransitionEvent> GetFsmTransitions();

		/// <summary>
		/// Record a full-state trace event enriched with replay-phase diagnostics.
		/// </summary>
		void RecordPhaseAwareState(FullStateTraceEvent evt, ReplayPhaseContext phaseContext, SchedulerPhaseMetrics schedulerMetrics, bool phaseCertificateTemplateReusable);

		/// <summary>
		/// Get per-thread trace events (Phase 5)
		/// </summary>
		const std::vector<FullStateTraceEvent> GetThreadTrace(int threadId);

		/// <summary>
		/// Get total trace count across all threads (Phase 5)
		/// </summary>
		int GetTotalTraceCount();

		/// <summary>
		/// Export binary trace in efficient format (Phase 5)
		/// </summary>
		void ExportBinaryTrace(const std::wstring &binaryFilePath);

		/// <summary>
		/// Serialize checkpoint to binary format (Phase 5)
		/// </summary>
	private:
		void SerializeCheckpoint(std::shared_ptr<BinaryWriter> writer, StateCheckpoint checkpoint);

		/// <summary>
		/// Serialize full state trace event to binary format (Phase 5)
		/// </summary>
		void SerializeFullStateEvent(std::shared_ptr<BinaryWriter> writer, FullStateTraceEvent evt);

		/// <summary>
		/// Clear all traces including per-thread traces (Phase 5)
		/// </summary>
	public:
		void ClearAllTraces();

		/// <summary>
		/// Flush trace to file
		/// </summary>
		void Flush();

	private:
		void FlushCSV();

		void FlushJSON();

		/// <summary>
		/// Get all recorded events (for testing)
		/// </summary>
	public:
		const std::vector<TraceEvent> GetEvents();

		// ─── Phase 11: v4 Typed Trace Event Recording ───────────────────────

		/// <summary>
		/// Record a v4 typed trace event.
		/// <para>
		/// The trace is append-only — events cannot be retroactively modified.
		/// Recording is always performed regardless of <see cref="TraceLevel"/>;
		/// v4 events are architecturally significant.
		/// </para>
		/// </summary>
		/// <param name="evt">The v4 event to record.</param>
		void RecordV4Event(V4TraceEvent evt) override;

		/// <summary>
		/// Returns all recorded v4 trace events for inspection and replay validation.
		/// The list is append-only during execution.
		/// </summary>
		const std::vector<V4TraceEvent> GetV4Events();

		/// <summary>
		/// Returns only the v4 events matching the given <paramref name="kind"/>.
		/// </summary>
		const std::vector<V4TraceEvent> GetV4Events(TraceEventKind kind);

		/// <summary>
		/// Returns only the v4 events emitted by the given <paramref name="vtId"/>.
		/// </summary>
		const std::vector<V4TraceEvent> GetV4EventsForVt(unsigned char vtId);

		/// <summary>
		/// Clear all v4 trace events (e.g., between test runs).
		/// </summary>
		void ClearV4Events();

		/// <summary>Total number of v4 events recorded.</summary>
		int getV4EventCount() const;
	};
}
