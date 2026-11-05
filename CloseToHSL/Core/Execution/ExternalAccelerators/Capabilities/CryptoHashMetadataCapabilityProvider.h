#pragma once

#include "IAcceleratorCapabilityProvider.h"
#include "AcceleratorCapabilityDescriptor.h"
#include <string>
#include <vector>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

	class CryptoHashMetadataCapabilityProvider final : public IAcceleratorCapabilityProvider
	{
	public:
		static inline const std::wstring AcceleratorId = L"crypto.hash.metadata.v1";
		static constexpr unsigned int CapabilityVersion = 1;

		const std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> GetCapabilities() override;
	};

}
