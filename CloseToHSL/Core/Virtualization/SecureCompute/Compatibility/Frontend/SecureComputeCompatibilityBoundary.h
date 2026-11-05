#pragma once
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeCompatibilityOperation : unsigned char
	{
		ReadProjection = 0,
		WriteProjection = 1,
		ActivateSecureCompute = 2,
		GrantSecureCompute = 3,
		StoreSecureState = 4
	};

	enum class SecureComputeCompatibilityBoundaryDecision : unsigned char
	{
		DeniedByDefault = 0,
		AllowedProjectionOnly = 1,
		DeniedVmxActivation = 2,
		DeniedVmxCapsAuthority = 3,
		DeniedVmcsStateStore = 4,
		DeniedWriteMutation = 5
	};

	struct SecureComputeCompatibilityBoundaryResult final
	{
		const SecureComputeCompatibilityBoundaryDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;
		SecureComputeCompatibilityBoundaryResult(SecureComputeCompatibilityBoundaryDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)),
			  IsAllowed(decision == SecureComputeCompatibilityBoundaryDecision::AllowedProjectionOnly) {}
		static const std::shared_ptr<SecureComputeCompatibilityBoundaryResult> AllowedProjectionOnly;
	};
	inline const std::shared_ptr<SecureComputeCompatibilityBoundaryResult>
		SecureComputeCompatibilityBoundaryResult::AllowedProjectionOnly =
			std::make_shared<SecureComputeCompatibilityBoundaryResult>(
				SecureComputeCompatibilityBoundaryDecision::AllowedProjectionOnly, std::wstring{});
}
