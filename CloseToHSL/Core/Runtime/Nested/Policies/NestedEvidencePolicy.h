#pragma once

#include "../Descriptors/YAKSys_Hybrid_CPU.Core.NestedDomainDescriptor.h"
#include "../../Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "../../Domains/Descriptors/Observability/YAKSys_Hybrid_CPU.Core.ObservabilityDescriptor.h"
#include <memory>
#include <string>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class NestedEvidencePolicyDecision : unsigned char
	{
		Allowed = 0,
		MissingDescriptor = 1,
		RuntimeAuthorityRequired = 2,
		HostEvidenceGateDenied = 3,
		HostOwnedEvidenceDenied = 4,
		EvidencePolicyDenied = 5,
		ObservabilityPolicyDenied = 6
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public readonly record struct NestedEvidencePolicyRequest(NestedDomainDescriptor? Descriptor, System.Nullable<EvidencePolicyDescriptor> EvidencePolicy, System.Nullable<ObservabilityDescriptor> Observability, EvidenceVisibilityClass EvidenceClass, bool RequiresGuestProjection);
//	public readonly record struct NestedEvidencePolicyRequest(NestedDomainDescriptor Descriptor, EvidencePolicyDescriptor EvidencePolicy, ObservabilityDescriptor Observability, EvidenceVisibilityClass EvidenceClass, bool RequiresGuestProjection);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedEvidencePolicyResult(NestedEvidencePolicyDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == NestedEvidencePolicyDecision.Allowed;
//			}
//		}
//
//		public static NestedEvidencePolicyResult Allowed
//		{
//			get;
//		} = new(NestedEvidencePolicyDecision.Allowed, "Nested evidence policy allowed.");
//
//		public static NestedEvidencePolicyResult Denied(NestedEvidencePolicyDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}

	struct NestedEvidencePolicyRequest final
	{
		std::shared_ptr<NestedDomainDescriptor> Descriptor;
		std::shared_ptr<EvidencePolicyDescriptor> EvidencePolicy;
		std::shared_ptr<ObservabilityDescriptor> Observability;
		EvidenceVisibilityClass EvidenceClass;
		bool RequiresGuestProjection;

		NestedEvidencePolicyRequest(std::shared_ptr<NestedDomainDescriptor> descriptor,
			std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy,
			std::shared_ptr<ObservabilityDescriptor> observability,
			EvidenceVisibilityClass evidenceClass, bool requiresGuestProjection)
			: Descriptor(descriptor), EvidencePolicy(evidencePolicy), Observability(observability),
			  EvidenceClass(evidenceClass), RequiresGuestProjection(requiresGuestProjection) {}
		};

	struct NestedEvidencePolicyResult final
	{
		const NestedEvidencePolicyDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;

		NestedEvidencePolicyResult(NestedEvidencePolicyDecision decision, std::wstring reason)
			: Decision(decision), Reason(reason), IsAllowed(decision == NestedEvidencePolicyDecision::Allowed) {}
	};
}
