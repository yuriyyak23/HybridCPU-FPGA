#include "RuntimeClusterAdmissionPreparation.DecisionDraft.h"

namespace YAKSys_Hybrid_CPU::Core
{

	RuntimeClusterAdmissionDecisionDraft::RuntimeClusterAdmissionDecisionDraft(unsigned long long pc, YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode, RuntimeClusterAdmissionDecisionKind decisionKind, RuntimeClusterAdmissionExecutionMode executionMode, unsigned char scalarIssueMask, unsigned char auxiliaryReservationMask, unsigned char blockedScalarCandidateMask, RuntimeClusterFallbackReasonMask fallbackReasonMask, int advisoryScalarIssueWidth, bool shouldProbeClusterPath, bool usesIssuePacketAsExecutionSource, bool retainsReferenceSequentialPath)
	{
		PC = pc;
		DecodeMode = decodeMode;
		DecisionKind = decisionKind;
		ExecutionMode = executionMode;
		ScalarIssueMask = scalarIssueMask;
		AuxiliaryReservationMask = auxiliaryReservationMask;
		BlockedScalarCandidateMask = blockedScalarCandidateMask;
		FallbackReasonMask = fallbackReasonMask;
		AdvisoryScalarIssueWidth = advisoryScalarIssueWidth;
		ShouldProbeClusterPath = shouldProbeClusterPath;
		UsesIssuePacketAsExecutionSource = usesIssuePacketAsExecutionSource;
		RetainsReferenceSequentialPath = retainsReferenceSequentialPath;
	}

	unsigned long long RuntimeClusterAdmissionDecisionDraft::getPC() const
	{
		return PC;
	}

	YAKSys_Hybrid_CPU::Core::DecodeMode RuntimeClusterAdmissionDecisionDraft::getDecodeMode() const
	{
		return DecodeMode;
	}

	RuntimeClusterAdmissionDecisionKind RuntimeClusterAdmissionDecisionDraft::getDecisionKind() const
	{
		return DecisionKind;
	}

	RuntimeClusterAdmissionExecutionMode RuntimeClusterAdmissionDecisionDraft::getExecutionMode() const
	{
		return ExecutionMode;
	}

	unsigned char RuntimeClusterAdmissionDecisionDraft::getScalarIssueMask() const
	{
		return ScalarIssueMask;
	}

	unsigned char RuntimeClusterAdmissionDecisionDraft::getAuxiliaryReservationMask() const
	{
		return AuxiliaryReservationMask;
	}

	unsigned char RuntimeClusterAdmissionDecisionDraft::getBlockedScalarCandidateMask() const
	{
		return BlockedScalarCandidateMask;
	}

	RuntimeClusterFallbackReasonMask RuntimeClusterAdmissionDecisionDraft::getFallbackReasonMask() const
	{
		return FallbackReasonMask;
	}

	int RuntimeClusterAdmissionDecisionDraft::getAdvisoryScalarIssueWidth() const
	{
		return AdvisoryScalarIssueWidth;
	}

	bool RuntimeClusterAdmissionDecisionDraft::getShouldProbeClusterPath() const
	{
		return ShouldProbeClusterPath;
	}

	bool RuntimeClusterAdmissionDecisionDraft::getUsesIssuePacketAsExecutionSource() const
	{
		return UsesIssuePacketAsExecutionSource;
	}

	bool RuntimeClusterAdmissionDecisionDraft::getRetainsReferenceSequentialPath() const
	{
		return RetainsReferenceSequentialPath;
	}

	RuntimeClusterAdmissionDecisionDraft RuntimeClusterAdmissionDecisionDraft::BindToCurrentSlot(unsigned char slotIndex) const
	{
		if (!getUsesIssuePacketAsExecutionSource() || slotIndex >= 8)
		{
			return *this;
		}

		unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
		if (((getScalarIssueMask() | getAuxiliaryReservationMask()) & slotBit) != 0)
		{
			return *this;
		}

		return RuntimeClusterAdmissionDecisionDraft(getPC(), getDecodeMode(), getDecisionKind(), getExecutionMode(), getScalarIssueMask(), getAuxiliaryReservationMask(), getBlockedScalarCandidateMask(), getFallbackReasonMask(), getAdvisoryScalarIssueWidth(), getShouldProbeClusterPath(), false, getRetainsReferenceSequentialPath());
	}

	RuntimeClusterAdmissionDecisionDraft RuntimeClusterAdmissionDecisionDraft::Create(RuntimeClusterAdmissionCandidateView candidateView)
	{
		return CreateExecutable(candidateView, false);
	}

	RuntimeClusterAdmissionDecisionDraft RuntimeClusterAdmissionDecisionDraft::CreateExecutable(RuntimeClusterAdmissionCandidateView candidateView, bool clusterPreparedModeEnabled)
	{
		RuntimeClusterAdmissionDecisionKind decisionKind = ResolveDecisionKind(candidateView);
		RuntimeClusterAdmissionExecutionMode executionMode = ResolveExecutionMode(candidateView, decisionKind, clusterPreparedModeEnabled);
		bool usesIssuePacketAsExecutionSource = clusterPreparedModeEnabled && (executionMode == RuntimeClusterAdmissionExecutionMode::ClusterPrepared || executionMode == RuntimeClusterAdmissionExecutionMode::ClusterPreparedRefined || candidateView.getAuxiliaryReservationMask() != 0);
		RuntimeClusterFallbackReasonMask fallbackReasonMask = candidateView.getFallbackReasonMask();

		if (executionMode == RuntimeClusterAdmissionExecutionMode::ReferenceSequentialFallback && fallbackReasonMask == RuntimeClusterFallbackReasonMask::None)
		{
			throw InvalidOperationException(L"ReferenceSequentialFallback requires a non-zero fallback reason mask derived from runtime diagnostics.");
		}

		// Stage 7 Phase B: when refined path active, issue from RefinedPreparedScalarMask
		unsigned char scalarIssueMask = executionMode == RuntimeClusterAdmissionExecutionMode::ClusterPreparedRefined ? candidateView.getRefinedPreparedScalarMask() : candidateView.getPreparedScalarMask();

		return RuntimeClusterAdmissionDecisionDraft(candidateView.getPC(), candidateView.getDecodeMode(), decisionKind, executionMode, scalarIssueMask, candidateView.getAuxiliaryReservationMask(), candidateView.getBlockedScalarCandidateMask(), fallbackReasonMask, candidateView.getAdvisoryScalarIssueWidth(), decisionKind == RuntimeClusterAdmissionDecisionKind::AdvisoryClusterCandidate, usesIssuePacketAsExecutionSource, !clusterPreparedModeEnabled);
	}

	RuntimeClusterAdmissionDecisionDraft RuntimeClusterAdmissionDecisionDraft::CreateEmpty(unsigned long long pc)
	{
		return RuntimeClusterAdmissionDecisionDraft(pc, DecodeMode::ReferenceSequentialMode, RuntimeClusterAdmissionDecisionKind::Empty, RuntimeClusterAdmissionExecutionMode::Empty, 0, 0, 0, RuntimeClusterFallbackReasonMask::None, 0, false, false, true);
	}

	RuntimeClusterAdmissionExecutionMode RuntimeClusterAdmissionDecisionDraft::ResolveExecutionMode(RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionKind decisionKind, bool clusterPreparedModeEnabled)
	{
		if (decisionKind == RuntimeClusterAdmissionDecisionKind::Empty)
		{
			return RuntimeClusterAdmissionExecutionMode::Empty;
		}

		if (decisionKind == RuntimeClusterAdmissionDecisionKind::AdvisoryAuxiliaryOnly)
		{
			return RuntimeClusterAdmissionExecutionMode::AuxiliaryOnlyReference;
		}

		if (!clusterPreparedModeEnabled)
		{
			return RuntimeClusterAdmissionExecutionMode::ReferenceSequential;
		}

		if (decisionKind != RuntimeClusterAdmissionDecisionKind::AdvisoryClusterCandidate)
		{
			return RuntimeClusterAdmissionExecutionMode::ReferenceSequential;
		}

		if (candidateView.getPreparedScalarMask() == 0)
		{
			// Stage 7 Phase B: refined-mask recovery — when conservative mask is empty
			// but hazard-triage-aware refined mask has ready candidates, promote to refined path.
			if (candidateView.getRefinedPreparedScalarMask() != 0)
			{
				return RuntimeClusterAdmissionExecutionMode::ClusterPreparedRefined;
			}

			return RuntimeClusterAdmissionExecutionMode::ReferenceSequentialFallback;
		}

		return RuntimeClusterAdmissionExecutionMode::ClusterPrepared;
	}

	RuntimeClusterAdmissionDecisionKind RuntimeClusterAdmissionDecisionDraft::ResolveDecisionKind(RuntimeClusterAdmissionCandidateView candidateView)
	{
		if (candidateView.getValidNonEmptyMask() == 0)
		{
			return RuntimeClusterAdmissionDecisionKind::Empty;
		}

		if (candidateView.getHasDraftCandidate() || candidateView.getHasClusterCandidate())
		{
			return RuntimeClusterAdmissionDecisionKind::AdvisoryClusterCandidate;
		}

		if (candidateView.getScalarCandidateMask() == 0 && candidateView.getAuxiliaryCandidateMask() != 0)
		{
			return RuntimeClusterAdmissionDecisionKind::AdvisoryAuxiliaryOnly;
		}

		return RuntimeClusterAdmissionDecisionKind::AdvisoryReferenceSequential;
	}
}
