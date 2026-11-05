#include "YAKSys_Hybrid_CPU.Core.VmxCompatRetireBoundary.h"
#include "VmxCompatRetireBoundary.h"

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VmxCompatRetireResult> VmxCompatRetireBoundary::ValidatePublication(std::shared_ptr<VmxCompatRetireRequest> request)
	{
		if (!request->RetireEffect.isValid())
		{
			return VmxCompatRetireResult::Denied(VmxCompatRetireDecision::InvalidRetireEffect, L"Compatibility retire publication requires a valid retire effect.");
		}

		if (!request->DescriptorValidated)
		{
			return VmxCompatRetireResult::Denied(VmxCompatRetireDecision::DescriptorValidationDenied, L"Compatibility retire publication requires descriptor validation.");
		}

		if (!request->EvidenceValidated)
		{
			return VmxCompatRetireResult::Denied(VmxCompatRetireDecision::EvidenceValidationDenied, L"Compatibility retire publication requires evidence validation.");
		}

		if (RequiresCompletionRoute(request->RetireEffect) && !request->CompletionRouteValidated)
		{
			return VmxCompatRetireResult::Denied(VmxCompatRetireDecision::CompletionRouteDenied, L"Compatibility retire publication requires validated completion routing.");
		}

		if (request->RetireEffect.isFaulted() && !IsTypedFault(request->RetireEffect.getCompletionKind()))
		{
			return VmxCompatRetireResult::Denied(VmxCompatRetireDecision::UntypedFaultDenied, L"Faulted VMX retire effects must publish a typed VMFail or VMAbort outcome.");
		}

		return VmxCompatRetireResult::Allowed();
	}

	bool VmxCompatRetireBoundary::RequiresCompletionRoute(VmxRetireEffect effect)
	{
		return effect.getExitGuestContextOnRetire() || effect.getCompletionKind() == VmxCompletionKind::VmExit;
	}

	bool VmxCompatRetireBoundary::IsTypedFault(VmxCompletionKind completionKind)
	{
		return (completionKind == VmxCompletionKind::VmFailValid || completionKind == VmxCompletionKind::VmFailInvalid || completionKind == VmxCompletionKind::VmAbort);
	}
}
