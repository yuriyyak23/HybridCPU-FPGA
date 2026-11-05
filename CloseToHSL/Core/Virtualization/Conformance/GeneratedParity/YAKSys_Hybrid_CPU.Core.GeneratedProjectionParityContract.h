#pragma once

#include "GeneratedProjectionParityContract.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class GeneratedProjectionParityContract final : public std::enable_shared_from_this<GeneratedProjectionParityContract>
	{
	public:
		GeneratedProjectionParityViolation Evaluate(std::shared_ptr<GeneratedProjectionParityRequest> request);

		bool IsSatisfied(std::shared_ptr<GeneratedProjectionParityRequest> request);
	};
}
