#include "DisabledNeutralVirtualizationOperationOwner.h"

namespace YAKSys_Hybrid_CPU::Core
{
	const std::shared_ptr<DisabledNeutralVirtualizationOperationOwner> DisabledNeutralVirtualizationOperationOwner::Instance(new DisabledNeutralVirtualizationOperationOwner());

	std::shared_ptr<DisabledNeutralVirtualizationOperationOwner> DisabledNeutralVirtualizationOperationOwner::getInstance()
	{
		return Instance;
	}

	DisabledNeutralVirtualizationOperationOwner::DisabledNeutralVirtualizationOperationOwner()
	{
	}

	std::shared_ptr<NeutralVirtualizationOperationOwnerResult> DisabledNeutralVirtualizationOperationOwner::Resolve(std::shared_ptr<NeutralVirtualizationOperationOwnerRequest> request)
	{
		return std::make_shared<NeutralVirtualizationOperationOwnerResult>(request->DecisionValidation.IsStructurallyValidGovernanceEvidence() ? NeutralVirtualizationOperationOwnerDecision::DeniedOwnerInterfaceDisabled : NeutralVirtualizationOperationOwnerDecision::DeniedDecisionArtifactMissing, L"The neutral operation-owner interface is disabled; D2 evidence cannot execute an operation.");
	}
}
