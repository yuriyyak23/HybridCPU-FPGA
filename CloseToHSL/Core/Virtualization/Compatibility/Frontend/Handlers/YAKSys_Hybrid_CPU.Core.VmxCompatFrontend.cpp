#include "YAKSys_Hybrid_CPU.Core.VmxCompatFrontend.h"
#include "VmxCompatFrontend.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VmxCompatFrontendAdmissionResult> VmxCompatFrontend::ValidateAdmission(std::shared_ptr<VmxCompatFrontendAdmissionRequest> request)
	{
		if (!request->DecodeBoundaryValidated)
		{
			return VmxCompatFrontendAdmissionResult::Denied(VmxCompatFrontendAdmissionDecision::DecodeBoundaryDenied, L"VMX compatibility frontend requires decode-boundary validation.");
		}

		if (!request->ProjectionBoundaryValidated)
		{
			return VmxCompatFrontendAdmissionResult::Denied(VmxCompatFrontendAdmissionDecision::ProjectionBoundaryDenied, L"VMX compatibility frontend requires projection-boundary validation.");
		}

		if (!request->RetireBoundaryValidated)
		{
			return VmxCompatFrontendAdmissionResult::Denied(VmxCompatFrontendAdmissionDecision::RetireBoundaryDenied, L"VMX compatibility frontend requires retire-boundary validation.");
		}

		if (!request->DescriptorValidated)
		{
			return VmxCompatFrontendAdmissionResult::Denied(VmxCompatFrontendAdmissionDecision::DescriptorValidationDenied, L"VMX compatibility frontend requires descriptor validation.");
		}

		if (!request->CapabilityValidated)
		{
			return VmxCompatFrontendAdmissionResult::Denied(VmxCompatFrontendAdmissionDecision::CapabilityValidationDenied, L"VMX compatibility frontend requires capability validation.");
		}

		if (!request->RuntimeAuthorityValidated)
		{
			return VmxCompatFrontendAdmissionResult::Denied(VmxCompatFrontendAdmissionDecision::RuntimeAuthorityDenied, L"VMX compatibility frontend requires runtime-owned authority validation.");
		}

		if (request->AttemptsDirectVmcsAuthority)
		{
			return VmxCompatFrontendAdmissionResult::Denied(VmxCompatFrontendAdmissionDecision::DirectVmcsAuthorityDenied, L"VMX compatibility frontend cannot make compatibility projection state authoritative.");
		}

		return VmxCompatFrontendAdmissionResult::Allowed();
	}

	bool VmxCompatFrontend::CanAdmit(std::shared_ptr<VmxCompatFrontendAdmissionRequest> request)
	{
		return ValidateAdmission(request)->IsAllowed;
	}
}
