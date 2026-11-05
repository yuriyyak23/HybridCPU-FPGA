#include "YAKSys_Hybrid_CPU.Core.Lane7CompletionPolicy.h"
#include "Lane7CompletionPolicy.h"

#include "../../../Completion/Routing/LaneCompletionRouting.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<Lane7CompletionPolicyResult> Lane7CompletionPolicy::Validate(std::shared_ptr<Lane7CompletionPolicyRequest> request)
	{
		if (request->Descriptor == nullptr)
		{
			return Lane7CompletionPolicyResult::Denied(Lane7CompletionPolicyDecision::MissingDescriptor, L"Lane7 completion policy requires a Lane7 accelerator descriptor.");
		}

		if (!request->Descriptor->isRuntimeAuthoritative())
		{
			return Lane7CompletionPolicyResult::Denied(Lane7CompletionPolicyDecision::RuntimeAuthorityRequired, L"Lane7 completion policy requires runtime-authoritative descriptors.");
		}

		if (!request->Descriptor->isPinnedToLane7())
		{
			return Lane7CompletionPolicyResult::Denied(Lane7CompletionPolicyDecision::WrongPinnedLane, L"Lane7 completion policy requires a descriptor pinned to Lane7.");
		}

		if (request->Descriptor->getCompletionRouteId() == 0)
		{
			return Lane7CompletionPolicyResult::Denied(Lane7CompletionPolicyDecision::MissingCompletionRouteBinding, L"Lane7 completion policy requires a completion route binding.");
		}

		if (request->RouteDescriptor == nullptr)
		{
			return Lane7CompletionPolicyResult::Denied(Lane7CompletionPolicyDecision::MissingRouteDescriptor, L"Lane7 completion policy requires a completion route descriptor.");
		}

		if (!request->RouteDescriptor->isRuntimeAuthoritative())
		{
			return Lane7CompletionPolicyResult::Denied(Lane7CompletionPolicyDecision::RouteAuthorityDenied, L"Lane7 completion route must be runtime-authoritative.");
		}

		if (!request->Completion || !request->Completion->IsValid())
		{
			return Lane7CompletionPolicyResult::Denied(Lane7CompletionPolicyDecision::InvalidCompletion, L"Lane7 completion policy rejected an invalid completion descriptor.");
		}

		if (request->Completion->SourceKind != LaneCompletionSourceKind::ExternalAcceleratorLane7 || request->Completion->LaneIndex != 7)
		{
			return Lane7CompletionPolicyResult::Denied(Lane7CompletionPolicyDecision::WrongCompletionSource, L"Lane7 completion policy accepts only Lane7 accelerator completions.");
		}

		if (!request->RouteDescriptor->AllowsSource(request->Completion->SourceKind))
		{
			return Lane7CompletionPolicyResult::Denied(Lane7CompletionPolicyDecision::RouteSourceDenied, L"Completion route descriptor denies Lane7 accelerator completions.");
		}

		if (request->RequiresCompatibilityProjection && (!request->Descriptor->getCanProjectToCompatibilityFrontend() || !request->RouteDescriptor->getCanProjectToCompatibilityFrontend()))
		{
			return Lane7CompletionPolicyResult::Denied(Lane7CompletionPolicyDecision::CompatibilityProjectionDenied, L"Lane7 completion policy requires descriptor-authorized compatibility projection.");
		}

		return Lane7CompletionPolicyResult::Allowed();
	}

	bool Lane7CompletionPolicy::CanPublish(std::shared_ptr<Lane7CompletionPolicyRequest> request)
	{
		return Validate(request)->IsAllowed();
	}
}
