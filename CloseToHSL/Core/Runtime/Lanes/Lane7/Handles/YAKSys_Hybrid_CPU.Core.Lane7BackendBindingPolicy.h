#pragma once

#include <memory>
#include "Lane7BackendBindingPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{
	class Lane7BackendBindingPolicy final : public std::enable_shared_from_this<Lane7BackendBindingPolicy>
	{
	public:
		std::shared_ptr<Lane7BackendBindingResult> Validate(std::shared_ptr<Lane7BackendBindingRequest> request);

		bool CanBind(std::shared_ptr<Lane7BackendBindingRequest> request);
	};
}
