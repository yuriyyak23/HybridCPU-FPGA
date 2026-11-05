#pragma once

#include "../../Decoder/ClusterIssuePreparation.h"
#include "RuntimeClusterAdmissionPreparation.FallbackDiagnosticsSnapshot.h"
#include "../../Decoder/DecodedBundleDescriptor.h"
#include "RuntimeClusterAdmissionPreparation.Preparation.h"
#include <vector>
#include <cmath>
#include <optional>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Internal advisory admission draft derived from the decoder-owned residual slot/admission carrier and
	/// the runtime-readable cluster preparation seam. This is preparation metadata only and must
	/// not alter the reference sequential execution path.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct RuntimeClusterAdmissionCandidateView
	class RuntimeClusterAdmissionCandidateView
	{
	private:
		unsigned long long PC;
		YAKSys_Hybrid_CPU::Core::DecodeMode DecodeMode;
		unsigned char ValidNonEmptyMask;
		unsigned char ScalarCandidateMask;
		unsigned char PreparedScalarMask;
		unsigned char RefinedPreparedScalarMask;
		unsigned char BlockedScalarCandidateMask;
		unsigned char AuxiliaryCandidateMask;
		unsigned char AuxiliaryReservationMask;
		ClusterFallbackDiagnosticsSnapshot FallbackDiagnosticsSnapshot;
		unsigned char RegisterHazardMask;
		unsigned char OrderingHazardMask;
		int AdvisoryScalarIssueWidth;
		int RefinedAdvisoryScalarIssueWidth;
		bool HasDraftCandidate;
		bool ShouldConsiderClusterAdmission;

		static constexpr int BundleWidth = 8;
		static constexpr int ScalarIssueWidthCeiling = 4;

	public:
		RuntimeClusterAdmissionCandidateView()
			: PC{}, DecodeMode{}, ValidNonEmptyMask{}, ScalarCandidateMask{}, PreparedScalarMask{},
			  RefinedPreparedScalarMask{}, BlockedScalarCandidateMask{}, AuxiliaryCandidateMask{},
			  AuxiliaryReservationMask{}, FallbackDiagnosticsSnapshot{}, RegisterHazardMask{},
			  OrderingHazardMask{}, AdvisoryScalarIssueWidth{}, RefinedAdvisoryScalarIssueWidth{},
			  HasDraftCandidate{}, ShouldConsiderClusterAdmission{}
		{
		}

		RuntimeClusterAdmissionCandidateView(unsigned long long pc, YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode, unsigned char validNonEmptyMask, unsigned char scalarCandidateMask, unsigned char preparedScalarMask, unsigned char refinedPreparedScalarMask, unsigned char blockedScalarCandidateMask, unsigned char auxiliaryCandidateMask, unsigned char auxiliaryReservationMask, ClusterFallbackDiagnosticsSnapshot fallbackDiagnosticsSnapshot, unsigned char registerHazardMask, unsigned char orderingHazardMask, int advisoryScalarIssueWidth, int refinedAdvisoryScalarIssueWidth, bool hasDraftCandidate, bool shouldConsiderClusterAdmission);

		unsigned long long getPC() const;
		YAKSys_Hybrid_CPU::Core::DecodeMode getDecodeMode() const;
		unsigned char getValidNonEmptyMask() const;
		unsigned char getScalarCandidateMask() const;

		/// <summary>
		/// Conservative prepared scalar mask from Phase 01 flat blocking.
		/// A slot is blocked if it participates in ANY dependency pair.
		/// Preserved for backward compatibility and differential verification.
		/// </summary>
		unsigned char getPreparedScalarMask() const;

		/// <summary>
		/// Refined prepared scalar mask from Phase 03 hazard-triage-aware readiness.
		/// A slot is blocked only when a <see cref="HazardTriageClass.HardReject"/> peer
		/// exists within the scalar group. This is a structural projection, not a scheduling decision.
		/// Invariant: <c>(RefinedPreparedScalarMask &amp; PreparedScalarMask) == PreparedScalarMask</c>
		/// (refined mask is a superset of conservative mask).
		/// </summary>
		unsigned char getRefinedPreparedScalarMask() const;

		unsigned char getBlockedScalarCandidateMask() const;
		unsigned char getAuxiliaryCandidateMask() const;
		unsigned char getAuxiliaryReservationMask() const;
		ClusterFallbackDiagnosticsSnapshot getFallbackDiagnosticsSnapshot() const;
		unsigned char getRegisterHazardMask() const;
		unsigned char getOrderingHazardMask() const;

		/// <summary>
		/// Advisory scalar issue width based on conservative <see cref="PreparedScalarMask"/>.
		/// Preserved for backward compatibility.
		/// </summary>
		int getAdvisoryScalarIssueWidth() const;

		/// <summary>
		/// Advisory scalar issue width based on refined <see cref="RefinedPreparedScalarMask"/>.
		/// Uses hazard-triage-aware readiness; never exceeds 4-way scalar ceiling.
		/// This is a structural projection for diagnostics, not a scheduling decision.
		/// </summary>
		int getRefinedAdvisoryScalarIssueWidth() const;

		bool getHasDraftCandidate() const;
		bool getShouldConsiderClusterAdmission() const;
		bool getHasClusterCandidate() const;
		bool getSuggestsFallbackDiagnostics() const;
		unsigned char getFallbackDiagnosticsMask() const;
		RuntimeClusterFallbackReasonMask getFallbackReasonMask() const;

		/// <summary>
		/// Stage 7 Phase B: true when conservative PreparedScalarMask is empty but
		/// hazard-triage-aware RefinedPreparedScalarMask has ready candidates.
		/// </summary>
		bool getHasRefinedWideReadyCandidates() const;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static RuntimeClusterAdmissionCandidateView Create(ulong pc, IReadOnlyList<DecodedBundleSlotDescriptor> slots, ClusterIssuePreparation clusterPreparation, RuntimeClusterAdmissionPreparation runtimePreparation)
		static RuntimeClusterAdmissionCandidateView Create(unsigned long long pc, const std::vector<DecodedBundleSlotDescriptor> &slots, std::shared_ptr<ClusterIssuePreparation> clusterPreparation, RuntimeClusterAdmissionPreparation runtimePreparation);

		static RuntimeClusterAdmissionCandidateView CreateEmpty(unsigned long long pc = 0);

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static byte BuildValidNonEmptyMask(IReadOnlyList<DecodedBundleSlotDescriptor> slots)
		static unsigned char BuildValidNonEmptyMask(const std::vector<DecodedBundleSlotDescriptor> &slots);

		static unsigned char AggregateAuxiliaryReservationMask(const std::vector<AuxiliaryClusterReservation> &auxiliaryReservations);

		static unsigned char ExtractParticipatingSlotMask(unsigned long long pairMask);

	};
}
