#pragma once
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class MemoryDomainRuntimeDecision : unsigned char
	{
		Allowed = 0,
		MissingDescriptor = 1,
		DescriptorAuthorityDenied = 2,
		MissingAddressSpace = 3,
		MissingTranslationPolicy = 4,
		InvalidTranslationControl = 5,
		SecondStageTranslationDenied = 6,
		MissingDirtyTrackingDescriptor = 7
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public readonly record struct MemoryDomainRuntimeRequest(MemoryDomainDescriptor? Descriptor, bool RequiresAddressSpace, bool RequiresTranslationPolicy, bool RequiresSecondStageTranslation, bool RequiresDirtyTracking);
//	public readonly record struct MemoryDomainRuntimeRequest(MemoryDomainDescriptor Descriptor, bool RequiresAddressSpace, bool RequiresTranslationPolicy, bool RequiresSecondStageTranslation, bool RequiresDirtyTracking);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MemoryDomainRuntimeResult(MemoryDomainRuntimeDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == MemoryDomainRuntimeDecision.Allowed;
//			}
//		}
//
//		public static MemoryDomainRuntimeResult Allowed
//		{
//			get;
//		} = new(MemoryDomainRuntimeDecision.Allowed, "Memory domain runtime admission allowed.");
//
//		public static MemoryDomainRuntimeResult Denied(MemoryDomainRuntimeDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}


class MemoryDomainDescriptor;
struct MemoryDomainRuntimeRequest final {
    std::shared_ptr<MemoryDomainDescriptor> Descriptor;
    bool RequiresAddressSpace = false, RequiresTranslationPolicy = false, RequiresSecondStageTranslation = false, RequiresDirtyTracking = false;
};
struct MemoryDomainRuntimeResult final {
    const MemoryDomainRuntimeDecision Decision; const std::wstring Reason;
    MemoryDomainRuntimeResult(MemoryDomainRuntimeDecision decision, std::wstring reason) : Decision(decision), Reason(std::move(reason)) {}
    bool isAllowed() const noexcept { return Decision == MemoryDomainRuntimeDecision::Allowed; }
    static const std::shared_ptr<MemoryDomainRuntimeResult> Allowed;
    static std::shared_ptr<MemoryDomainRuntimeResult> Denied(MemoryDomainRuntimeDecision decision, std::wstring reason) { return std::make_shared<MemoryDomainRuntimeResult>(decision, std::move(reason)); }
};
inline const std::shared_ptr<MemoryDomainRuntimeResult> MemoryDomainRuntimeResult::Allowed = std::make_shared<MemoryDomainRuntimeResult>(MemoryDomainRuntimeDecision::Allowed, L"Memory domain runtime admission allowed.");
}
