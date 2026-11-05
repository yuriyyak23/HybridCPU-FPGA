#pragma once

#include "../../Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "../Descriptors/Backend/SecureBackendOwnerDescriptor.h"
#include "../Authority/SecureRevocationEpoch.h"
#include "YAKSys_Hybrid_CPU.Core.SecureCompletionPublicationFence.h"
#include "../Policies/Backend/SecureBackendOwnerAdmissionPolicy.h"
#include "../Policies/Backend/SecureHypercallBackendContractAdmissionPolicy.h"
#include "../Policies/Io/SecureIoHypercallAdmissionPolicy.h"
#include "../../../Completion/Records/TrapCompletionPublicationFence.h"
#include <string>
#include <optional>
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecurePublicationPathKind : unsigned char
	{
		None = 0,
		ProofOnlyBackendOwnerAdmission = 1,
		AdmittedDeniedSecureHypercallAdmission = 2,
		RegistryBackedHypercallContractAdmission = 3,
		SecureIoPolicyAdmissionOnly = 4,
		GenericTrapRouteFlags = 5,
		VmxCompatibilityProjectionOnly = 6,
		NeutralRuntimeBackendResult = 7
	};

	enum class SecurePublicationBackendResultState : unsigned char
	{
		Missing = 0,
		InternalNeutralResult = 1
	};

	enum class SecurePublicationLadderStep : unsigned char
	{
		NoBackendResult = 0,
		InternalBackendResult = 1,
		InternalCompletionRecord = 2,
		CompletionPublication = 3,
		RetirePublication = 4
	};

	enum class SecureCompletionRetirePublicationDecision : unsigned char
	{
		DeniedNoBackendResult = 0,
		DeniedProofOnlyAdmission = 1,
		DeniedAdmittedDeniedAdmission = 2,
		DeniedRegistryBackedProofOnlyAdmission = 3,
		DeniedPolicyAdmissionOnly = 4,
		DeniedGenericTrapRouteNotSecureAuthority = 5,
		DeniedVmxProjectionOnly = 6,
		DeniedBackendResultOwner = 7,
		DeniedCompletionRecordMissing = 8,
		DeniedCompletionOwnerMissing = 9,
		DeniedCompletionOwnerSource = 10,
		DeniedCompletionOwnerEpoch = 11,
		DeniedCompletionOwnerProof = 12,
		DeniedCompletionFence = 13,
		CompletionPublishedRetirePending = 14,
		DeniedCompletionPublicationRequired = 15,
		DeniedRetireOwnerMissing = 16,
		DeniedRetireOwnerSource = 17,
		DeniedRetireOwnerEpoch = 18,
		DeniedRetireOwnerProof = 19,
		DeniedRetireFence = 20,
		DeniedRetireEvidence = 21,
		DeniedRetireMigrationClass = 22,
		RetirePublished = 23
	};

struct SecureCompletionPublicationAuthorityRequest final {
    SecurePublicationPathKind PathKind = SecurePublicationPathKind::None;
    SecurePublicationBackendResultState BackendResultState = SecurePublicationBackendResultState::Missing;
    std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> BackendResultOwner;
    std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> CompletionOwner;
    std::shared_ptr<SecureCompletionPublicationFence> PublicationFence;
    std::shared_ptr<SecureRevocationEpoch> CurrentEpoch = SecureRevocationEpoch::Unmaterialized;
    bool CompletionRecordMaterialized = false;
};

struct SecureCompletionRetirePublicationResult final {
    SecureCompletionRetirePublicationDecision Decision;
    SecurePublicationLadderStep LadderStep;
    bool CompletionPublished;
    bool RetirePublished;
    std::wstring Reason;
    bool IsDenied;
    bool IsCompletionOnly;

    SecureCompletionRetirePublicationResult(SecureCompletionRetirePublicationDecision decision,
        SecurePublicationLadderStep ladderStep, bool completionPublished, bool retirePublished, std::wstring reason)
        : Decision(decision), LadderStep(ladderStep), CompletionPublished(completionPublished),
          RetirePublished(retirePublished), Reason(std::move(reason)),
          IsDenied(!completionPublished && !retirePublished), IsCompletionOnly(completionPublished && !retirePublished) {}
    static std::shared_ptr<SecureCompletionRetirePublicationResult> Denied(
        SecureCompletionRetirePublicationDecision decision, SecurePublicationLadderStep ladderStep, const std::wstring& reason) {
        return std::make_shared<SecureCompletionRetirePublicationResult>(decision, ladderStep, false, false, reason);
    }
    static std::shared_ptr<SecureCompletionRetirePublicationResult> PublishedCompletion(const std::wstring& reason) {
        return std::make_shared<SecureCompletionRetirePublicationResult>(SecureCompletionRetirePublicationDecision::CompletionPublishedRetirePending,
            SecurePublicationLadderStep::CompletionPublication, true, false, reason);
    }
    static std::shared_ptr<SecureCompletionRetirePublicationResult> PublishedRetire() {
        return std::make_shared<SecureCompletionRetirePublicationResult>(SecureCompletionRetirePublicationDecision::RetirePublished,
            SecurePublicationLadderStep::RetirePublication, true, true, L"");
    }
};

struct SecureRetirePublicationAuthorityRequest final {
    SecureCompletionRetirePublicationResult CompletionPublication;
    std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> RetireOwner;
    std::shared_ptr<SecureCompletionPublicationFence> PublicationFence;
    std::shared_ptr<SecureRevocationEpoch> CurrentEpoch = SecureRevocationEpoch::Unmaterialized;
    EvidenceVisibilityClass EvidenceClass = EvidenceVisibilityClass::HostOwnedRuntimeEvidence;
    TrapCompletionMigrationClass MigrationClass = TrapCompletionMigrationClass::Unclassified;
};
	class SecureCompletionRetirePublicationAuthorityPolicy final : public std::enable_shared_from_this<SecureCompletionRetirePublicationAuthorityPolicy>
	{
	private:
		static inline const std::shared_ptr<SecureCompletionRetirePublicationAuthorityPolicy> Default = std::make_shared<SecureCompletionRetirePublicationAuthorityPolicy>();

	public:
		static std::shared_ptr<SecureCompletionRetirePublicationAuthorityPolicy> getDefault();

		std::shared_ptr<SecureCompletionRetirePublicationResult> AdmitCompletion(std::shared_ptr<SecureCompletionPublicationAuthorityRequest> request);

		std::shared_ptr<SecureCompletionRetirePublicationResult> AdmitRetire(std::shared_ptr<SecureRetirePublicationAuthorityRequest> request);

		std::shared_ptr<SecureCompletionRetirePublicationResult> AdmitCompletionFromBackendOwnerAdmission(std::shared_ptr<SecureBackendOwnerAdmissionResult> admission);

		std::shared_ptr<SecureCompletionRetirePublicationResult> AdmitRetireFromBackendOwnerAdmission(std::shared_ptr<SecureBackendOwnerAdmissionResult> admission);

		std::shared_ptr<SecureCompletionRetirePublicationResult> AdmitCompletionFromSecureIoHypercallAdmission(std::shared_ptr<SecureIoHypercallAdmissionResult> admission);

		std::shared_ptr<SecureCompletionRetirePublicationResult> AdmitRetireFromSecureIoHypercallAdmission(std::shared_ptr<SecureIoHypercallAdmissionResult> admission);

		std::shared_ptr<SecureCompletionRetirePublicationResult> AdmitCompletionFromHypercallContractAdmission(std::shared_ptr<SecureHypercallBackendContractAdmissionResult> admission);

		std::shared_ptr<SecureCompletionRetirePublicationResult> AdmitRetireFromHypercallContractAdmission(std::shared_ptr<SecureHypercallBackendContractAdmissionResult> admission);

		std::shared_ptr<SecureCompletionRetirePublicationResult> DenyGenericTrapRouteFlags(bool completionPublicationFlag, bool retirePublicationFlag);

		std::shared_ptr<SecureCompletionRetirePublicationResult> DenyVmxCompatibilityProjectionOnly();

	private:
		static std::shared_ptr<SecureCompletionRetirePublicationResult> DenyCurrentNonPublishingPath(SecurePublicationPathKind pathKind);

		static std::optional<std::shared_ptr<SecureCompletionRetirePublicationResult>> ValidateCompletionOwner(std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> &owner, std::shared_ptr<SecureRevocationEpoch> currentEpoch);

		static std::optional<std::shared_ptr<SecureCompletionRetirePublicationResult>> ValidateRetireOwner(std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> &owner, std::shared_ptr<SecureRevocationEpoch> currentEpoch);

		static std::optional<std::shared_ptr<SecureCompletionRetirePublicationResult>> ValidateOwner(std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> &owner, std::shared_ptr<SecureRevocationEpoch> currentEpoch, SecureCompletionRetirePublicationDecision missingDecision, SecureCompletionRetirePublicationDecision sourceDecision, SecureCompletionRetirePublicationDecision epochDecision);

		static bool IsCurrentNeutralOwner(std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> &owner, std::shared_ptr<SecureRevocationEpoch> currentEpoch);

		static bool CanRetireEvidence(EvidenceVisibilityClass evidenceClass);

		static bool CanRetireMigrationClass(TrapCompletionMigrationClass migrationClass);

		static std::shared_ptr<SecureCompletionRetirePublicationResult> DenyNoBackendResult();

		static std::shared_ptr<SecureCompletionRetirePublicationResult> Deny(SecureCompletionRetirePublicationDecision decision, SecurePublicationLadderStep ladderStep, const std::wstring &reason);
	};

}
