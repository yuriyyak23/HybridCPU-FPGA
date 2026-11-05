#pragma once
#include "../../Generated/AliasMaps/CompatAliasMap.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmxCompatProjectionDecision : unsigned char
	{
		Allowed = 0,
		UnknownAlias = 1,
		UnfrozenAbiAlias = 2,
		DescriptorValidationDenied = 3,
		EvidenceValidationDenied = 4,
		AuthoritativeMutationDenied = 5
	};

struct VmxCompatProjectionRequest {
    CompatAliasSourceKind SourceKind{}; std::wstring SourceName;
    bool DescriptorValidated{}, EvidenceValidated{}, AttemptsAuthoritativeMutation{};
};
struct VmxCompatProjectionResult {
    VmxCompatProjectionDecision Decision{}; std::shared_ptr<CompatAliasMapEntry> Alias; std::wstring Reason;
    bool getIsAllowed() const { return Decision == VmxCompatProjectionDecision::Allowed; }
    static std::shared_ptr<VmxCompatProjectionResult> Allowed(std::shared_ptr<CompatAliasMapEntry> alias) {
        return std::make_shared<VmxCompatProjectionResult>(VmxCompatProjectionResult{VmxCompatProjectionDecision::Allowed, std::move(alias), L""});
    }
    static std::shared_ptr<VmxCompatProjectionResult> Denied(VmxCompatProjectionDecision decision, std::wstring reason) {
        return std::make_shared<VmxCompatProjectionResult>(VmxCompatProjectionResult{decision, std::make_shared<CompatAliasMapEntry>(), std::move(reason)});
    }
};
}