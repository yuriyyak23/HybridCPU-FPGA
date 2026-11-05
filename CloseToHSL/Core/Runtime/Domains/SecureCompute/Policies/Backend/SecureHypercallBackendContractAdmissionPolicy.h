#pragma once

#include "../../Authority/SecureRevocationEpoch.h"
#include <memory>
#include <string>
#include <utility>

#include "../../Descriptors/Hypercalls/SecureHypercallBackendContract.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureHypercallBackendContractDecision : unsigned char
	{
		AllowedProofOnlyNoExecution = 0,
		DeniedUnresolvedContract = 1,
		DeniedUnknownServiceId = 2,
		DeniedDecodedLeafMismatch = 3,
		DeniedUnsupportedContractVersion = 4,
		DeniedMissingOwner = 5,
		DeniedWrongOwner = 6,
		DeniedOwnerEpochMismatch = 7,
		DeniedMissingGrant = 8,
		DeniedMissingEvidence = 9,
		DeniedStaleEvidence = 10,
		DeniedInvalidSharedBuffer = 11,
		DeniedRawPointerRepresentation = 12,
		DeniedInvalidOpaqueHandle = 13,
		DeniedReplayViolation = 14,
		DeniedCancelledBeforeExecution = 15,
		DeniedTransportOpcodeMismatch = 16
	};

struct SecureHypercallBackendContractAdmissionResult final {
    SecureHypercallBackendContractDecision Decision;
    bool ContractProofAccepted;
    bool BackendExecutionAuthorized;
    bool CompletionPublicationAuthorized;
    bool RetirePublicationAuthorized;
    std::wstring Reason;
    bool IsProofOnly;

    SecureHypercallBackendContractAdmissionResult(SecureHypercallBackendContractDecision decision,
        bool proofAccepted, bool executionAuthorized, bool completionAuthorized,
        bool retireAuthorized, std::wstring reason)
        : Decision(decision), ContractProofAccepted(proofAccepted),
          BackendExecutionAuthorized(executionAuthorized),
          CompletionPublicationAuthorized(completionAuthorized),
          RetirePublicationAuthorized(retireAuthorized), Reason(std::move(reason)),
          IsProofOnly(decision == SecureHypercallBackendContractDecision::AllowedProofOnlyNoExecution &&
              proofAccepted && !executionAuthorized && !completionAuthorized && !retireAuthorized) {}

    static std::shared_ptr<SecureHypercallBackendContractAdmissionResult> AllowedProofOnly() {
        return std::make_shared<SecureHypercallBackendContractAdmissionResult>(
            SecureHypercallBackendContractDecision::AllowedProofOnlyNoExecution, true, false, false, false,
            L"Secure hypercall owner/service contract proof accepted; execution and publication remain closed.");
    }
    static std::shared_ptr<SecureHypercallBackendContractAdmissionResult> Denied(
        SecureHypercallBackendContractDecision decision, const std::wstring& reason) {
        return std::make_shared<SecureHypercallBackendContractAdmissionResult>(decision, false, false, false, false, reason);
    }
};

	class SecureHypercallBackendContractAdmissionPolicy final : public std::enable_shared_from_this<SecureHypercallBackendContractAdmissionPolicy>
	{
	private:
		static inline const std::shared_ptr<SecureHypercallBackendContractAdmissionPolicy> Default = std::make_shared<SecureHypercallBackendContractAdmissionPolicy>();

	public:
		static std::shared_ptr<SecureHypercallBackendContractAdmissionPolicy> getDefault();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureHypercallBackendContractAdmissionResult Admit(SecureHypercallBackendContractDescriptor? contract, SecureHypercallBackendContractRequest request)
		std::shared_ptr<SecureHypercallBackendContractAdmissionResult> Admit(std::shared_ptr<SecureHypercallBackendContractDescriptor> contract, std::shared_ptr<SecureHypercallBackendContractRequest> request);

	private:
		static std::shared_ptr<SecureHypercallBackendContractAdmissionResult> AdmitArgument(std::shared_ptr<SecureHypercallBackendArgument> argument, std::shared_ptr<SecureHypercallBackendContractRequest> request);

		static std::shared_ptr<SecureHypercallBackendContractAdmissionResult> Deny(SecureHypercallBackendContractDecision decision, const std::wstring &reason);
	};

}
