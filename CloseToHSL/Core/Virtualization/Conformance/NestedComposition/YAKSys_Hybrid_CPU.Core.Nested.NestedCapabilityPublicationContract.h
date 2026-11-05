#pragma once

#include "../../Compatibility/Frontend/Projection/Nested/NestedDomainController.h"

#include "NestedCapabilityPublicationContract.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Nested
{
	class NestedCapabilityPublicationContract final : public std::enable_shared_from_this<NestedCapabilityPublicationContract>
	{
	public:
		NestedCapabilityPublicationContractViolation Evaluate(std::shared_ptr<NestedEnablementRequest> request);

		bool IsSatisfied(std::shared_ptr<NestedEnablementRequest> request);
	};
}

