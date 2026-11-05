#pragma once

#include "NestedCompletionMapper.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Nested;

namespace YAKSys_Hybrid_CPU::Core
{
	class NestedCompletionMapper final : public std::enable_shared_from_this<NestedCompletionMapper>
	{
	public:
		std::shared_ptr<NestedCompletionMappingResult> Validate(std::shared_ptr<NestedCompletionMappingRequest> request);

		bool CanMap(std::shared_ptr<NestedCompletionMappingRequest> request);
	};
}
