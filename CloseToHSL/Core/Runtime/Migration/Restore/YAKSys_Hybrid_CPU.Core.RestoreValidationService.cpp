#include "YAKSys_Hybrid_CPU.Core.RestoreValidationService.h"
#include "../Validation/YAKSys_Hybrid_CPU.Core.MigrationValidationPolicy.h"
#include "../Checkpoint/YAKSys_Hybrid_CPU.Core.DomainCheckpointImage.h"
#include <string>

namespace YAKSys_Hybrid_CPU::Core
{

		const std::shared_ptr<RestoreValidationResult> RestoreValidationResult::Allowed =
		std::make_shared<RestoreValidationResult>(RestoreValidationDecision::Allowed, MigrationValidationResult::Allowed, std::wstring{});

std::shared_ptr<RestoreValidationResult> RestoreValidationService::ValidateRestore(std::shared_ptr<DomainCheckpointImage> checkpoint, std::shared_ptr<MigrationValidationPolicy> policy, EvidenceRestorePolicy restorePolicy, unsigned long long expectedCheckpointEpoch)
	{
		if (!checkpoint || checkpoint->isEmpty())
		{
			return RestoreValidationResult::Denied(RestoreValidationDecision::EmptyCheckpoint, L"Restore requires a non-empty domain checkpoint image.");
		}

		if (!checkpoint->isDomainAuthoritative() || checkpoint->getContainsCompatibilityProjectionMetadata())
		{
			return RestoreValidationResult::Denied(RestoreValidationDecision::CompatibilityProjectionDenied, L"Restore cannot accept compatibility projection metadata as authoritative state.");
		}

		if (expectedCheckpointEpoch != 0 && checkpoint->getCheckpointEpoch() != expectedCheckpointEpoch)
		{
			return RestoreValidationResult::Denied(RestoreValidationDecision::CheckpointEpochMismatch, L"Restore checkpoint epoch does not match the expected domain epoch.");
		}

		if (!policy)
		{
			return RestoreValidationResult::Denied(RestoreValidationDecision::MigrationPolicyDenied, L"Restore requires a migration validation policy.");
		}

		std::shared_ptr<MigrationValidationResult> migrationResult = checkpoint->ValidateRestore(policy, restorePolicy);
		if (!migrationResult || !migrationResult->IsAllowed)
		{
			return RestoreValidationResult::Denied(RestoreValidationDecision::MigrationPolicyDenied, migrationResult ? migrationResult->Message : L"Migration validation returned no result.", migrationResult);
		}

		return RestoreValidationResult::Allowed;
	}
}






