#pragma once

namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }
#include "DecodedBundleDescriptor.h"
#include <vector>
#include <algorithm>
#include <optional>
#include <memory>
#include <utility>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class VtScalarCandidateSummary; }

namespace YAKSys_Hybrid_CPU::Core
{
	enum class DecodeMode : unsigned char
	{
		ReferenceSequentialMode = 0,
		ClusterPreparedMode = 1
	};

	enum class AuxiliaryClusterKind : unsigned char
	{
		Vector = 0,
		Memory = 1,
		DmaStream = 2,
		Control = 3,
		System = 4,
		Other = 5
	};

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ScalarClusterIssueEntry
	class ScalarClusterIssueEntry
	{
	private:
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> MicroOp;
		const unsigned char SlotIndex;
		const int VirtualThreadId;
		const int OwnerThreadId;
		const unsigned int OpCode;

	public:
		ScalarClusterIssueEntry(DecodedBundleSlotDescriptor slotDescriptor);

		ScalarClusterIssueEntry(unsigned char slotIndex, int virtualThreadId, int ownerThreadId, unsigned int opCode, std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> microOp);

		std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> getMicroOp() const;
		DecodedBundleSlotDescriptor getSlotDescriptor() const;
		unsigned char getSlotIndex() const;
		int getVirtualThreadId() const;
		int getOwnerThreadId() const;
		unsigned int getOpCode() const;

		ScalarClusterIssueEntry() : MicroOp(), SlotIndex(0), VirtualThreadId(0), OwnerThreadId(0), OpCode(0) {}
	};

	class ScalarClusterIssueGroup final : public std::enable_shared_from_this<ScalarClusterIssueGroup>
	{
	private:
		const unsigned char CandidateMask;
		const unsigned char PreparedMask;
		const std::vector<ScalarClusterIssueEntry> Entries;

	public:
		ScalarClusterIssueGroup(unsigned char candidateMask, unsigned char preparedMask, std::vector<ScalarClusterIssueEntry> entries);

		unsigned char getCandidateMask() const;
		unsigned char getPreparedMask() const;
		std::vector<ScalarClusterIssueEntry> getEntries() const;
		int getCount() const;

		/// <summary>
		/// Build per-VT scalar candidate summary from the prepared entries.
		/// VT indices outside [0..3] are clamped to avoid out-of-range access.
		/// </summary>
		VtScalarCandidateSummary BuildVtSummary();
	};

	/// <summary>
	/// Per-VT scalar candidate counts derived from the prepared scalar issue group.
	/// Makes VT-aware ownership explicit in the scalar targeting contract.
	/// VT0..VT3 correspond to the 4-way SMT virtual thread indices.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct VtScalarCandidateSummary
	class VtScalarCandidateSummary
	{
	private:
		const int Vt0Count;
		const int Vt1Count;
		const int Vt2Count;
		const int Vt3Count;

	public:
		VtScalarCandidateSummary(int vt0Count, int vt1Count, int vt2Count, int vt3Count);

		/// <summary>Prepared scalar candidates owned by VT0.</summary>
		int getVt0Count() const;

		/// <summary>Prepared scalar candidates owned by VT1.</summary>
		int getVt1Count() const;

		/// <summary>Prepared scalar candidates owned by VT2.</summary>
		int getVt2Count() const;

		/// <summary>Prepared scalar candidates owned by VT3.</summary>
		int getVt3Count() const;

		/// <summary>Total prepared scalar candidates across all VTs.</summary>
		int getTotalCount() const;

		/// <summary>Number of distinct VTs that contributed at least one prepared scalar candidate.</summary>
		int getActiveVtCount() const;

		/// <summary>
		/// Get the candidate count for a specific VT index (0..3).
		/// Returns 0 for out-of-range indices.
		/// </summary>
		int GetCountForVt(int vtIndex) const;

		VtScalarCandidateSummary() : Vt0Count(0), Vt1Count(0), Vt2Count(0), Vt3Count(0) {}
	};

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AuxiliaryClusterReservation
	class AuxiliaryClusterReservation
	{
	private:
		const AuxiliaryClusterKind Kind;
		const unsigned char SlotMask;

	public:
		AuxiliaryClusterReservation(AuxiliaryClusterKind kind, unsigned char slotMask);

		AuxiliaryClusterKind getKind() const;
		unsigned char getSlotMask() const;
		int getCount() const;

		AuxiliaryClusterReservation() : Kind(AuxiliaryClusterKind::Vector), SlotMask(0) {}
	};

	class ClusterIssuePreparation final : public std::enable_shared_from_this<ClusterIssuePreparation>
	{
	private:
		const unsigned long long PC;
		const YAKSys_Hybrid_CPU::Core::DecodeMode DecodeMode;
		const std::shared_ptr<ScalarClusterIssueGroup> ScalarClusterGroup;
		const std::vector<AuxiliaryClusterReservation> AuxiliaryReservations;
		const unsigned char FallbackDiagnosticsMask;
		const DecodedBundleAdmissionPrep AdmissionPrep;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<DecodedBundleDependencySummary> DependencySummary;
		const std::optional<DecodedBundleDependencySummary> DependencySummary;

		static constexpr int BundleWidth = 8;

		ClusterIssuePreparation(unsigned long long pc, YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode, std::shared_ptr<ScalarClusterIssueGroup> scalarClusterGroup, std::vector<AuxiliaryClusterReservation> auxiliaryReservations, unsigned char fallbackDiagnosticsMask, DecodedBundleAdmissionPrep admissionPrep, std::optional<DecodedBundleDependencySummary> dependencySummary);

	public:
		unsigned long long getPC() const;
		YAKSys_Hybrid_CPU::Core::DecodeMode getDecodeMode() const;
		std::shared_ptr<ScalarClusterIssueGroup> getScalarClusterGroup() const;
		std::vector<AuxiliaryClusterReservation> getAuxiliaryReservations() const;

		/// <summary>
		/// Decode-side residual mask of non-empty slots that were not prepared for cluster issue.
		/// Diagnostics-only contour for the narrow reference path; not a runtime semantic owner for
		/// system/CSR/privileged behavior.
		/// </summary>
		unsigned char getFallbackDiagnosticsMask() const;

		DecodedBundleAdmissionPrep getAdmissionPrep() const;
		std::optional<DecodedBundleDependencySummary> getDependencySummary() const;
		bool getHasScalarClusterCandidate() const;

		/// <summary>
		/// Advisory decode hint that the narrow reference path remains relevant for some slots.
		/// Diagnostics-only and intentionally separate from runtime legality/FSM authority.
		/// </summary>
		bool getSuggestFallbackDiagnostics() const;

		/// <summary>
		/// Create an empty handoff object for cases where no bundle has been decoded yet.
		/// </summary>
		static std::shared_ptr<ClusterIssuePreparation> CreateEmpty(unsigned long long pc = 0);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static ClusterIssuePreparation Create(ulong pc, IReadOnlyList<DecodedBundleSlotDescriptor> slots, DecodedBundleAdmissionPrep admissionPrep, System.Nullable<DecodedBundleDependencySummary> dependencySummary)
		static std::shared_ptr<ClusterIssuePreparation> Create(unsigned long long pc, const std::vector<DecodedBundleSlotDescriptor> &slots, DecodedBundleAdmissionPrep admissionPrep, std::optional<DecodedBundleDependencySummary> &dependencySummary);

		/// <summary>
		/// Create the advisory cluster handoff object from decoder-prepared metadata.
		/// Final acceptance remains on the runtime side.
		/// </summary>
	private:
		static std::shared_ptr<ClusterIssuePreparation> CreateCore(unsigned long long pc, std::shared_ptr<ScalarClusterIssueGroup> scalarClusterGroup, std::vector<AuxiliaryClusterReservation> &auxiliaryReservations, unsigned char fallbackDiagnosticsMask, DecodedBundleAdmissionPrep admissionPrep, std::optional<DecodedBundleDependencySummary> &dependencySummary);

		static unsigned char ResolvePreparedScalarMask(DecodedBundleAdmissionPrep admissionPrep, std::optional<DecodedBundleDependencySummary> &dependencySummary);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static byte BuildFallbackDiagnosticsMask(IReadOnlyList<DecodedBundleSlotDescriptor> slots, byte preparedScalarMask)
		static unsigned char BuildFallbackDiagnosticsMask(const std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char preparedScalarMask);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static AuxiliaryClusterReservation[] BuildAuxiliaryReservations(IReadOnlyList<DecodedBundleSlotDescriptor> slots, byte auxiliaryOpMask)
		static std::vector<AuxiliaryClusterReservation> BuildAuxiliaryReservations(const std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char auxiliaryOpMask);

		static AuxiliaryClusterKind ClassifyAuxiliaryReservation(DecodedBundleSlotDescriptor slot);

		static void AppendReservation(std::vector<AuxiliaryClusterReservation> &reservations, int &count, AuxiliaryClusterKind kind, unsigned char slotMask);
	};
}
