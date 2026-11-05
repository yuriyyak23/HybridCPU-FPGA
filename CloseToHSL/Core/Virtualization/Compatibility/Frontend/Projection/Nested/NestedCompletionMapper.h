#pragma once

#include "../../../../../Runtime/Nested/Descriptors/YAKSys_Hybrid_CPU.Core.NestedDomainDescriptor.h"
#include "../../../../../Runtime/Domains/Descriptors/CompletionRoute/YAKSys_Hybrid_CPU.Core.CompletionRouteDescriptor.h"
#include "../../../../Sideband/CompletionTransport/YAKSys_Hybrid_CPU.Core.CompletionSidebandEnvelope.h"
#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include <memory>
#include <string>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class NestedCompletionMappingDecision : unsigned char
	{
		Allowed = 0,
		MissingDescriptor = 1,
		RuntimeAuthorityRequired = 2,
		CompletionPayloadMissing = 3,
		HostEvidenceDenied = 4,
		LanePassthroughDenied = 5,
		CapabilityDenied = 6,
		CompatibilityProjectionDenied = 7
	};

	struct NestedCompletionMappingRequest final
	{
		std::shared_ptr<NestedDomainDescriptor> Descriptor;
		std::shared_ptr<CompletionRouteDescriptor> RouteDescriptor;
		std::shared_ptr<CompletionSidebandEnvelope> Completion;
		bool RequiresCompatibilityProjection;
	};

	struct NestedCompletionMappingResult final
	{
		NestedCompletionMappingDecision Decision;
		std::shared_ptr<CompletionSidebandEnvelope> Completion;
		VmExitReason ExitReason;
		unsigned long long Qualification;
		std::wstring Reason;
		bool getIsAllowed() const { return Decision == NestedCompletionMappingDecision::Allowed; }
		static std::shared_ptr<NestedCompletionMappingResult> Allowed(std::shared_ptr<CompletionSidebandEnvelope> completion) { return std::make_shared<NestedCompletionMappingResult>(NestedCompletionMappingResult{NestedCompletionMappingDecision::Allowed, std::move(completion), VmExitReason::None, 0, L"Nested completion mapping allowed."}); }
		static std::shared_ptr<NestedCompletionMappingResult> Denied(NestedCompletionMappingDecision decision, VmExitReason reason, unsigned long long qualification, std::wstring message) { return std::make_shared<NestedCompletionMappingResult>(NestedCompletionMappingResult{decision, CompletionSidebandEnvelope::getEmpty(), reason, qualification, std::move(message)}); }
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public readonly record struct NestedCompletionMappingRequest(NestedDomainDescriptor? Descriptor, System.Nullable<CompletionRouteDescriptor> RouteDescriptor, System.Nullable<CompletionSidebandEnvelope> Completion, bool RequiresCompatibilityProjection);
//	public readonly record struct NestedCompletionMappingRequest(NestedDomainDescriptor Descriptor, CompletionRouteDescriptor RouteDescriptor, CompletionSidebandEnvelope Completion, bool RequiresCompatibilityProjection);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedCompletionMappingResult(NestedCompletionMappingDecision Decision, CompletionSidebandEnvelope Completion, VmExitReason ExitReason, ulong Qualification, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == NestedCompletionMappingDecision.Allowed;
//			}
//		}
//
//		public static NestedCompletionMappingResult Allowed(CompletionSidebandEnvelope completion)
//		{
//			return new(NestedCompletionMappingDecision.Allowed, completion, VmExitReason.None, 0, "Nested completion mapping allowed.");
//		}
//
//		public static NestedCompletionMappingResult Denied(NestedCompletionMappingDecision decision, VmExitReason exitReason, ulong qualification, string reason)
//		{
//			return new(decision, CompletionSidebandEnvelope.Empty, exitReason, qualification, reason);
//		}
//	}


}
