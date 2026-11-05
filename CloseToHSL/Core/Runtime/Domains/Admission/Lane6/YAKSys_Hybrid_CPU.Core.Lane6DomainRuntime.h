#pragma once

#include "Lane6DomainRuntime.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class Lane6DomainRuntime final : public std::enable_shared_from_this<Lane6DomainRuntime>
	{
	public:
		std::shared_ptr<Lane6DomainRuntimeResult> Validate(std::shared_ptr<Lane6DomainRuntimeRequest> request);

		bool CanAdmit(std::shared_ptr<Lane6DomainRuntimeRequest> request);
	};
}
