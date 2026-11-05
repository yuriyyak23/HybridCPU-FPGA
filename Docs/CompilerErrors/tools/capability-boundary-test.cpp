#include "CloseToHSL/Core/Runtime/Capabilities/CapabilityBoundaryRequirement.h"
#include "CloseToHSL/Core/Runtime/Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core;
int main() {
 auto schema=CapabilityDescriptorSetSchema::getVmxCompatibility(); auto mask=VmxV2InstructionCaps::VmCall;
 assert(schema->MatchesCanonicalVmxCompatibilityBitSchema(CapabilityDescriptorSetSchema::getVmxCompatibilityBitSchema()));
 assert(schema->FilterCompatibilityCaps(~0ULL)==schema->getCompatibilityPublicationMask());
 auto set=std::make_shared<CapabilityDescriptorSet>(mask,mask,mask);
 assert(CapabilityBoundaryRequirement::None->IsSatisfiedBy(set));
 assert(CapabilityBoundaryRequirement::TypedGrant(mask,CapabilityGrantScope::CompatibilityProjection)->IsSatisfiedBy(set));
 assert(!CapabilityBoundaryRequirement::TypedGrant(VmxV2InstructionCaps::NestedVmx,CapabilityGrantScope::CompatibilityProjection)->IsSatisfiedBy(set));
 assert(!CapabilityBoundaryRequirement(mask,CapabilityGrantScope::CompatibilityProjection,false).IsSatisfiedBy(set));
 assert(!CapabilityBoundaryRequirement::TypedGrant(0,CapabilityGrantScope::None)->IsSatisfiedBy(set));
}
