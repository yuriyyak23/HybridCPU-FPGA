#include "SecurePositiveRuntimeExecutionActivationPolicy.h"
#include "../Backend/SecureBackendOwnerAdmissionPolicy.h"
#include "../../../../../Virtualization/SecureCompute/Conformance/NoEmission/SecureComputeControlledEmissionGatePolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecurePositiveRuntimeExecutionActivationPolicy> SecurePositiveRuntimeExecutionActivationPolicy::getFailClosed()
	{
		return FailClosed;
	}

	std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> SecurePositiveRuntimeExecutionActivationPolicy::Classify(std::shared_ptr<SecurePositiveRuntimeExecutionActivationRequest> request)
	{
		std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> currentGate = DenyCurrentEvidenceOnlyCandidate(request->Candidate);
		if (currentGate->Decision != SecurePositiveRuntimeExecutionActivationDecision::FutureGatedNoNamedPositivePath)
		{
			return currentGate;
		}

		if (request->Candidate != SecurePositiveRuntimePathCandidate::FutureNamedPositiveRuntimePath)
		{
			return currentGate;
		}

		if (!IsMaterializedNeutralOwner(request->RuntimeAuthorityOwner))
		{
			return Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedMissingRuntimeAuthorityOwner, request->Candidate, L"Phase 20 requires an exact neutral runtime authority owner before any positive path can execute.");
		}

		if (!request->BackendResultOwnerBoundaryProven)
		{
			return Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedMissingBackendResultOwnerBoundary, request->Candidate, L"Phase 20 requires a backend-result owner boundary that is not admission, visibility or manifest evidence.");
		}

		if (!request->OwnerPathReachabilityProven)
		{
			return Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedMissingOwnerPathReachabilityProof, request->Candidate, L"Phase 20 requires scoped owner/path/reachability proof from the named owner to the runtime path.");
		}

		if (!request->NeutralBackendResultProduced)
		{
			return Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedMissingNeutralBackendResult, request->Candidate, L"Phase 20 requires a neutral backend result produced by the named runtime owner.");
		}

		if (!request->OutputManifestCoverageComplete)
		{
			return Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedMissingMigrationOutputManifestEvidence, request->Candidate, L"Phase 20 requires complete migration/output-manifest coverage, but manifest coverage alone is not execution proof.");
		}

		if (!request->RestoreRulesClassified)
		{
			return Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedMissingRestoreRules, request->Candidate, L"Phase 20 requires restore-time revalidation or recomputation rules for every classified output.");
		}

		if (!request->Phase18NestedExecutionExcluded)
		{
			return Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedPhase18NestedDesignFence, request->Candidate, L"Phase 18 nested execution, mutable nested state and nested publication remain design-fenced.");
		}

		if (!request->CompilerNoEmissionDecisionRecorded)
		{
			return Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedMissingCompilerNoEmissionDecision, request->Candidate, L"Phase 20 requires the Phase 19 no-compiler-change decision for the named path.");
		}

		return Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedProductionReleaseGate, request->Candidate, L"Phase 20 planning evidence does not approve production activation; Phase 22 remains the release gate.");
	}

	std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> SecurePositiveRuntimeExecutionActivationPolicy::ClassifyPhase13(std::shared_ptr<SecureHypercallBackendContractAdmissionResult> admission)
	{
		return admission->IsProofOnly() ? Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedPhase13ProofOnlyAdmission, SecurePositiveRuntimePathCandidate::Phase13RegistryBackedHypercallContract, L"Registry-backed Phase 13 admission is proof-only and cannot become runtime execution.") : Deny(SecurePositiveRuntimeExecutionActivationDecision::FutureGatedNoNamedPositivePath, SecurePositiveRuntimePathCandidate::Phase13RegistryBackedHypercallContract, L"Phase 13 did not produce a positive named runtime path.");
	}

	std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> SecurePositiveRuntimeExecutionActivationPolicy::ClassifyBackendOwnerAdmission(std::shared_ptr<SecureBackendOwnerAdmissionResult> admission)
	{
		return admission->IsAllowed ? Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedProofOnlyOwnerAdmission, SecurePositiveRuntimePathCandidate::ProofOnlyBackendOwnerAdmission, L"Proof-only backend owner admission cannot become runtime execution.") : Deny(SecurePositiveRuntimeExecutionActivationDecision::FutureGatedNoNamedPositivePath, SecurePositiveRuntimePathCandidate::ProofOnlyBackendOwnerAdmission, L"Backend owner admission did not produce a positive named runtime path.");
	}

	std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> SecurePositiveRuntimeExecutionActivationPolicy::ClassifySecureIoHypercallAdmission(std::shared_ptr<SecureIoHypercallAdmissionResult> admission)
	{
		return admission->IsAdmittedDenied() ? Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedAdmittedDeniedHypercallAdmission, SecurePositiveRuntimePathCandidate::AdmittedDeniedSecureHypercallAdmission, L"Admitted-denied secure hypercall recognition cannot become runtime execution.") : admission->IsPolicyAdmissionOnly() ? Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedAdmittedDeniedHypercallAdmission, SecurePositiveRuntimePathCandidate::AdmittedDeniedSecureHypercallAdmission, L"Secure policy admission cannot become runtime execution.") : Deny(SecurePositiveRuntimeExecutionActivationDecision::FutureGatedNoNamedPositivePath, SecurePositiveRuntimePathCandidate::AdmittedDeniedSecureHypercallAdmission, L"Secure hypercall admission did not produce a positive named runtime path.");
	}

	std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> SecurePositiveRuntimeExecutionActivationPolicy::ClassifyPhase14(std::shared_ptr<SecureCompletionRetirePublicationResult> publication)
	{
		return Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedPhase14PublicationVocabularyOnly, SecurePositiveRuntimePathCandidate::Phase14PublicationLadderVocabulary, publication->CompletionPublished || publication->RetirePublished ? L"Phase 14 publication vocabulary cannot prove Phase 20 owner/path/reachability or backend-result production." : L"Phase 14 fail-closed publication denial cannot become runtime execution.");
	}

	std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> SecurePositiveRuntimeExecutionActivationPolicy::ClassifyPhase15(std::shared_ptr<SecureOutputManifestClassificationResult> manifest)
	{
		return manifest->ManifestClassified ? Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedPhase15ManifestOnlyEvidence, SecurePositiveRuntimePathCandidate::Phase15OutputManifestClassification, L"Phase 15 output-manifest coverage is required evidence, not runtime execution proof.") : Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedMissingMigrationOutputManifestEvidence, SecurePositiveRuntimePathCandidate::Phase15OutputManifestClassification, L"Missing Phase 15 output-manifest coverage blocks any Phase 20 positive path.");
	}

	std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> SecurePositiveRuntimeExecutionActivationPolicy::ClassifyPhase16(std::shared_ptr<SecureDebugAttestationVisibilityResult> visibility)
	{
		return visibility->IsAllowed ? Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedPhase16VisibilityOnlyEvidence, SecurePositiveRuntimePathCandidate::Phase16DebugAttestationVisibility, L"Phase 16 debug/attestation visibility cannot become runtime evidence.") : Deny(SecurePositiveRuntimeExecutionActivationDecision::FutureGatedNoNamedPositivePath, SecurePositiveRuntimePathCandidate::Phase16DebugAttestationVisibility, L"Phase 16 denied visibility does not produce a runtime path.");
	}

	std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> SecurePositiveRuntimeExecutionActivationPolicy::ClassifyPhase17(std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityResult> zeroAuthority)
	{
		return zeroAuthority->IsAllowed() ? Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedPhase17VmxZeroAuthorityOnly, SecurePositiveRuntimePathCandidate::Phase17VmxZeroAuthorityProjection, L"Phase 17 VMX compatibility projection remains zero-authority and cannot become runtime execution.") : Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedPhase17VmxZeroAuthorityOnly, SecurePositiveRuntimePathCandidate::Phase17VmxZeroAuthorityProjection, L"Phase 17 denial preserves zero-authority and does not produce a runtime path.");
	}

	std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> SecurePositiveRuntimeExecutionActivationPolicy::ClassifyPhase18(std::shared_ptr<SecureNestedDomainAdmissionResult> nested)
	{
		return nested->IsAllowed() ? Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedPhase18NestedDesignFence, SecurePositiveRuntimePathCandidate::Phase18NestedChildIntentDesignFence, L"Phase 18 nested child intent remains design-fenced and cannot become runtime execution.") : Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedPhase18NestedDesignFence, SecurePositiveRuntimePathCandidate::Phase18NestedChildIntentDesignFence, L"Phase 18 nested denial preserves the design fence and does not produce a runtime path.");
	}

	std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> SecurePositiveRuntimeExecutionActivationPolicy::ClassifyPhase19(std::shared_ptr<SecureComputeControlledEmissionResult> compilerDecision)
	{
		return compilerDecision->IsAllowed() ? Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedPhase19NoCompilerChangeOnly, SecurePositiveRuntimePathCandidate::Phase19NoCompilerChangeDecision, L"Phase 19 no-compiler-change is required, but it is not runtime execution proof.") : Deny(SecurePositiveRuntimeExecutionActivationDecision::DeniedPhase19NoCompilerChangeOnly, SecurePositiveRuntimePathCandidate::Phase19NoCompilerChangeDecision, L"Phase 19 denied compiler emission does not produce a runtime path.");
	}

	std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> SecurePositiveRuntimeExecutionActivationPolicy::DenyCurrentEvidenceOnlyCandidate(SecurePositiveRuntimePathCandidate candidate)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return candidate switch
	//			{
	//				SecurePositiveRuntimePathCandidate.Phase13RegistryBackedHypercallContract => Deny(
	//						SecurePositiveRuntimeExecutionActivationDecision.DeniedPhase13ProofOnlyAdmission, candidate, "Phase 13 registry-backed admission is proof-only and cannot execute."), SecurePositiveRuntimePathCandidate.ProofOnlyBackendOwnerAdmission => Deny(
	//						SecurePositiveRuntimeExecutionActivationDecision.DeniedProofOnlyOwnerAdmission,
	//						candidate,
	//						"Proof-only owner admission is not runtime execution."),
	//				SecurePositiveRuntimePathCandidate.AdmittedDeniedSecureHypercallAdmission =>
	//					Deny(
	//						SecurePositiveRuntimeExecutionActivationDecision.DeniedAdmittedDeniedHypercallAdmission,
	//						candidate,
	//						"Admitted-denied hypercall recognition is not runtime execution."),
	//				SecurePositiveRuntimePathCandidate.Phase14PublicationLadderVocabulary =>
	//					Deny(
	//						SecurePositiveRuntimeExecutionActivationDecision.DeniedPhase14PublicationVocabularyOnly,
	//						candidate,
	//						"Phase 14 publication ladder vocabulary is not a runtime owner path."),
	//				SecurePositiveRuntimePathCandidate.Phase15OutputManifestClassification =>
	//					Deny(
	//						SecurePositiveRuntimeExecutionActivationDecision.DeniedPhase15ManifestOnlyEvidence,
	//						candidate,
	//						"Phase 15 manifest classification is evidence coverage only."),
	//				SecurePositiveRuntimePathCandidate.Phase16DebugAttestationVisibility =>
	//					Deny(
	//						SecurePositiveRuntimeExecutionActivationDecision.DeniedPhase16VisibilityOnlyEvidence,
	//						candidate,
	//						"Phase 16 visibility is not runtime evidence."),
	//				SecurePositiveRuntimePathCandidate.Phase17VmxZeroAuthorityProjection =>
	//					Deny(
	//						SecurePositiveRuntimeExecutionActivationDecision.DeniedPhase17VmxZeroAuthorityOnly,
	//						candidate,
	//						"Phase 17 compatibility projection remains zero-authority."),
	//				SecurePositiveRuntimePathCandidate.Phase18NestedChildIntentDesignFence =>
	//					Deny(
	//						SecurePositiveRuntimeExecutionActivationDecision.DeniedPhase18NestedDesignFence,
	//						candidate,
	//						"Phase 18 nested child intent remains future/design-fenced."),
	//				SecurePositiveRuntimePathCandidate.Phase19NoCompilerChangeDecision =>
	//					Deny(
	//						SecurePositiveRuntimeExecutionActivationDecision.DeniedPhase19NoCompilerChangeOnly,
	//						candidate,
	//						"Phase 19 no-compiler-change is not runtime execution proof."),
	//				_ =>
	//					Deny(
	//						SecurePositiveRuntimeExecutionActivationDecision.FutureGatedNoNamedPositivePath,
	//						candidate,
	//						"Phase 20 has no locally proven named positive runtime owner/path/reachability chain."),
	//			};
	}

	bool SecurePositiveRuntimeExecutionActivationPolicy::IsMaterializedNeutralOwner(std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> &owner)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		return owner.has_value() && owner.value() && owner.value()->Materialized && owner.value()->HasIdentity && owner.value()->IsNeutralSource && owner.value()->HasProofChain && owner.value()->NegativeTestsPresent;
	}

	std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> SecurePositiveRuntimeExecutionActivationPolicy::Deny(SecurePositiveRuntimeExecutionActivationDecision decision, SecurePositiveRuntimePathCandidate candidate, const std::wstring &reason)
	{
		return SecurePositiveRuntimeExecutionActivationResult::FutureGated(decision, candidate, reason);
	}
}
