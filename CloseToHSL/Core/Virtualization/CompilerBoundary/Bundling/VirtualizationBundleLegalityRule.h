#pragma once
#include <memory>
#include <string>
#include <utility>
#include "../../../Runtime/Domains/Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.BundleLegalityDescriptor.h"
#include "../LaneBinding/VirtualizationLaneBindingPolicy.h"
#include "../Scheduling/VirtualizationSystemSingletonSchedulingRule.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VirtualizationBundleLegalityDecision : unsigned char
	{
		Allowed = 0,
		MissingDescriptor = 1,
		DescriptorValidationDenied = 2,
		RuntimeAuthorityRequired = 3,
		RuntimeValidationDenied = 4,
		CompilerEvidenceAsAuthorityDenied = 5,
		CompatibilityProjectionDenied = 6,
		LaneBindingDenied = 7,
		SchedulingDenied = 8
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public readonly record struct VirtualizationBundleLegalityRequest(BundleLegalityDescriptor? Descriptor, VirtualizationLaneBindingDecision LaneBindingDecision, VirtualizationSchedulingDecision SchedulingDecision, bool DescriptorValidated, bool RuntimeValidated, bool UsesCompilerEvidenceAsAuthority, bool RequiresCompatibilityProjection);
//	public readonly record struct VirtualizationBundleLegalityRequest(BundleLegalityDescriptor Descriptor, VirtualizationLaneBindingDecision LaneBindingDecision, VirtualizationSchedulingDecision SchedulingDecision, bool DescriptorValidated, bool RuntimeValidated, bool UsesCompilerEvidenceAsAuthority, bool RequiresCompatibilityProjection);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VirtualizationBundleLegalityResult(VirtualizationBundleLegalityDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == VirtualizationBundleLegalityDecision.Allowed;
//			}
//		}
//
//		public static VirtualizationBundleLegalityResult Allowed()
//		{
//			return new(VirtualizationBundleLegalityDecision.Allowed, "Bundle legality is runtime-authorized and validated.");
//		}
//
//		public static VirtualizationBundleLegalityResult Denied(VirtualizationBundleLegalityDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}


struct VirtualizationBundleLegalityRequest final
{
    std::shared_ptr<BundleLegalityDescriptor> Descriptor;
    VirtualizationLaneBindingDecision LaneBindingDecision{};
    VirtualizationSchedulingDecision SchedulingDecision{};
    bool DescriptorValidated = false;
    bool RuntimeValidated = false;
    bool UsesCompilerEvidenceAsAuthority = false;
    bool RequiresCompatibilityProjection = false;
};
struct VirtualizationBundleLegalityResult final
{
    VirtualizationBundleLegalityDecision Decision{};
    std::wstring Reason;
    bool IsAllowed = false;
    VirtualizationBundleLegalityResult(VirtualizationBundleLegalityDecision decision, std::wstring reason)
        : Decision(decision), Reason(std::move(reason)), IsAllowed(decision == VirtualizationBundleLegalityDecision::Allowed) {}
    static std::shared_ptr<VirtualizationBundleLegalityResult> Allowed() { return std::make_shared<VirtualizationBundleLegalityResult>(VirtualizationBundleLegalityDecision::Allowed, L"Bundle legality is runtime-authorized and validated."); }
    static std::shared_ptr<VirtualizationBundleLegalityResult> Denied(VirtualizationBundleLegalityDecision decision, std::wstring reason) { return std::make_shared<VirtualizationBundleLegalityResult>(decision, std::move(reason)); }
};
}
