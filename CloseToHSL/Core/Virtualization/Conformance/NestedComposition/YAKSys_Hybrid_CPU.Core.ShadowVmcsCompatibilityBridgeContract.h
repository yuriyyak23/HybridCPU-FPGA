#pragma once

#include "ShadowVmcsCompatibilityBridgeContract.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class ShadowVmcsCompatibilityBridgeContract final : public std::enable_shared_from_this<ShadowVmcsCompatibilityBridgeContract>
	{
	public:
		ShadowVmcsCompatibilityBridgeViolation Evaluate(std::shared_ptr<ShadowVmcsCompatibilityBridgeRequest> request);

		bool IsSatisfied(std::shared_ptr<ShadowVmcsCompatibilityBridgeRequest> request);
	};
}
