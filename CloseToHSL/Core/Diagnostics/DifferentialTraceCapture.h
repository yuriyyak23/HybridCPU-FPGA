#pragma once

#include "../Decoder/ClusterIssuePreparation.h"
#include "../Pipeline/Core/RuntimeClusterAdmissionPreparation.CandidateView.h"
#include "../Pipeline/Core/RuntimeClusterAdmissionPreparation.DecisionDraft.h"
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Immutable snapshot of decode-stage advisory chain metadata captured at one bundle decode point.
	/// Used for differential verification between reference sequential and cluster-prepared paths.
	/// All fields are derived from decode-time facts — no runtime mutable state.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct DifferentialTraceEntry
	class DifferentialTraceEntry
	{
	private:
		const unsigned long long PC;
		const YAKSys_Hybrid_CPU::Core::DecodeMode DecodeMode;
		const unsigned char PreparedScalarMask;
		const unsigned char RefinedPreparedScalarMask;
		const int AdvisoryScalarIssueWidth;
		const int RefinedAdvisoryScalarIssueWidth;
		const bool ShouldProbeClusterPath;
		const bool SuggestsFallbackDiagnostics;
		const bool RetainsReferenceSequentialPath;
		const unsigned char ScalarCandidateMask;
		const unsigned char BlockedScalarCandidateMask;
		const unsigned char RegisterHazardMask;
		const unsigned char OrderingHazardMask;

	public:
		DifferentialTraceEntry(unsigned long long pc, YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode, unsigned char preparedScalarMask, unsigned char refinedPreparedScalarMask, int advisoryScalarIssueWidth, int refinedAdvisoryScalarIssueWidth, bool shouldProbeClusterPath, bool suggestsFallbackDiagnostics, bool retainsReferenceSequentialPath, unsigned char scalarCandidateMask, unsigned char blockedScalarCandidateMask, unsigned char registerHazardMask, unsigned char orderingHazardMask);

		/// <summary>Program counter of the decoded bundle.</summary>
		unsigned long long getPC() const;

		/// <summary>Decode mode active at capture time.</summary>
		YAKSys_Hybrid_CPU::Core::DecodeMode getDecodeMode() const;

		/// <summary>Conservative prepared scalar mask from Phase 01 flat blocking.</summary>
		unsigned char getPreparedScalarMask() const;

		/// <summary>Refined prepared scalar mask from Phase 03 hazard-triage-aware readiness.</summary>
		unsigned char getRefinedPreparedScalarMask() const;

		/// <summary>Advisory scalar issue width based on conservative mask.</summary>
		int getAdvisoryScalarIssueWidth() const;

		/// <summary>Advisory scalar issue width based on refined mask.</summary>
		int getRefinedAdvisoryScalarIssueWidth() const;

		/// <summary>Whether the decision draft flagged this bundle for cluster-path probing.</summary>
		bool getShouldProbeClusterPath() const;

		/// <summary>Whether the admission prep suggests narrow fallback.</summary>
		bool getSuggestsFallbackDiagnostics() const;

		/// <summary>Whether the reference sequential path is retained (must always be true).</summary>
		bool getRetainsReferenceSequentialPath() const;

		/// <summary>Scalar candidate mask from admission prep.</summary>
		unsigned char getScalarCandidateMask() const;

		/// <summary>Blocked scalar candidate mask (candidates excluded from prepared group).</summary>
		unsigned char getBlockedScalarCandidateMask() const;

		/// <summary>Register hazard mask from dependency analysis.</summary>
		unsigned char getRegisterHazardMask() const;

		/// <summary>Ordering hazard mask from dependency analysis.</summary>
		unsigned char getOrderingHazardMask() const;

		/// <summary>
		/// Create a trace entry from the advisory chain objects materialized during decode.
		/// </summary>
		static DifferentialTraceEntry FromAdvisoryChain(RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionDraft decisionDraft);

		DifferentialTraceEntry() = default;
	};

	/// <summary>
	/// Append-only trace capture for differential verification.
	/// Collects <see cref="DifferentialTraceEntry"/> snapshots from decode-stage advisory chain
	/// materialization. This is a diagnostic facility — it does not modify pipeline execution.
	/// </summary>
	class DifferentialTraceCapture final : public std::enable_shared_from_this<DifferentialTraceCapture>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<DifferentialTraceEntry> _entries = [];
		std::vector<DifferentialTraceEntry> _entries = {};

		/// <summary>Number of captured entries.</summary>
	public:
		int getCount() const;

		/// <summary>
		/// Append a trace entry captured from advisory chain materialization.
		/// </summary>
		void AddEntry(DifferentialTraceEntry entry);

		/// <summary>
		/// Return a read-only view of all captured entries.
		/// </summary>
		const std::vector<DifferentialTraceEntry> GetEntries();

		/// <summary>
		/// Remove all captured entries and reset the trace to empty state.
		/// </summary>
		void Clear();
	};
}
