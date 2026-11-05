#pragma once
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class ExecutionDomainRuntimeDecision : unsigned char
	{
		Allowed = 0,
		MissingDescriptor = 1,
		DescriptorAuthorityDenied = 2,
		MissingBundleLegalityDescriptor = 3,
		MissingSchedulingBudgetDescriptor = 4,
		CompatibilityProjectionDenied = 5
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public readonly record struct ExecutionDomainRuntimeRequest(ExecutionDomainDescriptor? Descriptor, bool RequiresBundleLegality, bool RequiresSchedulingBudget, bool RequiresCompatibilityProjection);
//	public readonly record struct ExecutionDomainRuntimeRequest(ExecutionDomainDescriptor Descriptor, bool RequiresBundleLegality, bool RequiresSchedulingBudget, bool RequiresCompatibilityProjection);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct ExecutionDomainRuntimeResult(ExecutionDomainRuntimeDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == ExecutionDomainRuntimeDecision.Allowed;
//			}
//		}
//
//		public static ExecutionDomainRuntimeResult Allowed
//		{
//			get;
//		} = new(ExecutionDomainRuntimeDecision.Allowed, "Execution domain runtime admission allowed.");
//
//		public static ExecutionDomainRuntimeResult Denied(ExecutionDomainRuntimeDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}


class ExecutionDomainDescriptor;
struct ExecutionDomainRuntimeRequest final {
    std::shared_ptr<ExecutionDomainDescriptor> Descriptor;
    bool RequiresBundleLegality = false, RequiresSchedulingBudget = false, RequiresCompatibilityProjection = false;
};
struct ExecutionDomainRuntimeResult final {
    const ExecutionDomainRuntimeDecision Decision; const std::wstring Reason;
    ExecutionDomainRuntimeResult(ExecutionDomainRuntimeDecision decision, std::wstring reason) : Decision(decision), Reason(std::move(reason)) {}
    bool isAllowed() const noexcept { return Decision == ExecutionDomainRuntimeDecision::Allowed; }
    static const std::shared_ptr<ExecutionDomainRuntimeResult> Allowed;
    static std::shared_ptr<ExecutionDomainRuntimeResult> Denied(ExecutionDomainRuntimeDecision decision, std::wstring reason) { return std::make_shared<ExecutionDomainRuntimeResult>(decision, std::move(reason)); }
};
inline const std::shared_ptr<ExecutionDomainRuntimeResult> ExecutionDomainRuntimeResult::Allowed = std::make_shared<ExecutionDomainRuntimeResult>(ExecutionDomainRuntimeDecision::Allowed, L"Execution domain runtime admission allowed.");
}
