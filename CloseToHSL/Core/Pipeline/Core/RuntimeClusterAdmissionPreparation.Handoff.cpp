#include "RuntimeClusterAdmissionPreparation.Handoff.h"
#include "RuntimeClusterAdmissionPreparation.DecisionKind.h"
#include <utility>
namespace YAKSys_Hybrid_CPU::Core
{

	RuntimeClusterAdmissionHandoff::RuntimeClusterAdmissionHandoff(unsigned long long pc, YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode, std::optional<DecodedBundleDependencySummary> dependencySummary, DecodedBundleAdmissionPrep admissionPrep, std::shared_ptr<ClusterIssuePreparation> clusterPreparation, BundleIssuePacket issuePacket, RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionDraft decisionDraft, bool isHandoffReady, bool canSeedLegalityIntegration, bool canSeedClusterIntegration, bool retainsReferenceSequentialPath)
		: PC(pc), DecodeMode(decodeMode), DependencySummary(std::move(dependencySummary)), AdmissionPrep(admissionPrep), ClusterPreparation(std::move(clusterPreparation)),
		  IssuePacket(std::move(issuePacket)), CandidateView(std::move(candidateView)), DecisionDraft(std::move(decisionDraft)), IsHandoffReady(isHandoffReady),
		  CanSeedLegalityIntegration(canSeedLegalityIntegration), CanSeedClusterIntegration(canSeedClusterIntegration), RetainsReferenceSequentialPath(retainsReferenceSequentialPath)
	{
	}

	RuntimeClusterAdmissionHandoff::RuntimeClusterAdmissionHandoff()
		: RuntimeClusterAdmissionHandoff(0, DecodeMode::ReferenceSequentialMode, std::nullopt, DecodedBundleAdmissionPrep(),
			ClusterIssuePreparation::CreateEmpty(0), BundleIssuePacket::CreateEmpty(0), RuntimeClusterAdmissionCandidateView::CreateEmpty(0),
			RuntimeClusterAdmissionDecisionDraft::CreateEmpty(0), false, false, false, true)
	{
	}

	unsigned long long RuntimeClusterAdmissionHandoff::getPC() const
	{
		return PC;
	}

	YAKSys_Hybrid_CPU::Core::DecodeMode RuntimeClusterAdmissionHandoff::getDecodeMode() const
	{
		return DecodeMode;
	}

	std::optional<DecodedBundleDependencySummary> RuntimeClusterAdmissionHandoff::getDependencySummary() const
	{
		return DependencySummary;
	}

	DecodedBundleAdmissionPrep RuntimeClusterAdmissionHandoff::getAdmissionPrep() const
	{
		return AdmissionPrep;
	}

	std::shared_ptr<ClusterIssuePreparation> RuntimeClusterAdmissionHandoff::getClusterPreparation() const
	{
		return ClusterPreparation;
	}

	BundleIssuePacket RuntimeClusterAdmissionHandoff::getIssuePacket() const
	{
		return IssuePacket;
	}

	RuntimeClusterAdmissionCandidateView RuntimeClusterAdmissionHandoff::getCandidateView() const
	{
		return CandidateView;
	}

	RuntimeClusterAdmissionDecisionDraft RuntimeClusterAdmissionHandoff::getDecisionDraft() const
	{
		return DecisionDraft;
	}

	bool RuntimeClusterAdmissionHandoff::isHandoffReady() const
	{
		return IsHandoffReady;
	}

	bool RuntimeClusterAdmissionHandoff::getCanSeedLegalityIntegration() const
	{
		return CanSeedLegalityIntegration;
	}

	bool RuntimeClusterAdmissionHandoff::getCanSeedClusterIntegration() const
	{
		return CanSeedClusterIntegration;
	}

	bool RuntimeClusterAdmissionHandoff::getRetainsReferenceSequentialPath() const
	{
		return RetainsReferenceSequentialPath;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static RuntimeClusterAdmissionHandoff Create(ulong pc, IReadOnlyList<DecodedBundleSlotDescriptor> slots, ClusterIssuePreparation clusterPreparation, RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionDraft decisionDraft)
	RuntimeClusterAdmissionHandoff RuntimeClusterAdmissionHandoff::Create(unsigned long long pc, const std::vector<DecodedBundleSlotDescriptor> &slots, std::shared_ptr<ClusterIssuePreparation> clusterPreparation, RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionDraft decisionDraft)
	{
		ArgumentNullException::ThrowIfNull(slots);
		ArgumentNullException::ThrowIfNull(clusterPreparation);

		std::optional<DecodedBundleDependencySummary> dependencySummary = clusterPreparation->getDependencySummary();
		BundleIssuePacket issuePacket = BundleIssuePacket::Create(clusterPreparation, slots, candidateView, decisionDraft);
		bool isHandoffReady = candidateView.getValidNonEmptyMask() != 0;

		return RuntimeClusterAdmissionHandoff(pc, candidateView.getDecodeMode(), dependencySummary, clusterPreparation->getAdmissionPrep(), clusterPreparation, issuePacket, candidateView, decisionDraft, isHandoffReady, isHandoffReady && dependencySummary, isHandoffReady && decisionDraft.getDecisionKind() != RuntimeClusterAdmissionDecisionKind::Empty, decisionDraft.getRetainsReferenceSequentialPath());
	}

	RuntimeClusterAdmissionHandoff RuntimeClusterAdmissionHandoff::CreateEmpty(unsigned long long pc)
	{
		RuntimeClusterAdmissionCandidateView candidateView = RuntimeClusterAdmissionCandidateView::CreateEmpty(pc);
		RuntimeClusterAdmissionDecisionDraft decisionDraft = RuntimeClusterAdmissionDecisionDraft::CreateEmpty(pc);

		return RuntimeClusterAdmissionHandoff(pc, DecodeMode::ReferenceSequentialMode, std::nullopt, YAKSys_Hybrid_CPU::Core::DecodedBundleAdmissionPrep(), ClusterIssuePreparation::CreateEmpty(pc), BundleIssuePacket::CreateEmpty(pc), candidateView, decisionDraft, false, false, false, decisionDraft.getRetainsReferenceSequentialPath());
	}
}
