#include "YAKSys_Hybrid_CPU.Core.DomainValidationResult.h"
#include "../Services/DomainRuntimeOperation.h"
#include "../DomainBoundaryDescriptor.h"
#include "../../Capabilities/CapabilityBoundaryRequirement.h"

namespace YAKSys_Hybrid_CPU::Core
{
	const std::shared_ptr<DomainValidationResult> DomainValidationResult::Passed =
		std::shared_ptr<DomainValidationResult>(new DomainValidationResult(true, DomainValidationFailureReason::None, L""));

	DomainValidationResult::DomainValidationResult(bool isValid, DomainValidationFailureReason failureReason, const std::wstring &message)
		: IsValid(isValid), FailureReason(failureReason), Message(message)
	{
	}

	bool DomainValidationResult::isValid() const
	{
		return IsValid;
	}

	DomainValidationFailureReason DomainValidationResult::getFailureReason() const
	{
		return FailureReason;
	}

	std::wstring DomainValidationResult::getMessage() const
	{
		return Message;
	}

	std::shared_ptr<DomainValidationResult> DomainValidationResult::getPassed()
	{
		return Passed;
	}

	std::shared_ptr<DomainValidationResult> DomainValidationResult::Fail(DomainValidationFailureReason failureReason, const std::wstring &message)
	{
		return std::shared_ptr<DomainValidationResult>(new DomainValidationResult(false, failureReason, message));
	}

	std::shared_ptr<DomainValidationResult> DomainValidationResult::RequireRuntimeContext(std::shared_ptr<DomainRuntimeContext> context)
	{
		return RequireRuntimeContext(context, DomainBoundaryDescriptor::FullDomainRuntime);
	}

	std::shared_ptr<DomainValidationResult> DomainValidationResult::RequireRuntimeContext(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainBoundaryDescriptor> operationDomainRequirement)
	{
		if (operationDomainRequirement->RequiresExecutionDomain && !context->getHasExecutionDomain())
		{
			return Fail(DomainValidationFailureReason::MissingExecutionDomain);
		}

		if (operationDomainRequirement->RequiresMemoryDomain && !context->getHasMemoryDomain())
		{
			return Fail(DomainValidationFailureReason::MissingMemoryDomain);
		}

		if (operationDomainRequirement->RequiresIoDomain && !context->getHasIoDomain())
		{
			return Fail(DomainValidationFailureReason::MissingIoDomain);
		}

		return getPassed();
	}

	std::shared_ptr<DomainValidationResult> DomainValidationResult::RequireOperation(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainRuntimeOperation> operation, std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement)
	{
		if (operation->getRequiresCapabilityGrant() && (capabilityRequirement->CapabilityMask == 0 || !capabilityRequirement->IsSatisfiedBy(context->getCapabilities())))
		{
			return Fail(DomainValidationFailureReason::MissingCapabilityGrant);
		}

		if (!operation->getCanMutateAuthoritativeState() && !operation->isProjectionOnly() && !operation->isNoStateExecution())
		{
			return Fail(DomainValidationFailureReason::ProjectionCannotMutateState);
		}


		if (operation->isNoStateExecution() && operation->getSource() != DomainRuntimeOperationSource::RuntimeService)
		{
			return Fail(DomainValidationFailureReason::InvalidOperationAuthorityClass, L"No-state execution is available only to a neutral runtime service operation.");
		}

		return getPassed();
	}
}
