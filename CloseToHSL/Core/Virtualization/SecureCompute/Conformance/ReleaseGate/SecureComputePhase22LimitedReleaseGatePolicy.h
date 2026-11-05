#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputePhase22LimitedReleaseDecision : unsigned char
	{
		DeniedNoNamedPositiveRuntimePath = 0,
		DeniedPhase21MatrixMissing = 1,
		DeniedPhase21FutureGatedOnly = 2,
		DeniedMissingOwnerSpecificRfcAdr = 3,
		DeniedMissingProductionOwnerCode = 4,
		DeniedMissingOwnerPathReachability = 5,
		DeniedMissingTypedRequestResult = 6,
		DeniedMissingBackendResultOwnerBoundary = 7,
		DeniedMissingCompletionRetirePolicy = 8,
		DeniedMissingMigrationManifestRestoreEvidence = 9,
		DeniedMissingDebugAttestationLimits = 10,
		DeniedMissingVmxZeroAuthorityProjectionLimit = 11,
		DeniedMissingCompilerNoEmissionDecision = 12,
		DeniedPhase18NestedNotExcluded = 13,
		DeniedProductClaimOverreach = 14,
		DeniedMissingRollbackProcedure = 15,
		DeniedBackendExecutionAuthorityNotLocallyProven = 16,
		DeniedPhase22ManualApprovalNotImplemented = 17
	};

	struct SecureComputePhase22LimitedReleaseEvidenceRequest final
	{
		bool Phase21MatrixEvidenceAccepted, Phase21FuturePositiveTestsRemainGated, NamedPositiveRuntimePathProven;
		bool OwnerSpecificRfcAdrAccepted, ProductionOwnerCodeExists, OwnerPathReachabilityProven, TypedRequestResultModelExists;
		bool BackendResultOwnerBoundaryProven, CompletionRetirePolicyForNamedPathProven, MigrationManifestRestoreEvidenceComplete;
		bool DebugAttestationVisibilityLimitsProven, VmxProjectionAfterNeutralResultOnly, CompilerNoEmissionDecisionRecorded;
		bool Phase18NestedExecutionExcluded, ProductClaimScopedToNamedPath, BoundedRollbackProcedureReviewed, BackendExecutionAuthorityLocallyProven;
	};
	struct SecureComputePhase22LimitedReleaseGateResult final
	{
		SecureComputePhase22LimitedReleaseDecision Decision;
		bool ReleaseEvidenceAccepted, RuntimeExecutionAuthorized, BackendResultAccepted, CompletionPublicationAuthorized;
		bool RetirePublicationAuthorized, VmxAuthorityAuthorized, CompilerEmissionAuthorized, NestedExecutionAuthorized, ProductionReleaseApproved;
		std::wstring Reason;
		bool CreatesAnyReleaseAuthority() const noexcept { return RuntimeExecutionAuthorized || BackendResultAccepted || CompletionPublicationAuthorized || RetirePublicationAuthorized || VmxAuthorityAuthorized || CompilerEmissionAuthorized || NestedExecutionAuthorized || ProductionReleaseApproved; }
		static std::shared_ptr<SecureComputePhase22LimitedReleaseGateResult> Denied(SecureComputePhase22LimitedReleaseDecision decision, const std::wstring& reason)
		{
			return std::make_shared<SecureComputePhase22LimitedReleaseGateResult>(SecureComputePhase22LimitedReleaseGateResult{decision, false, false, false, false, false, false, false, false, false, reason});
		}
	};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecureComputePhase22LimitedReleaseEvidenceRequest(bool Phase21MatrixEvidenceAccepted, bool Phase21FuturePositiveTestsRemainGated, bool NamedPositiveRuntimePathProven, bool OwnerSpecificRfcAdrAccepted, bool ProductionOwnerCodeExists, bool OwnerPathReachabilityProven, bool TypedRequestResultModelExists, bool BackendResultOwnerBoundaryProven, bool CompletionRetirePolicyForNamedPathProven, bool MigrationManifestRestoreEvidenceComplete, bool DebugAttestationVisibilityLimitsProven, bool VmxProjectionAfterNeutralResultOnly, bool CompilerNoEmissionDecisionRecorded, bool Phase18NestedExecutionExcluded, bool ProductClaimScopedToNamedPath, bool BoundedRollbackProcedureReviewed, bool BackendExecutionAuthorityLocallyProven);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecureComputePhase22LimitedReleaseGateResult(SecureComputePhase22LimitedReleaseDecision Decision, bool ReleaseEvidenceAccepted, bool RuntimeExecutionAuthorized, bool BackendResultAccepted, bool CompletionPublicationAuthorized, bool RetirePublicationAuthorized, bool VmxAuthorityAuthorized, bool CompilerEmissionAuthorized, bool NestedExecutionAuthorized, bool ProductionReleaseApproved, string Reason)
//	{
//		public bool CreatesAnyReleaseAuthority
//		{
//			get
//			{
//				return RuntimeExecutionAuthorized || BackendResultAccepted || CompletionPublicationAuthorized || RetirePublicationAuthorized || VmxAuthorityAuthorized || CompilerEmissionAuthorized || NestedExecutionAuthorized || ProductionReleaseApproved;
//			}
//		}
//
//		public static SecureComputePhase22LimitedReleaseGateResult Denied(SecureComputePhase22LimitedReleaseDecision decision, string reason)
//		{
//			return new(decision, ReleaseEvidenceAccepted: false, RuntimeExecutionAuthorized: false, BackendResultAccepted: false, CompletionPublicationAuthorized: false, RetirePublicationAuthorized: false, VmxAuthorityAuthorized: false, CompilerEmissionAuthorized: false, NestedExecutionAuthorized: false, ProductionReleaseApproved: false, reason);
//		}
//	}

	class SecureComputePhase22LimitedReleaseGatePolicy final : public std::enable_shared_from_this<SecureComputePhase22LimitedReleaseGatePolicy>
	{
	private:
		static inline const std::shared_ptr<SecureComputePhase22LimitedReleaseGatePolicy> FailClosed = std::make_shared<SecureComputePhase22LimitedReleaseGatePolicy>();

	public:
		static std::shared_ptr<SecureComputePhase22LimitedReleaseGatePolicy> getFailClosed();

		std::shared_ptr<SecureComputePhase22LimitedReleaseGateResult> Classify(std::shared_ptr<SecureComputePhase22LimitedReleaseEvidenceRequest> request);

	private:
		static std::shared_ptr<SecureComputePhase22LimitedReleaseGateResult> Deny(SecureComputePhase22LimitedReleaseDecision decision, const std::wstring &reason);
	};

}
