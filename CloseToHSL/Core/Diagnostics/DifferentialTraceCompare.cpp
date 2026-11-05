#include "DifferentialTraceCompare.h"

namespace YAKSys_Hybrid_CPU::Core
{

	DifferentialTraceDiscrepancy::DifferentialTraceDiscrepancy(int entryIndex, unsigned long long pc, const std::wstring &fieldName, const std::wstring &legacyValue, const std::wstring &clusterPreparedValue)
		: EntryIndex(entryIndex),
		  PC(pc),
		  FieldName(fieldName),
		  LegacyValue(legacyValue),
		  ClusterPreparedValue(clusterPreparedValue)
	{
	}

	int DifferentialTraceDiscrepancy::getEntryIndex() const
	{
		return EntryIndex;
	}

	unsigned long long DifferentialTraceDiscrepancy::getPC() const
	{
		return PC;
	}

	std::wstring DifferentialTraceDiscrepancy::getFieldName() const
	{
		return FieldName;
	}

	std::wstring DifferentialTraceDiscrepancy::getLegacyValue() const
	{
		return LegacyValue;
	}

	std::wstring DifferentialTraceDiscrepancy::getClusterPreparedValue() const
	{
		return ClusterPreparedValue;
	}

	std::wstring DifferentialTraceDiscrepancy::ToString() const
	{
		return std::format(L"[{0}] PC=0x{1:X}: {2} reference={3} cluster={4}", getEntryIndex(), getPC(), getFieldName(), getLegacyValue(), getClusterPreparedValue());
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public DifferentialTraceCompareResult(IReadOnlyList<DifferentialTraceDiscrepancy> discrepancies, int comparedEntryCount, int legacyEntryCount, int clusterPreparedEntryCount)
	DifferentialTraceCompareResult::DifferentialTraceCompareResult(const std::vector<DifferentialTraceDiscrepancy> &discrepancies, int comparedEntryCount, int legacyEntryCount, int clusterPreparedEntryCount)
		: Discrepancies(discrepancies),
		  ComparedEntryCount(comparedEntryCount),
		  LegacyEntryCount(legacyEntryCount),
		  ClusterPreparedEntryCount(clusterPreparedEntryCount)
	{
	}

	const std::vector<DifferentialTraceDiscrepancy> DifferentialTraceCompareResult::getDiscrepancies() const
	{
		return Discrepancies;
	}

	int DifferentialTraceCompareResult::getComparedEntryCount() const
	{
		return ComparedEntryCount;
	}

	int DifferentialTraceCompareResult::getLegacyEntryCount() const
	{
		return LegacyEntryCount;
	}

	int DifferentialTraceCompareResult::getClusterPreparedEntryCount() const
	{
		return ClusterPreparedEntryCount;
	}

	bool DifferentialTraceCompareResult::getAreEquivalent() const
	{
		return getDiscrepancies().empty() && getLegacyEntryCount() == getClusterPreparedEntryCount();
	}

	bool DifferentialTraceCompareResult::getHasLengthMismatch() const
	{
		return getLegacyEntryCount() != getClusterPreparedEntryCount();
	}

	std::shared_ptr<DifferentialTraceCompareResult> DifferentialTraceCompare::Compare(std::shared_ptr<DifferentialTraceCapture> legacyTrace, std::shared_ptr<DifferentialTraceCapture> clusterPreparedTrace)
	{
		ArgumentNullException::ThrowIfNull(legacyTrace);
		ArgumentNullException::ThrowIfNull(clusterPreparedTrace);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<DifferentialTraceEntry> legacyEntries = legacyTrace.GetEntries();
		const std::vector<DifferentialTraceEntry> legacyEntries = legacyTrace->GetEntries();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<DifferentialTraceEntry> clusterEntries = clusterPreparedTrace.GetEntries();
		const std::vector<DifferentialTraceEntry> clusterEntries = clusterPreparedTrace->GetEntries();

		int compareCount = std::min(legacyEntries.size(), clusterEntries.size());
		std::vector<DifferentialTraceDiscrepancy> discrepancies = {};

		for (int i = 0; i < compareCount; i++)
		{
			CompareEntry(i, legacyEntries[i], clusterEntries[i], discrepancies);
		}

		return std::make_shared<DifferentialTraceCompareResult>(discrepancies, compareCount, legacyEntries.size(), clusterEntries.size());
	}

	void DifferentialTraceCompare::CompareEntry(int index, DifferentialTraceEntry legacy, DifferentialTraceEntry cluster, std::vector<DifferentialTraceDiscrepancy> &discrepancies)
	{
		unsigned long long pc = legacy.getPC();

		CheckField(index, pc, L"PC", legacy.getPC(), cluster.getPC(), discrepancies);
		CheckField(index, pc, L"PreparedScalarMask", legacy.getPreparedScalarMask(), cluster.getPreparedScalarMask(), discrepancies);
		CheckField(index, pc, L"RefinedPreparedScalarMask", legacy.getRefinedPreparedScalarMask(), cluster.getRefinedPreparedScalarMask(), discrepancies);
		CheckField(index, pc, L"AdvisoryScalarIssueWidth", legacy.getAdvisoryScalarIssueWidth(), cluster.getAdvisoryScalarIssueWidth(), discrepancies);
		CheckField(index, pc, L"RefinedAdvisoryScalarIssueWidth", legacy.getRefinedAdvisoryScalarIssueWidth(), cluster.getRefinedAdvisoryScalarIssueWidth(), discrepancies);
		CheckField(index, pc, L"ShouldProbeClusterPath", legacy.getShouldProbeClusterPath(), cluster.getShouldProbeClusterPath(), discrepancies);
		CheckField(index, pc, L"SuggestsFallbackDiagnostics", legacy.getSuggestsFallbackDiagnostics(), cluster.getSuggestsFallbackDiagnostics(), discrepancies);
		CheckField(index, pc, L"RetainsReferenceSequentialPath", legacy.getRetainsReferenceSequentialPath(), cluster.getRetainsReferenceSequentialPath(), discrepancies);
		CheckField(index, pc, L"ScalarCandidateMask", legacy.getScalarCandidateMask(), cluster.getScalarCandidateMask(), discrepancies);
		CheckField(index, pc, L"BlockedScalarCandidateMask", legacy.getBlockedScalarCandidateMask(), cluster.getBlockedScalarCandidateMask(), discrepancies);
		CheckField(index, pc, L"RegisterHazardMask", legacy.getRegisterHazardMask(), cluster.getRegisterHazardMask(), discrepancies);
		CheckField(index, pc, L"OrderingHazardMask", legacy.getOrderingHazardMask(), cluster.getOrderingHazardMask(), discrepancies);
	}
}
