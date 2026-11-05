#pragma once

#include "../../../Virtualization/Compatibility/Frontend/Projection/Nested/NestedDomainController.h"
#include "../Descriptors/YAKSys_Hybrid_CPU.Core.NestedDomainDescriptor.h"
#include <memory>
#include <string>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class NestedCapabilityFilterDecision : unsigned char
	{
		Allowed = 0,
		MissingDescriptor = 1,
		RuntimeAuthorityRequired = 2,
		CapabilityDenied = 3,
		HostEvidenceGateDenied = 4,
		LanePassthroughGateDenied = 5,
		DomainCompositionDenied = 6
	};

	struct NestedCapabilityFilterRequest final
	{
		const std::shared_ptr<NestedDomainDescriptor> Descriptor;
		const NestedCapabilityGrantMask RequiredCapabilities;
		const bool RequiresHostEvidenceExclusion;
		const bool RequiresLanePassthroughBlocked;
		const bool RequiresDomainComposition;

		NestedCapabilityFilterRequest(std::shared_ptr<NestedDomainDescriptor> descriptor,
			NestedCapabilityGrantMask requiredCapabilities, bool requiresHostEvidenceExclusion,
			bool requiresLanePassthroughBlocked, bool requiresDomainComposition)
			: Descriptor(std::move(descriptor)), RequiredCapabilities(requiredCapabilities),
			  RequiresHostEvidenceExclusion(requiresHostEvidenceExclusion),
			  RequiresLanePassthroughBlocked(requiresLanePassthroughBlocked),
			  RequiresDomainComposition(requiresDomainComposition) {}
	};

	struct NestedCapabilityFilterResult final
	{
		const NestedCapabilityFilterDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;

		NestedCapabilityFilterResult(NestedCapabilityFilterDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)),
			  IsAllowed(decision == NestedCapabilityFilterDecision::Allowed) {}

		static const std::shared_ptr<NestedCapabilityFilterResult> Allowed;
		static std::shared_ptr<NestedCapabilityFilterResult> Denied(
			NestedCapabilityFilterDecision decision, const std::wstring &reason)
		{
			return std::make_shared<NestedCapabilityFilterResult>(decision, reason);
		}
	};
}

