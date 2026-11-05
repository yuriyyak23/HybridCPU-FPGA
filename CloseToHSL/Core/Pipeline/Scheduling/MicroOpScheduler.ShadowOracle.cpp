#include "MicroOpScheduler.ShadowOracle.h"
#include <sstream>
#include <iomanip>

using namespace YAKSys_Hybrid_CPU::Core::Decoder;

namespace YAKSys_Hybrid_CPU::Core
{

	OracleGapRecord::OracleGapRecord(long long cycle, int oraclePackedSlots, int realPackedSlots, OracleGapCategory primaryCategory)
		: Cycle(cycle), OraclePackedSlots(oraclePackedSlots), RealPackedSlots(realPackedSlots), PrimaryCategory(primaryCategory) {}

	long long OracleGapRecord::getCycle() const
	{
		return Cycle;
	}

	int OracleGapRecord::getOraclePackedSlots() const
	{
		return OraclePackedSlots;
	}

	int OracleGapRecord::getRealPackedSlots() const
	{
		return RealPackedSlots;
	}

	OracleGapCategory OracleGapRecord::getPrimaryCategory() const
	{
		return PrimaryCategory;
	}

	int OracleGapRecord::getGap() const
	{
		return std::max(0, getOraclePackedSlots() - getRealPackedSlots());
	}

	OracleCounterexample::OracleCounterexample(long long cycle, OracleGapCategory category, int missedSlots, const std::wstring &description)
		: Cycle(cycle), Category(category), MissedSlots(missedSlots), Description(description) {}

	long long OracleCounterexample::getCycle() const
	{
		return Cycle;
	}

	OracleGapCategory OracleCounterexample::getCategory() const
	{
		return Category;
	}

	int OracleCounterexample::getMissedSlots() const
	{
		return MissedSlots;
	}

	std::wstring OracleCounterexample::getDescription() const
	{
		return Description;
	}

	std::wstring OracleCounterexample::Describe() const
	{
		std::wostringstream text;
		const wchar_t* categoryName = L"Unknown";
		switch (getCategory())
		{
		case OracleGapCategory::DonorRestriction: categoryName = L"DonorRestriction"; break;
		case OracleGapCategory::FairnessOrdering: categoryName = L"FairnessOrdering"; break;
		case OracleGapCategory::LegalityConservatism: categoryName = L"LegalityConservatism"; break;
		case OracleGapCategory::DomainIsolation: categoryName = L"DomainIsolation"; break;
		case OracleGapCategory::SpeculationBudget: categoryName = L"SpeculationBudget"; break;
		case OracleGapCategory::ClassCapacityDivergence: categoryName = L"ClassCapacityDivergence"; break;
		}
		text << L"Cycle " << getCycle() << L": " << categoryName << L", "
			<< getMissedSlots() << L" missed slot(s) — " << getDescription();
		return text.str();
	}

	OracleGapSummary::OracleGapSummary(long long totalOracleSlots, long long totalRealSlots, long long donorRestrictionGap, long long fairnessOrderingGap, long long legalityConservatismGap, long long domainIsolationGap, long long speculationBudgetGap, long long cyclesWithGap, long long totalCyclesAnalyzed)
		: TotalOracleSlots(totalOracleSlots), TotalRealSlots(totalRealSlots), DonorRestrictionGap(donorRestrictionGap),
		  FairnessOrderingGap(fairnessOrderingGap), LegalityConservatismGap(legalityConservatismGap),
		  DomainIsolationGap(domainIsolationGap), SpeculationBudgetGap(speculationBudgetGap),
		  CyclesWithGap(cyclesWithGap), TotalCyclesAnalyzed(totalCyclesAnalyzed) {}

	long long OracleGapSummary::getTotalOracleSlots() const
	{
		return TotalOracleSlots;
	}

	long long OracleGapSummary::getTotalRealSlots() const
	{
		return TotalRealSlots;
	}

	long long OracleGapSummary::getDonorRestrictionGap() const
	{
		return DonorRestrictionGap;
	}

	long long OracleGapSummary::getFairnessOrderingGap() const
	{
		return FairnessOrderingGap;
	}

	long long OracleGapSummary::getLegalityConservatismGap() const
	{
		return LegalityConservatismGap;
	}

	long long OracleGapSummary::getDomainIsolationGap() const
	{
		return DomainIsolationGap;
	}

	long long OracleGapSummary::getSpeculationBudgetGap() const
	{
		return SpeculationBudgetGap;
	}

	long long OracleGapSummary::getCyclesWithGap() const
	{
		return CyclesWithGap;
	}

	long long OracleGapSummary::getTotalCyclesAnalyzed() const
	{
		return TotalCyclesAnalyzed;
	}

	long long OracleGapSummary::getTotalGap() const
	{
		return getTotalOracleSlots() - getTotalRealSlots();
	}

	double OracleGapSummary::getOracleEfficiency() const
	{
		return getTotalOracleSlots() == 0 ? 1.0 : static_cast<double>(getTotalRealSlots()) / getTotalOracleSlots();
	}

	double OracleGapSummary::getGapRate() const
	{
		return getTotalOracleSlots() == 0 ? 0.0 : static_cast<double>(getTotalGap()) / getTotalOracleSlots();
	}

	std::wstring OracleGapSummary::Describe() const
	{
		std::wostringstream text;
		if (getTotalGap() <= 0)
		{
			text << L"No oracle gap detected across " << getTotalCyclesAnalyzed() << L" cycles.";
			return text.str();
		}

		text << L"Oracle gap: " << getTotalGap() << L" slots (" << std::fixed << std::setprecision(2)
			<< getGapRate() * 100.0 << L"%) across " << getCyclesWithGap() << L"/" << getTotalCyclesAnalyzed()
			<< L" cycles. Donor restriction: " << getDonorRestrictionGap()
			<< L", fairness ordering: " << getFairnessOrderingGap()
			<< L", legality conservatism: " << getLegalityConservatismGap()
			<< L", domain isolation: " << getDomainIsolationGap()
			<< L", speculation budget: " << getSpeculationBudgetGap() << L".";
		return text.str();
	}
}



