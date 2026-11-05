#pragma once

#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class VmxFunctionLeaf : int
	{
		None = 0,
		CapabilityQuery = 1,
		Lane7QueryCaps = 7,
		Lane7Submit = 8
	};

	enum class VmxFunctionLeafAdmissionDecision : unsigned char
	{
		Allowed = 0,
		UnsupportedLeaf = 1,
		FrozenAbiRequired = 2,
		DescriptorValidationDenied = 3,
		CapabilityValidationDenied = 4,
		Lane7GrantDenied = 5,
		FastPathGrantDenied = 6
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxFunctionLeafAdmissionRequest(VmxFunctionLeaf Leaf, bool IsFrozenAbiSurface, bool DescriptorValidated, bool CapabilityValidated, bool Lane7GrantValidated, bool UsesFastPath);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxFunctionLeafAdmissionResult(VmxFunctionLeafAdmissionDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == VmxFunctionLeafAdmissionDecision.Allowed;
//			}
//		}
//
//		public static VmxFunctionLeafAdmissionResult Allowed
//		{
//			get;
//		} = new(VmxFunctionLeafAdmissionDecision.Allowed, "VMFUNC leaf admission allowed.");
//
//		public static VmxFunctionLeafAdmissionResult Denied(VmxFunctionLeafAdmissionDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}

	struct VmxFunctionLeafAdmissionRequest final {
    VmxFunctionLeaf Leaf = VmxFunctionLeaf::None;
    bool IsFrozenAbiSurface = false, DescriptorValidated = false, CapabilityValidated = false, Lane7GrantValidated = false, UsesFastPath = false;
};
struct VmxFunctionLeafAdmissionResult final {
    const VmxFunctionLeafAdmissionDecision Decision;
    const std::wstring Reason;
    VmxFunctionLeafAdmissionResult(VmxFunctionLeafAdmissionDecision decision, std::wstring reason) : Decision(decision), Reason(std::move(reason)) {}
    bool isAllowed() const noexcept { return Decision == VmxFunctionLeafAdmissionDecision::Allowed; }
    static const std::shared_ptr<VmxFunctionLeafAdmissionResult> Allowed;
    static std::shared_ptr<VmxFunctionLeafAdmissionResult> Denied(VmxFunctionLeafAdmissionDecision decision, std::wstring reason) { return std::make_shared<VmxFunctionLeafAdmissionResult>(decision, std::move(reason)); }
};
inline const std::shared_ptr<VmxFunctionLeafAdmissionResult> VmxFunctionLeafAdmissionResult::Allowed = std::make_shared<VmxFunctionLeafAdmissionResult>(VmxFunctionLeafAdmissionDecision::Allowed, L"VMFUNC leaf admission allowed.");
class VmxFunctionLeafAdmissionPolicy final : public std::enable_shared_from_this<VmxFunctionLeafAdmissionPolicy>
	{
	public:
		std::shared_ptr<VmxFunctionLeafAdmissionResult> Validate(std::shared_ptr<VmxFunctionLeafAdmissionRequest> request);

		bool CanAdmit(std::shared_ptr<VmxFunctionLeafAdmissionRequest> request);

		static bool IsSupportedLeaf(VmxFunctionLeaf leaf);

		static bool RequiresLane7Grant(VmxFunctionLeaf leaf);
	};
}
