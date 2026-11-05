#pragma once

#include <memory>
#include <string>
#include <utility>
#include "../../Descriptors/Lane7Accelerator/Lane7AcceleratorDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{
	enum class Lane7DomainRuntimeDecision : unsigned char
	{
		Allowed = 0,
		MissingDescriptor = 1,
		RuntimeAuthorityRequired = 2,
		WrongPinnedLane = 3,
		MissingBackendBinding = 4,
		MissingHandleNamespace = 5,
		MissingTokenNamespace = 6,
		MissingCompletionRoute = 7,
		CompatibilityProjectionDenied = 8
	};

	struct Lane7DomainRuntimeRequest final
	{
		std::shared_ptr<Lane7AcceleratorDescriptor> Descriptor;
		bool RequiresBackendBinding = false;
		bool RequiresHandleNamespace = false;
		bool RequiresTokenNamespace = false;
		bool RequiresCompletionRoute = false;
		bool RequiresCompatibilityProjection = false;
	};

	struct Lane7DomainRuntimeResult final
	{
		Lane7DomainRuntimeDecision Decision;
		std::wstring Reason;

		bool isAllowed() const noexcept { return Decision == Lane7DomainRuntimeDecision::Allowed; }
		static const std::shared_ptr<Lane7DomainRuntimeResult> Allowed;
		static std::shared_ptr<Lane7DomainRuntimeResult> Denied(Lane7DomainRuntimeDecision decision, std::wstring reason)
		{
			return std::make_shared<Lane7DomainRuntimeResult>(Lane7DomainRuntimeResult{decision, std::move(reason)});
		}
	};

	inline const std::shared_ptr<Lane7DomainRuntimeResult> Lane7DomainRuntimeResult::Allowed =
		std::make_shared<Lane7DomainRuntimeResult>(Lane7DomainRuntimeResult{Lane7DomainRuntimeDecision::Allowed, L"Lane7 domain runtime admission allowed."});
}
