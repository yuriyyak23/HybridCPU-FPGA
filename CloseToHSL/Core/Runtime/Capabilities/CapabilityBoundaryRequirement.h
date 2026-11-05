#pragma once
#include "Grants/CapabilityGrant.h"
#include <memory>

// Description: Neutral capability-boundary requirement that requires typed grants; compatibility bit masks are projection-only.
namespace YAKSys_Hybrid_CPU::Core
{

class CapabilityDescriptorSet;
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