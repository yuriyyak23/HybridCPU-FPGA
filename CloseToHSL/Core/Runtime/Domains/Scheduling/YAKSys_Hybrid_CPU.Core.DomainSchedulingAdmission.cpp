#include "YAKSys_Hybrid_CPU.Core.DomainSchedulingAdmission.h"
#include "../Validation/DomainValidationResult.h"
#include "../Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.ExecutionDomainDescriptor.h"
#include "../Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.SchedulingBudgetDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<DomainValidationResult> DomainSchedulingAdmission::Admit(std::shared_ptr<DomainRuntimeContext> context, unsigned char laneId, unsigned long long operationsIssuedThisEpoch)
	{
		if (context->getExecution() == nullptr)
		{
			return DomainValidationResult::Fail(DomainValidationFailureReason::MissingExecutionDomain, L"Scheduling admission requires an execution-domain descriptor.");
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: SchedulingBudgetDescriptor? descriptor = context.Execution.SchedulingBudget;
		std::shared_ptr<SchedulingBudgetDescriptor> descriptor = context->getExecution()->getSchedulingBudget();
		if (descriptor == nullptr)
		{
			return DomainValidationResult::Fail(DomainValidationFailureReason::MissingSchedulingBudgetDescriptor, L"Scheduling admission requires a runtime-owned scheduling budget descriptor.");
		}

		if (!descriptor->isRuntimeAuthoritative())
		{
			return DomainValidationResult::Fail(DomainValidationFailureReason::RuntimeAuthorityMissing, L"Compatibility projection cannot own scheduling admission.");
		}

		if (!descriptor->AcceptsLane(laneId))
		{
			return DomainValidationResult::Fail(DomainValidationFailureReason::SchedulingLaneRejected, L"The scheduling budget descriptor rejects the requested lane.");
		}

		if (descriptor->getHasFiniteBudget() && operationsIssuedThisEpoch >= descriptor->getMaxOperationsPerEpoch())
		{
			return DomainValidationResult::Fail(DomainValidationFailureReason::SchedulingBudgetExceeded, L"The scheduling budget descriptor exhausted its current epoch budget.");
		}

		return DomainValidationResult::getPassed();
	}
}
