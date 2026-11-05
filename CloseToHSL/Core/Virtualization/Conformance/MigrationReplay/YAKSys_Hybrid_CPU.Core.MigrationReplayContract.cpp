#include "YAKSys_Hybrid_CPU.Core.MigrationReplayContract.h"

namespace YAKSys_Hybrid_CPU::Core
{
	const std::shared_ptr<MigrationReplayContractResult> MigrationReplayContractResult::Allowed =
		std::make_shared<MigrationReplayContractResult>(MigrationReplayContractDecision::Allowed,
			L"Migration replay contract allowed.");

	std::shared_ptr<MigrationReplayContractResult> MigrationReplayContract::ValidateReplay(
		std::shared_ptr<MigrationReplayContractRequest> request)
	{
		if (!request || !request->GoldenArtifactCaptured)
			return MigrationReplayContractResult::Denied(MigrationReplayContractDecision::MissingGoldenArtifact,
				L"Migration replay requires a golden artifact capture.");

		if (!request->DescriptorAuthorityValidated)
			return MigrationReplayContractResult::Denied(MigrationReplayContractDecision::DescriptorAuthorityDenied,
				L"Migration replay requires descriptor authority validation.");

		if (!request->MigrationValidationPassed)
			return MigrationReplayContractResult::Denied(MigrationReplayContractDecision::MigrationValidationDenied,
				L"Migration replay requires migration validation policy approval.");

		if (request->AttemptsHostOwnedEvidenceReplay || IsHostOwnedPayload(request->PayloadClass) ||
			IsHostOwnedEvidence(request->EvidenceClass))
			return MigrationReplayContractResult::Denied(MigrationReplayContractDecision::HostOwnedEvidenceReplayDenied,
				L"Migration replay cannot import host-owned runtime evidence.");

		if (!request->DeterministicReplay)
			return MigrationReplayContractResult::Denied(MigrationReplayContractDecision::NondeterministicReplayDenied,
				L"Migration replay must be deterministic across restore.");

		return MigrationReplayContractResult::Allowed;
	}

	bool MigrationReplayContract::CanReplay(std::shared_ptr<MigrationReplayContractRequest> request)
	{
		const auto result = ValidateReplay(std::move(request));
		return result && result->IsAllowed;
	}

	bool MigrationReplayContract::IsHostOwnedPayload(MigrationPayloadClass payloadClass)
	{
		return payloadClass == MigrationPayloadClass::HostOwnedRuntimeEvidence ||
			payloadClass == MigrationPayloadClass::SchedulerEvidence ||
			payloadClass == MigrationPayloadClass::BackendBindingEvidence ||
			payloadClass == MigrationPayloadClass::NativeTokenEvidence;
	}

	bool MigrationReplayContract::IsHostOwnedEvidence(EvidenceVisibilityClass evidenceClass)
	{
		return evidenceClass == EvidenceVisibilityClass::HostOwnedRuntimeEvidence ||
			evidenceClass == EvidenceVisibilityClass::SchedulerEvidence ||
			evidenceClass == EvidenceVisibilityClass::BackendBindingEvidence ||
			evidenceClass == EvidenceVisibilityClass::NativeTokenEvidence;
	}
}
