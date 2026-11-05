#pragma once

#include "VirtualizationNoEmissionRegressionGate.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VirtualizationNoEmissionRegressionGate final : public std::enable_shared_from_this<VirtualizationNoEmissionRegressionGate>
	{
	public:
		VirtualizationEmissionDecision Evaluate(std::shared_ptr<VirtualizationEmissionRequest> request);

		bool CanEmit(std::shared_ptr<VirtualizationEmissionRequest> request);
	};
}
