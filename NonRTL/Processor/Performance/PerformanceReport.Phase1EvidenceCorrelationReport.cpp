#include "PerformanceReport.Phase1EvidenceCorrelationReport.h"

namespace YAKSys_Hybrid_CPU
{

	Phase1EvidenceCorrelationReport::Phase1EvidenceCorrelationReport(bool replayEpochsAligned, bool averageEpochLengthAligned, bool stableDonorRatioAligned, bool usefulnessAligned, bool invalidationsAligned, bool eligibilityAligned, bool determinismAligned, int mismatchCount)
		: ReplayEpochsAligned(replayEpochsAligned),
		  AverageEpochLengthAligned(averageEpochLengthAligned),
		  StableDonorRatioAligned(stableDonorRatioAligned),
		  UsefulnessAligned(usefulnessAligned),
		  InvalidationsAligned(invalidationsAligned),
		  EligibilityAligned(eligibilityAligned),
		  DeterminismAligned(determinismAligned),
		  MismatchCount(mismatchCount)
	{
	}

	bool Phase1EvidenceCorrelationReport::getReplayEpochsAligned() const
	{
		return ReplayEpochsAligned;
	}

	bool Phase1EvidenceCorrelationReport::getAverageEpochLengthAligned() const
	{
		return AverageEpochLengthAligned;
	}

	bool Phase1EvidenceCorrelationReport::getStableDonorRatioAligned() const
	{
		return StableDonorRatioAligned;
	}

	bool Phase1EvidenceCorrelationReport::getUsefulnessAligned() const
	{
		return UsefulnessAligned;
	}

	bool Phase1EvidenceCorrelationReport::getInvalidationsAligned() const
	{
		return InvalidationsAligned;
	}

	bool Phase1EvidenceCorrelationReport::getEligibilityAligned() const
	{
		return EligibilityAligned;
	}

	bool Phase1EvidenceCorrelationReport::getDeterminismAligned() const
	{
		return DeterminismAligned;
	}

	int Phase1EvidenceCorrelationReport::getMismatchCount() const
	{
		return MismatchCount;
	}

	bool Phase1EvidenceCorrelationReport::isAligned() const
	{
		return getMismatchCount() == 0;
	}

	std::wstring Phase1EvidenceCorrelationReport::Describe() const
	{
		if (isAligned())
		{
			return L"Aligned: perf-side counters, trace-side evidence, and repeated-run determinism agree.";
		}

		return std::format(L"Mismatch detected across {0} validation dimensions.", getMismatchCount());
	}
}
