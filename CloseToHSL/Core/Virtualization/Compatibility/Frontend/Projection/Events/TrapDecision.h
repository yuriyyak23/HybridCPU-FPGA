#pragma once
#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include "../../../FrozenAbi/VmcsFieldAliases/VmxExitQualification.h"
#include "../../../../../Runtime/Events/Traps/YAKSys_Hybrid_CPU.Core.TrapRequest.h"
#include <memory>
namespace YAKSys_Hybrid_CPU::Core {
class TrapDecision {
public:
    const bool ShouldExit; const VmExitReason ExitReason; const std::shared_ptr<VmxExitQualification> ExitQualification; const std::shared_ptr<TrapRequest> Request;
    TrapDecision(bool exit, VmExitReason reason, std::shared_ptr<VmxExitQualification> qualification, std::shared_ptr<TrapRequest> request) : ShouldExit(exit), ExitReason(reason), ExitQualification(std::move(qualification)), Request(std::move(request)) {}
    static std::shared_ptr<TrapDecision> NoExit(std::shared_ptr<TrapRequest> request) { return std::make_shared<TrapDecision>(false,VmExitReason::None,VmxExitQualification::None,std::move(request)); }
    static std::shared_ptr<TrapDecision> Exit(std::shared_ptr<TrapRequest> request, VmExitReason reason) {
        auto leaf=static_cast<unsigned short>((static_cast<unsigned char>(request->TargetKind) << 8) | static_cast<unsigned char>(request->AccessType));
        auto descriptor=request->TargetKind == TrapTargetKind::CompatibilityOperation ? request->Target : request->Target | ((request->Auxiliary & 0xffffULL) << 16);
        return std::make_shared<TrapDecision>(true,reason,std::make_shared<VmxExitQualification>(leaf,VmxInvalidationScope::None,descriptor),std::move(request));
    }
};
}