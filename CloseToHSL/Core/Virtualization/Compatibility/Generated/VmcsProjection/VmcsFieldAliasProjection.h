#pragma once
#include "../../FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "../../../../Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmcsFieldAliasAccess : unsigned char
	{
		Read = 0,
		Write = 1
	};

	enum class VmcsFieldAliasDecision : unsigned char
	{
		Allowed = 0,
		UndefinedField = 1,
		MissingGeneratedAlias = 2,
		DescriptorValidationDenied = 3,
		EvidencePolicyDenied = 4,
		HostOwnedEvidenceDenied = 5,
		WriteDenied = 6
	};

struct VmcsFieldAliasRequest {
    VmcsField Field{}; VmcsFieldAliasAccess Access{}; EvidenceVisibilityClass EvidenceClass{};
    bool GeneratedAliasDeclared{}, DescriptorValidated{}, AllowWrite{};
};
struct VmcsFieldAliasResult {
    const VmcsFieldAliasDecision Decision; const VmcsField Field; const std::wstring Reason; const bool IsAllowed;
    VmcsFieldAliasResult(VmcsFieldAliasDecision decision, VmcsField field, std::wstring reason) : Decision(decision), Field(field), Reason(std::move(reason)), IsAllowed(decision == VmcsFieldAliasDecision::Allowed) {}
    static std::shared_ptr<VmcsFieldAliasResult> Allowed(VmcsField field) { return std::make_shared<VmcsFieldAliasResult>(VmcsFieldAliasDecision::Allowed, field, L""); }
    static std::shared_ptr<VmcsFieldAliasResult> Denied(VmcsFieldAliasDecision decision, VmcsField field, std::wstring reason) { return std::make_shared<VmcsFieldAliasResult>(decision, field, std::move(reason)); }
};
}