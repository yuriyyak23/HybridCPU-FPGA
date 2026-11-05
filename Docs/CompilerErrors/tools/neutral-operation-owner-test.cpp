#include "../../CloseToHSL/Core/Runtime/Events/Hypercalls/DisabledNeutralVirtualizationOperationOwner.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core;
int main() {
 auto owner=DisabledNeutralVirtualizationOperationOwner::getInstance();
 auto req=std::make_shared<NeutralVirtualizationOperationOwnerRequest>();
 req->DecisionValidation={VirtualizationOperationDecisionValidationDecision::ValidGovernanceArtifactOnly,L""};
 auto result=owner->Resolve(req); assert(result->Decision==NeutralVirtualizationOperationOwnerDecision::DeniedOwnerInterfaceDisabled);
 assert(!result->BackendExecutionAuthorized() && !result->CompletionPublicationAuthorized() && !result->RetirePublicationAuthorized());
 req->DecisionValidation.Decision=VirtualizationOperationDecisionValidationDecision::DeniedSchemaVersion;
 assert(owner->Resolve(req)->Decision==NeutralVirtualizationOperationOwnerDecision::DeniedDecisionArtifactMissing);
}
