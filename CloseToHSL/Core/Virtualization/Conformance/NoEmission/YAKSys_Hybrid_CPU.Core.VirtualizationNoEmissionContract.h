#pragma once
#include "VirtualizationNoEmissionContract.h"

#include "../../CompilerBoundary/NoEmission/YAKSys_Hybrid_CPU.Core.VirtualizationNoEmissionRegressionGate.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VirtualizationNoEmissionContract final : public std::enable_shared_from_this<VirtualizationNoEmissionContract>
	{
	private:
		const std::shared_ptr<VirtualizationNoEmissionRegressionGate> _gate;

	public:
		VirtualizationNoEmissionContract();

		VirtualizationNoEmissionContract(std::shared_ptr<VirtualizationNoEmissionRegressionGate> gate);

		std::shared_ptr<VirtualizationNoEmissionContractResult> Validate(std::shared_ptr<VirtualizationNoEmissionContractRequest> request);

		bool IsSatisfied(std::shared_ptr<VirtualizationNoEmissionContractRequest> request);
	};
}
