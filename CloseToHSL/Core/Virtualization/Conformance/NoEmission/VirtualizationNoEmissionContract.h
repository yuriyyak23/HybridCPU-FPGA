#pragma once
#include <memory>
#include <string>
#include <utility>
#include "../../CompilerBoundary/NoEmission/VirtualizationNoEmissionRegressionGate.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VirtualizationNoEmissionContractDecision : unsigned char
	{
		Allowed = 0,
		NoEmissionGateDenied = 1,
		MissingGoldenArtifact = 2,
		GeneratedProjectionRequired = 3,
		HostEvidenceEmissionDenied = 4,
		NativeTokenEmissionDenied = 5
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VirtualizationNoEmissionContractRequest(VirtualizationEmissionRequest Emission, bool GoldenArtifactCaptured);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VirtualizationNoEmissionContractResult(VirtualizationNoEmissionContractDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == VirtualizationNoEmissionContractDecision.Allowed;
//			}
//		}
//
//		public static VirtualizationNoEmissionContractResult Allowed
//		{
//			get;
//		} = new(VirtualizationNoEmissionContractDecision.Allowed, "No-emission conformance contract allowed.");
//
//		public static VirtualizationNoEmissionContractResult Denied(VirtualizationNoEmissionContractDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}


struct VirtualizationNoEmissionContractRequest final
{
    VirtualizationEmissionRequest Emission{};
    bool GoldenArtifactCaptured = false;
};
struct VirtualizationNoEmissionContractResult final
{
    VirtualizationNoEmissionContractDecision Decision{};
    std::wstring Reason;
    bool IsAllowed = false;
    VirtualizationNoEmissionContractResult(VirtualizationNoEmissionContractDecision decision, std::wstring reason)
        : Decision(decision), Reason(std::move(reason)), IsAllowed(decision == VirtualizationNoEmissionContractDecision::Allowed) {}
    static const std::shared_ptr<VirtualizationNoEmissionContractResult> Allowed;
    static std::shared_ptr<VirtualizationNoEmissionContractResult> Denied(VirtualizationNoEmissionContractDecision decision, std::wstring reason) { return std::make_shared<VirtualizationNoEmissionContractResult>(decision, std::move(reason)); }
};
inline const std::shared_ptr<VirtualizationNoEmissionContractResult> VirtualizationNoEmissionContractResult::Allowed =
    std::make_shared<VirtualizationNoEmissionContractResult>(VirtualizationNoEmissionContractDecision::Allowed, L"No-emission conformance contract allowed.");
}
