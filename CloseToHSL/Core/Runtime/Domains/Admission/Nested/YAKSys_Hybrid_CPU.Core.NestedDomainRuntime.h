#pragma once

#include "NestedDomainRuntime.h"

namespace YAKSys_Hybrid_CPU::Core
{
	class NestedDomainRuntime final : public std::enable_shared_from_this<NestedDomainRuntime>
	{
	public:
		std::shared_ptr<NestedDomainRuntimeResult> Validate(std::shared_ptr<NestedDomainRuntimeRequest> request);
		bool CanAdmit(std::shared_ptr<NestedDomainRuntimeRequest> request);
	};
}
