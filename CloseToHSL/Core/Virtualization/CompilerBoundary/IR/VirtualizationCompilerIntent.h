#pragma once
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VirtualizationCompilerIntentKind : unsigned char
	{
		None = 0,
		CompatibilityFrontendOperation = 1,
		DescriptorProjection = 2,
		DirectSubstrateMutation = 3,
		HostEvidencePublication = 4,
		NativeTokenPublication = 5
	};

	enum class VirtualizationCompilerIntentDecision : unsigned char
	{
		Allowed = 0,
		MissingIntent = 1,
		DeniedNonCompatibilityFrontend = 2,
		DeniedDirectSubstrateMutation = 3,
		DeniedHostEvidenceIntent = 4,
		DeniedNativeTokenIntent = 5,
		DeniedUnvalidatedDescriptor = 6
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VirtualizationCompilerIntentRequest(VirtualizationCompilerIntentKind IntentKind, bool IsCompatibilityFrontend, bool DescriptorValidated, bool EmitsHostOwnedEvidence, bool EmitsNativeLaneToken, bool AttemptsDirectSubstrateMutation);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VirtualizationCompilerIntentResult(VirtualizationCompilerIntentDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == VirtualizationCompilerIntentDecision.Allowed;
//			}
//		}
//
//		public static VirtualizationCompilerIntentResult Allowed
//		{
//			get;
//		} = new(VirtualizationCompilerIntentDecision.Allowed, string.Empty);
//
//		public static VirtualizationCompilerIntentResult Denied(VirtualizationCompilerIntentDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}


struct VirtualizationCompilerIntentRequest final
{
    VirtualizationCompilerIntentKind IntentKind{};
    bool IsCompatibilityFrontend = false;
    bool DescriptorValidated = false;
    bool EmitsHostOwnedEvidence = false;
    bool EmitsNativeLaneToken = false;
    bool AttemptsDirectSubstrateMutation = false;
};
struct VirtualizationCompilerIntentResult final
{
    VirtualizationCompilerIntentDecision Decision{};
    std::wstring Reason;
    bool IsAllowed = false;
    VirtualizationCompilerIntentResult(VirtualizationCompilerIntentDecision decision, std::wstring reason)
        : Decision(decision), Reason(std::move(reason)), IsAllowed(decision == VirtualizationCompilerIntentDecision::Allowed) {}
    static std::shared_ptr<VirtualizationCompilerIntentResult> Allowed() { return std::make_shared<VirtualizationCompilerIntentResult>(VirtualizationCompilerIntentDecision::Allowed, L""); }
    static std::shared_ptr<VirtualizationCompilerIntentResult> Denied(VirtualizationCompilerIntentDecision decision, std::wstring reason) { return std::make_shared<VirtualizationCompilerIntentResult>(decision, std::move(reason)); }
};
}
