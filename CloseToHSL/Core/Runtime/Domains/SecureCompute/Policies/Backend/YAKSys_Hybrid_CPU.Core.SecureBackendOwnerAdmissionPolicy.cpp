#include "YAKSys_Hybrid_CPU.Core.SecureBackendOwnerAdmissionPolicy.h"
#include "../../Descriptors/Backend/SecureBackendOwnerDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureBackendOwnerAdmissionPolicy> SecureBackendOwnerAdmissionPolicy::getDefault()
	{
		return Default;
	}

	std::shared_ptr<SecureBackendOwnerAdmissionResult> SecureBackendOwnerAdmissionPolicy::Admit(std::shared_ptr<SecureBackendOwnerAdmissionRequest> request)
	{
		std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> owner = request->Owner;
		if (!owner || !owner.value() || !owner.value()->Materialized || !owner.value()->HasIdentity)
		{
			return Deny(SecureBackendOwnerAdmissionDecision::DeniedMissingNeutralOwner, L"Secure backend owner proof requires a materialized neutral runtime owner descriptor.");
		}

		if (!owner.value()->IsNeutralSource)
		{
			return Deny(SecureBackendOwnerAdmissionDecision::DeniedNonNeutralAuthoritySource, L"Secure backend owner proof cannot be sourced from compatibility projection, VMX, VMCS or VmxCaps vocabulary.");
		}

		if (request->RfcAdrState != SecureBackendRfcAdrState::Approved)
		{
			return Deny(SecureBackendOwnerAdmissionDecision::DeniedMissingRfcAdrApproval, L"Secure backend owner proof requires a separate approved RFC/ADR gate.");
		}

		if (!owner.value()->HasProofChain)
		{
			return Deny(SecureBackendOwnerAdmissionDecision::DeniedMissingProofChain, L"Secure backend owner proof requires grant, evidence, completion and retire proof chain validation.");
		}

		if (!owner.value()->Epoch->Equals(request->CurrentEpoch))
		{
			return Deny(SecureBackendOwnerAdmissionDecision::DeniedStaleEpoch, L"Secure backend owner proof epoch must match the current policy epoch.");
		}

		if (!owner.value()->NegativeTestsPresent)
		{
			return Deny(SecureBackendOwnerAdmissionDecision::DeniedMissingNegativeTests, L"Secure backend owner proof requires matching negative conformance tests before any runtime-execution claim.");
		}

		if (request->RequestsBackendExecution)
		{
			return Deny(SecureBackendOwnerAdmissionDecision::DeniedBackendExecutionClosed, L"Secure backend execution remains closed; this gate accepts owner proof only.");
		}

		return SecureBackendOwnerAdmissionResult::AllowedProofOnly();
	}

	std::shared_ptr<SecureBackendOwnerAdmissionResult> SecureBackendOwnerAdmissionPolicy::Deny(SecureBackendOwnerAdmissionDecision decision, const std::wstring &reason)
	{
		return SecureBackendOwnerAdmissionResult::Denied(decision, reason);
	}
}
