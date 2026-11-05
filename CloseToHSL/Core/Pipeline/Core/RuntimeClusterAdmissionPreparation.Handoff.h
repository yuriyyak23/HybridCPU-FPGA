#pragma once

#include "../../Decoder/ClusterIssuePreparation.h"
#include "../../Decoder/DecodedBundleDescriptor.h"
#include "RuntimeClusterAdmissionPreparation.BundleIssuePacket.h"
#include "RuntimeClusterAdmissionPreparation.CandidateView.h"
#include "RuntimeClusterAdmissionPreparation.DecisionDraft.h"
#include <vector>
#include <optional>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct RuntimeClusterAdmissionHandoff
	class RuntimeClusterAdmissionHandoff
	{
	private:
		unsigned long long PC;
		YAKSys_Hybrid_CPU::Core::DecodeMode DecodeMode;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<DecodedBundleDependencySummary> DependencySummary;
		std::optional<DecodedBundleDependencySummary> DependencySummary;
		DecodedBundleAdmissionPrep AdmissionPrep;
		std::shared_ptr<ClusterIssuePreparation> ClusterPreparation;
		BundleIssuePacket IssuePacket;
		RuntimeClusterAdmissionCandidateView CandidateView;
		RuntimeClusterAdmissionDecisionDraft DecisionDraft;
		bool IsHandoffReady;
		bool CanSeedLegalityIntegration;
		bool CanSeedClusterIntegration;
		bool RetainsReferenceSequentialPath;

	public:
		RuntimeClusterAdmissionHandoff(unsigned long long pc, YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode, std::optional<DecodedBundleDependencySummary> dependencySummary, DecodedBundleAdmissionPrep admissionPrep, std::shared_ptr<ClusterIssuePreparation> clusterPreparation, BundleIssuePacket issuePacket, RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionDraft decisionDraft, bool isHandoffReady, bool canSeedLegalityIntegration, bool canSeedClusterIntegration, bool retainsReferenceSequentialPath);

		unsigned long long getPC() const;
		YAKSys_Hybrid_CPU::Core::DecodeMode getDecodeMode() const;

		/// <summary>
		/// Bundle-wide dependency matrices (RAW/WAW/WAR/control/memory).
		/// Downstream may call <see cref="DecodedBundleDependencySummary.QuerySlotHazards"/>
		/// and <see cref="DecodedBundleDependencySummary.ComputeRefinedWideReadyScalarMask"/>
		/// for fine-grained hazard analysis. Advisory only.
		/// </summary>
		std::optional<DecodedBundleDependencySummary> getDependencySummary() const;

		/// <summary>
		/// Conservative admission prep from Phase 01 decode.
		/// Contains <see cref="DecodedBundleAdmissionPrep.ScalarCandidateMask"/>,
		/// <see cref="DecodedBundleAdmissionPrep.WideReadyScalarMask"/> (conservative),
		/// and <see cref="DecodedBundleAdmissionPrep.SuggestNarrowFallback"/>.
		/// </summary>
		DecodedBundleAdmissionPrep getAdmissionPrep() const;

		/// <summary>
		/// Decoder-side cluster preparation including scalar group (up to 4 entries)
		/// and auxiliary reservations by class.
		/// </summary>
		std::shared_ptr<ClusterIssuePreparation> getClusterPreparation() const;

		/// <summary>
		/// Fixed-shape issue packet for the current bundle-step.
		/// Always carries packetized decode/admission metadata and may also become the
		/// live execution-source lane set when <see cref="RuntimeClusterAdmissionDecisionDraft.UsesIssuePacketAsExecutionSource"/>
		/// is true.
		/// </summary>
		BundleIssuePacket getIssuePacket() const;

		/// <summary>
		/// Runtime advisory candidate view with both conservative and refined scalar masks.
		/// Phase 04 adds <see cref="RuntimeClusterAdmissionCandidateView.RefinedPreparedScalarMask"/>
		/// and <see cref="RuntimeClusterAdmissionCandidateView.RefinedAdvisoryScalarIssueWidth"/>.
		/// </summary>
		RuntimeClusterAdmissionCandidateView getCandidateView() const;

		/// <summary>
		/// Runtime decision draft. <see cref="RuntimeClusterAdmissionDecisionDraft.ShouldProbeClusterPath"/>
		/// flags bundles viable for cluster-path diagnostics, while
		/// <see cref="RuntimeClusterAdmissionDecisionDraft.UsesIssuePacketAsExecutionSource"/> and
		/// <see cref="RuntimeClusterAdmissionDecisionDraft.RetainsReferenceSequentialPath"/> describe how
		/// execute-stage materialization should treat the packet versus the retained reference path.
		/// </summary>
		RuntimeClusterAdmissionDecisionDraft getDecisionDraft() const;

		/// <summary>True when the bundle has non-empty valid slots and handoff metadata is populated.</summary>
		bool isHandoffReady() const;

		/// <summary>
		/// True when the bundle has a dependency summary and legality gates
		/// (QuerySlotHazards, ComputeRefinedWideReadyScalarMask) are available for fine-grained query.
		/// </summary>
		bool getCanSeedLegalityIntegration() const;

		/// <summary>
		/// True when the bundle has a non-empty decision draft and downstream has admission metadata
		/// worth consuming. This does not, by itself, say whether execution stays on the retained
		/// reference path or materializes from the packet; callers must read <see cref="DecisionDraft"/>.
		/// </summary>
		bool getCanSeedClusterIntegration() const;

		/// <summary>
		/// True when the handoff still keeps the reference sequential path available next to any packetized
		/// execution state. This may be false in cluster-prepared execution modes that intentionally
		/// materialize execute lanes from <see cref="IssuePacket"/>.
		/// </summary>
		bool getRetainsReferenceSequentialPath() const;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static RuntimeClusterAdmissionHandoff Create(ulong pc, IReadOnlyList<DecodedBundleSlotDescriptor> slots, ClusterIssuePreparation clusterPreparation, RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionDraft decisionDraft)
		static RuntimeClusterAdmissionHandoff Create(unsigned long long pc, const std::vector<DecodedBundleSlotDescriptor> &slots, std::shared_ptr<ClusterIssuePreparation> clusterPreparation, RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionDraft decisionDraft);

		static RuntimeClusterAdmissionHandoff CreateEmpty(unsigned long long pc = 0);

		RuntimeClusterAdmissionHandoff();
	};
}
