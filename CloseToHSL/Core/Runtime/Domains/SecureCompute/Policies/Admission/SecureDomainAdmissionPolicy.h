#pragma once
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureDomainOperationClass : unsigned char
	{
		Ordinary = 0,
		EnterSecureDomain = 1,
		TouchSecureMemory = 2,
		CreateEvidence = 3,
		PublishCompletion = 4,
		PublishRetireSideEffect = 5,
		SecureIo = 6,
		SecureHypercall = 7,
		SecureMigration = 8,
		NestedSecureDomain = 9,
		CompatibilityProjection = 10
	};

	enum class SecureDomainAdmissionDecision : unsigned char
	{
		AllowedNoEffect = 0,
		AllowedSecureOperation = 1,
		DeniedMissingDescriptor = 2,
		DeniedDisabledDescriptor = 3,
		DeniedUnmaterializedDescriptor = 4,
		DeniedMissingMeasurement = 5,
		DeniedMissingMemoryPolicy = 6,
		DeniedMissingEvidencePolicy = 7,
		DeniedMissingMigrationPolicy = 8,
		DeniedMissingIoPolicy = 9,
		DeniedMissingHypercallPolicy = 10,
		DeniedOrdinaryOverDenyGuard = 11,
		DeniedMemoryDomainBindingMismatch = 12,
		DeniedUnsupportedOperationClass = 13
	};

struct SecureDomainAdmissionResult {
    const SecureDomainAdmissionDecision Decision; const std::wstring Reason; const bool IsAllowed;
    SecureDomainAdmissionResult(SecureDomainAdmissionDecision d, std::wstring r) : Decision(d), Reason(std::move(r)), IsAllowed(d == SecureDomainAdmissionDecision::AllowedNoEffect || d == SecureDomainAdmissionDecision::AllowedSecureOperation) {}
    static std::shared_ptr<SecureDomainAdmissionResult> Denied(SecureDomainAdmissionDecision d, std::wstring r) { return std::make_shared<SecureDomainAdmissionResult>(d, std::move(r)); }
    static const std::shared_ptr<SecureDomainAdmissionResult> AllowedNoEffect, AllowedSecureOperation;
};
inline const std::shared_ptr<SecureDomainAdmissionResult> SecureDomainAdmissionResult::AllowedNoEffect = std::make_shared<SecureDomainAdmissionResult>(SecureDomainAdmissionDecision::AllowedNoEffect, L"");
inline const std::shared_ptr<SecureDomainAdmissionResult> SecureDomainAdmissionResult::AllowedSecureOperation = std::make_shared<SecureDomainAdmissionResult>(SecureDomainAdmissionDecision::AllowedSecureOperation, L"");
}