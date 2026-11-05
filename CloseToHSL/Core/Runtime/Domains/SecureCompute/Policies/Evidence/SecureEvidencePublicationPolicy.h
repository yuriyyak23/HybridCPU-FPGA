#pragma once

#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class SecureEvidencePublicationDecision : unsigned char
	{
		Allowed = 0,
		DeniedMissingSecureEvidencePolicy = 1,
		DeniedMissingNeutralEvidencePolicy = 2,
		DeniedHostOwnedEvidence = 3,
		DeniedRecomputedEvidence = 4,
		DeniedNeutralEvidencePolicy = 5,
		DeniedSecureEvidencePolicy = 6,
		DeniedCompatibilityAliasPolicy = 7,
		DeniedCompletionFence = 8,
		DeniedRetireFence = 9,
		DeniedSidebandVisibility = 10,
		DeniedStaleEvidenceEpoch = 11
	};

	struct SecureEvidencePublicationResult final {
		const SecureEvidencePublicationDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;

		SecureEvidencePublicationResult(SecureEvidencePublicationDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)), IsAllowed(decision == SecureEvidencePublicationDecision::Allowed) {}

		static const std::shared_ptr<SecureEvidencePublicationResult> Allowed;
		static std::shared_ptr<SecureEvidencePublicationResult> Denied(SecureEvidencePublicationDecision decision, std::wstring reason) {
			return std::make_shared<SecureEvidencePublicationResult>(decision, std::move(reason));
		}
	};

	inline const std::shared_ptr<SecureEvidencePublicationResult> SecureEvidencePublicationResult::Allowed = std::make_shared<SecureEvidencePublicationResult>(SecureEvidencePublicationDecision::Allowed, L"");
}
