#include "YAKSys_Hybrid_CPU.Core.SecureComputeCompatibilityProjectionService.h"
#include "SecureComputeVmReadVisibilityPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeCompatibilityProjectionService::SecureComputeCompatibilityProjectionService() : SecureComputeCompatibilityProjectionService(std::make_shared<SecureComputeVmReadVisibilityPolicy>())
	{
	}

	SecureComputeCompatibilityProjectionService::SecureComputeCompatibilityProjectionService(std::shared_ptr<SecureComputeVmReadVisibilityPolicy> visibilityPolicy) : _visibilityPolicy(visibilityPolicy)
	{
	}

	std::shared_ptr<SecureComputeCompatibilityProjectionResult> SecureComputeCompatibilityProjectionService::ProjectReadOnlyValue(std::shared_ptr<SecureComputeCompatibilityProjectionRequest> request, unsigned long long value)
	{
		if (request == nullptr)
		{
			return std::make_shared<SecureComputeCompatibilityProjectionResult>(SecureComputeVmReadVisibilityDecision::DeniedNoNeutralOwner, false, 0);
		}

		const SecureComputeVmReadVisibilityDecision decision = _visibilityPolicy->Validate(
			request->HasNeutralOwner,
			request->HasReadOnlySource,
			request->SecureVisibilityAllowed,
			request->MigrationClassified,
			request->ConformanceProven);
		const bool valueAvailable = decision == SecureComputeVmReadVisibilityDecision::AllowedReadOnlyProjection;

		return std::make_shared<SecureComputeCompatibilityProjectionResult>(decision, valueAvailable, valueAvailable ? value : 0);
	}
}
