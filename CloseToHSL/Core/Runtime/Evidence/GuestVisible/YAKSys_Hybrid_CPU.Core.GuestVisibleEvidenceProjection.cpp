#include "YAKSys_Hybrid_CPU.Core.GuestVisibleEvidenceProjection.h"

namespace YAKSys_Hybrid_CPU::Core
{

	GuestVisibleEvidenceProjectionDecision GuestVisibleEvidenceProjection::Evaluate(std::shared_ptr<EvidenceSidebandEnvelope> envelope, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, std::shared_ptr<ObservabilityDescriptor> observability)
	{
		if (envelope == nullptr || !envelope->getHasPayload())
		{
			return GuestVisibleEvidenceProjectionDecision::DeniedEmptyPayload;
		}

		if (envelope->isHostOwned() || envelope->getRequiresHostHandling())
		{
			return GuestVisibleEvidenceProjectionDecision::DeniedHostOwnedEvidence;
		}

		if (evidencePolicy == nullptr || !evidencePolicy->CanExposeToGuest(envelope->getVisibilityClass()))
		{
			return GuestVisibleEvidenceProjectionDecision::DeniedByEvidencePolicy;
		}

		if (observability == nullptr || !observability->CanPublishToGuest(envelope->getVisibilityClass()))
		{
			return GuestVisibleEvidenceProjectionDecision::DeniedByObservabilityPolicy;
		}

		return GuestVisibleEvidenceProjectionDecision::Allowed;
	}

	bool GuestVisibleEvidenceProjection::TryProject(std::shared_ptr<EvidenceSidebandEnvelope> envelope, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, std::shared_ptr<ObservabilityDescriptor> observability, std::shared_ptr<GuestVisibleEvidenceRecord> &record, GuestVisibleEvidenceProjectionDecision &decision)
	{
		decision = Evaluate(envelope, evidencePolicy, observability);
		if (decision != GuestVisibleEvidenceProjectionDecision::Allowed)
		{
			record = nullptr;
			return false;
		}

		record = std::make_shared<GuestVisibleEvidenceRecord>(envelope->getSubjectId(), envelope->getSequence(), envelope->getPayloadHash(), envelope->getVisibilityClass());
		return true;
	}
}
