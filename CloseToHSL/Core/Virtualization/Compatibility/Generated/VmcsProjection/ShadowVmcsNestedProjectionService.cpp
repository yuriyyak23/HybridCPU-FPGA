#include "ShadowVmcsNestedProjectionService.h"
#include "../../../../Runtime/Nested/Validation/NestedValidationResult.h"

using namespace YAKSys_Hybrid_CPU::Core::Nested;
using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{

	bool ShadowVmcsNestedProjectionService::isRetirementFenced()
	{
		return true;
	}

	ShadowVmcsNestedProjectionService::ShadowVmcsNestedProjectionService(std::shared_ptr<VmcsV2Descriptor> descriptor)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
	}

	bool ShadowVmcsNestedProjectionService::TryEnable(std::shared_ptr<NestedDomainDescriptor> domain, std::shared_ptr<NestedEnablementRequest> request, std::shared_ptr<NestedValidationResult> &validation)
	{
		ArgumentNullException::ThrowIfNull(domain);
		validation = NestedValidationResult::Fail(NestedValidationCode::CompatibilityProjectionFailed, L"Shadow VMCS block was removed without replacement; compatibility nested admission cannot bypass the neutral nested projection/checkpoint service.");
		return false;
	}

	void ShadowVmcsNestedProjectionService::Disable(std::shared_ptr<NestedDomainDescriptor> domain)
	{
		ArgumentNullException::ThrowIfNull(domain);
	}
}
