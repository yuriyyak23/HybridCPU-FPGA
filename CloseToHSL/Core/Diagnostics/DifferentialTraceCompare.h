#pragma once

#include "DifferentialTraceCapture.h"
#include <string>
#include <vector>
#include <cmath>
#include <format>
#include <concepts>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Immutable discrepancy record from differential trace comparison.
	/// Identifies a specific field mismatch at a specific trace entry index
	/// with slot-level precision.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct DifferentialTraceDiscrepancy
	class DifferentialTraceDiscrepancy
	{
	private:
		const int EntryIndex;
		const unsigned long long PC;
		const std::wstring FieldName;
		const std::wstring LegacyValue;
		const std::wstring ClusterPreparedValue;

	public:
		DifferentialTraceDiscrepancy(int entryIndex, unsigned long long pc, const std::wstring &fieldName, const std::wstring &legacyValue, const std::wstring &clusterPreparedValue);

		/// <summary>Index of the trace entry where the discrepancy was found.</summary>
		int getEntryIndex() const;

		/// <summary>Program counter of the bundle with the discrepancy.</summary>
		unsigned long long getPC() const;

		/// <summary>Name of the field that differs between the two traces.</summary>
		std::wstring getFieldName() const;

		/// <summary>Value of the field in the reference sequential trace.</summary>
		std::wstring getLegacyValue() const;

		/// <summary>Value of the field in the cluster-prepared trace.</summary>
		std::wstring getClusterPreparedValue() const;

		std::wstring ToString() const;

		DifferentialTraceDiscrepancy() = default;
	};

	/// <summary>
	/// Comparison result from differential trace analysis.
	/// Contains all discrepancies found and summary statistics.
	/// </summary>
	class DifferentialTraceCompareResult final : public std::enable_shared_from_this<DifferentialTraceCompareResult>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<DifferentialTraceDiscrepancy> Discrepancies;
		const std::vector<DifferentialTraceDiscrepancy> Discrepancies;
		const int ComparedEntryCount;
		const int LegacyEntryCount;
		const int ClusterPreparedEntryCount;

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public DifferentialTraceCompareResult(IReadOnlyList<DifferentialTraceDiscrepancy> discrepancies, int comparedEntryCount, int legacyEntryCount, int clusterPreparedEntryCount)
		DifferentialTraceCompareResult(const std::vector<DifferentialTraceDiscrepancy> &discrepancies, int comparedEntryCount, int legacyEntryCount, int clusterPreparedEntryCount);

		/// <summary>All field-level discrepancies found during comparison.</summary>
		const std::vector<DifferentialTraceDiscrepancy> getDiscrepancies() const;

		/// <summary>Number of entry pairs actually compared (min of both trace lengths).</summary>
		int getComparedEntryCount() const;

		/// <summary>Total entries in the legacy trace.</summary>
		int getLegacyEntryCount() const;

		/// <summary>Total entries in the cluster-prepared trace.</summary>
		int getClusterPreparedEntryCount() const;

		/// <summary>True when the traces are semantically equivalent (no discrepancies, same length).</summary>
		bool getAreEquivalent() const;

		/// <summary>True when the traces have different lengths (structural divergence).</summary>
		bool getHasLengthMismatch() const;
	};

	/// <summary>
	/// A/B comparison engine for differential trace verification.
	/// Compares two <see cref="DifferentialTraceCapture"/> instances (reference vs cluster-prepared)
	/// and reports discrepancies with slot-level precision.
	/// This is a diagnostic facility that does not modify pipeline state.
	/// </summary>
	class DifferentialTraceCompare final
	{
		/// <summary>
		/// Compare two trace captures and return all discrepancies.
		/// Fields compared: PC, PreparedScalarMask, RefinedPreparedScalarMask,
		/// AdvisoryScalarIssueWidth, RefinedAdvisoryScalarIssueWidth,
		/// ShouldProbeClusterPath, SuggestsFallbackDiagnostics, RetainsReferenceSequentialPath,
		/// ScalarCandidateMask, BlockedScalarCandidateMask, RegisterHazardMask, OrderingHazardMask.
		/// </summary>
		/// <param name="legacyTrace">Trace captured in reference sequential mode.</param>
		/// <param name="clusterPreparedTrace">Trace captured in cluster-prepared mode.</param>
	public:
		static std::shared_ptr<DifferentialTraceCompareResult> Compare(std::shared_ptr<DifferentialTraceCapture> legacyTrace, std::shared_ptr<DifferentialTraceCapture> clusterPreparedTrace);

	private:
		static void CompareEntry(int index, DifferentialTraceEntry legacy, DifferentialTraceEntry cluster, std::vector<DifferentialTraceDiscrepancy> &discrepancies);

        template<typename T>
            requires (std::integral<T> || requires(T value) {
                { value.Equals(value) } -> std::convertible_to<bool>;
                { value.ToString() } -> std::convertible_to<std::wstring>;
            })
        static void CheckField(int index, unsigned long long pc, const std::wstring &fieldName, T legacyValue, T clusterValue, std::vector<DifferentialTraceDiscrepancy> &discrepancies)
        {
            bool equal;
            if constexpr (std::integral<T>) {
                equal = legacyValue == clusterValue;
            } else {
                equal = legacyValue.Equals(clusterValue);
            }
            if (!equal) {
                auto stringify = [](const T &value) -> std::wstring {
                    if constexpr (std::same_as<T, bool>) return value ? L"True" : L"False";
                    else if constexpr (std::integral<T>) return std::to_wstring(value);
                    else return value.ToString();
                };
                discrepancies.emplace_back(index, pc, fieldName, stringify(legacyValue), stringify(clusterValue));
            }
        }
	};
}
