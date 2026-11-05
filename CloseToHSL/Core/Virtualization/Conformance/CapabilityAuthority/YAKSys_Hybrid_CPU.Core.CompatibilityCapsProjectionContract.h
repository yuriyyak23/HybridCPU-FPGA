#pragma once

#include "../../../Runtime/Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
#include "CompatibilityCapsProjectionContract.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CompatibilityCapsProjectionContract final : public std::enable_shared_from_this<CompatibilityCapsProjectionContract>
	{
	public:
		CompatibilityCapsProjectionViolation Evaluate(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long projectedCompatibilityCaps);

		bool IsSatisfied(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long projectedCompatibilityCaps);
	};
}
