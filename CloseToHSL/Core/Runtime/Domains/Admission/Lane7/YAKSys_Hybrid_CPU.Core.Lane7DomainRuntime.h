#pragma once

#include "Lane7DomainRuntime.h"
#include "../../Descriptors/Lane7Accelerator/YAKSys_Hybrid_CPU.Core.Lane7AcceleratorDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class Lane7DomainRuntime final : public std::enable_shared_from_this<Lane7DomainRuntime>
	{
	public:
		std::shared_ptr<Lane7DomainRuntimeResult> Validate(std::shared_ptr<Lane7DomainRuntimeRequest> request);
		bool CanAdmit(std::shared_ptr<Lane7DomainRuntimeRequest> request);
	};
}
