#pragma once
#include "VirtualizationBundleLegalityRule.h"

#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VirtualizationBundleLegalityRule final : public std::enable_shared_from_this<VirtualizationBundleLegalityRule>
	{
	public:
		std::shared_ptr<VirtualizationBundleLegalityResult> Evaluate(std::shared_ptr<VirtualizationBundleLegalityRequest> request);

		bool CanBundle(std::shared_ptr<VirtualizationBundleLegalityRequest> request);
	};
}
