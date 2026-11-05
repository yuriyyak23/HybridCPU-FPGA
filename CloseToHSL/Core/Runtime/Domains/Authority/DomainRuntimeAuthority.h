#pragma once
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class DomainRuntimeAuthorityDecision : unsigned char
	{
		Allowed = 0,
		MissingRuntimeContext = 1,
		MissingRootAuthority = 2,
		FrontendActivationDenied = 3,
		CapabilityDenied = 4,
		MutationDenied = 5,
		ProjectionDenied = 6
	};

struct DomainRuntimeAuthorityResult {
    const DomainRuntimeAuthorityDecision Decision;
    const std::wstring Message;
    const bool IsAllowed;
    DomainRuntimeAuthorityResult(DomainRuntimeAuthorityDecision decision, std::wstring message)
        : Decision(decision), Message(std::move(message)), IsAllowed(decision == DomainRuntimeAuthorityDecision::Allowed) {}
    static std::shared_ptr<DomainRuntimeAuthorityResult> Denied(DomainRuntimeAuthorityDecision decision, std::wstring message) {
        return std::make_shared<DomainRuntimeAuthorityResult>(decision, std::move(message));
    }
    static const std::shared_ptr<DomainRuntimeAuthorityResult> Allowed;
};
inline const std::shared_ptr<DomainRuntimeAuthorityResult> DomainRuntimeAuthorityResult::Allowed =
    std::make_shared<DomainRuntimeAuthorityResult>(DomainRuntimeAuthorityDecision::Allowed, L"");
}