#include "YAKSys_Hybrid_CPU.Core.NestedMemoryDomainComposer.h"
#include "NestedMemoryDomainComposer.h"
#include "../../../Virtualization/Compatibility/Frontend/Projection/Nested/NestedDomainController.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<NestedMemoryDomainCompositionResult> NestedMemoryDomainComposer::Validate(std::shared_ptr<NestedMemoryDomainCompositionRequest> request)
	{
		if (request->NestedDescriptor == nullptr)
		{
			return NestedMemoryDomainCompositionResult::Denied(NestedMemoryDomainCompositionDecision::MissingNestedDescriptor, L"Nested memory composition requires a nested domain descriptor.");
		}

		if (!request->NestedDescriptor->isRuntimeAuthoritative())
		{
			return NestedMemoryDomainCompositionResult::Denied(NestedMemoryDomainCompositionDecision::RuntimeAuthorityRequired, L"Nested memory composition requires runtime-authoritative descriptors.");
		}

		if (!request->NestedDescriptor->getCanComposeDomain())
		{
			return NestedMemoryDomainCompositionResult::Denied(NestedMemoryDomainCompositionDecision::DomainCompositionDenied, L"Nested memory composition requires validated domain composition.");
		}

		if (!request->NestedDescriptor->HasCapability(YAKSys_Hybrid_CPU::Core::Nested::NestedCapabilityGrantMask::NestedMemoryComposition))
		{
			return NestedMemoryDomainCompositionResult::Denied(NestedMemoryDomainCompositionDecision::CapabilityDenied, L"Nested memory composition requires the nested memory-composition capability grant.");
		}

		if (request->ChildMemory == nullptr)
		{
			return NestedMemoryDomainCompositionResult::Denied(NestedMemoryDomainCompositionDecision::MissingChildMemoryDomain, L"Nested memory composition requires a child memory-domain descriptor.");
		}

		if (request->HostMemory == nullptr)
		{
			return NestedMemoryDomainCompositionResult::Denied(NestedMemoryDomainCompositionDecision::MissingHostMemoryDomain, L"Nested memory composition requires a host memory-domain descriptor.");
		}

		if (!request->ChildMemory->getOwnsSecondStageTranslation() || !request->ChildMemory->getHasValidTranslationControl())
		{
			return NestedMemoryDomainCompositionResult::Denied(NestedMemoryDomainCompositionDecision::ChildTranslationAuthorityDenied, L"Child memory-domain descriptor does not own valid second-stage translation.");
		}

		if (!request->HostMemory->getOwnsSecondStageTranslation() || !request->HostMemory->getHasValidTranslationControl())
		{
			return NestedMemoryDomainCompositionResult::Denied(NestedMemoryDomainCompositionDecision::HostTranslationAuthorityDenied, L"Host memory-domain descriptor does not own valid second-stage translation.");
		}

		if (!request->Context.IsValid)
		{
			return NestedMemoryDomainCompositionResult::Denied(NestedMemoryDomainCompositionDecision::InvalidCompositionContext, L"Nested memory composition context is invalid.");
		}

		if (request->RequiresCompatibilityProjection && !request->NestedDescriptor->getCanProjectToCompatibilityFrontend())
		{
			return NestedMemoryDomainCompositionResult::Denied(NestedMemoryDomainCompositionDecision::CompatibilityProjectionDenied, L"Nested memory composition descriptor denies compatibility projection.");
		}

		return NestedMemoryDomainCompositionResult::Allowed();
	}

	bool NestedMemoryDomainComposer::CanCompose(std::shared_ptr<NestedMemoryDomainCompositionRequest> request)
	{
		return Validate(request)->IsAllowed;
	}
}
