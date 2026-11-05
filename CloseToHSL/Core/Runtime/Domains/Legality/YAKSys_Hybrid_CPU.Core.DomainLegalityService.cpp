#include "YAKSys_Hybrid_CPU.Core.DomainLegalityService.h"
#include "../../Capabilities/Grants/CapabilityGrant.h"
#include "../../Capabilities/CapabilityBoundaryRequirement.h"
#include "../DomainBoundaryDescriptor.h"
#include "../Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.ExecutionDomainDescriptor.h"
#include "../Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.BundleLegalityDescriptor.h"
#include "../Validation/DomainValidationResult.h"
#include "../Services/DomainRuntimeOperation.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<DomainValidationResult> DomainLegalityService::Validate(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainRuntimeOperation> operation, unsigned long long requiredCapabilityMask)
	{
		std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement = requiredCapabilityMask == 0 ? CapabilityBoundaryRequirement::None : CapabilityBoundaryRequirement::TypedGrant(requiredCapabilityMask, CapabilityGrantScope::CompatibilityProjection);

		return Validate(context, operation, capabilityRequirement);
	}

	std::shared_ptr<DomainValidationResult> DomainLegalityService::Validate(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainRuntimeOperation> operation, std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement)
	{
		return Validate(context, operation, DomainBoundaryDescriptor::FullDomainRuntime, capabilityRequirement);
	}

	std::shared_ptr<DomainValidationResult> DomainLegalityService::Validate(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainRuntimeOperation> operation, std::shared_ptr<DomainBoundaryDescriptor> operationDomainRequirement, std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement)
	{
		std::shared_ptr<DomainValidationResult> contextValidation = DomainValidationResult::RequireRuntimeContext(context, operationDomainRequirement);
		if (!contextValidation->isValid())
		{
			return contextValidation;
		}

		std::shared_ptr<DomainValidationResult> operationValidation = DomainValidationResult::RequireOperation(context, operation, capabilityRequirement);
		if (!operationValidation->isValid())
		{
			return operationValidation;
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: BundleLegalityDescriptor? descriptor = context.Execution!.BundleLegality;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: BundleLegalityDescriptor? descriptor = context.Execution.BundleLegality;
		std::shared_ptr<BundleLegalityDescriptor> descriptor = context->getExecution()->getBundleLegality();
		if (descriptor == nullptr)
		{
			return DomainValidationResult::Fail(DomainValidationFailureReason::MissingBundleLegalityDescriptor, L"Domain legality requires an execution-domain bundle legality descriptor.");
		}

		if (!descriptor->isRuntimeAuthoritative())
		{
			return DomainValidationResult::Fail(DomainValidationFailureReason::RuntimeAuthorityMissing, L"Compatibility projection or compiler evidence cannot own runtime legality.");
		}

		if (operation->getSource() == DomainRuntimeOperationSource::CompatibilityFrontend && !descriptor->getCanProjectToCompatibilityFrontend())
		{
			return DomainValidationResult::Fail(DomainValidationFailureReason::CompatibilityProjectionDenied, L"The runtime legality descriptor denies compatibility projection.");
		}

		return DomainValidationResult::getPassed();
	}
}
