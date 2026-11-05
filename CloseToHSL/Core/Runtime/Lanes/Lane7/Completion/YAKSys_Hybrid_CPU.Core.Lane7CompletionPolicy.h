#pragma once
#include "Lane7CompletionPolicy.h"

#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class Lane7CompletionPolicy final : public std::enable_shared_from_this<Lane7CompletionPolicy>
	{
	public:
		std::shared_ptr<Lane7CompletionPolicyResult> Validate(std::shared_ptr<Lane7CompletionPolicyRequest> request);

		bool CanPublish(std::shared_ptr<Lane7CompletionPolicyRequest> request);
	};
}
