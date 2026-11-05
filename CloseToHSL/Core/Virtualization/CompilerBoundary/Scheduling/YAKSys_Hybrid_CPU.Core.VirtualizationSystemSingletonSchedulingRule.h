#pragma once

#include "VirtualizationSystemSingletonSchedulingRule.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VirtualizationSystemSingletonSchedulingRule final : public std::enable_shared_from_this<VirtualizationSystemSingletonSchedulingRule>
	{
	public:
		VirtualizationSchedulingDecision Evaluate(std::shared_ptr<VirtualizationSystemSingletonSchedulingRequest> request);

		bool CanSchedule(std::shared_ptr<VirtualizationSystemSingletonSchedulingRequest> request);
	};
}
