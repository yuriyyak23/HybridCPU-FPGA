#include "YAKSys_Hybrid_CPU.Core.ShadowVmcsTypedFailureContract.h"
#include "../../../Runtime/Nested/Validation/NestedValidationResult.h"

namespace YAKSys_Hybrid_CPU::Core
{

	ShadowVmcsTypedFailureViolation ShadowVmcsTypedFailureContract::Evaluate(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Nested::NestedValidationResult> validation)
	{
		if (validation == nullptr)
		{
			return ShadowVmcsTypedFailureViolation::MissingFailure;
		}

		if (validation->Succeeded)
		{
			return ShadowVmcsTypedFailureViolation::MissingFailure;
		}

		if (validation->Code != YAKSys_Hybrid_CPU::Core::Nested::NestedValidationCode::CompatibilityProjectionFailed)
		{
			return ShadowVmcsTypedFailureViolation::NonGenericFailureCode;
		}

		return StringHelper::isEmptyOrWhiteSpace(validation->Message) ? ShadowVmcsTypedFailureViolation::MissingFailureMessage : ShadowVmcsTypedFailureViolation::None;
	}

	bool ShadowVmcsTypedFailureContract::IsSatisfied(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Nested::NestedValidationResult> validation)
	{
		return Evaluate(validation) == ShadowVmcsTypedFailureViolation::None;
	}
}
