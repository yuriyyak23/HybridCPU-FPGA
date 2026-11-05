#pragma once
#include "../../../Domains/Descriptors/Lane7Accelerator/YAKSys_Hybrid_CPU.Core.Lane7AcceleratorDescriptor.h"
#include "../../../Domains/Descriptors/CompletionRoute/YAKSys_Hybrid_CPU.Core.CompletionRouteDescriptor.h"
#include <memory>
#include <string>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class Lane7CompletionPolicyDecision : unsigned char
	{
		Allowed = 0,
		MissingDescriptor = 1,
		RuntimeAuthorityRequired = 2,
		WrongPinnedLane = 3,
		MissingCompletionRouteBinding = 4,
		MissingRouteDescriptor = 5,
		RouteAuthorityDenied = 6,
		InvalidCompletion = 7,
		WrongCompletionSource = 8,
		RouteSourceDenied = 9,
		CompatibilityProjectionDenied = 10
	};

struct Lane7CompletionPolicyRequest final { std::shared_ptr<Lane7AcceleratorDescriptor> Descriptor; std::shared_ptr<CompletionRouteDescriptor> RouteDescriptor; std::shared_ptr<LaneCompletionDescriptor> Completion; bool RequiresCompatibilityProjection=false; };
struct Lane7CompletionPolicyResult final {
 Lane7CompletionPolicyDecision Decision; std::wstring Reason;
 bool IsAllowed() const { return Decision == Lane7CompletionPolicyDecision::Allowed; }
 static std::shared_ptr<Lane7CompletionPolicyResult> Allowed() { return std::make_shared<Lane7CompletionPolicyResult>(Lane7CompletionPolicyDecision::Allowed,L"Lane7 completion policy allowed."); }
 static std::shared_ptr<Lane7CompletionPolicyResult> Denied(Lane7CompletionPolicyDecision decision, std::wstring reason) { return std::make_shared<Lane7CompletionPolicyResult>(decision,std::move(reason)); }
};
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public readonly record struct Lane7CompletionPolicyRequest(Lane7AcceleratorDescriptor? Descriptor, System.Nullable<CompletionRouteDescriptor> RouteDescriptor, LaneCompletionDescriptor Completion, bool RequiresCompatibilityProjection);
//	public readonly record struct Lane7CompletionPolicyRequest(Lane7AcceleratorDescriptor Descriptor, CompletionRouteDescriptor RouteDescriptor, LaneCompletionDescriptor Completion, bool RequiresCompatibilityProjection);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Lane7CompletionPolicyResult(Lane7CompletionPolicyDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == Lane7CompletionPolicyDecision.Allowed;
//			}
//		}
//
//		public static Lane7CompletionPolicyResult Allowed
//		{
//			get;
//		} = new(Lane7CompletionPolicyDecision.Allowed, "Lane7 completion policy allowed.");
//
//		public static Lane7CompletionPolicyResult Denied(Lane7CompletionPolicyDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}


}
