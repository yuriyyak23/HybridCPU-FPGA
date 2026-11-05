#pragma once

#include "../Validation/YAKSys_Hybrid_CPU.Core.DomainValidationResult.h"
#include "../Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeOperation.h"
#include "../Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class DomainLegalityService final : public std::enable_shared_from_this<DomainLegalityService>
	{
	public:
		std::shared_ptr<DomainValidationResult> Validate(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainRuntimeOperation> operation, unsigned long long requiredCapabilityMask = 0);

		std::shared_ptr<DomainValidationResult> Validate(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainRuntimeOperation> operation, std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement);

		std::shared_ptr<DomainValidationResult> Validate(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainRuntimeOperation> operation, std::shared_ptr<DomainBoundaryDescriptor> operationDomainRequirement, std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement);
	};
}
