#pragma once

#include "ShadowVmcsTypedFailureContract.h"
#include <memory>
#include "../../../../../tangible_string_helper.h"
#include "../../../Runtime/Nested/Validation/NestedValidationResult.h"

namespace YAKSys_Hybrid_CPU::Core
{
	class ShadowVmcsTypedFailureContract final : public std::enable_shared_from_this<ShadowVmcsTypedFailureContract>
	{
	public:
		ShadowVmcsTypedFailureViolation Evaluate(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Nested::NestedValidationResult> validation);

		bool IsSatisfied(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Nested::NestedValidationResult> validation);
	};
}
