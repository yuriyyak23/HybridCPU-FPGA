#include "SecureComputePhase22LimitedReleaseGatePolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureComputePhase22LimitedReleaseGatePolicy> SecureComputePhase22LimitedReleaseGatePolicy::getFailClosed()
	{
		return FailClosed;
	}

	std::shared_ptr<SecureComputePhase22LimitedReleaseGateResult> SecureComputePhase22LimitedReleaseGatePolicy::Classify(std::shared_ptr<SecureComputePhase22LimitedReleaseEvidenceRequest> request)
	{
		if (!request->Phase21MatrixEvidenceAccepted)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedPhase21MatrixMissing, L"Phase 22 requires a consumed Phase 21 conformance matrix.");
		}

		if (!request->NamedPositiveRuntimePathProven)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedNoNamedPositiveRuntimePath, L"Phase 22 cannot approve release without a named positive runtime owner/path/reachability chain.");
		}

		if (request->Phase21FuturePositiveTestsRemainGated)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedPhase21FutureGatedOnly, L"Phase 22 cannot consume a Phase 21 matrix whose positive tests remain future-gated.");
		}

		if (!request->OwnerSpecificRfcAdrAccepted)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedMissingOwnerSpecificRfcAdr, L"Phase 22 requires the owner-specific RFC/ADR for the named path.");
		}

		if (!request->ProductionOwnerCodeExists)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedMissingProductionOwnerCode, L"Phase 22 requires production owner code, not a test fixture or documentation claim.");
		}

		if (!request->OwnerPathReachabilityProven)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedMissingOwnerPathReachability, L"Phase 22 requires scoped owner/path/reachability proof for the named path.");
		}

		if (!request->TypedRequestResultModelExists)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedMissingTypedRequestResult, L"Phase 22 requires a typed request/result model for the named path.");
		}

		if (!request->BackendResultOwnerBoundaryProven)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedMissingBackendResultOwnerBoundary, L"Phase 22 requires a backend-result owner boundary separate from admission.");
		}

		if (!request->CompletionRetirePolicyForNamedPathProven)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedMissingCompletionRetirePolicy, L"Phase 22 requires named-path completion and retire publication policy evidence.");
		}

		if (!request->MigrationManifestRestoreEvidenceComplete)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedMissingMigrationManifestRestoreEvidence, L"Phase 22 requires complete migration/output-manifest and restore evidence.");
		}

		if (!request->DebugAttestationVisibilityLimitsProven)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedMissingDebugAttestationLimits, L"Phase 22 requires debug/attestation visibility limits.");
		}

		if (!request->VmxProjectionAfterNeutralResultOnly)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedMissingVmxZeroAuthorityProjectionLimit, L"Phase 22 requires VMX projection to remain after-neutral-result-only and zero-authority.");
		}

		if (!request->CompilerNoEmissionDecisionRecorded)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedMissingCompilerNoEmissionDecision, L"Phase 22 requires the Phase 19 no-compiler-change decision.");
		}

		if (!request->Phase18NestedExecutionExcluded)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedPhase18NestedNotExcluded, L"Phase 22 requires Phase 18 nested execution to remain excluded.");
		}

		if (!request->ProductClaimScopedToNamedPath)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedProductClaimOverreach, L"Phase 22 requires product wording scoped to the named limited path.");
		}

		if (!request->BoundedRollbackProcedureReviewed)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedMissingRollbackProcedure, L"Phase 22 requires a bounded rollback procedure.");
		}

		if (!request->BackendExecutionAuthorityLocallyProven)
		{
			return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedBackendExecutionAuthorityNotLocallyProven, L"Phase 22 requires local backend execution authority proof before release review.");
		}

		return Deny(SecureComputePhase22LimitedReleaseDecision::DeniedPhase22ManualApprovalNotImplemented, L"The current Phase 22 policy is fail-closed; release approval must be implemented only with named-path evidence.");
	}

	std::shared_ptr<SecureComputePhase22LimitedReleaseGateResult> SecureComputePhase22LimitedReleaseGatePolicy::Deny(SecureComputePhase22LimitedReleaseDecision decision, const std::wstring &reason)
	{
		return SecureComputePhase22LimitedReleaseGateResult::Denied(decision, reason);
	}
}
