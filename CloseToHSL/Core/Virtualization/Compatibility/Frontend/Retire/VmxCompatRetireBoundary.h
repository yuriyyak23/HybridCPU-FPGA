#pragma once
#include <memory>
#include <string>
#include <utility>
#include "VmxRetireModel.h"

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmxCompatRetireDecision : unsigned char
	{
		Allowed = 0,
		InvalidRetireEffect = 1,
		DescriptorValidationDenied = 2,
		EvidenceValidationDenied = 3,
		CompletionRouteDenied = 4,
		UntypedFaultDenied = 5
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxCompatRetireResult(VmxCompatRetireDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == VmxCompatRetireDecision.Allowed;
//			}
//		}
//
//		public static VmxCompatRetireResult Allowed
//		{
//			get;
//		} = new(VmxCompatRetireDecision.Allowed, string.Empty);
//
//		public static VmxCompatRetireResult Denied(VmxCompatRetireDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxCompatRetireRequest(VmxRetireEffect RetireEffect, bool DescriptorValidated, bool EvidenceValidated, bool CompletionRouteValidated);


struct VmxCompatRetireResult final
{
    VmxCompatRetireDecision Decision{};
    std::wstring Reason;
    bool IsAllowed = false;
    VmxCompatRetireResult(VmxCompatRetireDecision decision, std::wstring reason)
        : Decision(decision), Reason(std::move(reason)), IsAllowed(decision == VmxCompatRetireDecision::Allowed) {}
    static std::shared_ptr<VmxCompatRetireResult> Allowed() { return std::make_shared<VmxCompatRetireResult>(VmxCompatRetireDecision::Allowed, L""); }
    static std::shared_ptr<VmxCompatRetireResult> Denied(VmxCompatRetireDecision decision, std::wstring reason) { return std::make_shared<VmxCompatRetireResult>(decision, std::move(reason)); }
};
struct VmxCompatRetireRequest final
{
    VmxRetireEffect RetireEffect{};
    bool DescriptorValidated = false;
    bool EvidenceValidated = false;
    bool CompletionRouteValidated = false;
};
}
