#pragma once

#include <string>
#include <cmath>
#include <format>

namespace YAKSys_Hybrid_CPU::Core::Decoder {}

using namespace YAKSys_Hybrid_CPU::Core::Decoder;

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Reason category for oracle-gap decomposition.
	/// Each bucket explains why the real scheduler missed an oracle-legal slot.
	/// </summary>
	enum class OracleGapCategory : unsigned char
	{
		/// <summary>No gap — real scheduler matched oracle packing.</summary>
		None = 0,

		/// <summary>Gap caused by replay-stable donor mask restricting injectable slots.</summary>
		DonorRestriction,

		/// <summary>Gap caused by fairness/credit or TDM ordering skipping a ready candidate.</summary>
		FairnessOrdering,

		/// <summary>Gap caused by legality-layer conservatism (safety mask false reject).</summary>
		LegalityConservatism,

		/// <summary>Gap caused by domain-isolation or kernel-to-user enforcement.</summary>
		DomainIsolation,

		/// <summary>Gap caused by speculation budget exhaustion.</summary>
		SpeculationBudget,

		/// <summary>Phase 07: Gap caused by class-capacity template divergence between runs.</summary>
		ClassCapacityDivergence
	};

	/// <summary>
	/// Per-cycle oracle gap record: captures the difference between oracle and real packing.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct OracleGapRecord
	class OracleGapRecord
	{
	private:
		const long long Cycle;
		const int OraclePackedSlots;
		const int RealPackedSlots;
		const OracleGapCategory PrimaryCategory;

	public:
		OracleGapRecord(long long cycle, int oraclePackedSlots, int realPackedSlots, OracleGapCategory primaryCategory);

		/// <summary>Scheduling cycle when this gap was observed.</summary>
		long long getCycle() const;

		/// <summary>Number of slots the oracle reference contour packed.</summary>
		int getOraclePackedSlots() const;

		/// <summary>Number of slots the real scheduler packed.</summary>
		int getRealPackedSlots() const;

		/// <summary>Primary reason bucket for the gap.</summary>
		OracleGapCategory getPrimaryCategory() const;

		/// <summary>Gap magnitude (oracle - real). Zero or positive.</summary>
		int getGap() const;

		OracleGapRecord() = default;
	};

	/// <summary>
	/// Structured counterexample: a specific scheduling scenario where the real scheduler
	/// missed an oracle-legal injection and the reason can be localized.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct OracleCounterexample
	class OracleCounterexample
	{
	private:
		const long long Cycle;
		const OracleGapCategory Category;
		const int MissedSlots;
		const std::wstring Description;

	public:
		OracleCounterexample(long long cycle, OracleGapCategory category, int missedSlots, const std::wstring &description);

		/// <summary>Cycle where the counterexample was observed.</summary>
		long long getCycle() const;

		/// <summary>Reason bucket classifying the false reject or missed opportunity.</summary>
		OracleGapCategory getCategory() const;

		/// <summary>Number of slots missed in this counterexample.</summary>
		int getMissedSlots() const;

		/// <summary>Human-readable description of the divergence shape.</summary>
		std::wstring getDescription() const;

		std::wstring Describe() const;

		OracleCounterexample() = default;
	};

	/// <summary>
	/// Aggregated oracle-gap summary across all scheduling cycles.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct OracleGapSummary
	class OracleGapSummary
	{
	private:
		const long long TotalOracleSlots;
		const long long TotalRealSlots;
		const long long DonorRestrictionGap;
		const long long FairnessOrderingGap;
		const long long LegalityConservatismGap;
		const long long DomainIsolationGap;
		const long long SpeculationBudgetGap;
		const long long CyclesWithGap;
		const long long TotalCyclesAnalyzed;

	public:
		OracleGapSummary(long long totalOracleSlots, long long totalRealSlots, long long donorRestrictionGap, long long fairnessOrderingGap, long long legalityConservatismGap, long long domainIsolationGap, long long speculationBudgetGap, long long cyclesWithGap, long long totalCyclesAnalyzed);

		/// <summary>Total slots packed by oracle across all analyzed cycles.</summary>
		long long getTotalOracleSlots() const;

		/// <summary>Total slots packed by real scheduler across all analyzed cycles.</summary>
		long long getTotalRealSlots() const;

		/// <summary>Gap slots attributed to donor/replay mask restrictions.</summary>
		long long getDonorRestrictionGap() const;

		/// <summary>Gap slots attributed to fairness/TDM ordering.</summary>
		long long getFairnessOrderingGap() const;

		/// <summary>Gap slots attributed to legality-layer conservatism.</summary>
		long long getLegalityConservatismGap() const;

		/// <summary>Gap slots attributed to domain-isolation enforcement.</summary>
		long long getDomainIsolationGap() const;

		/// <summary>Gap slots attributed to speculation budget exhaustion.</summary>
		long long getSpeculationBudgetGap() const;

		/// <summary>Number of scheduling cycles where oracle packed more than real.</summary>
		long long getCyclesWithGap() const;

		/// <summary>Total scheduling cycles analyzed.</summary>
		long long getTotalCyclesAnalyzed() const;

		/// <summary>Total gap (oracle - real) across all categories.</summary>
		long long getTotalGap() const;

		/// <summary>Oracle efficiency rate: what fraction of oracle slots the real scheduler captured.</summary>
		double getOracleEfficiency() const;

		/// <summary>Gap rate: fraction of oracle slots missed by the real scheduler.</summary>
		double getGapRate() const;

		std::wstring Describe() const;

		OracleGapSummary() = default;
	};

	/// <summary>
	/// Phase 5: Shadow Oracle Scheduler — legality-preserving reference contour.
	///
	/// This partial class adds a diagnostic shadow-oracle path that estimates
	/// what a stronger scheduler could have packed under the same bundle and
	/// safety contract, but without replay-stable donor mask restrictions,
	/// fairness ordering, or speculation budget limits.
	///
	/// The oracle contour is opt-in, comparison-only, and never drives
	/// production scheduling decisions.
	/// </summary>
}
