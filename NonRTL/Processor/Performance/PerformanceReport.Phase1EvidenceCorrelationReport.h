#pragma once

#include <string>
#include <format>

namespace YAKSys_Hybrid_CPU
{
	/// <summary>
	/// Correlates perf-side and trace-side Phase 1 evidence for regression-baseline validation.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct Phase1EvidenceCorrelationReport
	class Phase1EvidenceCorrelationReport
	{
	private:
		const bool ReplayEpochsAligned{};
		const bool AverageEpochLengthAligned{};
		const bool StableDonorRatioAligned{};
		const bool UsefulnessAligned{};
		const bool InvalidationsAligned{};
		const bool EligibilityAligned{};
		const bool DeterminismAligned{};
		const int MismatchCount{};

	public:
		Phase1EvidenceCorrelationReport(bool replayEpochsAligned, bool averageEpochLengthAligned, bool stableDonorRatioAligned, bool usefulnessAligned, bool invalidationsAligned, bool eligibilityAligned, bool determinismAligned, int mismatchCount);

		bool getReplayEpochsAligned() const;

		bool getAverageEpochLengthAligned() const;

		bool getStableDonorRatioAligned() const;

		bool getUsefulnessAligned() const;

		bool getInvalidationsAligned() const;

		bool getEligibilityAligned() const;

		bool getDeterminismAligned() const;

		int getMismatchCount() const;

		bool isAligned() const;

		std::wstring Describe() const;

		Phase1EvidenceCorrelationReport() = default;
	};
}
