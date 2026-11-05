#pragma once

#include <string>
#include <optional>
#include <memory>
#include "../../Authority/SecureRevocationEpoch.h"
#include "../Backend/SecureBackendOwnerAdmissionPolicy.h"
#include "../../../../../Virtualization/SecureCompute/Conformance/NoEmission/SecureComputeControlledEmissionGatePolicy.h"
#include "../../Checkpoint/SecureOutputManifestClassificationPolicy.h"
#include "../Debug/SecureDebugAttestationVisibilityPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecurePositiveRuntimePathCandidate : unsigned char
	{
		None = 0,
		ProofOnlyBackendOwnerAdmission = 1,
		AdmittedDeniedSecureHypercallAdmission = 2,
		Phase13RegistryBackedHypercallContract = 3,
		Phase14PublicationLadderVocabulary = 4,
		Phase15OutputManifestClassification = 5,
		Phase16DebugAttestationVisibility = 6,
		Phase17VmxZeroAuthorityProjection = 7,
		Phase18NestedChildIntentDesignFence = 8,
		Phase19NoCompilerChangeDecision = 9,
		FutureNamedPositiveRuntimePath = 10
	};

	enum class SecurePositiveRuntimeExecutionActivationDecision : unsigned char
	{
		FutureGatedNoNamedPositivePath = 0,
		DeniedProofOnlyOwnerAdmission = 1,
		DeniedAdmittedDeniedHypercallAdmission = 2,
		DeniedPhase13ProofOnlyAdmission = 3,
		DeniedPhase14PublicationVocabularyOnly = 4,
		DeniedPhase15ManifestOnlyEvidence = 5,
		DeniedPhase16VisibilityOnlyEvidence = 6,
		DeniedPhase17VmxZeroAuthorityOnly = 7,
		DeniedPhase18NestedDesignFence = 8,
		DeniedPhase19NoCompilerChangeOnly = 9,
		DeniedMissingRuntimeAuthorityOwner = 10,
		DeniedMissingBackendResultOwnerBoundary = 11,
		DeniedMissingOwnerPathReachabilityProof = 12,
		DeniedMissingNeutralBackendResult = 13,
		DeniedMissingMigrationOutputManifestEvidence = 14,
		DeniedMissingRestoreRules = 15,
		DeniedMissingCompilerNoEmissionDecision = 16,
		DeniedProductionReleaseGate = 17
	};

    enum class SecureBackendOwnerSource : unsigned char;
    enum class SecureBackendOwnerAdmissionDecision : unsigned char;
    enum class SecureHypercallBackendContractDecision : unsigned char;
    enum class SecureIoHypercallAdmissionDecision : unsigned char;
    enum class SecureCompletionRetirePublicationDecision : unsigned char;
    enum class SecurePublicationLadderStep : unsigned char;
    enum class SecureOutputManifestClassificationDecision : unsigned char;
    enum class SecureOutputManifestEntryKind : unsigned char;
    enum class SecureCheckpointPayloadDecision : unsigned char;
    enum class SecureDebugAttestationVisibilityDecision : unsigned char;
    enum class SecureEvidenceVisibilityClass : unsigned char;
    enum class SecureComputeNamedPathVmxZeroAuthorityDecision : unsigned char;
    enum class SecureComputeNamedPositivePath : unsigned char;
    enum class SecureNestedDomainAdmissionDecision : unsigned char;
    enum class SecureComputeControlledEmissionDecision : unsigned char;
    enum class SecureComputeCompilerEmissionPath : unsigned char;

    enum class SecureBackendOwnerSource : unsigned char;
    enum class SecureBackendOwnerAdmissionDecision : unsigned char;
    enum class SecureHypercallBackendContractDecision : unsigned char;
    enum class SecureIoHypercallAdmissionDecision : unsigned char;
    enum class SecureCompletionRetirePublicationDecision : unsigned char;
    enum class SecurePublicationLadderStep : unsigned char;
    enum class SecureOutputManifestClassificationDecision : unsigned char;
    enum class SecureOutputManifestEntryKind : unsigned char;
    enum class SecureCheckpointPayloadDecision : unsigned char;
    enum class SecureDebugAttestationVisibilityDecision : unsigned char;
    enum class SecureEvidenceVisibilityClass : unsigned char;
    enum class SecureComputeNamedPathVmxZeroAuthorityDecision : unsigned char;
    enum class SecureComputeNamedPositivePath : unsigned char;
    enum class SecureNestedDomainAdmissionDecision : unsigned char;
    enum class SecureComputeControlledEmissionDecision : unsigned char;
    enum class SecureComputeCompilerEmissionPath : unsigned char;

    enum class SecureBackendOwnerSource : unsigned char;
    enum class SecureBackendOwnerAdmissionDecision : unsigned char;
    enum class SecureHypercallBackendContractDecision : unsigned char;
    enum class SecureIoHypercallAdmissionDecision : unsigned char;
    enum class SecureCompletionRetirePublicationDecision : unsigned char;
    enum class SecurePublicationLadderStep : unsigned char;
    enum class SecureOutputManifestClassificationDecision : unsigned char;
    enum class SecureOutputManifestEntryKind : unsigned char;
    enum class SecureCheckpointPayloadDecision : unsigned char;
    enum class SecureDebugAttestationVisibilityDecision : unsigned char;
    enum class SecureEvidenceVisibilityClass : unsigned char;
    enum class SecureComputeNamedPathVmxZeroAuthorityDecision : unsigned char;
    enum class SecureComputeNamedPositivePath : unsigned char;
    enum class SecureNestedDomainAdmissionDecision : unsigned char;
    enum class SecureComputeControlledEmissionDecision : unsigned char;
    enum class SecureComputeCompilerEmissionPath : unsigned char;

    enum class SecureBackendOwnerSource : unsigned char;
    enum class SecureBackendOwnerAdmissionDecision : unsigned char;
    enum class SecureHypercallBackendContractDecision : unsigned char;
    enum class SecureIoHypercallAdmissionDecision : unsigned char;
    enum class SecureCompletionRetirePublicationDecision : unsigned char;
    enum class SecurePublicationLadderStep : unsigned char;
    enum class SecureOutputManifestClassificationDecision : unsigned char;
    enum class SecureOutputManifestEntryKind : unsigned char;
    enum class SecureCheckpointPayloadDecision : unsigned char;
    enum class SecureDebugAttestationVisibilityDecision : unsigned char;
    enum class SecureEvidenceVisibilityClass : unsigned char;
    enum class SecureComputeNamedPathVmxZeroAuthorityDecision : unsigned char;
    enum class SecureComputeNamedPositivePath : unsigned char;
    enum class SecureNestedDomainAdmissionDecision : unsigned char;
    enum class SecureComputeControlledEmissionDecision : unsigned char;
    enum class SecureComputeCompilerEmissionPath : unsigned char;
    struct SecureHypercallBackendContractAdmissionResult final
    {
        SecureHypercallBackendContractDecision Decision{};
        bool ContractProofAccepted = false, BackendExecutionAuthorized = false, CompletionPublicationAuthorized = false, RetirePublicationAuthorized = false;
        std::wstring Reason;
        bool IsProofOnly() const noexcept { return static_cast<unsigned char>(Decision) == 0 && ContractProofAccepted && !BackendExecutionAuthorized && !CompletionPublicationAuthorized && !RetirePublicationAuthorized; }
    };
    struct SecureIoHypercallAdmissionResult final
    {
        SecureIoHypercallAdmissionDecision Decision{};
        bool BackendExecutionAuthorized = false, CompletionPublicationAuthorized = false, RetirePublicationAuthorized = false;
        std::wstring Reason;
        bool IsAdmittedDenied() const noexcept { return static_cast<unsigned char>(Decision) == 1 && !BackendExecutionAuthorized; }
        bool IsPolicyAdmissionOnly() const noexcept { return static_cast<unsigned char>(Decision) == 0 && !BackendExecutionAuthorized && !CompletionPublicationAuthorized && !RetirePublicationAuthorized; }
    };
    struct SecureCompletionRetirePublicationResult final
    {
        SecureCompletionRetirePublicationDecision Decision{};
        SecurePublicationLadderStep LadderStep{};
        bool CompletionPublished = false, RetirePublished = false;
        std::wstring Reason;
    };
    struct SecureComputeNamedPathVmxZeroAuthorityResult final
    {
        SecureComputeNamedPathVmxZeroAuthorityDecision Decision{};
        SecureComputeNamedPositivePath Path{};
        bool CompatibilityProjectionAllowed = false, VmxActivationAuthorized = false, VmxCapsAuthorityAuthorized = false, VmcsStateStoreAuthorized = false;
        bool ActiveVmcsPointerIdentityAuthorized = false, VmreadSecureStateAuthorityAuthorized = false, VmwriteSecureStateMutationAuthorized = false;
        bool VmcsCheckpointAuthorityAuthorized = false, CompletionPublicationAuthorized = false, RetirePublicationAuthorized = false;
        std::wstring Reason;
        bool IsAllowed() const noexcept { return static_cast<unsigned char>(Decision) == 0; }
    };
    struct SecureNestedDomainAdmissionResult final
    {
        SecureNestedDomainAdmissionDecision Decision{};
        std::wstring Reason;
        bool BackendSuccessAuthorized = false, MutableNestedStateAuthorized = false;
        bool IsAllowed() const noexcept { return static_cast<unsigned char>(Decision) <= 1; }
    };
    struct SecurePositiveRuntimeExecutionActivationRequest final
    {
        SecurePositiveRuntimePathCandidate Candidate = SecurePositiveRuntimePathCandidate::None;
        std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> RuntimeAuthorityOwner;
        bool BackendResultOwnerBoundaryProven = false, OwnerPathReachabilityProven = false, NeutralBackendResultProduced = false;
        bool OutputManifestCoverageComplete = false, RestoreRulesClassified = false, CompilerNoEmissionDecisionRecorded = false;
        bool Phase18NestedExecutionExcluded = true;
    };
    struct SecurePositiveRuntimeExecutionActivationResult final
    {
        SecurePositiveRuntimeExecutionActivationDecision Decision = SecurePositiveRuntimeExecutionActivationDecision::FutureGatedNoNamedPositivePath;
        SecurePositiveRuntimePathCandidate Candidate = SecurePositiveRuntimePathCandidate::None;
        bool RuntimeExecutionAuthorized = false, BackendResultAccepted = false, CompletionPublicationAuthorized = false, RetirePublicationAuthorized = false;
        bool VmxAuthorityAuthorized = false, CompilerEmissionAuthorized = false, NestedExecutionAuthorized = false, ProductionReleaseApproved = false;
        std::wstring Reason;
        bool IsFutureGated() const noexcept { return !RuntimeExecutionAuthorized && !BackendResultAccepted && !CompletionPublicationAuthorized && !RetirePublicationAuthorized && !VmxAuthorityAuthorized && !CompilerEmissionAuthorized && !NestedExecutionAuthorized && !ProductionReleaseApproved; }
        static std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> FutureGated(SecurePositiveRuntimeExecutionActivationDecision decision, SecurePositiveRuntimePathCandidate candidate, const std::wstring& reason)
        {
            auto result = std::make_shared<SecurePositiveRuntimeExecutionActivationResult>();
            result->Decision = decision; result->Candidate = candidate; result->Reason = reason; return result;
        }
    };
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecurePositiveRuntimeExecutionActivationRequest(SecurePositiveRuntimePathCandidate Candidate, System.Nullable<SecureBackendOwnerDescriptor> RuntimeAuthorityOwner = nullptr, bool BackendResultOwnerBoundaryProven = false, bool OwnerPathReachabilityProven = false, bool NeutralBackendResultProduced = false, bool OutputManifestCoverageComplete = false, bool RestoreRulesClassified = false, bool CompilerNoEmissionDecisionRecorded = false, bool Phase18NestedExecutionExcluded = true);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecurePositiveRuntimeExecutionActivationResult(SecurePositiveRuntimeExecutionActivationDecision Decision, SecurePositiveRuntimePathCandidate Candidate, bool RuntimeExecutionAuthorized, bool BackendResultAccepted, bool CompletionPublicationAuthorized, bool RetirePublicationAuthorized, bool VmxAuthorityAuthorized, bool CompilerEmissionAuthorized, bool NestedExecutionAuthorized, bool ProductionReleaseApproved, string Reason)
//	{
//		public bool IsFutureGated
//		{
//			get
//			{
//				return !RuntimeExecutionAuthorized && !BackendResultAccepted && !CompletionPublicationAuthorized && !RetirePublicationAuthorized && !VmxAuthorityAuthorized && !CompilerEmissionAuthorized && !NestedExecutionAuthorized && !ProductionReleaseApproved;
//			}
//		}
//
//		public static SecurePositiveRuntimeExecutionActivationResult FutureGated(SecurePositiveRuntimeExecutionActivationDecision decision, SecurePositiveRuntimePathCandidate candidate, string reason)
//		{
//			return new(decision, candidate, RuntimeExecutionAuthorized: false, BackendResultAccepted: false, CompletionPublicationAuthorized: false, RetirePublicationAuthorized: false, VmxAuthorityAuthorized: false, CompilerEmissionAuthorized: false, NestedExecutionAuthorized: false, ProductionReleaseApproved: false, reason);
//		}
//	}

	class SecurePositiveRuntimeExecutionActivationPolicy final : public std::enable_shared_from_this<SecurePositiveRuntimeExecutionActivationPolicy>
	{
	private:
		static inline const std::shared_ptr<SecurePositiveRuntimeExecutionActivationPolicy> FailClosed = std::make_shared<SecurePositiveRuntimeExecutionActivationPolicy>();

	public:
		static std::shared_ptr<SecurePositiveRuntimeExecutionActivationPolicy> getFailClosed();

		std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> Classify(std::shared_ptr<SecurePositiveRuntimeExecutionActivationRequest> request);

		std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> ClassifyPhase13(std::shared_ptr<SecureHypercallBackendContractAdmissionResult> admission);

		std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> ClassifyBackendOwnerAdmission(std::shared_ptr<SecureBackendOwnerAdmissionResult> admission);

		std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> ClassifySecureIoHypercallAdmission(std::shared_ptr<SecureIoHypercallAdmissionResult> admission);

		std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> ClassifyPhase14(std::shared_ptr<SecureCompletionRetirePublicationResult> publication);

		std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> ClassifyPhase15(std::shared_ptr<SecureOutputManifestClassificationResult> manifest);

		std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> ClassifyPhase16(std::shared_ptr<SecureDebugAttestationVisibilityResult> visibility);

		std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> ClassifyPhase17(std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityResult> zeroAuthority);

		std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> ClassifyPhase18(std::shared_ptr<SecureNestedDomainAdmissionResult> nested);

		std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> ClassifyPhase19(std::shared_ptr<SecureComputeControlledEmissionResult> compilerDecision);

	private:
		static std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> DenyCurrentEvidenceOnlyCandidate(SecurePositiveRuntimePathCandidate candidate);

		static bool IsMaterializedNeutralOwner(std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> &owner);

		static std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> Deny(SecurePositiveRuntimeExecutionActivationDecision decision, SecurePositiveRuntimePathCandidate candidate, const std::wstring &reason);
	};

}
