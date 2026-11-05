#include "VmxCapabilityDescriptorSource.h"

namespace YAKSys_Hybrid_CPU::Core
{

	StaticVmxCapabilityDescriptorSource::StaticVmxCapabilityDescriptorSource(std::shared_ptr<CapabilityDescriptorSet> descriptorSet) : _descriptorSet((descriptorSet != nullptr) ? descriptorSet : CapabilityDescriptorSet::getEmpty())
	{
	}

	std::shared_ptr<CapabilityDescriptorSet> StaticVmxCapabilityDescriptorSource::GetCapabilityDescriptorSet()
	{
		return _descriptorSet;
	}
}
