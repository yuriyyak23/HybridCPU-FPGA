#pragma once

#include "../Cache/SemanticBundleCache.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <limits>
#include <stdexcept>
#include <functional>
#include <optional>
#include <utility>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../tangible_stringbuilder.h"

namespace HybridCpu::ShadowFrontend
{
	struct TraceRow final
	{
		int Sequence = 0;
		std::wstring Workload;
		std::wstring Provenance;
		std::shared_ptr<BundleRequest> Request;
		int Occupancy = 0;
		std::wstring Contour;
		std::optional<FrontendEvent> Event;
		bool Committed = false;
		bool ReplayObserved = false;

		TraceRow(int sequence, std::wstring workload, std::wstring provenance,
			std::shared_ptr<BundleRequest> request, int occupancy, std::wstring contour,
			std::optional<FrontendEvent> event, bool committed, bool replayObserved)
			: Sequence(sequence), Workload(std::move(workload)), Provenance(std::move(provenance)),
			  Request(std::move(request)), Occupancy(occupancy), Contour(std::move(contour)),
			  Event(event), Committed(committed), ReplayObserved(replayObserved) {}
	};

	struct ReplayObservation final
	{
		int Version = 1, Seed = 0, Sequence = 0;
		std::wstring Provenance;
		std::shared_ptr<BundleRequest> Request;
		std::shared_ptr<DecodeContext> Context;
		FreshnessToken Freshness;
		unsigned long long ReplayGeneration = 0;
		std::optional<FrontendEvent> Event;
		unsigned long long CommittedOrder = 0;
		bool Committed = false, Clear = false;
		std::wstring RawHex;
		bool Allowed = false;
		std::optional<std::wstring> FetchFault;
		std::shared_ptr<HotBinding> CandidateBinding, CurrentBinding;
		std::optional<std::wstring> CandidateRawHex;
		std::optional<std::vector<HotGuard>> RejectedGuards;
		std::optional<std::wstring> ExpectedEncodedHex, ExpectedAccessFault;
		IngressOutcome ExpectedOutcome = IngressOutcome::Cold;
		int Capacity = 64, Ways = 4, ByteBudget = std::numeric_limits<int>::max();
		bool Enabled = true;
		ReplacementPolicy Policy = ReplacementPolicy::QuotaBorrow;
		unsigned char ActiveVts = 15;
		std::shared_ptr<LookupTag> CandidateTag;
		std::optional<unsigned long long> CandidateReplayGeneration;
		std::wstring SidebandContour = L"default-no-annotations";
		std::wstring ReplayProtocol = L"absolute-state-prime-once-v1";
	};

	struct ModelRun final
	{
		std::wstring Workload, Mode;
		int Capacity = 0;
		std::wstring PolicyName;
		int Budget = 0;
		std::shared_ptr<TelemetrySnapshot> Counters;
		int ResidentBytes = 0;
		double ModeledCycles = 0, HostMilliseconds = 0;
		long long HostAllocatedBytes = 0;
		std::unordered_map<FrontendEvent, long long> Invalidations;
		std::optional<std::shared_ptr<RetainedGraphAccounting>> RetainedGraph;
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record TraceRow(int Sequence, string Workload, string Provenance, BundleRequest Request, int Occupancy, string Contour, System.Nullable<FrontendEvent> Event, bool Committed, bool ReplayObserved);
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record LatencyModel(double Lookup = 1, double Cold = 40, double Warm = 2, double Hot = 1, double Fill = 1, double Retry = 0, double FetchFault = 1, double TerminalFault = 1, double Permission = 1, double Guard = .1, double Restore = 1, double IntegrityByte = .001)
//	{
//		// Attempt work survives a retry/fault: count actual decode calls and successful fills.
//		// Cold is the combined successful-fetch/decode assumption; Retry is only extra control overhead.
//		// These coefficients remain uncalibrated and do not model physical guard/restore traffic.
//		public double Evaluate(FrontendTelemetry t, bool enabled)
//		{
//			return Evaluate(t.Snapshot(), enabled);
//		}
//		public double Evaluate(TelemetrySnapshot t, bool enabled)
//		{
//			return t.DecoderCalls * Cold + t.SemanticHits * Warm + t.ValidatedHotHits * Hot + t.Retries * Retry + (t.FetchAttempts - t.RawBundleFetches) * FetchFault + t.AccessFaults * TerminalFault + t.PermissionChecks * Permission + t.HotGuardChecks * Guard + t.RestoreAttempts * Restore + t.IntegrityBytesRead * IntegrityByte + (enabled ? (t.DbcLookups + t.HotCandidates) * Lookup + t.Fills * Fill : 0);
//		}
//	}

	// Observations are independent of generator conventions: complete pre-request
	// state plus expected terminal facts. Raw bytes are oracle input, never a lookup tag.
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public sealed record ReplayObservation(int Version, int Seed, int Sequence, string Provenance, BundleRequest Request, DecodeContext Context, FreshnessToken Freshness, ulong ReplayGeneration, System.Nullable<FrontendEvent> Event, ulong CommittedOrder, bool Committed, bool Clear, string RawHex, bool Allowed, string? FetchFault, System.Nullable<HotBinding> CandidateBinding, System.Nullable<HotBinding> CurrentBinding, string? CandidateRawHex, ImmutableArray<HotGuard> RejectedGuards, string? ExpectedEncodedHex, string? ExpectedAccessFault, IngressOutcome ExpectedOutcome, int Capacity = 64, int Ways = 4, int ByteBudget = int.MaxValue, bool Enabled = true, ReplacementPolicy Policy = ReplacementPolicy.QuotaBorrow, byte ActiveVts = 15, System.Nullable<LookupTag> CandidateTag = null, System.Nullable<ulong> CandidateReplayGeneration = null, string SidebandContour = "default-no-annotations", string ReplayProtocol = "absolute-state-prime-once-v1");
//	public sealed record ReplayObservation(int Version, int Seed, int Sequence, string Provenance, BundleRequest Request, DecodeContext Context, FreshnessToken Freshness, ulong ReplayGeneration, System.Nullable<FrontendEvent> Event, ulong CommittedOrder, bool Committed, bool Clear, string RawHex, bool Allowed, string FetchFault, System.Nullable<HotBinding> CandidateBinding, System.Nullable<HotBinding> CurrentBinding, string CandidateRawHex, ImmutableArray<HotGuard> RejectedGuards, string ExpectedEncodedHex, string ExpectedAccessFault, IngressOutcome ExpectedOutcome, int Capacity = 64, int Ways = 4, int ByteBudget = int.MaxValue, bool Enabled = true, ReplacementPolicy Policy = ReplacementPolicy.QuotaBorrow, byte ActiveVts = 15, System.Nullable<LookupTag> CandidateTag = nullptr, System.Nullable<ulong> CandidateReplayGeneration = nullptr, string SidebandContour = "default-no-annotations", string ReplayProtocol = "absolute-state-prime-once-v1");
	class ObservationTrace final
	{
	public:
		static constexpr int Version = 1;
		static constexpr int MaxRows = 100000, MaxLineBytes = 1048576;
		static void Validate(std::shared_ptr<ReplayObservation> row);
		static void Write(const std::wstring &path, std::vector<std::shared_ptr<ReplayObservation>> &rows);
		static std::vector<std::shared_ptr<ReplayObservation>> Read(const std::wstring &path);
	};
	class TraceMinimizer final
	{
		// Predicate must rebuild its oracle/cache each time. Exceptions are not silently
		// classified as mismatches. Deterministic deletion gives a 1-minimal subsequence.
	public:
		template<typename T>
		static std::vector<T> Minimize(std::vector<T> input, std::function<bool(const std::vector<T>&)> stillFails, int maxEvaluations = 10000)
		{
			std::vector<T> current = std::move(input);
			if (maxEvaluations < 1 || !stillFails(current))
			{
				throw std::invalid_argument("Input is not a reproduced failure.");
			}
			int evaluations = 1;
			for (size_t i = 0; i < current.size();)
			{
				if (evaluations >= maxEvaluations)
				{
					throw InvalidOperationException(L"Minimization budget exhausted; no minimality claim.");
				}
				std::vector<T> trial = current;
				trial.erase(trial.begin() + static_cast<typename std::vector<T>::difference_type>(i));
				evaluations++;
				if (stillFails(trial))
				{
					current = trial;
					i = 0;
				}
				else
				{
					i++;
				}
			}
			return current;
		}
	};
	class TraceModel final
	{
	private:
		static inline const std::vector<std::wstring> Workloads = {L"scalar-1", L"scalar-2", L"scalar-4", L"scalar-8", L"branchy", L"streaming", L"vector", L"tile", L"small-loop", L"interleaved-loops", L"smt-same-code", L"smt-different-code", L"context-churn", L"smc-jit"};
		static inline const std::vector<std::wstring> WeightedNonLoop = {L"scalar-1", L"scalar-2", L"scalar-4", L"scalar-8", L"branchy", L"smt-different-code"};

	public:
		static std::vector<std::wstring> getWorkloads();
		// Methodology fixed before measurements: equal weights over these six non-loop classes.
		static std::vector<std::wstring> getWeightedNonLoop();
		static std::vector<std::shared_ptr<TraceRow>> Generate(const std::wstring &workload, int count = 256);
		static std::vector<std::shared_ptr<TraceRow>> ReadJsonLines(const std::wstring &path);
		static void WriteJsonLines(const std::wstring &path, std::vector<std::shared_ptr<TraceRow>> &trace);
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record ModelRun(string Workload, string Mode, int Capacity, string Policy, int Budget, TelemetrySnapshot Counters, int ResidentBytes, double ModeledCycles, double HostMilliseconds, long HostAllocatedBytes, Dictionary<FrontendEvent, long> Invalidations, System.Nullable<RetainedGraphAccounting> RetainedGraph = nullptr);
	class ModelHarness final
	{
		// Test bridge supplies the real LoopBuffer observation adapter. This production
		// harness remains independent of ISE and does not manufacture HOT observations.
	public:
		static std::vector<std::shared_ptr<ModelRun>> Sweep(std::function<ModelRun(std::vector<std::shared_ptr<TraceRow>>, const std::wstring&, int, ReplacementPolicy, int)> run);
	};

}
