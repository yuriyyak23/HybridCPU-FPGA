#pragma once

#include "../../Descriptors/Backend/SecureBackendOwnerDescriptor.h"
#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureBackendOwnerAdmissionDecision : unsigned char
	{
		AllowedProofOnlyNoExecution = 0,
		DeniedMissingNeutralOwner = 1,
		DeniedNonNeutralAuthoritySource = 2,
		DeniedMissingRfcAdrApproval = 3,
		DeniedMissingProofChain = 4,
		DeniedStaleEpoch = 5,
		DeniedMissingNegativeTests = 6,
		DeniedBackendExecutionClosed = 7
	};

struct SecureBackendOwnerAdmissionRequest final {
    std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> Owner;
    SecureBackendRfcAdrState RfcAdrState = SecureBackendRfcAdrState::Missing;
    std::shared_ptr<SecureRevocationEpoch> CurrentEpoch = SecureRevocationEpoch::Unmaterialized;
    bool RequestsBackendExecution = false;
};

struct SecureBackendOwnerAdmissionResult final {
    SecureBackendOwnerAdmissionDecision Decision;
    bool ProofChainAccepted;
    bool BackendExecutionAuthorized;
    std::wstring Reason;
    bool IsAllowed;

    SecureBackendOwnerAdmissionResult(SecureBackendOwnerAdmissionDecision decision, bool proofChainAccepted,
        bool backendExecutionAuthorized, std::wstring reason)
        : Decision(decision), ProofChainAccepted(proofChainAccepted),
          BackendExecutionAuthorized(backendExecutionAuthorized), Reason(std::move(reason)),
          IsAllowed(decision == SecureBackendOwnerAdmissionDecision::AllowedProofOnlyNoExecution) {}

    static std::shared_ptr<SecureBackendOwnerAdmissionResult> AllowedProofOnly() {
        return std::make_shared<SecureBackendOwnerAdmissionResult>(
            SecureBackendOwnerAdmissionDecision::AllowedProofOnlyNoExecution, true, false,
            L"Neutral backend owner proof chain is accepted only as policy evidence; secure backend execution remains closed.");
    }
    static std::shared_ptr<SecureBackendOwnerAdmissionResult> Denied(
        SecureBackendOwnerAdmissionDecision decision, const std::wstring& reason) {
        return std::make_shared<SecureBackendOwnerAdmissionResult>(decision, false, false, reason);
    }
};

}
