#include "YAKSys_Hybrid_CPU.Core.NestedCompletionMapper.h"
#include "NestedCompletionMapper.h"
#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include "NestedDomainController.h"

using namespace YAKSys_Hybrid_CPU::Core::Nested;

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<NestedCompletionMappingResult> NestedCompletionMapper::Validate(std::shared_ptr<NestedCompletionMappingRequest> request)
	{
		if (request->Descriptor == nullptr)
		{
			return NestedCompletionMappingResult::Denied(NestedCompletionMappingDecision::MissingDescriptor, VmExitReason::SecurityPolicyViolation, 0, L"Nested completion mapping requires a nested domain descriptor.");
		}

		if (!request->Descriptor->isRuntimeAuthoritative())
		{
			return NestedCompletionMappingResult::Denied(NestedCompletionMappingDecision::RuntimeAuthorityRequired, VmExitReason::SecurityPolicyViolation, 0, L"Nested completion mapping requires runtime-authoritative descriptors.");
		}

		if (request->Completion == nullptr || !request->Completion->getHasPayload())
		{
			return NestedCompletionMappingResult::Denied(NestedCompletionMappingDecision::CompletionPayloadMissing, VmExitReason::None, 0, L"Nested completion mapping requires a completion sideband payload.");
		}

		if (request->Completion->getRequiresHostHandling())
		{
			return NestedCompletionMappingResult::Denied(NestedCompletionMappingDecision::HostEvidenceDenied, VmExitReason::SecurityPolicyViolation, request->Completion->getSequence(), L"Nested completion mapping cannot expose host-owned completion evidence.");
		}

		if (!request->Descriptor->getLanePassthroughBlocked())
		{
			return NestedCompletionMappingResult::Denied(NestedCompletionMappingDecision::LanePassthroughDenied, VmExitReason::SecurityPolicyViolation, request->Completion->getSequence(), L"Nested completion mapping requires Lane6/Lane7 passthrough to be blocked.");
		}

		if (!request->Descriptor->HasCapability(NestedCapabilityGrantMask::ExitMapping))
		{
			return NestedCompletionMappingResult::Denied(NestedCompletionMappingDecision::CapabilityDenied, VmExitReason::SecurityPolicyViolation, request->Completion->getSequence(), L"Nested completion mapping requires the nested exit-mapping capability grant.");
		}

		if (request->RequiresCompatibilityProjection && (request->RouteDescriptor == nullptr || !request->RouteDescriptor->getCanProjectToCompatibilityFrontend()))
		{
			return NestedCompletionMappingResult::Denied(NestedCompletionMappingDecision::CompatibilityProjectionDenied, VmExitReason::SecurityPolicyViolation, request->Completion->getRouteId(), L"Nested completion mapping requires descriptor-authorized compatibility projection.");
		}

		return NestedCompletionMappingResult::Allowed(request->Completion);
	}

	bool NestedCompletionMapper::CanMap(std::shared_ptr<NestedCompletionMappingRequest> request)
	{
		return Validate(request)->getIsAllowed();
	}
}
