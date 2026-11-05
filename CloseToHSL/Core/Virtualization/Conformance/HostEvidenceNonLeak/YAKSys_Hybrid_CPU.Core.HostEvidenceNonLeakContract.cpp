#include "YAKSys_Hybrid_CPU.Core.HostEvidenceNonLeakContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	bool HostEvidenceNonLeakContract::IsHostOwnedEvidenceClass(EvidenceVisibilityClass visibilityClass)
	{
		return (visibilityClass == EvidenceVisibilityClass::HostOwnedRuntimeEvidence || visibilityClass == EvidenceVisibilityClass::SchedulerEvidence || visibilityClass == EvidenceVisibilityClass::BackendBindingEvidence || visibilityClass == EvidenceVisibilityClass::NativeTokenEvidence);
	}

	HostEvidenceNonLeakViolation HostEvidenceNonLeakContract::ValidateGuestVisibility(EvidenceVisibilityClass visibilityClass, bool isHostOwned, bool isGuestVisible)
	{
		if (!isGuestVisible)
		{
			return HostEvidenceNonLeakViolation::None;
		}

		return isHostOwned || IsHostOwnedEvidenceClass(visibilityClass) ? HostEvidenceNonLeakViolation::GuestVisibleProjectionLeak : HostEvidenceNonLeakViolation::None;
	}

	HostEvidenceNonLeakViolation HostEvidenceNonLeakContract::ValidateEvidenceEnvelope(std::shared_ptr<EvidenceSidebandEnvelope> envelope, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy)
	{
		if (envelope == nullptr)
		{
			return HostEvidenceNonLeakViolation::None;
		}

		return envelope->getRequiresHostHandling() && envelope->CanExposeToGuest(evidencePolicy) ? HostEvidenceNonLeakViolation::EvidenceEnvelopeLeak : HostEvidenceNonLeakViolation::None;
	}

	HostEvidenceNonLeakViolation HostEvidenceNonLeakContract::ValidateCompletionEnvelope(std::shared_ptr<CompletionSidebandEnvelope> envelope, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy)
	{
		if (envelope == nullptr)
		{
			return HostEvidenceNonLeakViolation::None;
		}

		return envelope->getRequiresHostHandling() && envelope->CanExposeToGuest(evidencePolicy) ? HostEvidenceNonLeakViolation::CompletionEnvelopeLeak : HostEvidenceNonLeakViolation::None;
	}

	HostEvidenceNonLeakViolation HostEvidenceNonLeakContract::ValidateDescriptorEnvelope(std::shared_ptr<DescriptorSidebandEnvelope> envelope, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy)
	{
		if (envelope == nullptr)
		{
			return HostEvidenceNonLeakViolation::None;
		}

		return envelope->getRequiresHostHandling() && envelope->CanExposeToGuest(evidencePolicy) ? HostEvidenceNonLeakViolation::DescriptorEnvelopeLeak : HostEvidenceNonLeakViolation::None;
	}
}
