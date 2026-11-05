#pragma once

#include "YAKSys_Hybrid_CPU.Core.CapabilityGrantAuthorityContract.h"
#include "../../../Runtime/Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
#include "CapabilityGrantCollectionAuthorityContract.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CapabilityGrantCollectionAuthorityContract final : public std::enable_shared_from_this<CapabilityGrantCollectionAuthorityContract>
	{
	private:
		const std::shared_ptr<CapabilityGrantAuthorityContract> _grantAuthority = std::make_shared<CapabilityGrantAuthorityContract>();

	public:
		CapabilityGrantCollectionAuthorityViolation Evaluate(std::shared_ptr<CapabilityDescriptorSet> descriptorSet);

		bool IsSatisfied(std::shared_ptr<CapabilityDescriptorSet> descriptorSet);
	};
}
