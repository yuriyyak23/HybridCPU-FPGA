#include "SecureComputePhase21ConformanceEvidencePolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureComputePhase21ConformanceEvidencePolicy> SecureComputePhase21ConformanceEvidencePolicy::getFailClosed()
	{
		return FailClosed;
	}

	std::shared_ptr<SecureComputePhase21ConformanceEvidenceResult> SecureComputePhase21ConformanceEvidencePolicy::Classify(std::shared_ptr<SecureComputePhase21ConformanceEvidenceRequest> request)
	{
		std::shared_ptr<SecureComputePhase21ConformanceEvidenceResult> shortcut = DenyAuthorityShortcut(request);
		if (shortcut->Decision != SecureComputePhase21ConformanceDecision::CurrentMatrixAcceptedFuturePositiveGated)
		{
			return shortcut;
		}

		std::shared_ptr<SecureComputePhase21ConformanceEvidenceResult> missingEvidence = DenyMissingEvidence(request);
		if (missingEvidence->Decision != SecureComputePhase21ConformanceDecision::CurrentMatrixAcceptedFuturePositiveGated)
		{
			return missingEvidence;
		}

		if (request->PositiveRuntimePathProven)
		{
			return SecureComputePhase21ConformanceEvidenceResult::Accepted(SecureComputePhase21ConformanceDecision::PositivePathEvidenceMatrixAcceptedPhase22Required, false, L"Phase 21 can package a named positive-path evidence matrix for Phase 22, but it does not approve release.");
		}

		return SecureComputePhase21ConformanceEvidenceResult::Accepted(SecureComputePhase21ConformanceDecision::CurrentMatrixAcceptedFuturePositiveGated, true, L"Phase 21 accepts the current negative conformance matrix only as future-gated evidence.");
	}

	std::shared_ptr<SecureComputePhase21ConformanceEvidenceResult> SecureComputePhase21ConformanceEvidencePolicy::DenyAuthorityShortcut(std::shared_ptr<SecureComputePhase21ConformanceEvidenceRequest> request)
	{
		if (request->RequestsProductionActivation)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedProductionActivationOverclaim, L"Phase 21 conformance evidence cannot approve production activation.");
		}

		if (request->RequestsCompilerSecureEmission)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedCompilerEmissionOverclaim, L"Phase 21 conformance evidence cannot open compiler secure emission.");
		}

		if (request->RequestsVmxOwnedSecureComputeAuthority)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedVmxAuthorityOverclaim, L"Phase 21 conformance evidence cannot make VMX own SecureCompute authority.");
		}

		if (request->OpensPhase18NestedExecution)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedPhase18NestedOpened, L"Phase 21 keeps Phase 18 nested execution future/design-fenced.");
		}

		if (request->TreatsManifestOnlyAsExecutionProof)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedManifestOnlyExecutionProof, L"Phase 21 requires Phase 15 manifest coverage but manifest-only evidence is not execution proof.");
		}

		if (request->TreatsDebugAttestationAsRuntimeEvidence)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedVisibilityOnlyRuntimeEvidence, L"Phase 21 requires Phase 16 visibility coverage but debug/attestation visibility is not runtime evidence.");
		}

		return SecureComputePhase21ConformanceEvidenceResult::Denied(SecureComputePhase21ConformanceDecision::CurrentMatrixAcceptedFuturePositiveGated, L"");
	}

	std::shared_ptr<SecureComputePhase21ConformanceEvidenceResult> SecureComputePhase21ConformanceEvidencePolicy::DenyMissingEvidence(std::shared_ptr<SecureComputePhase21ConformanceEvidenceRequest> request)
	{
		if (!request->Phase13RegistryBackedAdmissionCannotExecuteCovered)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedMissingPhase13RegistryBackedAdmissionEvidence, L"Phase 21 requires Phase 13 registry-backed admission no-execution evidence.");
		}

		if (!request->Phase14CompletionRetireFailClosedCovered)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedMissingPhase14CompletionRetireEvidence, L"Phase 21 requires Phase 14 completion/retire fail-closed evidence.");
		}

		if (!request->Phase15ManifestCoverageAndManifestOnlyDenialCovered)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedMissingPhase15ManifestEvidence, L"Phase 21 requires Phase 15 manifest coverage and manifest-only denial evidence.");
		}

		if (!request->Phase16VisibilityCannotBecomeRuntimeEvidenceCovered)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedMissingPhase16VisibilityEvidence, L"Phase 21 requires Phase 16 debug/attestation visibility denial evidence.");
		}

		if (!request->Phase17VmxZeroAuthorityCovered)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedMissingPhase17VmxZeroAuthorityEvidence, L"Phase 21 requires Phase 17 VMX zero-authority evidence.");
		}

		if (!request->Phase18NestedDesignFenceCovered)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedMissingPhase18NestedDesignFenceEvidence, L"Phase 21 requires explicit Phase 18 nested design-fence evidence.");
		}

		if (!request->Phase19NoCompilerChangeCovered)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedMissingPhase19NoCompilerChangeEvidence, L"Phase 21 requires Phase 19 no-compiler-change evidence.");
		}

		if (!request->Phase20FutureGatedRuntimeActivationCovered)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedMissingPhase20FutureGatedRuntimeEvidence, L"Phase 21 requires Phase 20 future-gated runtime activation evidence.");
		}

		if (!request->ReleaseEvidenceBoundaryCovered)
		{
			return Deny(SecureComputePhase21ConformanceDecision::DeniedMissingReleaseEvidenceBoundary, L"Phase 21 requires release-evidence boundary coverage for Phase 22 consumption.");
		}

		return SecureComputePhase21ConformanceEvidenceResult::Denied(SecureComputePhase21ConformanceDecision::CurrentMatrixAcceptedFuturePositiveGated, L"");
	}

	std::shared_ptr<SecureComputePhase21ConformanceEvidenceResult> SecureComputePhase21ConformanceEvidencePolicy::Deny(SecureComputePhase21ConformanceDecision decision, const std::wstring &reason)
	{
		return SecureComputePhase21ConformanceEvidenceResult::Denied(decision, reason);
	}
}
