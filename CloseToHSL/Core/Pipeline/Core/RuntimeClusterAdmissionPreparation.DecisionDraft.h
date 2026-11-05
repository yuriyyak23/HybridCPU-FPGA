#pragma once

#include "../../Decoder/ClusterIssuePreparation.h"
#include "RuntimeClusterAdmissionPreparation.DecisionKind.h"
#include "RuntimeClusterAdmissionPreparation.ExecutionMode.h"
#include "RuntimeClusterAdmissionPreparation.FallbackDiagnosticsSnapshot.h"
#include "RuntimeClusterAdmissionPreparation.CandidateView.h"
#include "tangible_exception_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Internal runtime decision draft consumed from <see cref="RuntimeClusterAdmissionCandidateView"/>.
	/// This normalizes runtime-readable admission facts into one place and keeps three concerns explicit:
	/// diagnostic cluster-probe metadata, whether execution may materialize from the issue packet,
	/// and whether the reference sequential path is still retained alongside that packet state.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct RuntimeClusterAdmissionDecisionDraft
	class RuntimeClusterAdmissionDecisionDraft
	{
	private:
		unsigned long long PC;
		YAKSys_Hybrid_CPU::Core::DecodeMode DecodeMode;
		RuntimeClusterAdmissionDecisionKind DecisionKind;
		RuntimeClusterAdmissionExecutionMode ExecutionMode;
		unsigned char ScalarIssueMask;
		unsigned char AuxiliaryReservationMask;
		unsigned char BlockedScalarCandidateMask;
		RuntimeClusterFallbackReasonMask FallbackReasonMask;
		int AdvisoryScalarIssueWidth;
		bool ShouldProbeClusterPath;
		bool UsesIssuePacketAsExecutionSource;
		bool RetainsReferenceSequentialPath;

	public:
		RuntimeClusterAdmissionDecisionDraft(unsigned long long pc, YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode, RuntimeClusterAdmissionDecisionKind decisionKind, RuntimeClusterAdmissionExecutionMode executionMode, unsigned char scalarIssueMask, unsigned char auxiliaryReservationMask, unsigned char blockedScalarCandidateMask, RuntimeClusterFallbackReasonMask fallbackReasonMask, int advisoryScalarIssueWidth, bool shouldProbeClusterPath, bool usesIssuePacketAsExecutionSource, bool retainsReferenceSequentialPath);

		unsigned long long getPC() const;
		YAKSys_Hybrid_CPU::Core::DecodeMode getDecodeMode() const;
		RuntimeClusterAdmissionDecisionKind getDecisionKind() const;
		RuntimeClusterAdmissionExecutionMode getExecutionMode() const;
		unsigned char getScalarIssueMask() const;
		unsigned char getAuxiliaryReservationMask() const;
		unsigned char getBlockedScalarCandidateMask() const;
		RuntimeClusterFallbackReasonMask getFallbackReasonMask() const;
		int getAdvisoryScalarIssueWidth() const;

		/// <summary>
		/// Diagnostic flag indicating whether the bundle should still feed cluster-path probe telemetry.
		/// </summary>
		bool getShouldProbeClusterPath() const;

		/// <summary>
		/// True when execute-stage materialization may use the issue packet as the live source for
		/// this decode result.
		/// </summary>
		bool getUsesIssuePacketAsExecutionSource() const;

		/// <summary>
		/// True when the reference sequential path stays available alongside the packetized admission state.
		/// </summary>
		bool getRetainsReferenceSequentialPath() const;

		/// <summary>
		/// Restricts issue-packet execution to decodes whose current slot actually belongs to the
		/// selected scalar or authoritative auxiliary packet. This prevents unrelated deferred
		/// slots from incorrectly triggering later packet work while preserving bundle-level
		/// cluster diagnostics.
		/// </summary>
		RuntimeClusterAdmissionDecisionDraft BindToCurrentSlot(unsigned char slotIndex) const;

		static RuntimeClusterAdmissionDecisionDraft Create(RuntimeClusterAdmissionCandidateView candidateView);

		static RuntimeClusterAdmissionDecisionDraft CreateExecutable(RuntimeClusterAdmissionCandidateView candidateView, bool clusterPreparedModeEnabled);

		static RuntimeClusterAdmissionDecisionDraft CreateEmpty(unsigned long long pc = 0);

	private:
		static RuntimeClusterAdmissionExecutionMode ResolveExecutionMode(RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionKind decisionKind, bool clusterPreparedModeEnabled);

		static RuntimeClusterAdmissionDecisionKind ResolveDecisionKind(RuntimeClusterAdmissionCandidateView candidateView);

		RuntimeClusterAdmissionDecisionDraft() = default;
	};
}
