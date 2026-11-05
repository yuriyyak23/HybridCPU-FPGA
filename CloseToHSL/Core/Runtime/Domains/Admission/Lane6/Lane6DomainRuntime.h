#pragma once

#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core { class Lane6DomainDescriptor; }

namespace YAKSys_Hybrid_CPU::Core
{

	enum class Lane6DomainRuntimeDecision : unsigned char
	{
		Allowed = 0,
		MissingDescriptor = 1,
		RuntimeAuthorityRequired = 2,
		WrongPinnedLane = 3,
		MissingTokenNamespace = 4,
		MissingQueueNamespace = 5,
		MissingFenceDomain = 6,
		CompatibilityProjectionDenied = 7
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public readonly record struct Lane6DomainRuntimeRequest(Lane6DomainDescriptor? Descriptor, bool RequiresTokenNamespace, bool RequiresQueueNamespace, bool RequiresFenceDomain, bool RequiresCompatibilityProjection);
//	public readonly record struct Lane6DomainRuntimeRequest(Lane6DomainDescriptor Descriptor, bool RequiresTokenNamespace, bool RequiresQueueNamespace, bool RequiresFenceDomain, bool RequiresCompatibilityProjection);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Lane6DomainRuntimeResult(Lane6DomainRuntimeDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == Lane6DomainRuntimeDecision.Allowed;
//			}
//		}
//
//		public static Lane6DomainRuntimeResult Allowed
//		{
//			get;
//		} = new(Lane6DomainRuntimeDecision.Allowed, "Lane6 domain runtime admission allowed.");
//
//		public static Lane6DomainRuntimeResult Denied(Lane6DomainRuntimeDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}


	struct Lane6DomainRuntimeRequest final
	{
		const std::shared_ptr<Lane6DomainDescriptor> Descriptor;
		const bool RequiresTokenNamespace;
		const bool RequiresQueueNamespace;
		const bool RequiresFenceDomain;
		const bool RequiresCompatibilityProjection;

		Lane6DomainRuntimeRequest(std::shared_ptr<Lane6DomainDescriptor> descriptor, bool requiresTokenNamespace, bool requiresQueueNamespace, bool requiresFenceDomain, bool requiresCompatibilityProjection)
			: Descriptor(std::move(descriptor)),
			  RequiresTokenNamespace(requiresTokenNamespace),
			  RequiresQueueNamespace(requiresQueueNamespace),
			  RequiresFenceDomain(requiresFenceDomain),
			  RequiresCompatibilityProjection(requiresCompatibilityProjection)
		{
		}
	};

	struct Lane6DomainRuntimeResult final
	{
		const Lane6DomainRuntimeDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;

		Lane6DomainRuntimeResult(Lane6DomainRuntimeDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)), IsAllowed(decision == Lane6DomainRuntimeDecision::Allowed)
		{
		}

		static std::shared_ptr<Lane6DomainRuntimeResult> Allowed()
		{
			return std::make_shared<Lane6DomainRuntimeResult>(Lane6DomainRuntimeDecision::Allowed, L"Lane6 domain runtime admission allowed.");
		}

		static std::shared_ptr<Lane6DomainRuntimeResult> Denied(Lane6DomainRuntimeDecision decision, std::wstring reason)
		{
			return std::make_shared<Lane6DomainRuntimeResult>(decision, std::move(reason));
		}
	};
}
