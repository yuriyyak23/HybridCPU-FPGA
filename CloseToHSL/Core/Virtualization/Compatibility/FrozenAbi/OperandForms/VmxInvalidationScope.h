#pragma once

#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class VmxInvalidationScope : unsigned char
	{
		None = 0,
		SingleContext = 1,
		AllContexts = 2,
		SingleAddress = 3
	};

	enum class VmxInvalidationScopeAdmissionDecision : unsigned char
	{
		Allowed = 0,
		UnsupportedScope = 1,
		DescriptorValidationDenied = 2,
		CapabilityValidationDenied = 3,
		RuntimeInvalidationDenied = 4,
		TranslationAuthorityDenied = 5,
		DirectMmuMutationDenied = 6
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxInvalidationScopeAdmissionRequest(VmxInvalidationScope Scope, bool DescriptorValidated, bool CapabilityValidated, bool RuntimeInvalidationValidated, bool TranslationAuthorityValidated, bool AttemptsDirectMmuMutation);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxInvalidationScopeAdmissionResult(VmxInvalidationScopeAdmissionDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == VmxInvalidationScopeAdmissionDecision.Allowed;
//			}
//		}
//
//		public static VmxInvalidationScopeAdmissionResult Allowed
//		{
//			get;
//		} = new(VmxInvalidationScopeAdmissionDecision.Allowed, "VMX invalidation scope admission allowed.");
//
//		public static VmxInvalidationScopeAdmissionResult Denied(VmxInvalidationScopeAdmissionDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}

	struct VmxInvalidationScopeAdmissionRequest final {
    VmxInvalidationScope Scope = VmxInvalidationScope::None;
    bool DescriptorValidated = false, CapabilityValidated = false, RuntimeInvalidationValidated = false, TranslationAuthorityValidated = false, AttemptsDirectMmuMutation = false;
};
struct VmxInvalidationScopeAdmissionResult final {
    const VmxInvalidationScopeAdmissionDecision Decision;
    const std::wstring Reason;
    VmxInvalidationScopeAdmissionResult(VmxInvalidationScopeAdmissionDecision decision, std::wstring reason) : Decision(decision), Reason(std::move(reason)) {}
    bool isAllowed() const noexcept { return Decision == VmxInvalidationScopeAdmissionDecision::Allowed; }
    static const std::shared_ptr<VmxInvalidationScopeAdmissionResult> Allowed;
    static std::shared_ptr<VmxInvalidationScopeAdmissionResult> Denied(VmxInvalidationScopeAdmissionDecision decision, std::wstring reason) { return std::make_shared<VmxInvalidationScopeAdmissionResult>(decision, std::move(reason)); }
};
inline const std::shared_ptr<VmxInvalidationScopeAdmissionResult> VmxInvalidationScopeAdmissionResult::Allowed = std::make_shared<VmxInvalidationScopeAdmissionResult>(VmxInvalidationScopeAdmissionDecision::Allowed, L"VMX invalidation scope admission allowed.");
class VmxInvalidationScopeAdmissionPolicy final : public std::enable_shared_from_this<VmxInvalidationScopeAdmissionPolicy>
	{
	public:
		std::shared_ptr<VmxInvalidationScopeAdmissionResult> Validate(std::shared_ptr<VmxInvalidationScopeAdmissionRequest> request);

		bool CanAdmit(std::shared_ptr<VmxInvalidationScopeAdmissionRequest> request);

		static bool IsSupportedScope(VmxInvalidationScope scope);
	};
}
