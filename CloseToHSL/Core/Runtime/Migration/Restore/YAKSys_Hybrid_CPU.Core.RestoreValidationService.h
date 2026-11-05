#pragma once

#include "../Validation/MigrationValidationPolicy.h"
#include "../../Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	class DomainCheckpointImage;
	class MigrationValidationPolicy;

	enum class RestoreValidationDecision : unsigned char
	{
		Allowed = 0,
		EmptyCheckpoint = 1,
		CompatibilityProjectionDenied = 2,
		CheckpointEpochMismatch = 3,
		MigrationPolicyDenied = 4
	};

	struct RestoreValidationResult final
	{
		const RestoreValidationDecision Decision;
		const std::shared_ptr<MigrationValidationResult> MigrationResult;
		const std::wstring Message;
		const bool IsAllowed;

		RestoreValidationResult(RestoreValidationDecision decision,
			std::shared_ptr<MigrationValidationResult> migrationResult, std::wstring message)
			: Decision(decision), MigrationResult(std::move(migrationResult)),
			  Message(std::move(message)), IsAllowed(decision == RestoreValidationDecision::Allowed) {}

		static const std::shared_ptr<RestoreValidationResult> Allowed;

		static std::shared_ptr<RestoreValidationResult> Denied(RestoreValidationDecision decision,
			const std::wstring &message,
			std::shared_ptr<MigrationValidationResult> migrationResult = MigrationValidationResult::Allowed)
		{
			return std::make_shared<RestoreValidationResult>(decision, std::move(migrationResult), message);
		}
	};

	class RestoreValidationService final : public std::enable_shared_from_this<RestoreValidationService>
	{
	public:
		std::shared_ptr<RestoreValidationResult> ValidateRestore(std::shared_ptr<DomainCheckpointImage> checkpoint,
			std::shared_ptr<MigrationValidationPolicy> policy, EvidenceRestorePolicy restorePolicy,
			unsigned long long expectedCheckpointEpoch = 0);
	};
}
