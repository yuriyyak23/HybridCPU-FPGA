#pragma once
#include <memory>
#include <string>
#include <utility>
#include "../IR/VirtualizationCompilerIntent.h"
#include "../NoEmission/VirtualizationNoEmissionRegressionGate.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VirtualizationLoweringDecision : unsigned char
	{
		Allowed = 0,
		IntentDenied = 1,
		DescriptorValidationDenied = 2,
		CapabilityValidationDenied = 3,
		RuntimeValidationDenied = 4,
		NoEmissionDenied = 5,
		DirectHandlerEmissionDenied = 6
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VirtualizationLoweringRequest(VirtualizationCompilerIntentResult IntentResult, VirtualizationEmissionDecision EmissionDecision, bool DescriptorValidated, bool CapabilityValidated, bool RuntimeValidated, bool EmitsDirectVmxHandler);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VirtualizationLoweringResult(VirtualizationLoweringDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == VirtualizationLoweringDecision.Allowed;
//			}
//		}
//
//		public static VirtualizationLoweringResult Allowed
//		{
//			get;
//		} = new(VirtualizationLoweringDecision.Allowed, string.Empty);
//
//		public static VirtualizationLoweringResult Denied(VirtualizationLoweringDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}


struct VirtualizationLoweringRequest final
{
    std::shared_ptr<VirtualizationCompilerIntentResult> IntentResult;
    VirtualizationEmissionDecision EmissionDecision{};
    bool DescriptorValidated = false;
    bool CapabilityValidated = false;
    bool RuntimeValidated = false;
    bool EmitsDirectVmxHandler = false;
};
struct VirtualizationLoweringResult final
{
    VirtualizationLoweringDecision Decision{};
    std::wstring Reason;
    bool IsAllowed = false;
    VirtualizationLoweringResult(VirtualizationLoweringDecision decision, std::wstring reason)
        : Decision(decision), Reason(std::move(reason)), IsAllowed(decision == VirtualizationLoweringDecision::Allowed) {}
    static std::shared_ptr<VirtualizationLoweringResult> Allowed() { return std::make_shared<VirtualizationLoweringResult>(VirtualizationLoweringDecision::Allowed, L""); }
    static std::shared_ptr<VirtualizationLoweringResult> Denied(VirtualizationLoweringDecision decision, std::wstring reason) { return std::make_shared<VirtualizationLoweringResult>(decision, std::move(reason)); }
};
}
