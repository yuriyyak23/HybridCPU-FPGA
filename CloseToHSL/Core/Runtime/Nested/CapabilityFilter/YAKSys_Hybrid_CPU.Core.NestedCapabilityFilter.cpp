#include "YAKSys_Hybrid_CPU.Core.NestedCapabilityFilter.h"

namespace YAKSys_Hybrid_CPU::Core
{
	const std::shared_ptr<NestedCapabilityFilterResult> NestedCapabilityFilterResult::Allowed =
		std::make_shared<NestedCapabilityFilterResult>(NestedCapabilityFilterDecision::Allowed,
			L"Nested capability filter allowed.");

	std::shared_ptr<NestedCapabilityFilterResult> NestedCapabilityFilter::Validate(
		std::shared_ptr<NestedCapabilityFilterRequest> request)
	{
		if (!request || !request->Descriptor)
			return NestedCapabilityFilterResult::Denied(NestedCapabilityFilterDecision::MissingDescriptor,
				L"Nested capability filtering requires a nested-domain descriptor.");

		const auto &descriptor = request->Descriptor;
		if (!descriptor->isRuntimeAuthoritative())
			return NestedCapabilityFilterResult::Denied(NestedCapabilityFilterDecision::RuntimeAuthorityRequired,
				L"Nested capability filtering requires a runtime-authoritative descriptor.");

		if (!descriptor->HasCapability(request->RequiredCapabilities))
			return NestedCapabilityFilterResult::Denied(NestedCapabilityFilterDecision::CapabilityDenied,
				L"Required nested capability grants are not present.");

		if (request->RequiresHostEvidenceExclusion && !descriptor->getHostEvidenceExcluded())
			return NestedCapabilityFilterResult::Denied(NestedCapabilityFilterDecision::HostEvidenceGateDenied,
				L"Nested composition requires host-owned evidence exclusion.");

		if (request->RequiresLanePassthroughBlocked && !descriptor->getLanePassthroughBlocked())
			return NestedCapabilityFilterResult::Denied(NestedCapabilityFilterDecision::LanePassthroughGateDenied,
				L"Nested composition requires Lane6/Lane7 passthrough to be blocked.");

		if (request->RequiresDomainComposition && !descriptor->getCanComposeDomain())
			return NestedCapabilityFilterResult::Denied(NestedCapabilityFilterDecision::DomainCompositionDenied,
				L"Nested domain composition is not enabled by descriptor gates.");

		return NestedCapabilityFilterResult::Allowed;
	}

	bool NestedCapabilityFilter::CanPass(std::shared_ptr<NestedCapabilityFilterRequest> request)
	{
		const auto result = Validate(std::move(request));
		return result && result->IsAllowed;
	}
}
