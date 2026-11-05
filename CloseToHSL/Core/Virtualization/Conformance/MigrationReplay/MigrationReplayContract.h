#pragma once

#include "../../../Runtime/Migration/Format/MigrationDescriptor.h"
#include "../../../Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class MigrationReplayContractDecision : unsigned char
	{
		Allowed = 0,
		MissingGoldenArtifact = 1,
		DescriptorAuthorityDenied = 2,
		MigrationValidationDenied = 3,
		HostOwnedEvidenceReplayDenied = 4,
		NondeterministicReplayDenied = 5
	};

	struct MigrationReplayContractRequest final
	{
		const MigrationPayloadClass PayloadClass;
		const EvidenceVisibilityClass EvidenceClass;
		const EvidenceRestorePolicy RestorePolicy;
		const bool GoldenArtifactCaptured;
		const bool DescriptorAuthorityValidated;
		const bool MigrationValidationPassed;
		const bool DeterministicReplay;
		const bool AttemptsHostOwnedEvidenceReplay;

		MigrationReplayContractRequest(MigrationPayloadClass payloadClass, EvidenceVisibilityClass evidenceClass,
			EvidenceRestorePolicy restorePolicy, bool goldenArtifactCaptured, bool descriptorAuthorityValidated,
			bool migrationValidationPassed, bool deterministicReplay, bool attemptsHostOwnedEvidenceReplay)
			: PayloadClass(payloadClass), EvidenceClass(evidenceClass), RestorePolicy(restorePolicy),
			  GoldenArtifactCaptured(goldenArtifactCaptured), DescriptorAuthorityValidated(descriptorAuthorityValidated),
			  MigrationValidationPassed(migrationValidationPassed), DeterministicReplay(deterministicReplay),
			  AttemptsHostOwnedEvidenceReplay(attemptsHostOwnedEvidenceReplay) {}
	};

	struct MigrationReplayContractResult final
	{
		const MigrationReplayContractDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;

		MigrationReplayContractResult(MigrationReplayContractDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)),
			  IsAllowed(decision == MigrationReplayContractDecision::Allowed) {}

		static const std::shared_ptr<MigrationReplayContractResult> Allowed;
		static std::shared_ptr<MigrationReplayContractResult> Denied(
			MigrationReplayContractDecision decision, const std::wstring &reason)
		{
			return std::make_shared<MigrationReplayContractResult>(decision, reason);
		}
	};
}
