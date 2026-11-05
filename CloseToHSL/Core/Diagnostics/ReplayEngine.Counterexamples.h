#pragma once

#include "../Pipeline/Scheduling/MicroOpScheduler.ShadowOracle.h"
#include <string>
#include <format>

using namespace YAKSys_Hybrid_CPU::Core;

namespace HybridCPU_ISE::Core
{
	/// <summary>
	/// Aggregated counterexample evidence summary for Phase 5 refinement guidance.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct CounterexampleEvidenceSummary
	class CounterexampleEvidenceSummary
	{
	private:
		const int TotalCounterexamples;
		const int DonorRestrictionCount;
		const int FairnessOrderingCount;
		const int LegalityConservatismCount;
		const int DomainIsolationCount;
		const int SpeculationBudgetCount;
		const long long TotalMissedSlots;
		const int ClassCapacityDivergenceCount;

	public:
		CounterexampleEvidenceSummary(int totalCounterexamples, int donorRestrictionCount, int fairnessOrderingCount, int legalityConservatismCount, int domainIsolationCount, int speculationBudgetCount, long long totalMissedSlots, int classCapacityDivergenceCount = 0);

		/// <summary>Total number of counterexamples captured.</summary>
		int getTotalCounterexamples() const;

		/// <summary>Counterexamples attributed to donor/replay mask restrictions.</summary>
		int getDonorRestrictionCount() const;

		/// <summary>Counterexamples attributed to fairness ordering.</summary>
		int getFairnessOrderingCount() const;

		/// <summary>Counterexamples attributed to legality conservatism.</summary>
		int getLegalityConservatismCount() const;

		/// <summary>Counterexamples attributed to domain isolation.</summary>
		int getDomainIsolationCount() const;

		/// <summary>Counterexamples attributed to speculation budget exhaustion.</summary>
		int getSpeculationBudgetCount() const;

		/// <summary>Total missed slots across all counterexamples.</summary>
		long long getTotalMissedSlots() const;

		/// <summary>Phase 07: Counterexamples attributed to class-capacity template divergence.</summary>
		int getClassCapacityDivergenceCount() const;

		/// <summary>Dominant category: the reason bucket with the most counterexamples.</summary>
		OracleGapCategory getDominantCategory() const;

		std::wstring Describe() const;

		CounterexampleEvidenceSummary() = default;
	};

	/// <summary>
	/// Phase 5: Counterexample extraction and refinement guidance helpers.
	///
	/// This partial class adds methods for extracting structured counterexample
	/// evidence from oracle gap records. The evidence is diagnostic-only and does
	/// not auto-mutate production legality rules.
	/// </summary>
}
