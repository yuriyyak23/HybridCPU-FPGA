#pragma once

#include "NestedCapabilityFilter.h"

#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Nested;

namespace YAKSys_Hybrid_CPU::Core
{
	class NestedCapabilityFilter final : public std::enable_shared_from_this<NestedCapabilityFilter>
	{
	public:
		std::shared_ptr<NestedCapabilityFilterResult> Validate(std::shared_ptr<NestedCapabilityFilterRequest> request);

		bool CanPass(std::shared_ptr<NestedCapabilityFilterRequest> request);
	};
}

