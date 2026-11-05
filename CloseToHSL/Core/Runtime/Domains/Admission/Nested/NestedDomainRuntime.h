#pragma once

#include "../../../Nested/CapabilityFilter/NestedCapabilityFilter.h"
#include "../../../Nested/Descriptors/YAKSys_Hybrid_CPU.Core.NestedDomainDescriptor.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class NestedDomainRuntimeDecision : unsigned char
	{
		Allowed = 0,
		MissingDescriptor = 1,
		RuntimeAuthorityRequired = 2,
		DomainCompositionDenied = 3,
		CapabilityFilterDenied = 4,
		CompatibilityProjectionDenied = 5
	};

	struct NestedDomainRuntimeRequest final
	{
		const std::shared_ptr<NestedDomainDescriptor> Descriptor;
		const std::shared_ptr<NestedCapabilityFilterResult> CapabilityFilter;
		const bool RequiresCompatibilityProjection;

		NestedDomainRuntimeRequest(std::shared_ptr<NestedDomainDescriptor> descriptor,
			std::shared_ptr<NestedCapabilityFilterResult> capabilityFilter,
			bool requiresCompatibilityProjection)
			: Descriptor(std::move(descriptor)), CapabilityFilter(std::move(capabilityFilter)),
			  RequiresCompatibilityProjection(requiresCompatibilityProjection) {}
	};

	struct NestedDomainRuntimeResult final
	{
		const NestedDomainRuntimeDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;

		NestedDomainRuntimeResult(NestedDomainRuntimeDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)),
			  IsAllowed(decision == NestedDomainRuntimeDecision::Allowed) {}

		static const std::shared_ptr<NestedDomainRuntimeResult> Allowed;
		static std::shared_ptr<NestedDomainRuntimeResult> Denied(
			NestedDomainRuntimeDecision decision, const std::wstring &reason)
		{
			return std::make_shared<NestedDomainRuntimeResult>(decision, reason);
		}
	};
}
