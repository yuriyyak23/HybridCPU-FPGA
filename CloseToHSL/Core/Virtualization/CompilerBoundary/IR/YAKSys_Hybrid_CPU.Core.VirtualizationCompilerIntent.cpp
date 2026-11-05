#include "YAKSys_Hybrid_CPU.Core.VirtualizationCompilerIntent.h"
#include "VirtualizationCompilerIntent.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VirtualizationCompilerIntentResult> VirtualizationCompilerIntent::Evaluate(std::shared_ptr<VirtualizationCompilerIntentRequest> request)
	{
		if (request->IntentKind == VirtualizationCompilerIntentKind::None)
		{
			return VirtualizationCompilerIntentResult::Denied(VirtualizationCompilerIntentDecision::MissingIntent, L"Virtualization compiler intent must be explicit.");
		}

		if (!request->IsCompatibilityFrontend)
		{
			return VirtualizationCompilerIntentResult::Denied(VirtualizationCompilerIntentDecision::DeniedNonCompatibilityFrontend, L"VMX compiler intent may only target the compatibility frontend boundary.");
		}

		if (!request->DescriptorValidated)
		{
			return VirtualizationCompilerIntentResult::Denied(VirtualizationCompilerIntentDecision::DeniedUnvalidatedDescriptor, L"Virtualization compiler intent requires descriptor validation.");
		}

		if (request->AttemptsDirectSubstrateMutation || request->IntentKind == VirtualizationCompilerIntentKind::DirectSubstrateMutation)
		{
			return VirtualizationCompilerIntentResult::Denied(VirtualizationCompilerIntentDecision::DeniedDirectSubstrateMutation, L"Compiler intent cannot directly mutate authoritative substrate state.");
		}

		if (request->EmitsHostOwnedEvidence || request->IntentKind == VirtualizationCompilerIntentKind::HostEvidencePublication)
		{
			return VirtualizationCompilerIntentResult::Denied(VirtualizationCompilerIntentDecision::DeniedHostEvidenceIntent, L"Compiler intent cannot emit host-owned evidence.");
		}

		if (request->EmitsNativeLaneToken || request->IntentKind == VirtualizationCompilerIntentKind::NativeTokenPublication)
		{
			return VirtualizationCompilerIntentResult::Denied(VirtualizationCompilerIntentDecision::DeniedNativeTokenIntent, L"Compiler intent cannot emit native lane tokens.");
		}

		return VirtualizationCompilerIntentResult::Allowed();
	}
}
