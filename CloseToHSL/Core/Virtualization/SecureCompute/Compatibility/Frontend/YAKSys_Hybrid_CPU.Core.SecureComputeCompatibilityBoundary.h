#pragma once

#include "SecureComputeCompatibilityBoundary.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeCompatibilityBoundary final : public std::enable_shared_from_this<SecureComputeCompatibilityBoundary>
	{
	public:
		std::shared_ptr<SecureComputeCompatibilityBoundaryResult> Admit(SecureComputeCompatibilityOperation operation, bool projectionOnly);
	};
}
