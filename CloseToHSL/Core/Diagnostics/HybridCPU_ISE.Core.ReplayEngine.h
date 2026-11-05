#pragma once

#include "ReplayEngine.Types.h"
#include "TraceSink.h"
#include "../Pipeline/Certificates/ReplayPhaseSubstrate.h"
#include "ReplayEngine.Counterexamples.h"
#include "../Architecture/Common/BinaryIOCompat.h"
#include "../Pipeline/Scheduling/MicroOpScheduler.ShadowOracle.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <any>
#include <functional>
#include <optional>
#include <tuple>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../../tangible_filesystem.h"

using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core;



namespace HybridCPU_ISE::Core
{
	using System::IO::BinaryReader;
	/// <summary>
	/// Aggregated counterexample evidence summary for Phase 5 refinement guidance.
	/// </summary>

		/// <summary>Total number of counterexamples captured.</summary>

		/// <summary>Counterexamples attributed to donor/replay mask restrictions.</summary>

		/// <summary>Counterexamples attributed to fairness ordering.</summary>

		/// <summary>Counterexamples attributed to legality conservatism.</summary>

		/// <summary>Counterexamples attributed to domain isolation.</summary>

		/// <summary>Counterexamples attributed to speculation budget exhaustion.</summary>

		/// <summary>Total missed slots across all counterexamples.</summary>

		/// <summary>Phase 07: Counterexamples attributed to class-capacity template divergence.</summary>

		/// <summary>Dominant category: the reason bucket with the most counterexamples.</summary>






	/// <summary>
	/// Phase 5: Counterexample extraction and refinement guidance helpers.
	///
	/// This partial class adds methods for extracting structured counterexample
	/// evidence from oracle gap records. The evidence is diagnostic-only and does
	/// not auto-mutate production legality rules.
	/// </summary>
	/// <summary>
	/// Replay engine for deterministic execution reproduction (Phase 5)
	/// Supports checkpoint-based fast-forward and cycle-accurate replay
	/// </summary>
	class ReplayEngine : public std::enable_shared_from_this<ReplayEngine>
	{
		/// </summary>
	public:
		ReplayEnvelopeReport CompareReplayPhaseBehaviorWithinEnvelope(std::shared_ptr<ReplayEngine> other, ReplayEnvelopeConfiguration configuration);

		/// <summary>
		/// Aggregate replay-heavy trace evidence for practical Phase 1 validation passes.
		/// </summary>
		ReplayTraceEvidenceSummary SummarizeReplayPhaseEvidence();

		/// <summary>
		/// Aggregate replay-heavy evidence per epoch for long practical trace analysis.
		/// </summary>
		std::vector<ReplayEpochEvidenceSummary> SummarizeReplayEpochEvidence();

	private:
		static bool HasInvalidationSignal(FullStateTraceEvent evt);

		static ReplayPhaseInvalidationReason GetDominantInvalidationReason(FullStateTraceEvent evt);

		static bool IsDensePhaseTimelineSample(FullStateTraceEvent evt);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool HasBasePolicy(string? policy, string expectedPolicy)
		static bool HasBasePolicy(const std::wstring &policy, const std::wstring &expectedPolicy);

		static bool TryCompareReplayContractEvent(FullStateTraceEvent expected, FullStateTraceEvent actual, std::wstring &mismatchField, std::wstring &expectedValue, std::wstring &actualValue);

		static bool TryCompareReplayEvent(FullStateTraceEvent expected, FullStateTraceEvent actual, std::wstring &mismatchField, std::wstring &expectedValue, std::wstring &actualValue);

		static bool TryCompareEventWithinEnvelope(FullStateTraceEvent expected, FullStateTraceEvent actual, ReplayEnvelopeConfiguration configuration, std::wstring &mismatchField, std::wstring &expectedValue, std::wstring &actualValue, std::wstring &allowedEnvelope, int &envelopeChecks);

		static bool TryCompareWithinDelta(const std::wstring &field, long long expected, long long actual, long long maxDelta, std::wstring &mismatchField, std::wstring &expectedValue, std::wstring &actualValue, std::wstring &allowedEnvelope);

		static bool TryCompareArrayWithinDelta(const std::wstring &field, std::shared_ptr<std::vector<int>> expected, std::shared_ptr<std::vector<int>> actual, int maxDelta, std::wstring &mismatchField, std::wstring &expectedValue, std::wstring &actualValue, std::wstring &allowedEnvelope, int &envelopeChecks);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool Mismatch(string field, Object? expected, Object? actual, out string mismatchField, out string expectedValue, out string actualValue)
		static bool Mismatch(const std::wstring &field, std::any expected, std::any actual, std::wstring &mismatchField, std::wstring &expectedValue, std::wstring &actualValue);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static string FormatValue(Object? value)
		static std::wstring FormatValue(std::any value);

		static std::wstring FormatArray(std::shared_ptr<std::vector<int>> values);

		static int CountBits(unsigned char value);


		/// <summary>
		/// Extract a counterexample evidence summary from oracle gap counterexamples.
		/// </summary>
	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static CounterexampleEvidenceSummary ExtractCounterexampleEvidence(IReadOnlyList<OracleCounterexample> counterexamples)
		static CounterexampleEvidenceSummary ExtractCounterexampleEvidence(const std::vector<OracleCounterexample> &counterexamples);

		/// <summary>
		/// Generate refinement guidance strings from counterexample evidence.
		/// These are human-readable hints, not automatic rule mutations.
		/// </summary>
		static const std::vector<std::wstring> GenerateRefinementGuidance(CounterexampleEvidenceSummary evidence);


	private:
		static inline const std::wstring DenseTimelinePolicyName = L"ReplayAwarePhase1.DenseTimeline";

		static bool IsSupportedBinaryTraceVersion(unsigned short version);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private TraceSink? _trace;
		std::shared_ptr<TraceSink> _trace;
		int _currentCheckpointIndex = 0;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<int, int> _perThreadEventIndex;
		std::unordered_map<int, int> _perThreadEventIndex;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<StateCheckpoint> _checkpoints;
		std::vector<StateCheckpoint> _checkpoints;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<int, List<FullStateTraceEvent>> _perThreadTraces;
		std::unordered_map<int, std::vector<FullStateTraceEvent>> _perThreadTraces;
		std::wstring _traceFilePath;

	public:
		ReplayEngine(const std::wstring &traceFilePath = L"");

		/// <summary>
		/// Load trace from TraceSink object
		/// </summary>
		void LoadFromTraceSink(std::shared_ptr<TraceSink> traceSink);

		/// <summary>
		/// Load binary trace file
		/// </summary>
	private:
		void LoadBinaryTrace(const std::wstring &filePath);

		/// <summary>
		/// Deserialize checkpoint from binary format
		/// </summary>
		StateCheckpoint DeserializeCheckpoint(std::shared_ptr<BinaryReader> reader);

		/// <summary>
		/// Deserialize full state event from binary format
		/// </summary>
		FullStateTraceEvent DeserializeFullStateEvent(std::shared_ptr<BinaryReader> reader, unsigned short version);

		/// <summary>
		/// Find nearest checkpoint before or at target cycle
		/// </summary>
		std::optional<StateCheckpoint> FindNearestCheckpoint(long long targetCycle);

		/// <summary>
		/// Get events for a specific cycle across all threads
		/// </summary>
		std::vector<FullStateTraceEvent> GetEventsForCycle(long long cycle);

		/// <summary>
		/// Replay execution up to specific cycle
		/// NOTE: This is a simplified implementation that would need integration
		/// with actual Processor struct for full replay capability
		/// </summary>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool ReplayToCycle(long targetCycle, System.Action<FullStateTraceEvent>? eventCallback = null)
		bool ReplayToCycle(long long targetCycle, std::function<void(FullStateTraceEvent)> eventCallback = nullptr);

		/// <summary>
		/// Get trace statistics: total events, non-empty threads, and maximum cycle.
		/// </summary>
		std::tuple<int, int, long long> GetStatistics();

		/// <summary>
		/// Verify trace integrity
		/// </summary>
	public:
		bool VerifyTraceIntegrity();

		/// <summary>
		/// Verify replay-phase metadata consistency across the captured trace.
		/// </summary>
		bool VerifyReplayPhaseConsistency();

		/// <summary>
		/// Compare two repeated runs and report whether replay-heavy behavior stayed deterministic.
		/// </summary>
		static ReplayDeterminismReport CompareRepeatedRuns(std::shared_ptr<TraceSink> baselineTrace, std::shared_ptr<TraceSink> candidateTrace);

		/// <summary>
		/// Compare two repeated runs while tolerating bounded timing/resource perturbations.
		/// </summary>
		static ReplayEnvelopeReport CompareRepeatedRunsWithinEnvelope(std::shared_ptr<TraceSink> baselineTrace, std::shared_ptr<TraceSink> candidateTrace, ReplayEnvelopeConfiguration configuration);

		/// <summary>
		/// Summarize replay-heavy evidence directly from a trace sink.
		/// </summary>
		static ReplayTraceEvidenceSummary SummarizeReplayPhaseEvidence(std::shared_ptr<TraceSink> traceSink);

		/// <summary>
		/// Summarize per-epoch replay-heavy evidence directly from a trace sink.
		/// </summary>
		static std::vector<ReplayEpochEvidenceSummary> SummarizeReplayEpochEvidence(std::shared_ptr<TraceSink> traceSink);

		/// <summary>
		/// Compare replay-phase behavior between this run and another run.
		/// </summary>
		ReplayDeterminismReport CompareReplayPhaseBehavior(std::shared_ptr<ReplayEngine> other);

		/// <summary>
		/// Compare replay-phase behavior while tolerating bounded timing/resource perturbations.
	};
}
