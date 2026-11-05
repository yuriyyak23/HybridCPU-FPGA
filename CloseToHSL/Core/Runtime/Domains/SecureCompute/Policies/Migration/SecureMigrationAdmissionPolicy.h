#pragma once

#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class SecureMigrationAdmissionDecision : unsigned char
	{
		Allowed = 0,
		DeniedMissingMigrationPolicy = 1,
		DeniedMigrationDisabled = 2,
		DeniedPolicyEpochRollback = 3,
		DeniedStaleMeasurementEpoch = 4,
		DeniedMeasurementRevalidationRequired = 5,
		DeniedMeasurementReattestationRequired = 6,
		DeniedStaleGrantEpoch = 7,
		DeniedHostOwnedEvidence = 8,
		DeniedVmcsProjectionAuthority = 9,
		DeniedCompatibilityMetadataAuthority = 10,
		DeniedPrivateMemoryWithoutSealedEncryptedContract = 11,
		DeniedDebugTraceAsGuestState = 12,
		DeniedRawSecret = 13,
		DeniedActiveHostPointer = 14,
		DeniedPayloadClass = 15,
		DeniedRestorePolicy = 16,
		DeniedGrantProvenanceMissing = 17,
		DeniedGrantRestoreProvenanceRevalidation = 18
	};

	struct SecureMigrationAdmissionResult final {
		const SecureMigrationAdmissionDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;
		SecureMigrationAdmissionResult(SecureMigrationAdmissionDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)), IsAllowed(decision == SecureMigrationAdmissionDecision::Allowed) {}
		static const std::shared_ptr<SecureMigrationAdmissionResult> Allowed;
		static std::shared_ptr<SecureMigrationAdmissionResult> Denied(SecureMigrationAdmissionDecision decision, std::wstring reason) {
			return std::make_shared<SecureMigrationAdmissionResult>(decision, std::move(reason));
		}
	};
	inline const std::shared_ptr<SecureMigrationAdmissionResult> SecureMigrationAdmissionResult::Allowed = std::make_shared<SecureMigrationAdmissionResult>(SecureMigrationAdmissionDecision::Allowed, L"");

	struct SecurePrivateMemorySealedPayloadContract final
	{
		bool HasSealedPayload;
		bool HasEncryptedPayload;
		bool HasNeutralKeyOwner;
		bool HasEvidencePolicy;
		bool HasRestoreValidationProof;
		bool ContainsRawSealingKey;

		SecurePrivateMemorySealedPayloadContract(bool sealedPayload = false, bool encryptedPayload = false, bool neutralKeyOwner = false, bool evidencePolicy = false, bool restoreValidationProof = false, bool rawSealingKey = false)
			: HasSealedPayload(sealedPayload), HasEncryptedPayload(encryptedPayload), HasNeutralKeyOwner(neutralKeyOwner), HasEvidencePolicy(evidencePolicy), HasRestoreValidationProof(restoreValidationProof), ContainsRawSealingKey(rawSealingKey) {}

		bool IsComplete() const noexcept {
			return HasSealedPayload && HasEncryptedPayload && HasNeutralKeyOwner && HasEvidencePolicy && HasRestoreValidationProof && !ContainsRawSealingKey;
		}
	};
}
