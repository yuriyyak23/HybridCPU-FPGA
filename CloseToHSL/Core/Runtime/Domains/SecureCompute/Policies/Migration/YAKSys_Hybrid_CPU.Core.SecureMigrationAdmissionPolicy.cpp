#include "YAKSys_Hybrid_CPU.Core.SecureMigrationAdmissionPolicy.h"
#include "../../Descriptors/Migration/SecureMigrationDescriptor.h"
#include "../../Authority/SecureGrantHandle.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureMigrationAdmissionPolicy> SecureMigrationAdmissionPolicy::getDefault()
	{
		return Default;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureMigrationAdmissionResult AdmitCheckpointPayload(SecureMigrationDescriptor? migration, SecureCheckpointPayloadClass payloadClass, System.Nullable<SecurePrivateMemorySealedPayloadContract> privateMemoryContract = null)
	std::shared_ptr<SecureMigrationAdmissionResult> SecureMigrationAdmissionPolicy::AdmitCheckpointPayload(std::shared_ptr<SecureMigrationDescriptor> migration, SecureCheckpointPayloadClass payloadClass, std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> privateMemoryContract)
	{
		std::shared_ptr<SecureMigrationAdmissionResult> migrationState = ValidateMigrationEnabled(migration);
		if (!migrationState->IsAllowed)
		{
			return migrationState;
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (!migration!.PolicyEpoch.IsMaterialized)
		if (migration->getPolicyEpoch() == nullptr || !migration->getPolicyEpoch()->isMaterialized())
		{
			return Deny(SecureMigrationAdmissionDecision::DeniedPolicyEpochRollback, L"Secure migration policy requires a materialized policy epoch.");
		}

		switch (payloadClass)
		{
		case SecureCheckpointPayloadClass::HostOwnedEvidence:
		case SecureCheckpointPayloadClass::SchedulerEvidence:
		case SecureCheckpointPayloadClass::BackendBindingEvidence:
		case SecureCheckpointPayloadClass::NativeTokenEvidence:
			return Deny(SecureMigrationAdmissionDecision::DeniedHostOwnedEvidence, L"Host-owned evidence classes are recomputed after restore and cannot be checkpoint authority.");
		case SecureCheckpointPayloadClass::VmcsProjectionMetadata:
			return Deny(SecureMigrationAdmissionDecision::DeniedVmcsProjectionAuthority, L"VMCS projection metadata cannot be secure migration authority.");
		case SecureCheckpointPayloadClass::CompatibilityProjectionMetadata:
			return Deny(SecureMigrationAdmissionDecision::DeniedCompatibilityMetadataAuthority, L"Compatibility projection metadata cannot be secure migration authority.");
		case SecureCheckpointPayloadClass::DebugTrace:
			return Deny(SecureMigrationAdmissionDecision::DeniedDebugTraceAsGuestState, L"Debug traces cannot be restored as guest-visible secure state.");
		case SecureCheckpointPayloadClass::RawMeasurementSecret:
		case SecureCheckpointPayloadClass::RawSealingKey:
			return Deny(SecureMigrationAdmissionDecision::DeniedRawSecret, L"Raw measurement secrets and raw sealing keys are not checkpoint payloads.");
		case SecureCheckpointPayloadClass::ActiveHostPointer:
			return Deny(SecureMigrationAdmissionDecision::DeniedActiveHostPointer, L"Active host pointers are not portable secure migration payloads.");
		case SecureCheckpointPayloadClass::SecurePrivateMemory:
			return AdmitPrivateMemoryPayload(migration, privateMemoryContract);
		default:
			return SecureMigrationAdmissionResult::Allowed;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureMigrationAdmissionResult AdmitRestore(SecureMigrationDescriptor? migration, System.Nullable<DomainMeasurementDescriptor> measurement, System.Nullable<SecureMemoryDomainDescriptor> memory, SecureRevocationEpoch expectedPolicyEpoch, SecureGrantHandle restoredGrant, bool measurementRevalidated, bool reattestationCompleted, bool grantProvenanceValidated = false)
	std::shared_ptr<SecureMigrationAdmissionResult> SecureMigrationAdmissionPolicy::AdmitRestore(std::shared_ptr<SecureMigrationDescriptor> migration, std::shared_ptr<DomainMeasurementDescriptor> measurement, std::shared_ptr<SecureMemoryDomainDescriptor> memory, std::shared_ptr<SecureRevocationEpoch> expectedPolicyEpoch, std::shared_ptr<SecureGrantHandle> restoredGrant, bool measurementRevalidated, bool reattestationCompleted, bool grantProvenanceValidated)
	{
		std::shared_ptr<SecureMigrationAdmissionResult> migrationState = ValidateMigrationEnabled(migration);
		if (!migrationState->IsAllowed)
		{
			return migrationState;
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (!expectedPolicyEpoch.IsMaterialized || !expectedPolicyEpoch.IsCurrent(migration!.PolicyEpoch.Current))
		if (expectedPolicyEpoch == nullptr || !expectedPolicyEpoch->isMaterialized() || migration->getPolicyEpoch() == nullptr || !expectedPolicyEpoch->IsCurrent(migration->getPolicyEpoch()->Current))
		{
			return Deny(SecureMigrationAdmissionDecision::DeniedPolicyEpochRollback, L"Secure restore policy epoch is stale or rolled back.");
		}

		if (migration->getMeasurementRestorePolicy() == SecureMeasurementRestorePolicy::Denied)
		{
			return Deny(SecureMigrationAdmissionDecision::DeniedRestorePolicy, L"Secure restore requires an explicit measurement restore policy.");
		}

		if (restoredGrant != nullptr && restoredGrant->Kind != SecureGrantHandleKind::None && migration->getGrantRestorePolicy() == SecureGrantRestorePolicy::Denied)
		{
			return Deny(SecureMigrationAdmissionDecision::DeniedRestorePolicy, L"Secure restore requires an explicit grant restore policy.");
		}

		if (restoredGrant != nullptr && restoredGrant->Kind != SecureGrantHandleKind::None && (!restoredGrant->hasScalarShape() || !restoredGrant->hasProvenance()))
		{
			return Deny(SecureMigrationAdmissionDecision::DeniedGrantProvenanceMissing, L"Secure restore rejected a grant handle without runtime provenance.");
		}

		if (restoredGrant != nullptr && restoredGrant->Kind != SecureGrantHandleKind::None && !restoredGrant->MatchesEpoch(*migration->getPolicyEpoch()))
		{
			return Deny(SecureMigrationAdmissionDecision::DeniedStaleGrantEpoch, L"Secure restore rejected a stale grant epoch.");
		}

		if (restoredGrant != nullptr && restoredGrant->Kind != SecureGrantHandleKind::None && !grantProvenanceValidated)
		{
			return Deny(SecureMigrationAdmissionDecision::DeniedGrantRestoreProvenanceRevalidation, L"Secure restore must rederive or revalidate restored grant provenance.");
		}

		if (migration->getRequiresReattestation() && !reattestationCompleted)
		{
			return Deny(SecureMigrationAdmissionDecision::DeniedMeasurementReattestationRequired, L"Secure restore requires re-attestation before secure-domain entry.");
		}

		if (migration->getRequiresMeasurementRevalidation() && !measurementRevalidated)
		{
			return Deny(SecureMigrationAdmissionDecision::DeniedMeasurementRevalidationRequired, L"Secure restore requires measurement revalidation before secure-domain entry.");
		}

		if (migration->getRequiresMeasurementRevalidation() || migration->getRequiresReattestation())
		{
			if (measurement == nullptr || !measurement->isMaterialized() || !measurement->IsCurrentFor(migration->getPolicyEpoch()))
			{
				return Deny(SecureMigrationAdmissionDecision::DeniedStaleMeasurementEpoch, L"Secure restore requires a materialized measurement at the current migration epoch.");
			}
		}

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (memory != nullptr && memory->isMaterialized() && memory->getHasPrivateMemory() && migration->getPrivateMemoryPolicy() == SecurePrivateMemoryMigrationPolicy::Denied)
		{
			return Deny(SecureMigrationAdmissionDecision::DeniedPrivateMemoryWithoutSealedEncryptedContract, L"Secure restore cannot import private memory without an explicit private-memory migration policy.");
		}

		return SecureMigrationAdmissionResult::Allowed;
	}

	std::shared_ptr<SecureMigrationAdmissionResult> SecureMigrationAdmissionPolicy::AdmitPrivateMemoryPayload(std::shared_ptr<SecureMigrationDescriptor> migration, const std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> &privateMemoryContract)
	{
		if (!migration->getAllowsPrivateMemoryPayload() || (!privateMemoryContract.has_value() || !privateMemoryContract.value() || !privateMemoryContract.value()->IsComplete()))
		{
			return Deny(SecureMigrationAdmissionDecision::DeniedPrivateMemoryWithoutSealedEncryptedContract, L"Secure private memory migration requires explicit sealed and encrypted payload contract proof.");
		}

		return SecureMigrationAdmissionResult::Allowed;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static SecureMigrationAdmissionResult ValidateMigrationEnabled(SecureMigrationDescriptor? migration)
	std::shared_ptr<SecureMigrationAdmissionResult> SecureMigrationAdmissionPolicy::ValidateMigrationEnabled(std::shared_ptr<SecureMigrationDescriptor> migration)
	{
		if (migration == nullptr)
		{
			return Deny(SecureMigrationAdmissionDecision::DeniedMissingMigrationPolicy, L"Secure migration requires a migration policy descriptor.");
		}

		if (!migration->getAllowsMigration())
		{
			return Deny(SecureMigrationAdmissionDecision::DeniedMigrationDisabled, L"Secure migration is disabled by policy.");
		}

		return SecureMigrationAdmissionResult::Allowed;
	}

	std::shared_ptr<SecureMigrationAdmissionResult> SecureMigrationAdmissionPolicy::Deny(SecureMigrationAdmissionDecision decision, const std::wstring &reason)
	{
		return SecureMigrationAdmissionResult::Denied(decision, reason);
	}
}
