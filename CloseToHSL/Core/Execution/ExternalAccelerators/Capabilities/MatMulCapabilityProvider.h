#pragma once

#include "IAcceleratorCapabilityProvider.h"
#include "AcceleratorCapabilityDescriptor.h"
#include <string>
#include <vector>
#include <memory>
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors { class AcceleratorCommandDescriptor; }

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

	class MatMulCapabilityProvider final : public IAcceleratorCapabilityProvider
	{
	public:
		static inline const std::wstring AcceleratorId = L"matmul.fixture.v1";
		static constexpr unsigned int CapabilityVersion = 1;

		const std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> GetCapabilities() override;

		static bool Matches(std::shared_ptr<AcceleratorCommandDescriptor> descriptor);
	};

}
