$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\FrozenAbi\VmcsFieldAliases\VmxExitQualification.h'
$s=@'
#pragma once
#include "../OperandForms/VmxInvalidationScope.h"
#include <memory>
namespace YAKSys_Hybrid_CPU::Core {
class VmxExitQualification final {
public:
    const unsigned short Leaf;
    const VmxInvalidationScope Scope;
    const unsigned long long Descriptor;
    VmxExitQualification(unsigned short leaf = 0, VmxInvalidationScope scope = VmxInvalidationScope::None, unsigned long long descriptor = 0) : Leaf(leaf), Scope(scope), Descriptor(descriptor) {}
    unsigned long long Encode() const { return static_cast<unsigned long long>(Leaf) | (static_cast<unsigned long long>(Scope) << 16) | ((Descriptor & 0xffffffffULL) << 32); }
    static const std::shared_ptr<VmxExitQualification> None;
};
inline const std::shared_ptr<VmxExitQualification> VmxExitQualification::None = std::make_shared<VmxExitQualification>();
}
'@
[IO.File]::WriteAllText($p,$s)