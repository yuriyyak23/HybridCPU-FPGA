#include "YAKSys_Hybrid_CPU.Core.Lane6DomainRuntime.h"
#include "Lane6DomainRuntime.h"
#include "../../Descriptors/Lane6Domain/YAKSys_Hybrid_CPU.Core.Lane6DomainDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<Lane6DomainRuntimeResult> Lane6DomainRuntime::Validate(std::shared_ptr<Lane6DomainRuntimeRequest> request)
	{
		if (request == nullptr || request->Descriptor == nullptr)
		{
			return Lane6DomainRuntimeResult::Denied(Lane6DomainRuntimeDecision::MissingDescriptor, L"Lane6 runtime requires a Lane6 domain descriptor.");
		}

		if (!request->Descriptor->isRuntimeAuthoritative())
		{
			return Lane6DomainRuntimeResult::Denied(Lane6DomainRuntimeDecision::RuntimeAuthorityRequired, L"Lane6 runtime requires a runtime-authoritative descriptor.");
		}

		if (!request->Descriptor->isPinnedToLane6())
		{
			return Lane6DomainRuntimeResult::Denied(Lane6DomainRuntimeDecision::WrongPinnedLane, L"Lane6 descriptor must be pinned to Lane6.");
		}

		if (request->RequiresTokenNamespace && request->Descriptor->getTokenNamespaceId() == 0)
		{
			return Lane6DomainRuntimeResult::Denied(Lane6DomainRuntimeDecision::MissingTokenNamespace, L"Lane6 runtime requires a token namespace binding.");
		}

		if (request->RequiresQueueNamespace && request->Descriptor->getQueueNamespaceId() == 0)
		{
			return Lane6DomainRuntimeResult::Denied(Lane6DomainRuntimeDecision::MissingQueueNamespace, L"Lane6 runtime requires a queue namespace binding.");
		}

		if (request->RequiresFenceDomain && request->Descriptor->getFenceDomainId() == 0)
		{
			return Lane6DomainRuntimeResult::Denied(Lane6DomainRuntimeDecision::MissingFenceDomain, L"Lane6 runtime requires a fence-domain binding.");
		}

		if (request->RequiresCompatibilityProjection && !request->Descriptor->getCanProjectToCompatibilityFrontend())
		{
			return Lane6DomainRuntimeResult::Denied(Lane6DomainRuntimeDecision::CompatibilityProjectionDenied, L"Lane6 descriptor denies compatibility projection.");
		}

		return Lane6DomainRuntimeResult::Allowed();
	}

	bool Lane6DomainRuntime::CanAdmit(std::shared_ptr<Lane6DomainRuntimeRequest> request)
	{
		return Validate(request)->IsAllowed;
	}
}
