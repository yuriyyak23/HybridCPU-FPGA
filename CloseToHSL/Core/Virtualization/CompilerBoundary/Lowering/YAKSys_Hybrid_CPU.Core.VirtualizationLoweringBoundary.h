#pragma once
#include "VirtualizationLoweringBoundary.h"

#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VirtualizationLoweringBoundary final : public std::enable_shared_from_this<VirtualizationLoweringBoundary>
	{
	public:
		std::shared_ptr<VirtualizationLoweringResult> ValidateLowering(std::shared_ptr<VirtualizationLoweringRequest> request);
	};
}
