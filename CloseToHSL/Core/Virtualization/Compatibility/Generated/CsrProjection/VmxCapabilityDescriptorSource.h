#pragma once

#include "../../../../Runtime/Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	class IVmxCapabilityDescriptorSource
	{
	public:
		virtual std::shared_ptr<CapabilityDescriptorSet> GetCapabilityDescriptorSet() = 0;
	};

	class StaticVmxCapabilityDescriptorSource final : public IVmxCapabilityDescriptorSource
	{
	private:
		const std::shared_ptr<CapabilityDescriptorSet> _descriptorSet;

	public:
		StaticVmxCapabilityDescriptorSource(std::shared_ptr<CapabilityDescriptorSet> descriptorSet);

		std::shared_ptr<CapabilityDescriptorSet> GetCapabilityDescriptorSet() override;
	};

}
