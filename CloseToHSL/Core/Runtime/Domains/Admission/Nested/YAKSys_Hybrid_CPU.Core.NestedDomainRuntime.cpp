#include "YAKSys_Hybrid_CPU.Core.NestedDomainRuntime.h"

namespace YAKSys_Hybrid_CPU::Core
{
	const std::shared_ptr<NestedDomainRuntimeResult> NestedDomainRuntimeResult::Allowed =
		std::make_shared<NestedDomainRuntimeResult>(NestedDomainRuntimeDecision::Allowed,
			L"Nested domain runtime admission allowed.");

	std::shared_ptr<NestedDomainRuntimeResult> NestedDomainRuntime::Validate(
		std::shared_ptr<NestedDomainRuntimeRequest> request)
	{
		if (!request || !request->Descriptor)
			return NestedDomainRuntimeResult::Denied(NestedDomainRuntimeDecision::MissingDescriptor,
				L"Nested domain runtime admission requires a descriptor.");

		const auto &descriptor = request->Descriptor;
		if (!descriptor->isRuntimeAuthoritative())
			return NestedDomainRuntimeResult::Denied(NestedDomainRuntimeDecision::RuntimeAuthorityRequired,
				L"Nested domain runtime admission requires runtime-authoritative descriptors.");

		if (!descriptor->getCanComposeDomain())
			return NestedDomainRuntimeResult::Denied(NestedDomainRuntimeDecision::DomainCompositionDenied,
				L"Nested domain runtime admission requires validated domain composition.");

		if (!request->CapabilityFilter || !request->CapabilityFilter->IsAllowed)
			return NestedDomainRuntimeResult::Denied(NestedDomainRuntimeDecision::CapabilityFilterDenied,
				request->CapabilityFilter ? request->CapabilityFilter->Reason :
				L"Nested domain runtime admission requires a capability-filter result.");

		if (request->RequiresCompatibilityProjection && !descriptor->getCanProjectToCompatibilityFrontend())
			return NestedDomainRuntimeResult::Denied(NestedDomainRuntimeDecision::CompatibilityProjectionDenied,
				L"Nested compatibility projection requires descriptor-authorized projection.");

		return NestedDomainRuntimeResult::Allowed;
	}

	bool NestedDomainRuntime::CanAdmit(std::shared_ptr<NestedDomainRuntimeRequest> request)
	{
		const auto result = Validate(std::move(request));
		return result && result->IsAllowed;
	}
}
