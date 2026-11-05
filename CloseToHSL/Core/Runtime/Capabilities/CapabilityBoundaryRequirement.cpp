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