#pragma once
#include "VirtualizationCompilerIntent.h"

#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VirtualizationCompilerIntent final : public std::enable_shared_from_this<VirtualizationCompilerIntent>
	{
	public:
		std::shared_ptr<VirtualizationCompilerIntentResult> Evaluate(std::shared_ptr<VirtualizationCompilerIntentRequest> request);
	};
}
