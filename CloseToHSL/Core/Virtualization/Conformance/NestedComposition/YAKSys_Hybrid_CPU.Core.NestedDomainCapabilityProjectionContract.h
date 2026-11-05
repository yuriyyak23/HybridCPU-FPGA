#pragma once

#include "../../Compatibility/Frontend/Projection/Nested/NestedDomainController.h"

#include "NestedDomainCapabilityProjectionContract.h"
#include <memory>


namespace YAKSys_Hybrid_CPU::Core
{
	class NestedDomainCapabilityProjectionContract final : public std::enable_shared_from_this<NestedDomainCapabilityProjectionContract>
	{
	public:
		NestedDomainCapabilityProjectionViolation Evaluate(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Nested::NestedCapabilityPublication> publication);

		bool IsSatisfied(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Nested::NestedCapabilityPublication> publication);
	};
}

