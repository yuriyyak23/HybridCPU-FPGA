#pragma once
#include <memory>
#include <string>
#include <utility>
#include "VmcsV2Header.h"
#include "VmcsV2DescriptorProjection.h"

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmcsV2DescriptorProjectionDecision : unsigned char
	{
		Allowed = 0,
		MissingGeneratedAliasMap = 1,
		DescriptorValidationDenied = 2,
		EvidenceValidationDenied = 3,
		WritableProjectionDenied = 4,
		AuthoritativeMutationDenied = 5
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmcsV2DescriptorProjectionRequest(bool GeneratedAliasMapDeclared, bool DescriptorValidated, bool EvidenceValidated, bool ReadOnlyProjection, bool AttemptsAuthoritativeMutation);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmcsV2DescriptorProjectionResult(VmcsV2DescriptorProjectionDecision Decision, uint RevisionId, uint SizeBytes, string Baseline, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == VmcsV2DescriptorProjectionDecision.Allowed;
//			}
//		}
//
//		public static VmcsV2DescriptorProjectionResult Allowed()
//		{
//			return new(VmcsV2DescriptorProjectionDecision.Allowed, VmcsV2Header.CurrentRevisionId, VmcsV2Header.CurrentSizeBytes, VmcsV2Header.CompatibilityBaseline, string.Empty);
//		}
//
//		public static VmcsV2DescriptorProjectionResult Denied(VmcsV2DescriptorProjectionDecision decision, string reason)
//		{
//			return new(decision, 0, 0, string.Empty, reason);
//		}
//	}


struct VmcsV2DescriptorProjectionRequest final
{
    bool GeneratedAliasMapDeclared = false;
    bool DescriptorValidated = false;
    bool EvidenceValidated = false;
    bool ReadOnlyProjection = false;
    bool AttemptsAuthoritativeMutation = false;
};
struct VmcsV2DescriptorProjectionResult final
{
    VmcsV2DescriptorProjectionDecision Decision{};
    unsigned int RevisionId = 0;
    unsigned int SizeBytes = 0;
    std::wstring Baseline;
    std::wstring Reason;
    bool IsAllowed = false;
    VmcsV2DescriptorProjectionResult(VmcsV2DescriptorProjectionDecision decision, unsigned int revision, unsigned int size, std::wstring baseline, std::wstring reason)
        : Decision(decision), RevisionId(revision), SizeBytes(size), Baseline(std::move(baseline)), Reason(std::move(reason)), IsAllowed(decision == VmcsV2DescriptorProjectionDecision::Allowed) {}
    static std::shared_ptr<VmcsV2DescriptorProjectionResult> Allowed() { return std::make_shared<VmcsV2DescriptorProjectionResult>(VmcsV2DescriptorProjectionDecision::Allowed, VmcsV2Header::CurrentRevisionId, VmcsV2Header::CurrentSizeBytes, VmcsV2Header::CompatibilityBaseline, L""); }
    static std::shared_ptr<VmcsV2DescriptorProjectionResult> Denied(VmcsV2DescriptorProjectionDecision decision, std::wstring reason) { return std::make_shared<VmcsV2DescriptorProjectionResult>(decision, 0, 0, L"", std::move(reason)); }
};
}
