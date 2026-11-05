#pragma once

#include <memory>
#include <string>
#include <utility>
#include "../CapabilityFilter/NestedCapabilityFilter.h"
#include "../../Domains/Admission/Nested/NestedDomainRuntime.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class NestedProjectionDecision : unsigned char
	{
		Allowed = 0,
		MissingDescriptor = 1,
		RuntimeAdmissionDenied = 2,
		CapabilityFilterDenied = 3,
		EvidencePolicyDenied = 4,
		CompletionMappingDenied = 5,
		CompatibilityProjectionDenied = 6
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public readonly record struct NestedProjectionRequest(NestedDomainDescriptor? Descriptor, NestedDomainRuntimeResult RuntimeAdmission, NestedCapabilityFilterResult CapabilityFilter, NestedEvidencePolicyResult EvidencePolicy, NestedCompletionMappingResult CompletionMapping, bool RequiresCompletionMapping, bool RequiresCompatibilityProjection);
//	public readonly record struct NestedProjectionRequest(NestedDomainDescriptor Descriptor, NestedDomainRuntimeResult RuntimeAdmission, NestedCapabilityFilterResult CapabilityFilter, NestedEvidencePolicyResult EvidencePolicy, NestedCompletionMappingResult CompletionMapping, bool RequiresCompletionMapping, bool RequiresCompatibilityProjection);
	struct NestedProjectionGateResult final
	{
		const bool IsAllowed;
		const std::wstring Reason;
		NestedProjectionGateResult(bool isAllowed = false, std::wstring reason = L"")
			: IsAllowed(isAllowed), Reason(std::move(reason)) {}
	};

	struct NestedProjectionRequest final
	{
		const std::shared_ptr<NestedDomainDescriptor> Descriptor;
		const NestedDomainRuntimeResult RuntimeAdmission;
		const NestedCapabilityFilterResult CapabilityFilter;
		const NestedProjectionGateResult EvidencePolicy;
		const NestedProjectionGateResult CompletionMapping;
		const bool RequiresCompletionMapping;
		const bool RequiresCompatibilityProjection;

		NestedProjectionRequest(std::shared_ptr<NestedDomainDescriptor> descriptor,
			NestedDomainRuntimeResult runtimeAdmission, NestedCapabilityFilterResult capabilityFilter,
			NestedProjectionGateResult evidencePolicy, NestedProjectionGateResult completionMapping,
			bool requiresCompletionMapping, bool requiresCompatibilityProjection)
			: Descriptor(std::move(descriptor)), RuntimeAdmission(std::move(runtimeAdmission)),
			  CapabilityFilter(std::move(capabilityFilter)), EvidencePolicy(std::move(evidencePolicy)),
			  CompletionMapping(std::move(completionMapping)), RequiresCompletionMapping(requiresCompletionMapping),
			  RequiresCompatibilityProjection(requiresCompatibilityProjection) {}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedProjectionResult(NestedProjectionDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == NestedProjectionDecision.Allowed;
//			}
//		}
//
//		public static NestedProjectionResult Allowed
//		{
//			get;
//		} = new(NestedProjectionDecision.Allowed, "Nested compatibility projection allowed.");
//
//		public static NestedProjectionResult Denied(NestedProjectionDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}
	struct NestedProjectionResult final
	{
		const NestedProjectionDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;

		NestedProjectionResult(NestedProjectionDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)), IsAllowed(decision == NestedProjectionDecision::Allowed) {}

		static const std::shared_ptr<NestedProjectionResult> Allowed;
		static std::shared_ptr<NestedProjectionResult> Denied(NestedProjectionDecision decision, const std::wstring &reason)
		{
			return std::make_shared<NestedProjectionResult>(decision, reason);
		}
	};
}
