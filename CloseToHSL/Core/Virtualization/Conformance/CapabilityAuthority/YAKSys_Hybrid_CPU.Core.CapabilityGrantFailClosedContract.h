#pragma once

#include "../../../Runtime/Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
#include "CapabilityGrantFailClosedContract.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CapabilityGrantFailClosedContract final : public std::enable_shared_from_this<CapabilityGrantFailClosedContract>
	{
	public:
		CapabilityGrantFailClosedViolation Evaluate(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long unmappedCapabilityMask);

		bool IsSatisfied(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long unmappedCapabilityMask);
	};
}
