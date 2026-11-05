#pragma once

#include "VectorStreamDomainRuntime.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VectorStreamDomainRuntime final : public std::enable_shared_from_this<VectorStreamDomainRuntime>
	{
	public:
		std::shared_ptr<VectorStreamDomainRuntimeResult> Validate(std::shared_ptr<VectorStreamDomainRuntimeRequest> request);

		bool CanAdmit(std::shared_ptr<VectorStreamDomainRuntimeRequest> request);
	};
}
