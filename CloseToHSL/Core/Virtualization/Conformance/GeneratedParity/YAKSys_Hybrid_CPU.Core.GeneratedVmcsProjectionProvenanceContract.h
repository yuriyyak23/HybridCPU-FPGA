#pragma once

#include "YAKSys_Hybrid_CPU.Core.VmcsFieldProjectionSchemaConformanceContract.h"
#include "YAKSys_Hybrid_CPU.Core.GeneratedProjectionParityContract.h"
#include "GeneratedVmcsProjectionProvenanceContract.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class GeneratedVmcsProjectionProvenanceContract final : public std::enable_shared_from_this<GeneratedVmcsProjectionProvenanceContract>
	{
	private:
		const std::shared_ptr<VmcsFieldProjectionSchemaConformanceContract> _fieldSchema;
		const std::shared_ptr<GeneratedProjectionParityContract> _generatedParity;

	public:
		GeneratedVmcsProjectionProvenanceContract();

		GeneratedVmcsProjectionProvenanceContract(std::shared_ptr<VmcsFieldProjectionSchemaConformanceContract> fieldSchema, std::shared_ptr<GeneratedProjectionParityContract> generatedParity);

		GeneratedVmcsProjectionProvenanceViolation Evaluate(std::shared_ptr<GeneratedVmcsProjectionProvenanceRequest> request);

		bool IsSatisfied(std::shared_ptr<GeneratedVmcsProjectionProvenanceRequest> request);
	};
}
