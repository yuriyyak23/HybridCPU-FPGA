#include "YAKSys_Hybrid_CPU.Core.RetireEvidenceBoundary.h"
#include "RetireEvidenceBoundary.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<RetireEvidenceResult> RetireEvidenceBoundary::ValidatePublication(std::shared_ptr<RetireEvidencePublicationRequest> request, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, std::shared_ptr<ObservabilityDescriptor> observability)
	{
		if (!request || !request->RetireEffect.isValid())
		{
			return RetireEvidenceResult::Denied(RetireEvidenceDecision::InvalidRetireEffect, L"Retire evidence publication requires a valid retire effect.");
		}

		return request->GuestVisiblePublication ? ValidateGuestPublication(request->EvidenceClass, evidencePolicy, observability) : ValidateHostLocalCapture(request->EvidenceClass, observability);
	}

	std::shared_ptr<RetireEvidenceResult> RetireEvidenceBoundary::ValidateGuestPublication(EvidenceVisibilityClass evidenceClass, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, std::shared_ptr<ObservabilityDescriptor> observability)
	{
		if (!evidencePolicy || !observability)
		{
			return RetireEvidenceResult::Denied(RetireEvidenceDecision::GuestPublicationDenied,
				L"Guest-visible retire publication requires evidence and observability policies.");
		}

		if (evidencePolicy->MustRecomputeAfterRestore(evidenceClass))
		{
			return RetireEvidenceResult::Denied(RetireEvidenceDecision::HostOwnedEvidenceDenied, L"Host-owned retire evidence cannot be published as guest-visible state.");
		}

		if (!evidencePolicy->CanExposeToGuest(evidenceClass))
		{
			return RetireEvidenceResult::Denied(RetireEvidenceDecision::GuestPublicationDenied, L"Evidence policy does not permit guest-visible retire publication.");
		}

		if (!observability->CanPublishToGuest(evidenceClass))
		{
			return RetireEvidenceResult::Denied(RetireEvidenceDecision::GuestPublicationDenied, L"Observability policy does not permit guest-visible retire publication.");
		}

		return RetireEvidenceResult::Allowed;
	}

	std::shared_ptr<RetireEvidenceResult> RetireEvidenceBoundary::ValidateHostLocalCapture(EvidenceVisibilityClass evidenceClass, std::shared_ptr<ObservabilityDescriptor> observability)
	{
		if (!observability)
		{
			return RetireEvidenceResult::Denied(RetireEvidenceDecision::HostCaptureDenied,
				L"Host-local retire capture requires an observability policy.");
		}

		if (!observability->CanCaptureHostLocal(evidenceClass))
		{
			return RetireEvidenceResult::Denied(RetireEvidenceDecision::HostCaptureDenied, L"Observability policy does not permit host-local retire capture.");
		}

		return RetireEvidenceResult::Allowed;
	}
}
