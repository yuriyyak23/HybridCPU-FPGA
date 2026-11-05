#include "NestedDomainProjectionCheckpointService.h"
#include "../Migration/Restore/RestoreValidationService.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<NestedDomainProjectionCheckpointResult> NestedDomainProjectionCheckpointService::Validate(std::shared_ptr<NestedDomainProjectionCheckpointRequest> request)
	{
		std::shared_ptr<NestedProjectionResult> projectionResult = _projectionService->Validate(request->ProjectionRequest);
		if (!projectionResult->IsAllowed)
		{
			return NestedDomainProjectionCheckpointResult::DeniedProjection(projectionResult);
		}

		if (request->Checkpoint == nullptr || request->MigrationPolicy == nullptr)
		{
			return NestedDomainProjectionCheckpointResult::DeniedRestore(projectionResult, RestoreValidationResult::Denied(RestoreValidationDecision::EmptyCheckpoint, L"Nested domain restore requires a checkpoint image and migration policy."));
		}

		std::shared_ptr<RestoreValidationResult> restoreResult = _restoreService->ValidateRestore(request->Checkpoint, request->MigrationPolicy, request->RestorePolicy, request->ExpectedCheckpointEpoch);
		if (!restoreResult->IsAllowed)
		{
			return NestedDomainProjectionCheckpointResult::DeniedRestore(projectionResult, restoreResult);
		}

		return NestedDomainProjectionCheckpointResult::Allowed(projectionResult, restoreResult);
	}
}
