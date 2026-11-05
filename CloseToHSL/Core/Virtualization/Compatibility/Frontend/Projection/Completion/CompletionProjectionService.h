#pragma once
#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include <memory>
namespace YAKSys_Hybrid_CPU::Core {
struct VmxCompletionProjection {
    const VmExitReason ExitReason; const unsigned long long ExitQualification, GuestPhysicalAddress, EptViolationQualification;
    VmxCompletionProjection(VmExitReason reason, unsigned long long qualification, unsigned long long address, unsigned long long aux)
        : ExitReason(reason), ExitQualification(qualification), GuestPhysicalAddress(address), EptViolationQualification(aux) {}
    static const std::shared_ptr<VmxCompletionProjection> None;
};
inline const std::shared_ptr<VmxCompletionProjection> VmxCompletionProjection::None = std::make_shared<VmxCompletionProjection>(VmExitReason::None, 0, 0, 0);
}