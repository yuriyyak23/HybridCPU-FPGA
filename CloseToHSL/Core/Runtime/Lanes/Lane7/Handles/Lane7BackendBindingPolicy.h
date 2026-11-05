#pragma once

#include "../../../Domains/Descriptors/Lane7Accelerator/Lane7AcceleratorDescriptor.h"
#include "Lane7HandleNamespace.h"
#include "YAKSys_Hybrid_CPU.Core.Lane7HandleNamespace.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class Lane7BackendBindingDecision : unsigned char
	{
		Allowed = 0,
		MissingDescriptor = 1,
		RuntimeAuthorityRequired = 2,
		WrongPinnedLane = 3,
		BackendBindingMissing = 4,
		HandleNamespaceAuthorityDenied = 5,
		NativeBackendHandleExposureDenied = 6,
		CompatibilityProjectionDenied = 7
	};

	struct Lane7BackendBindingRequest final
	{
		std::shared_ptr<Lane7AcceleratorDescriptor> Descriptor;
		std::shared_ptr<Lane7HandleNamespace> HandleNamespace;
		bool RequiresBackendBinding;
		bool RequiresCompatibilityProjection;

		Lane7BackendBindingRequest(std::shared_ptr<Lane7AcceleratorDescriptor> descriptor, std::shared_ptr<Lane7HandleNamespace> handleNamespace, bool requiresBackendBinding, bool requiresCompatibilityProjection)
			: Descriptor(std::move(descriptor)), HandleNamespace(std::move(handleNamespace)), RequiresBackendBinding(requiresBackendBinding), RequiresCompatibilityProjection(requiresCompatibilityProjection) {}
	};

	struct Lane7BackendBindingResult final
	{
		Lane7BackendBindingDecision Decision;
		std::wstring Reason;
		bool IsAllowed;

		Lane7BackendBindingResult(Lane7BackendBindingDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)), IsAllowed(decision == Lane7BackendBindingDecision::Allowed) {}

		static std::shared_ptr<Lane7BackendBindingResult> Allowed()
		{
			return std::make_shared<Lane7BackendBindingResult>(Lane7BackendBindingDecision::Allowed, L"Lane7 backend binding policy allowed.");
		}

		static std::shared_ptr<Lane7BackendBindingResult> Denied(Lane7BackendBindingDecision decision, std::wstring reason)
		{
			return std::make_shared<Lane7BackendBindingResult>(decision, std::move(reason));
		}
	};
}
