#include "RuntimeClusterAdmissionPreparation.CandidateView.h"

#include <bit>

#include "tangible_exception_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{

	RuntimeClusterAdmissionCandidateView::RuntimeClusterAdmissionCandidateView(unsigned long long pc, YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode, unsigned char validNonEmptyMask, unsigned char scalarCandidateMask, unsigned char preparedScalarMask, unsigned char refinedPreparedScalarMask, unsigned char blockedScalarCandidateMask, unsigned char auxiliaryCandidateMask, unsigned char auxiliaryReservationMask, ClusterFallbackDiagnosticsSnapshot fallbackDiagnosticsSnapshot, unsigned char registerHazardMask, unsigned char orderingHazardMask, int advisoryScalarIssueWidth, int refinedAdvisoryScalarIssueWidth, bool hasDraftCandidate, bool shouldConsiderClusterAdmission)
	{
		PC = pc;
		DecodeMode = decodeMode;
		ValidNonEmptyMask = validNonEmptyMask;
		ScalarCandidateMask = scalarCandidateMask;
		PreparedScalarMask = preparedScalarMask;
		RefinedPreparedScalarMask = refinedPreparedScalarMask;
		BlockedScalarCandidateMask = blockedScalarCandidateMask;
		AuxiliaryCandidateMask = auxiliaryCandidateMask;
		AuxiliaryReservationMask = auxiliaryReservationMask;
		FallbackDiagnosticsSnapshot = fallbackDiagnosticsSnapshot;
		RegisterHazardMask = registerHazardMask;
		OrderingHazardMask = orderingHazardMask;
		AdvisoryScalarIssueWidth = advisoryScalarIssueWidth;
		RefinedAdvisoryScalarIssueWidth = refinedAdvisoryScalarIssueWidth;
		HasDraftCandidate = hasDraftCandidate;
		ShouldConsiderClusterAdmission = shouldConsiderClusterAdmission;
	}

	unsigned long long RuntimeClusterAdmissionCandidateView::getPC() const
	{
		return PC;
	}

	YAKSys_Hybrid_CPU::Core::DecodeMode RuntimeClusterAdmissionCandidateView::getDecodeMode() const
	{
		return DecodeMode;
	}

	unsigned char RuntimeClusterAdmissionCandidateView::getValidNonEmptyMask() const
	{
		return ValidNonEmptyMask;
	}

	unsigned char RuntimeClusterAdmissionCandidateView::getScalarCandidateMask() const
	{
		return ScalarCandidateMask;
	}

	unsigned char RuntimeClusterAdmissionCandidateView::getPreparedScalarMask() const
	{
		return PreparedScalarMask;
	}

	unsigned char RuntimeClusterAdmissionCandidateView::getRefinedPreparedScalarMask() const
	{
		return RefinedPreparedScalarMask;
	}

	unsigned char RuntimeClusterAdmissionCandidateView::getBlockedScalarCandidateMask() const
	{
		return BlockedScalarCandidateMask;
	}

	unsigned char RuntimeClusterAdmissionCandidateView::getAuxiliaryCandidateMask() const
	{
		return AuxiliaryCandidateMask;
	}

	unsigned char RuntimeClusterAdmissionCandidateView::getAuxiliaryReservationMask() const
	{
		return AuxiliaryReservationMask;
	}

	ClusterFallbackDiagnosticsSnapshot RuntimeClusterAdmissionCandidateView::getFallbackDiagnosticsSnapshot() const
	{
		return FallbackDiagnosticsSnapshot;
	}

	unsigned char RuntimeClusterAdmissionCandidateView::getRegisterHazardMask() const
	{
		return RegisterHazardMask;
	}

	unsigned char RuntimeClusterAdmissionCandidateView::getOrderingHazardMask() const
	{
		return OrderingHazardMask;
	}

	int RuntimeClusterAdmissionCandidateView::getAdvisoryScalarIssueWidth() const
	{
		return AdvisoryScalarIssueWidth;
	}

	int RuntimeClusterAdmissionCandidateView::getRefinedAdvisoryScalarIssueWidth() const
	{
		return RefinedAdvisoryScalarIssueWidth;
	}

	bool RuntimeClusterAdmissionCandidateView::getHasDraftCandidate() const
	{
		return HasDraftCandidate;
	}

	bool RuntimeClusterAdmissionCandidateView::getShouldConsiderClusterAdmission() const
	{
		return ShouldConsiderClusterAdmission;
	}

	bool RuntimeClusterAdmissionCandidateView::getHasClusterCandidate() const
	{
		return getShouldConsiderClusterAdmission() && getScalarCandidateMask() != 0;
	}

	bool RuntimeClusterAdmissionCandidateView::getSuggestsFallbackDiagnostics() const
	{
		return getFallbackDiagnosticsSnapshot().getSuggestsFallbackDiagnostics();
	}

	unsigned char RuntimeClusterAdmissionCandidateView::getFallbackDiagnosticsMask() const
	{
		return getFallbackDiagnosticsSnapshot().getFallbackDiagnosticsMask();
	}

	RuntimeClusterFallbackReasonMask RuntimeClusterAdmissionCandidateView::getFallbackReasonMask() const
	{
		return getFallbackDiagnosticsSnapshot().getFallbackReasonMask();
	}

	bool RuntimeClusterAdmissionCandidateView::getHasRefinedWideReadyCandidates() const
	{
		return getRefinedPreparedScalarMask() != 0 && getPreparedScalarMask() == 0;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static RuntimeClusterAdmissionCandidateView Create(ulong pc, IReadOnlyList<DecodedBundleSlotDescriptor> slots, ClusterIssuePreparation clusterPreparation, RuntimeClusterAdmissionPreparation runtimePreparation)
	RuntimeClusterAdmissionCandidateView RuntimeClusterAdmissionCandidateView::Create(unsigned long long pc, const std::vector<DecodedBundleSlotDescriptor> &slots, std::shared_ptr<ClusterIssuePreparation> clusterPreparation, RuntimeClusterAdmissionPreparation runtimePreparation)
	{
		ArgumentNullException::ThrowIfNull(slots);
		ArgumentNullException::ThrowIfNull(clusterPreparation);

		unsigned char validNonEmptyMask = BuildValidNonEmptyMask(slots);
		DecodedBundleAdmissionPrep admissionPrep = clusterPreparation->getAdmissionPrep();
		unsigned char auxiliaryReservationMask = AggregateAuxiliaryReservationMask(clusterPreparation->getAuxiliaryReservations());
		unsigned char registerHazardMask = 0;
		unsigned char orderingHazardMask = 0;

		std::optional<DecodedBundleDependencySummary> dependencySummary = clusterPreparation->getDependencySummary();
		if (dependencySummary)
		{
			DecodedBundleDependencySummary summary = dependencySummary.value();
			registerHazardMask = ExtractParticipatingSlotMask(summary.getRawDependencyMask() | summary.getWawDependencyMask() | summary.getWarDependencyMask());
			orderingHazardMask = ExtractParticipatingSlotMask(summary.getControlConflictMask() | summary.getMemoryConflictMask());
		}

		unsigned char preparedScalarMask = runtimePreparation.getPreparedScalarMask();
		unsigned char blockedScalarCandidateMask = static_cast<unsigned char>(admissionPrep.getScalarCandidateMask() & ~preparedScalarMask);
		int advisoryScalarIssueWidth = runtimePreparation.getShouldConsiderClusterAdmission() ? std::min(ScalarIssueWidthCeiling, runtimePreparation.getPreparedScalarCount()) : 0;

		// Phase 04: Compute refined readiness from hazard-triage-aware projection
		unsigned char refinedPreparedScalarMask = preparedScalarMask;
		if (dependencySummary)
		{
			unsigned char scalarEligibleMask = admissionPrep.getScalarCandidateMask();
			refinedPreparedScalarMask = dependencySummary.value().ComputeRefinedWideReadyScalarMask(scalarEligibleMask);
		}

		int refinedScalarCount = std::popcount(static_cast<unsigned int>(refinedPreparedScalarMask));
		int refinedAdvisoryScalarIssueWidth = runtimePreparation.getShouldConsiderClusterAdmission() ? std::min(ScalarIssueWidthCeiling, refinedScalarCount) : 0;

		RuntimeClusterFallbackReasonMask fallbackReasonMask = ClusterFallbackDiagnosticsSnapshot::DeriveReasonMask(runtimePreparation.getFallbackDiagnosticsMask(), blockedScalarCandidateMask, registerHazardMask, orderingHazardMask);

		bool hasDraftCandidate = runtimePreparation.getShouldConsiderClusterAdmission() && admissionPrep.getScalarCandidateMask() != 0;

		return RuntimeClusterAdmissionCandidateView(pc, runtimePreparation.getDecodeMode(), validNonEmptyMask, admissionPrep.getScalarCandidateMask(), preparedScalarMask, refinedPreparedScalarMask, blockedScalarCandidateMask, admissionPrep.getAuxiliaryOpMask(), auxiliaryReservationMask, ClusterFallbackDiagnosticsSnapshot(runtimePreparation.getFallbackDiagnosticsMask(), runtimePreparation.getSuggestsFallbackDiagnostics(), fallbackReasonMask), registerHazardMask, orderingHazardMask, advisoryScalarIssueWidth, refinedAdvisoryScalarIssueWidth, hasDraftCandidate, runtimePreparation.getShouldConsiderClusterAdmission());
	}

	RuntimeClusterAdmissionCandidateView RuntimeClusterAdmissionCandidateView::CreateEmpty(unsigned long long pc)
	{
		return RuntimeClusterAdmissionCandidateView(pc, DecodeMode::ReferenceSequentialMode, 0, 0, 0, 0, 0, 0, 0, ClusterFallbackDiagnosticsSnapshot(0, false), 0, 0, 0, 0, false, false);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static byte BuildValidNonEmptyMask(IReadOnlyList<DecodedBundleSlotDescriptor> slots)
	unsigned char RuntimeClusterAdmissionCandidateView::BuildValidNonEmptyMask(const std::vector<DecodedBundleSlotDescriptor> &slots)
	{
		unsigned char validNonEmptyMask = 0;
		int slotCount = static_cast<int>(std::min<std::size_t>(BundleWidth, slots.size()));

		for (int slotIndex = 0; slotIndex < slotCount; slotIndex++)
		{
			DecodedBundleSlotDescriptor slot = slots[slotIndex];
			if (slot.isValid() && !slot.GetRuntimeExecutionIsEmptyOrNop())
			{
				validNonEmptyMask |= static_cast<unsigned char>(1 << slotIndex);
			}
		}

		return validNonEmptyMask;
	}

	unsigned char RuntimeClusterAdmissionCandidateView::AggregateAuxiliaryReservationMask(const std::vector<AuxiliaryClusterReservation> &auxiliaryReservations)
	{
		unsigned char reservationMask = 0;
		for (int i = 0; i < auxiliaryReservations.size(); i++)
		{
			reservationMask |= auxiliaryReservations[i].getSlotMask();
		}

		return reservationMask;
	}

	unsigned char RuntimeClusterAdmissionCandidateView::ExtractParticipatingSlotMask(unsigned long long pairMask)
	{
		unsigned char slotMask = 0;

		for (int sourceSlotIndex = 0; sourceSlotIndex < BundleWidth; sourceSlotIndex++)
		{
			for (int targetSlotIndex = sourceSlotIndex + 1; targetSlotIndex < BundleWidth; targetSlotIndex++)
			{
				unsigned long long slotPairBit = 1ULL << ((sourceSlotIndex * BundleWidth) + targetSlotIndex);
				if ((pairMask & slotPairBit) == 0)
				{
					continue;
				}

				slotMask |= static_cast<unsigned char>(1 << sourceSlotIndex);
				slotMask |= static_cast<unsigned char>(1 << targetSlotIndex);
			}
		}

		return slotMask;
	}
}
