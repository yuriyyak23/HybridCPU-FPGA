#pragma once

#include <memory>
#include <string>
#include <utility>
#include "../../Authority/SecureRevocationEpoch.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureIoHypercallAdmissionDecision : unsigned char
	{
		AllowedIo = 0,
		AllowedAdmittedDenied = 1,
		DeniedMissingIoOwner = 2,
		DeniedPrivateMemoryAccess = 3,
		DeniedSharedBufferBinding = 4,
		DeniedMissingTypedGrant = 5,
		DeniedMissingHypercallPolicy = 6,
		DeniedHypercallId = 7,
		DeniedRawPrivatePointer = 8,
		DeniedForgedOpaqueHandle = 9,
		DeniedMissingNeutralBackendOwner = 10,
		DeniedDomainValidation = 11,
		DeniedEvidence = 12,
		DeniedCompletionFence = 13,
		DeniedRetireFence = 14,
		DeniedBackendSuccessClosed = 15
	};

struct SecureIoHypercallAdmissionResult final {
    SecureIoHypercallAdmissionDecision Decision;
    bool BackendExecutionAuthorized;
    bool CompletionPublicationAuthorized;
    bool RetirePublicationAuthorized;
    std::wstring Reason;
    bool IsAllowed;
    bool IsAdmittedDenied;
    bool IsPolicyAdmissionOnly;

    SecureIoHypercallAdmissionResult(SecureIoHypercallAdmissionDecision decision,
        bool executionAuthorized, bool completionAuthorized, bool retireAuthorized, std::wstring reason)
        : Decision(decision), BackendExecutionAuthorized(executionAuthorized),
          CompletionPublicationAuthorized(completionAuthorized), RetirePublicationAuthorized(retireAuthorized),
          Reason(std::move(reason)),
          IsAllowed(decision == SecureIoHypercallAdmissionDecision::AllowedIo || decision == SecureIoHypercallAdmissionDecision::AllowedAdmittedDenied),
          IsAdmittedDenied(decision == SecureIoHypercallAdmissionDecision::AllowedAdmittedDenied && !executionAuthorized),
          IsPolicyAdmissionOnly(decision == SecureIoHypercallAdmissionDecision::AllowedIo && !executionAuthorized && !completionAuthorized && !retireAuthorized) {}

    static std::shared_ptr<SecureIoHypercallAdmissionResult> AllowedIo() {
        return std::make_shared<SecureIoHypercallAdmissionResult>(SecureIoHypercallAdmissionDecision::AllowedIo, false, false, false,
            L"Secure I/O policy admission succeeded; backend execution and publication remain closed.");
    }
    static std::shared_ptr<SecureIoHypercallAdmissionResult> AllowedAdmittedDenied() {
        return std::make_shared<SecureIoHypercallAdmissionResult>(SecureIoHypercallAdmissionDecision::AllowedAdmittedDenied, false, false, false,
            L"Secure hypercall admission is recognized but backend execution remains closed.");
    }
    static std::shared_ptr<SecureIoHypercallAdmissionResult> Denied(SecureIoHypercallAdmissionDecision decision, const std::wstring& reason) {
        return std::make_shared<SecureIoHypercallAdmissionResult>(decision, false, false, false, reason);
    }
};

}
