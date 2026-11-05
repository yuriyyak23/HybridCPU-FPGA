#pragma once
#include "../Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CapabilityNegotiationDecision : unsigned char
	{
		Granted = 0,
		DeniedEmptyMask = 1,
		DeniedMissingDescriptorSet = 2,
		DeniedMissingHardwareCapability = 3,
		DeniedRuntimeDisabled = 4,
		DeniedDomainNotGranted = 5
	};
	struct CapabilityNegotiationResult final
	{
		const CapabilityNegotiationDecision Decision;
		const std::shared_ptr<CapabilityGrant> Grant;
		CapabilityNegotiationResult(CapabilityNegotiationDecision decision, std::shared_ptr<CapabilityGrant> grant) : Decision(decision), Grant(std::move(grant)) {}
		bool isGranted() const noexcept { return Decision == CapabilityNegotiationDecision::Granted && Grant && Grant->isGranted(); }
		static std::shared_ptr<CapabilityNegotiationResult> Denied(CapabilityNegotiationDecision decision) { return std::make_shared<CapabilityNegotiationResult>(decision, CapabilityGrant::getDenied()); }
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CapabilityNegotiationResult(CapabilityNegotiationDecision Decision, CapabilityGrant Grant)
//	{
//		public bool IsGranted
//		{
//			get
//			{
//				return Decision == CapabilityNegotiationDecision.Granted && Grant.IsGranted;
//			}
//		}
//
//		public static CapabilityNegotiationResult Denied(CapabilityNegotiationDecision decision)
//		{
//			return new(decision, CapabilityGrant.Denied);
//		}
//	}


}
