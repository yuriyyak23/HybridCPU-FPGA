#pragma once

#include "../Descriptors/YAKSys_Hybrid_CPU.Core.NestedDomainDescriptor.h"
#include "../../Domains/Descriptors/MemoryDomain/YAKSys_Hybrid_CPU.Core.MemoryDomainDescriptor.h"
#include "NestedMemoryCompositionService.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class NestedMemoryDomainCompositionDecision : unsigned char
	{
		Allowed = 0,
		MissingNestedDescriptor = 1,
		RuntimeAuthorityRequired = 2,
		DomainCompositionDenied = 3,
		CapabilityDenied = 4,
		MissingChildMemoryDomain = 5,
		MissingHostMemoryDomain = 6,
		ChildTranslationAuthorityDenied = 7,
		HostTranslationAuthorityDenied = 8,
		InvalidCompositionContext = 9,
		CompatibilityProjectionDenied = 10
	};

	struct NestedMemoryDomainCompositionRequest final
	{
		const std::shared_ptr<NestedDomainDescriptor> NestedDescriptor;
		const std::shared_ptr<MemoryDomainDescriptor> ChildMemory;
		const std::shared_ptr<MemoryDomainDescriptor> HostMemory;
		const YAKSys_Hybrid_CPU::Memory::NestedMemoryCompositionContext Context;
		const bool RequiresCompatibilityProjection;

		NestedMemoryDomainCompositionRequest(std::shared_ptr<NestedDomainDescriptor> nestedDescriptor,
			std::shared_ptr<MemoryDomainDescriptor> childMemory, std::shared_ptr<MemoryDomainDescriptor> hostMemory,
			YAKSys_Hybrid_CPU::Memory::NestedMemoryCompositionContext context, bool requiresCompatibilityProjection)
			: NestedDescriptor(std::move(nestedDescriptor)), ChildMemory(std::move(childMemory)), HostMemory(std::move(hostMemory)),
			  Context(std::move(context)), RequiresCompatibilityProjection(requiresCompatibilityProjection) {}
	};

	struct NestedMemoryDomainCompositionResult final
	{
		const NestedMemoryDomainCompositionDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;

		NestedMemoryDomainCompositionResult(NestedMemoryDomainCompositionDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)), IsAllowed(decision == NestedMemoryDomainCompositionDecision::Allowed) {}

		static std::shared_ptr<NestedMemoryDomainCompositionResult> Allowed()
		{
			return std::make_shared<NestedMemoryDomainCompositionResult>(NestedMemoryDomainCompositionDecision::Allowed,
				L"Nested memory domain composition allowed.");
		}

		static std::shared_ptr<NestedMemoryDomainCompositionResult> Denied(NestedMemoryDomainCompositionDecision decision, std::wstring reason)
		{
			return std::make_shared<NestedMemoryDomainCompositionResult>(decision, std::move(reason));
		}
	};
}
