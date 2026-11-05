#include "YAKSys_Hybrid_CPU.Core.NestedEvidencePolicy.h"
#include "NestedEvidencePolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<NestedEvidencePolicyResult> NestedEvidencePolicy::Validate(std::shared_ptr<NestedEvidencePolicyRequest> request)
	{
		if (request == nullptr || request->Descriptor == nullptr)
		{
			return std::make_shared<NestedEvidencePolicyResult>(NestedEvidencePolicyDecision::MissingDescriptor, L"Nested evidence policy requires a nested domain descriptor.");
		}

		if (!request->Descriptor->isRuntimeAuthoritative())
		{
			return std::make_shared<NestedEvidencePolicyResult>(NestedEvidencePolicyDecision::RuntimeAuthorityRequired, L"Nested evidence policy requires runtime-authoritative descriptors.");
		}

		if (!request->Descriptor->getHostEvidenceExcluded())
		{
			return std::make_shared<NestedEvidencePolicyResult>(NestedEvidencePolicyDecision::HostEvidenceGateDenied, L"Nested domain composition must exclude host-owned evidence.");
		}

		if (IsHostOwnedEvidence(request->EvidenceClass))
		{
			return std::make_shared<NestedEvidencePolicyResult>(NestedEvidencePolicyDecision::HostOwnedEvidenceDenied, L"Nested guest projection cannot expose host-owned evidence classes.");
		}

		if (request->RequiresGuestProjection && (request->EvidencePolicy == nullptr || !request->EvidencePolicy->CanExposeToGuest(request->EvidenceClass)))
		{
			return std::make_shared<NestedEvidencePolicyResult>(NestedEvidencePolicyDecision::EvidencePolicyDenied, L"Evidence policy denies nested guest projection for this evidence class.");
		}

		if (request->RequiresGuestProjection && (request->Observability == nullptr || !request->Observability->CanPublishToGuest(request->EvidenceClass)))
		{
			return std::make_shared<NestedEvidencePolicyResult>(NestedEvidencePolicyDecision::ObservabilityPolicyDenied, L"Observability policy denies nested guest publication for this evidence class.");
		}

		return std::make_shared<NestedEvidencePolicyResult>(NestedEvidencePolicyDecision::Allowed, L"Nested evidence policy allowed.");
	}

	bool NestedEvidencePolicy::CanExpose(std::shared_ptr<NestedEvidencePolicyRequest> request)
	{
		return Validate(request)->IsAllowed;
	}

	bool NestedEvidencePolicy::IsHostOwnedEvidence(EvidenceVisibilityClass evidenceClass)
	{
		return (evidenceClass == EvidenceVisibilityClass::HostOwnedRuntimeEvidence || evidenceClass == EvidenceVisibilityClass::SchedulerEvidence || evidenceClass == EvidenceVisibilityClass::BackendBindingEvidence || evidenceClass == EvidenceVisibilityClass::NativeTokenEvidence);
	}
}
