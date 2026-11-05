#pragma once

#include "../../../../../Runtime/Nested/Descriptors/YAKSys_Hybrid_CPU.Core.NestedDomainDescriptor.h"
#include "../../../../../Runtime/Nested/Projection/INestedProjectionService.h"
#include "../../../../../System/Vmcs/V2/VmcsV2Descriptor.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core::Nested
{
	class NestedDomainController final
	{
	public:
		static bool TryEnable(std::shared_ptr<NestedDomainDescriptor> domain, std::shared_ptr<INestedProjectionService> projectionService, std::shared_ptr<NestedEnablementRequest> request, std::shared_ptr<NestedValidationResult> &validation);

		static void Disable(std::shared_ptr<NestedDomainDescriptor> domain, std::shared_ptr<INestedProjectionService> projectionService);


		static bool TryEnable(std::shared_ptr<VmcsV2Descriptor> descriptor, std::shared_ptr<NestedEnablementRequest> request, std::shared_ptr<VmcsV2ValidationResult> &validation);

		static void Disable(std::shared_ptr<VmcsV2Descriptor> descriptor);

	private:
		static std::shared_ptr<NestedDomainDescriptor> CreateCompatibilityProjectionDomain();

		static std::shared_ptr<NestedDomainDescriptor> CreateCompatibilityProjectionDomain(std::shared_ptr<NestedEnablementRequest> request);

		static std::shared_ptr<VmcsV2ValidationResult> ToVmcsValidation(std::shared_ptr<NestedValidationResult> validation);
	};
}


