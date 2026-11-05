#pragma once

#include "SecureComputeCompatibilityProjectionService.h"
#include "YAKSys_Hybrid_CPU.Core.SecureComputeVmReadVisibilityPolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeCompatibilityProjectionService final : public std::enable_shared_from_this<SecureComputeCompatibilityProjectionService>
	{
	private:
		const std::shared_ptr<SecureComputeVmReadVisibilityPolicy> _visibilityPolicy;

	public:
		SecureComputeCompatibilityProjectionService();

		SecureComputeCompatibilityProjectionService(std::shared_ptr<SecureComputeVmReadVisibilityPolicy> visibilityPolicy);

		std::shared_ptr<SecureComputeCompatibilityProjectionResult> ProjectReadOnlyValue(std::shared_ptr<SecureComputeCompatibilityProjectionRequest> request, unsigned long long value);
	};
}
