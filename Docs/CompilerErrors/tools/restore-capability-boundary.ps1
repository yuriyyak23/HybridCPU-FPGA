$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Capabilities'
$p=Join-Path $base 'CapabilityBoundaryRequirement.h'; $s=[IO.File]::ReadAllText($p)
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); if($i -lt 0){throw 'anchor missing'}
$s=$s.Substring(0,$i)+@'
struct CapabilityBoundaryRequirement {
    const unsigned long long CapabilityMask;
    const CapabilityGrantScope Scope;
    const bool RequiresTypedGrant;
    CapabilityBoundaryRequirement(unsigned long long mask, CapabilityGrantScope scope, bool typed)
        : CapabilityMask(mask), Scope(scope), RequiresTypedGrant(typed) {}
    static const std::shared_ptr<CapabilityBoundaryRequirement> None;
    static std::shared_ptr<CapabilityBoundaryRequirement> TypedGrant(unsigned long long mask, CapabilityGrantScope scope) {
        return std::make_shared<CapabilityBoundaryRequirement>(mask, scope, true);
    }
    bool IsSatisfiedBy(std::shared_ptr<CapabilityDescriptorSet> descriptorSet) const;
};
inline const std::shared_ptr<CapabilityBoundaryRequirement> CapabilityBoundaryRequirement::None =
    std::make_shared<CapabilityBoundaryRequirement>(0, CapabilityGrantScope::None, false);
}
'@
$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"Grants/CapabilityGrant.h`"`r`n#include <memory>")
$s=$s.Replace('struct CapabilityBoundaryRequirement {',"class CapabilityDescriptorSet;`r`nstruct CapabilityBoundaryRequirement {")
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'CapabilityBoundaryRequirement.cpp'
$s=@'
#include "CapabilityBoundaryRequirement.h"
#include "Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
namespace YAKSys_Hybrid_CPU::Core {
bool CapabilityBoundaryRequirement::IsSatisfiedBy(std::shared_ptr<CapabilityDescriptorSet> descriptorSet) const {
    if (CapabilityMask == 0) return !RequiresTypedGrant;
    if (!RequiresTypedGrant) return false;
    std::shared_ptr<CapabilityGrant> grant;
    return descriptorSet->getTypedGrants()->TryGetGrant(CapabilityMask, Scope, grant) && grant->getHasTypedAuthority();
}
}
'@
[IO.File]::WriteAllText($p,$s)
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\Authority\YAKSys_Hybrid_CPU.Core.DomainRuntimeAuthority.cpp'
$s=[IO.File]::ReadAllText($p); $s=$s.Replace('#include "YAKSys_Hybrid_CPU.Core.DomainRuntimeAuthority.h"', "#include `"YAKSys_Hybrid_CPU.Core.DomainRuntimeAuthority.h`"`r`n#include `"../../Capabilities/CapabilityBoundaryRequirement.h`""); [IO.File]::WriteAllText($p,$s)