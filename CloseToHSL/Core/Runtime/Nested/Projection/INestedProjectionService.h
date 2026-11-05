#pragma once

#include "../Descriptors/YAKSys_Hybrid_CPU.Core.NestedDomainDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Nested
{
	struct NestedEnablementRequest;
	struct NestedValidationResult;

	class INestedProjectionService
	{
	public:
		virtual bool TryEnable(std::shared_ptr<NestedDomainDescriptor> domain, std::shared_ptr<NestedEnablementRequest> request, std::shared_ptr<NestedValidationResult> &validation) = 0;

		virtual void Disable(std::shared_ptr<NestedDomainDescriptor> domain) = 0;
	};

}
