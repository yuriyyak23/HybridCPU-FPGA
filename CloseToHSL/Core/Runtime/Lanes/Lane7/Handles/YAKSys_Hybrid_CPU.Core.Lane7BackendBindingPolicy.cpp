#include "YAKSys_Hybrid_CPU.Core.Lane7BackendBindingPolicy.h"
#include "Lane7BackendBindingPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<Lane7BackendBindingResult> Lane7BackendBindingPolicy::Validate(std::shared_ptr<Lane7BackendBindingRequest> request)
	{
		if (request->Descriptor == nullptr)
		{
			return Lane7BackendBindingResult::Denied(Lane7BackendBindingDecision::MissingDescriptor, L"Lane7 backend binding policy requires a Lane7 accelerator descriptor.");
		}

		if (!request->Descriptor->isRuntimeAuthoritative())
		{
			return Lane7BackendBindingResult::Denied(Lane7BackendBindingDecision::RuntimeAuthorityRequired, L"Lane7 backend binding policy requires runtime-authoritative descriptors.");
		}

		if (!request->Descriptor->isPinnedToLane7())
		{
			return Lane7BackendBindingResult::Denied(Lane7BackendBindingDecision::WrongPinnedLane, L"Lane7 backend binding policy requires a descriptor pinned to Lane7.");
		}

		if ((request->RequiresBackendBinding || request->Descriptor->getRequiresRuntimeBackendBinding()) && !request->Descriptor->getHasBackendBinding())
		{
			return Lane7BackendBindingResult::Denied(Lane7BackendBindingDecision::BackendBindingMissing, L"Lane7 backend binding policy requires a runtime backend binding.");
		}

		if (request->HandleNamespace != nullptr && !request->HandleNamespace->isRuntimeAuthoritative())
		{
			return Lane7BackendBindingResult::Denied(Lane7BackendBindingDecision::HandleNamespaceAuthorityDenied, L"Lane7 backend binding policy requires a runtime-owned handle namespace.");
		}

		if (request->HandleNamespace != nullptr && request->HandleNamespace->getExposesNativeBackendHandles())
		{
			return Lane7BackendBindingResult::Denied(Lane7BackendBindingDecision::NativeBackendHandleExposureDenied, L"Lane7 backend binding policy cannot expose native backend handles.");
		}

		if (request->RequiresCompatibilityProjection && !request->Descriptor->getCanProjectToCompatibilityFrontend())
		{
			return Lane7BackendBindingResult::Denied(Lane7BackendBindingDecision::CompatibilityProjectionDenied, L"Lane7 backend binding descriptor denies compatibility projection.");
		}

		return Lane7BackendBindingResult::Allowed();
	}

	bool Lane7BackendBindingPolicy::CanBind(std::shared_ptr<Lane7BackendBindingRequest> request)
	{
		return Validate(request)->IsAllowed;
	}
}
