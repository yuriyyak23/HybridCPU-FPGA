#include "CloseToHSL/Core/Runtime/Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "CloseToHSL/Core/Runtime/Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core;
int main(){ auto c=std::make_shared<DomainRuntimeContext>(nullptr,nullptr,nullptr,CapabilityDescriptorSet::getEmpty(),nullptr,19,42); assert(!c->getHasRequiredDomains()); assert(c->IsBoundToDomain(19) && !c->IsBoundToDomain(0)); assert(c->IsBoundToAddressSpace(42) && !c->IsBoundToAddressSpace(19)); auto copy=c->WithCapabilities(CapabilityDescriptorSet::getEmpty()); assert(copy->getDomainTag()==19 && copy->getAddressSpaceTag()==42 && copy!=c); auto d=std::make_shared<DomainRuntimeContext>(); assert(!d->IsBoundToDomain(0)); }
