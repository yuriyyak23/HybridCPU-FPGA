#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputePhase21ConformanceDecision : unsigned char
	{
		CurrentMatrixAcceptedFuturePositiveGated = 0,
		PositivePathEvidenceMatrixAcceptedPhase22Required = 1,
		DeniedMissingPhase13RegistryBackedAdmissionEvidence = 2,
		DeniedMissingPhase14CompletionRetireEvidence = 3,
		DeniedMissingPhase15ManifestEvidence = 4,
		DeniedMissingPhase16VisibilityEvidence = 5,
		DeniedMissingPhase17VmxZeroAuthorityEvidence = 6,
		DeniedMissingPhase18NestedDesignFenceEvidence = 7,
		DeniedMissingPhase19NoCompilerChangeEvidence = 8,
		DeniedMissingPhase20FutureGatedRuntimeEvidence = 9,
		DeniedMissingReleaseEvidenceBoundary = 10,
		DeniedProductionActivationOverclaim = 11,
		DeniedCompilerEmissionOverclaim = 12,
		DeniedVmxAuthorityOverclaim = 13,
		DeniedPhase18NestedOpened = 14,
		DeniedManifestOnlyExecutionProof = 15,
		DeniedVisibilityOnlyRuntimeEvidence = 16
	};

	struct SecureComputePhase21ConformanceEvidenceRequest final
	{
		bool Phase13RegistryBackedAdmissionCannotExecuteCovered, Phase14CompletionRetireFailClosedCovered, Phase15ManifestCoverageAndManifestOnlyDenialCovered;
		bool Phase16VisibilityCannotBecomeRuntimeEvidenceCovered, Phase17VmxZeroAuthorityCovered, Phase18NestedDesignFenceCovered;
		bool Phase19NoCompilerChangeCovered, Phase20FutureGatedRuntimeActivationCovered, ReleaseEvidenceBoundaryCovered;
		bool PositiveRuntimePathProven = false, RequestsProductionActivation = false, RequestsCompilerSecureEmission = false;
		bool RequestsVmxOwnedSecureComputeAuthority = false, OpensPhase18NestedExecution = false, TreatsManifestOnlyAsExecutionProof = false;
		bool TreatsDebugAttestationAsRuntimeEvidence = false;
	};
	struct SecureComputePhase21ConformanceEvidenceResult final
	{
		SecureComputePhase21ConformanceDecision Decision;
		bool MatrixEvidenceAccepted, FuturePositiveTestsRemainGated, RuntimeExecutionAuthorized, BackendResultAccepted;
		bool CompletionPublicationAuthorized, RetirePublicationAuthorized, VmxAuthorityAuthorized, CompilerEmissionAuthorized;
		bool NestedExecutionAuthorized, ProductionReleaseApproved;
		std::wstring Reason;
		bool CreatesAnyRuntimeAuthority() const noexcept { return RuntimeExecutionAuthorized || BackendResultAccepted || CompletionPublicationAuthorized || RetirePublicationAuthorized || VmxAuthorityAuthorized || CompilerEmissionAuthorized || NestedExecutionAuthorized || ProductionReleaseApproved; }
		static std::shared_ptr<SecureComputePhase21ConformanceEvidenceResult> Accepted(SecureComputePhase21ConformanceDecision decision, bool futurePositiveTestsRemainGated, const std::wstring& reason)
		{
			return std::make_shared<SecureComputePhase21ConformanceEvidenceResult>(SecureComputePhase21ConformanceEvidenceResult{decision, true, futurePositiveTestsRemainGated, false, false, false, false, false, false, false, false, reason});
		}
		static std::shared_ptr<SecureComputePhase21ConformanceEvidenceResult> Denied(SecureComputePhase21ConformanceDecision decision, const std::wstring& reason)
		{
			return std::make_shared<SecureComputePhase21ConformanceEvidenceResult>(SecureComputePhase21ConformanceEvidenceResult{decision, false, true, false, false, false, false, false, false, false, false, reason});
		}
	};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecureComputePhase21ConformanceEvidenceRequest(bool Phase13RegistryBackedAdmissionCannotExecuteCovered, bool Phase14CompletionRetireFailClosedCovered, bool Phase15ManifestCoverageAndManifestOnlyDenialCovered, bool Phase16VisibilityCannotBecomeRuntimeEvidenceCovered, bool Phase17VmxZeroAuthorityCovered, bool Phase18NestedDesignFenceCovered, bool Phase19NoCompilerChangeCovered, bool Phase20FutureGatedRuntimeActivationCovered, bool ReleaseEvidenceBoundaryCovered, bool PositiveRuntimePathProven = false, bool RequestsProductionActivation = false, bool RequestsCompilerSecureEmission = false, bool RequestsVmxOwnedSecureComputeAuthority = false, bool OpensPhase18NestedExecution = false, bool TreatsManifestOnlyAsExecutionProof = false, bool TreatsDebugAttestationAsRuntimeEvidence = false);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecureComputePhase21ConformanceEvidenceResult(SecureComputePhase21ConformanceDecision Decision, bool MatrixEvidenceAccepted, bool FuturePositiveTestsRemainGated, bool RuntimeExecutionAuthorized, bool BackendResultAccepted, bool CompletionPublicationAuthorized, bool RetirePublicationAuthorized, bool VmxAuthorityAuthorized, bool CompilerEmissionAuthorized, bool NestedExecutionAuthorized, bool ProductionReleaseApproved, string Reason)
//	{
//		public bool CreatesAnyRuntimeAuthority
//		{
//			get
//			{
//				return RuntimeExecutionAuthorized || BackendResultAccepted || CompletionPublicationAuthorized || RetirePublicationAuthorized || VmxAuthorityAuthorized || CompilerEmissionAuthorized || NestedExecutionAuthorized || ProductionReleaseApproved;
//			}
//		}
//
//		public static SecureComputePhase21ConformanceEvidenceResult Accepted(SecureComputePhase21ConformanceDecision decision, bool futurePositiveTestsRemainGated, string reason)
//		{
//			return new(decision, MatrixEvidenceAccepted: true, FuturePositiveTestsRemainGated: futurePositiveTestsRemainGated, RuntimeExecutionAuthorized: false, BackendResultAccepted: false, CompletionPublicationAuthorized: false, RetirePublicationAuthorized: false, VmxAuthorityAuthorized: false, CompilerEmissionAuthorized: false, NestedExecutionAuthorized: false, ProductionReleaseApproved: false, reason);
//		}
//
//		public static SecureComputePhase21ConformanceEvidenceResult Denied(SecureComputePhase21ConformanceDecision decision, string reason)
//		{
//			return new(decision, MatrixEvidenceAccepted: false, FuturePositiveTestsRemainGated: true, RuntimeExecutionAuthorized: false, BackendResultAccepted: false, CompletionPublicationAuthorized: false, RetirePublicationAuthorized: false, VmxAuthorityAuthorized: false, CompilerEmissionAuthorized: false, NestedExecutionAuthorized: false, ProductionReleaseApproved: false, reason);
//		}
//	}

	class SecureComputePhase21ConformanceEvidencePolicy final : public std::enable_shared_from_this<SecureComputePhase21ConformanceEvidencePolicy>
	{
	private:
		static inline const std::shared_ptr<SecureComputePhase21ConformanceEvidencePolicy> FailClosed = std::make_shared<SecureComputePhase21ConformanceEvidencePolicy>();

	public:
		static std::shared_ptr<SecureComputePhase21ConformanceEvidencePolicy> getFailClosed();

		std::shared_ptr<SecureComputePhase21ConformanceEvidenceResult> Classify(std::shared_ptr<SecureComputePhase21ConformanceEvidenceRequest> request);

	private:
		static std::shared_ptr<SecureComputePhase21ConformanceEvidenceResult> DenyAuthorityShortcut(std::shared_ptr<SecureComputePhase21ConformanceEvidenceRequest> request);

		static std::shared_ptr<SecureComputePhase21ConformanceEvidenceResult> DenyMissingEvidence(std::shared_ptr<SecureComputePhase21ConformanceEvidenceRequest> request);

		static std::shared_ptr<SecureComputePhase21ConformanceEvidenceResult> Deny(SecureComputePhase21ConformanceDecision decision, const std::wstring &reason);
	};

}
