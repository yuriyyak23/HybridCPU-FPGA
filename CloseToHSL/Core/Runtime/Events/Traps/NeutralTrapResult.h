#pragma once
#include "YAKSys_Hybrid_CPU.Core.TrapRequest.h"
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class NeutralTrapResultKind : unsigned char
	{
		None = 0,
		InstructionIntercept = 1,
		CsrIntercept = 2,
		CompatibilityOperationIntercept = 3,
		MemoryIntercept = 4,
		LaneOperationIntercept = 5,
		PreemptionTimerExpired = 6,
		SecurityPolicyViolation = 7
	};

class NeutralTrapResult final {
public:
    const bool ShouldTrap; const NeutralTrapResultKind Kind; const std::shared_ptr<TrapRequest> Request;
    NeutralTrapResult(bool shouldTrap, NeutralTrapResultKind kind, std::shared_ptr<TrapRequest> request) : ShouldTrap(shouldTrap), Kind(kind), Request(std::move(request)) {}
    static std::shared_ptr<NeutralTrapResult> Continue(std::shared_ptr<TrapRequest> request) { return std::make_shared<NeutralTrapResult>(false, NeutralTrapResultKind::None, std::move(request)); }
    static std::shared_ptr<NeutralTrapResult> Trap(std::shared_ptr<TrapRequest> request, NeutralTrapResultKind kind) { return std::make_shared<NeutralTrapResult>(kind != NeutralTrapResultKind::None, kind, std::move(request)); }
    static std::shared_ptr<NeutralTrapResult> Denied(std::shared_ptr<TrapRequest> request) { return Trap(std::move(request), NeutralTrapResultKind::SecurityPolicyViolation); }
};
}