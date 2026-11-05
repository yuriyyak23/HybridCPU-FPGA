#pragma once

#include "../Validation/YAKSys_Hybrid_CPU.Core.DomainValidationResult.h"
#include "../Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class DomainSchedulingAdmission final : public std::enable_shared_from_this<DomainSchedulingAdmission>
	{
	public:
		std::shared_ptr<DomainValidationResult> Admit(std::shared_ptr<DomainRuntimeContext> context, unsigned char laneId, unsigned long long operationsIssuedThisEpoch = 0);
	};
}
