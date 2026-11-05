#pragma once

#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeDomainDescriptor;
	class SecureAuthorityBounds;
	class SecureChildDomainIntentDescriptor;
	struct SecureRevocationEpoch;

	enum class SecureNestedCheckpointPayloadClass : unsigned char
	{
		NeutralChildIntentDescriptor = 0,
		CompatibilityProjectionMetadata = 1,
		ShadowVmcsCompatibilityBridge = 2,
		Vmcs12Authority = 3,
		Vmcs02Authority = 4,
		MutableShadowVmcsAuthority = 5
	};

	enum class SecureNestedDomainAdmissionDecision : unsigned char
	{
		AllowedNoEffect = 0,
		AllowedDesignFence = 1,
		DeniedMissingChildIntentOwner = 2,
		DeniedMissingParentSecureDescriptor = 3,
		DeniedUnmaterializedChildIntent = 4,
		DeniedChildIntentProvenanceMissing = 5,
		DeniedStaleParentChildEpoch = 6,
		DeniedChildPolicyExpansion = 7,
		DeniedChildCompatibilityProjectionExpansion = 8,
		DeniedChildMigrationPayloadExpansion = 9,
		DeniedHostEvidenceLeakage = 10,
		DeniedNestedProjectionExpansion = 11,
		DeniedNestedVmcsAuthority = 12,
		DeniedMutableShadowVmcsAuthority = 13
	};

	struct SecureNestedDomainAdmissionRequest final {
		std::shared_ptr<SecureComputeDomainDescriptor> ParentDescriptor;
		std::shared_ptr<SecureAuthorityBounds> ParentBounds;
		std::shared_ptr<SecureChildDomainIntentDescriptor> ChildIntent;
		std::shared_ptr<SecureRevocationEpoch> CurrentEpoch;
		bool HasNeutralChildIntentOwner = false;
		bool ParentHostEvidenceExposedToChild = false;
		bool ChildHostEvidenceExposedToParent = false;
		bool NestedProjectionExceedsParent = false;
		SecureNestedCheckpointPayloadClass CheckpointPayloadClass = SecureNestedCheckpointPayloadClass::NeutralChildIntentDescriptor;
		bool ShadowVmcsStoresMutableAuthority = false;
	};

	struct SecureNestedDomainAdmissionResult final {
		const SecureNestedDomainAdmissionDecision Decision;
		const std::wstring Reason;
		const bool BackendSuccessAuthorized;
		const bool MutableNestedStateAuthorized;
		const bool IsAllowed;
		SecureNestedDomainAdmissionResult(SecureNestedDomainAdmissionDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)), BackendSuccessAuthorized(false), MutableNestedStateAuthorized(false), IsAllowed(decision == SecureNestedDomainAdmissionDecision::AllowedNoEffect || decision == SecureNestedDomainAdmissionDecision::AllowedDesignFence) {}
		static const std::shared_ptr<SecureNestedDomainAdmissionResult> AllowedNoEffect;
		static const std::shared_ptr<SecureNestedDomainAdmissionResult> AllowedDesignFence;
		static std::shared_ptr<SecureNestedDomainAdmissionResult> Denied(SecureNestedDomainAdmissionDecision decision, std::wstring reason) {
			return std::make_shared<SecureNestedDomainAdmissionResult>(decision, std::move(reason));
		}
	};
	inline const std::shared_ptr<SecureNestedDomainAdmissionResult> SecureNestedDomainAdmissionResult::AllowedNoEffect = std::make_shared<SecureNestedDomainAdmissionResult>(SecureNestedDomainAdmissionDecision::AllowedNoEffect, L"");
	inline const std::shared_ptr<SecureNestedDomainAdmissionResult> SecureNestedDomainAdmissionResult::AllowedDesignFence = std::make_shared<SecureNestedDomainAdmissionResult>(SecureNestedDomainAdmissionDecision::AllowedDesignFence, L"");
}
