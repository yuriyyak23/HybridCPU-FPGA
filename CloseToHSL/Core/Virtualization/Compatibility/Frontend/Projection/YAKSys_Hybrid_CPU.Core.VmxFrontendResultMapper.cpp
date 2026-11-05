#include "YAKSys_Hybrid_CPU.Core.VmxFrontendResultMapper.h"

using namespace YAKSys_Hybrid_CPU::Core::Nested;
using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{

	VmxFrontendResult VmxFrontendResultMapper::FromNestedValidation(std::shared_ptr<NestedValidationResult> validation)
	{
		if (!validation)
		{
			return VmxFrontendResult::FailInvalid(VmFailCode::UnknownVmcsField, L"Nested validation result is required.");
		}
		if (validation->Succeeded)
		{
			return VmxFrontendResult::Success();
		}
		switch (validation->Code)
		{
		case NestedValidationCode::ProjectionDenied:
			return VmxFrontendResult::FailValid(VmFailCode::VmcsFieldAccessDenied, validation->Message);
		case NestedValidationCode::CompatibilityProjectionFailed:
			return VmxFrontendResult::FailInvalid(VmFailCode::GuestGprPersistenceIncomplete, validation->Message);
		default:
			return VmxFrontendResult::FailInvalid(VmFailCode::UnknownVmcsField, validation->Message);
		}
	}
}
