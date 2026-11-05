#pragma once
#include "../Projection/VmxFrontendResultMapper.h"

#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Nested;
using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{
	class VmxFrontendResultMapper final : public std::enable_shared_from_this<VmxFrontendResultMapper>
	{
	public:
		VmxFrontendResult FromNestedValidation(std::shared_ptr<NestedValidationResult> validation);
	};
}
