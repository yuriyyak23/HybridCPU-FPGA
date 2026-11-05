#include "YAKSys_Hybrid_CPU.Core.Lane7DomainRuntime.h"
#include "../../Descriptors/Lane7Accelerator/YAKSys_Hybrid_CPU.Core.Lane7AcceleratorDescriptor.h"
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	std::shared_ptr<Lane7DomainRuntimeResult> Lane7DomainRuntime::Validate(std::shared_ptr<Lane7DomainRuntimeRequest> request)
	{
		if (request == nullptr || request->Descriptor == nullptr)
		{
			return Lane7DomainRuntimeResult::Denied(Lane7DomainRuntimeDecision::MissingDescriptor, L"Lane7 runtime requires a Lane7 accelerator descriptor.");
		}

		const auto& descriptor = request->Descriptor;
		if (!descriptor->isRuntimeAuthoritative())
		{
			return Lane7DomainRuntimeResult::Denied(Lane7DomainRuntimeDecision::RuntimeAuthorityRequired, L"Lane7 runtime requires a runtime-authoritative descriptor.");
		}

		if (!descriptor->isPinnedToLane7())
		{
			return Lane7DomainRuntimeResult::Denied(Lane7DomainRuntimeDecision::WrongPinnedLane, L"Lane7 descriptor must be pinned to Lane7.");
		}

		if ((request->RequiresBackendBinding || descriptor->getRequiresRuntimeBackendBinding()) && !descriptor->getHasBackendBinding())
		{
			return Lane7DomainRuntimeResult::Denied(Lane7DomainRuntimeDecision::MissingBackendBinding, L"Lane7 runtime requires a backend binding.");
		}

		if (request->RequiresHandleNamespace && descriptor->getHandleNamespaceId() == 0)
		{
			return Lane7DomainRuntimeResult::Denied(Lane7DomainRuntimeDecision::MissingHandleNamespace, L"Lane7 runtime requires a handle namespace binding.");
		}

		if (request->RequiresTokenNamespace && descriptor->getTokenNamespaceId() == 0)
		{
			return Lane7DomainRuntimeResult::Denied(Lane7DomainRuntimeDecision::MissingTokenNamespace, L"Lane7 runtime requires a token namespace binding.");
		}

		if (request->RequiresCompletionRoute && descriptor->getCompletionRouteId() == 0)
		{
			return Lane7DomainRuntimeResult::Denied(Lane7DomainRuntimeDecision::MissingCompletionRoute, L"Lane7 runtime requires a completion route binding.");
		}

		if (request->RequiresCompatibilityProjection && !descriptor->getCanProjectToCompatibilityFrontend())
		{
			return Lane7DomainRuntimeResult::Denied(Lane7DomainRuntimeDecision::CompatibilityProjectionDenied, L"Lane7 descriptor denies compatibility projection.");
		}

		return Lane7DomainRuntimeResult::Allowed;
	}

	bool Lane7DomainRuntime::CanAdmit(std::shared_ptr<Lane7DomainRuntimeRequest> request)
	{
		return Validate(std::move(request))->isAllowed();
	}
}
