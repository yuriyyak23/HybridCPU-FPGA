#include "YAKSys_Hybrid_CPU.Core.HostOwnedEvidenceBoundary.h"
#include "HostOwnedEvidenceBoundary.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<HostOwnedEvidenceBoundaryResult> HostOwnedEvidenceBoundary::ValidateGuestProjection(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, std::shared_ptr<ObservabilityDescriptor> observability, EvidenceVisibilityClass evidenceClass)
	{
		if (!evidencePolicy->CanExposeToGuest(evidenceClass))
		{
			return HostOwnedEvidenceBoundaryResult::Denied(HostOwnedEvidenceBoundaryDecision::GuestProjectionDenied, L"Evidence policy does not permit guest projection for this evidence class.");
		}

		if (!observability->CanPublishToGuest(evidenceClass))
		{
			return HostOwnedEvidenceBoundaryResult::Denied(HostOwnedEvidenceBoundaryDecision::GuestProjectionDenied, L"Observability policy does not permit guest publication for this evidence class.");
		}

		return HostOwnedEvidenceBoundaryResult::Allowed;
	}

	std::shared_ptr<HostOwnedEvidenceBoundaryResult> HostOwnedEvidenceBoundary::ValidateHostLocalCapture(std::shared_ptr<ObservabilityDescriptor> observability, EvidenceVisibilityClass evidenceClass)
	{
		if (!observability->CanCaptureHostLocal(evidenceClass))
		{
			return HostOwnedEvidenceBoundaryResult::Denied(HostOwnedEvidenceBoundaryDecision::HostCaptureDenied, L"Observability policy does not permit host-local capture for this evidence class.");
		}

		return HostOwnedEvidenceBoundaryResult::Allowed;
	}

	std::shared_ptr<HostOwnedEvidenceBoundaryResult> HostOwnedEvidenceBoundary::ValidateRestore(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, EvidenceVisibilityClass evidenceClass, EvidenceRestorePolicy restorePolicy)
	{
		if (evidencePolicy->MustRecomputeAfterRestore(evidenceClass))
		{
			return HostOwnedEvidenceBoundaryResult::Denied(HostOwnedEvidenceBoundaryDecision::RestoreRequiresRecompute, L"Host-owned evidence must be recomputed after restore.");
		}

		if (!evidencePolicy->CanSerializeAcrossMigration(evidenceClass, restorePolicy))
		{
			return HostOwnedEvidenceBoundaryResult::Denied(HostOwnedEvidenceBoundaryDecision::RestoreDenied, L"Evidence policy does not permit migration restore for this evidence class.");
		}

		return HostOwnedEvidenceBoundaryResult::Allowed;
	}
}
