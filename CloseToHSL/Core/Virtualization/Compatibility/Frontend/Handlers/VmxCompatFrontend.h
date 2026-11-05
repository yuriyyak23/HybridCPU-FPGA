#pragma once
#include <memory>
#include <string>
#include <utility>
#include "VmxCompatFrontend.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmxCompatFrontendAdmissionDecision : unsigned char
	{
		Allowed = 0,
		DecodeBoundaryDenied = 1,
		ProjectionBoundaryDenied = 2,
		RetireBoundaryDenied = 3,
		DescriptorValidationDenied = 4,
		CapabilityValidationDenied = 5,
		RuntimeAuthorityDenied = 6,
		DirectVmcsAuthorityDenied = 7
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxCompatFrontendAdmissionRequest(bool DecodeBoundaryValidated, bool ProjectionBoundaryValidated, bool RetireBoundaryValidated, bool DescriptorValidated, bool CapabilityValidated, bool RuntimeAuthorityValidated, bool AttemptsDirectVmcsAuthority);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxCompatFrontendAdmissionResult(VmxCompatFrontendAdmissionDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == VmxCompatFrontendAdmissionDecision.Allowed;
//			}
//		}
//
//		public static VmxCompatFrontendAdmissionResult Allowed
//		{
//			get;
//		} = new(VmxCompatFrontendAdmissionDecision.Allowed, "VMX compatibility frontend admission allowed.");
//
//		public static VmxCompatFrontendAdmissionResult Denied(VmxCompatFrontendAdmissionDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}


struct VmxCompatFrontendAdmissionRequest final
{
    bool DecodeBoundaryValidated = false;
    bool ProjectionBoundaryValidated = false;
    bool RetireBoundaryValidated = false;
    bool DescriptorValidated = false;
    bool CapabilityValidated = false;
    bool RuntimeAuthorityValidated = false;
    bool AttemptsDirectVmcsAuthority = false;
};
struct VmxCompatFrontendAdmissionResult final
{
    VmxCompatFrontendAdmissionDecision Decision{};
    std::wstring Reason;
    bool IsAllowed = false;
    VmxCompatFrontendAdmissionResult(VmxCompatFrontendAdmissionDecision decision, std::wstring reason)
        : Decision(decision), Reason(std::move(reason)), IsAllowed(decision == VmxCompatFrontendAdmissionDecision::Allowed) {}
    static std::shared_ptr<VmxCompatFrontendAdmissionResult> Allowed() { return std::make_shared<VmxCompatFrontendAdmissionResult>(VmxCompatFrontendAdmissionDecision::Allowed, L"VMX compatibility frontend admission allowed."); }
    static std::shared_ptr<VmxCompatFrontendAdmissionResult> Denied(VmxCompatFrontendAdmissionDecision decision, std::wstring reason) { return std::make_shared<VmxCompatFrontendAdmissionResult>(decision, std::move(reason)); }
};
}
