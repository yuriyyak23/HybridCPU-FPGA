#include "ReplayEngine.Counterexamples.h"

using namespace YAKSys_Hybrid_CPU::Core;

namespace HybridCPU_ISE::Core
{

	CounterexampleEvidenceSummary::CounterexampleEvidenceSummary(int totalCounterexamples, int donorRestrictionCount, int fairnessOrderingCount, int legalityConservatismCount, int domainIsolationCount, int speculationBudgetCount, long long totalMissedSlots, int classCapacityDivergenceCount)
		: TotalCounterexamples(totalCounterexamples),
		  DonorRestrictionCount(donorRestrictionCount),
		  FairnessOrderingCount(fairnessOrderingCount),
		  LegalityConservatismCount(legalityConservatismCount),
		  DomainIsolationCount(domainIsolationCount),
		  SpeculationBudgetCount(speculationBudgetCount),
		  TotalMissedSlots(totalMissedSlots),
		  ClassCapacityDivergenceCount(classCapacityDivergenceCount)
	{
	}

	int CounterexampleEvidenceSummary::getTotalCounterexamples() const
	{
		return TotalCounterexamples;
	}

	int CounterexampleEvidenceSummary::getDonorRestrictionCount() const
	{
		return DonorRestrictionCount;
	}

	int CounterexampleEvidenceSummary::getFairnessOrderingCount() const
	{
		return FairnessOrderingCount;
	}

	int CounterexampleEvidenceSummary::getLegalityConservatismCount() const
	{
		return LegalityConservatismCount;
	}

	int CounterexampleEvidenceSummary::getDomainIsolationCount() const
	{
		return DomainIsolationCount;
	}

	int CounterexampleEvidenceSummary::getSpeculationBudgetCount() const
	{
		return SpeculationBudgetCount;
	}

	long long CounterexampleEvidenceSummary::getTotalMissedSlots() const
	{
		return TotalMissedSlots;
	}

	int CounterexampleEvidenceSummary::getClassCapacityDivergenceCount() const
	{
		return ClassCapacityDivergenceCount;
	}

	OracleGapCategory CounterexampleEvidenceSummary::getDominantCategory() const
	{
		if (getTotalCounterexamples() == 0)
		{
			return OracleGapCategory::None;
		}

		int max = getDonorRestrictionCount();
		OracleGapCategory cat = OracleGapCategory::DonorRestriction;

		if (getFairnessOrderingCount() > max)
		{
			max = getFairnessOrderingCount();
			cat = OracleGapCategory::FairnessOrdering;
		}
		if (getLegalityConservatismCount() > max)
		{
			max = getLegalityConservatismCount();
			cat = OracleGapCategory::LegalityConservatism;
		}
		if (getDomainIsolationCount() > max)
		{
			max = getDomainIsolationCount();
			cat = OracleGapCategory::DomainIsolation;
		}
		if (getSpeculationBudgetCount() > max)
		{
			max = getSpeculationBudgetCount();
			cat = OracleGapCategory::SpeculationBudget;
		}
		if (getClassCapacityDivergenceCount() > max)
		{
			cat = OracleGapCategory::ClassCapacityDivergence;
		}

		return cat;
	}

	std::wstring CounterexampleEvidenceSummary::Describe() const
	{
		if (getTotalCounterexamples() == 0)
		{
			return L"No counterexamples captured.";
		}

		const wchar_t *dominantCategory = L"None";
		switch (getDominantCategory())
		{
		case OracleGapCategory::DonorRestriction:
			dominantCategory = L"DonorRestriction";
			break;
		case OracleGapCategory::FairnessOrdering:
			dominantCategory = L"FairnessOrdering";
			break;
		case OracleGapCategory::LegalityConservatism:
			dominantCategory = L"LegalityConservatism";
			break;
		case OracleGapCategory::DomainIsolation:
			dominantCategory = L"DomainIsolation";
			break;
		case OracleGapCategory::SpeculationBudget:
			dominantCategory = L"SpeculationBudget";
			break;
		case OracleGapCategory::ClassCapacityDivergence:
			dominantCategory = L"ClassCapacityDivergence";
			break;
		case OracleGapCategory::None:
		default:
			dominantCategory = L"None";
			break;
		}

		return std::format(L"{} counterexample(s), {} total missed slot(s). Dominant: {}. Breakdown: donor={}, fairness={}, legality={}, domain={}, specBudget={}, classCapacity={}.",
			getTotalCounterexamples(), getTotalMissedSlots(), dominantCategory,
			getDonorRestrictionCount(), getFairnessOrderingCount(),
			getLegalityConservatismCount(), getDomainIsolationCount(),
			getSpeculationBudgetCount(), getClassCapacityDivergenceCount());
	}
}
