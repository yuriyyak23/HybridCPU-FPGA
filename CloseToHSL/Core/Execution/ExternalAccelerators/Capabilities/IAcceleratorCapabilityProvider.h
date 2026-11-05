#pragma once

#include "AcceleratorCapabilityDescriptor.h"
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

	/// <summary>
	/// Provides external accelerator capability metadata only.
	/// </summary>
	class IAcceleratorCapabilityProvider
	{
	public:
		virtual const std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> GetCapabilities() = 0;
	};

}
