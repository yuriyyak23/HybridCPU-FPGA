#include "SecureComputeControlledEmissionGatePolicy.h"
#include "YAKSys_Hybrid_CPU.Core.SecureComputeNoEmissionContract.h"
#include "SecureComputeNoEmissionContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureComputeControlledEmissionGatePolicy> SecureComputeControlledEmissionGatePolicy::getFailClosed()
	{
		return FailClosed;
	}

	std::shared_ptr<SecureComputeControlledEmissionResult> SecureComputeControlledEmissionGatePolicy::Classify(std::shared_ptr<SecureComputeControlledEmissionRequest> request)
	{
		std::shared_ptr<SecureComputeControlledEmissionResult> noEmissionViolation = DenyNoEmissionViolation(request);
		if (!noEmissionViolation->IsAllowed())
		{
			return noEmissionViolation;
		}

		if (!request->RequestsCompilerEmission)
		{
			return SecureComputeControlledEmissionResult::NoEmissionPreserved(request->Path, L"No compiler change is required; SecureCompute compiler no-emission remains closed.");
		}

		if (!request->HasPositiveNeutralRuntimeOwner)
		{
			return Deny(SecureComputeControlledEmissionDecision::DeniedMissingPositiveRuntimeOwner, request, L"Controlled emission requires a positive neutral runtime owner first.");
		}

		if (!request->HasControlledEmissionRfc)
		{
			return Deny(SecureComputeControlledEmissionDecision::DeniedMissingControlledEmissionRfc, request, L"Controlled emission requires a separate compiler RFC.");
		}

		if (!request->HasReleaseApproval)
		{
			return Deny(SecureComputeControlledEmissionDecision::DeniedMissingReleaseApproval, request, L"Controlled emission requires named-path release approval.");
		}

		if (!request->BackendExecutionAuthorized)
		{
			return Deny(SecureComputeControlledEmissionDecision::DeniedBackendExecutionClosed, request, L"Controlled emission cannot bypass backend execution closure.");
		}

		return Deny(SecureComputeControlledEmissionDecision::DeniedCompilerEmissionFutureGated, request, L"Phase 19 records the controlled-emission gate only; secure compiler emission remains future-gated.");
	}

	std::shared_ptr<SecureComputeControlledEmissionResult> SecureComputeControlledEmissionGatePolicy::DenyNoEmissionViolation(std::shared_ptr<SecureComputeControlledEmissionRequest> request)
	{
		std::shared_ptr<SecureComputeNoEmissionContract> noEmission = std::make_shared<SecureComputeNoEmissionContract>();
		SecureComputeNoEmissionViolation violation = noEmission->Validate(request->EmitsNewInstructionEncoding, request->EmitsNewOperandFormat, request->EmitsCapabilityAwareLoadStoreFetch, request->EmitsVmxSecureMode);

        switch (violation)
        {
        case SecureComputeNoEmissionViolation::NewInstructionEncoding:
            return Deny(SecureComputeControlledEmissionDecision::DeniedNewInstructionEncoding, request, L"SecureCompute compiler no-emission denies new instruction encodings.");
        case SecureComputeNoEmissionViolation::NewOperandFormat:
            return Deny(SecureComputeControlledEmissionDecision::DeniedNewOperandFormat, request, L"SecureCompute compiler no-emission denies new operand formats.");
        case SecureComputeNoEmissionViolation::CapabilityAwareLoadStoreFetch:
            return Deny(SecureComputeControlledEmissionDecision::DeniedCapabilityAwareLoadStoreFetch, request, L"SecureCompute compiler no-emission denies capability-aware memory instructions.");
        case SecureComputeNoEmissionViolation::VmxSecureModeEmission:
            return Deny(SecureComputeControlledEmissionDecision::DeniedVmxSecureModeEmission, request, L"SecureCompute compiler no-emission denies VMX secure-mode emission.");
        default:
            return SecureComputeControlledEmissionResult::NoEmissionPreserved(request->Path, L"");
        }
	}

	std::shared_ptr<SecureComputeControlledEmissionResult> SecureComputeControlledEmissionGatePolicy::Deny(SecureComputeControlledEmissionDecision decision, std::shared_ptr<SecureComputeControlledEmissionRequest> request, const std::wstring &reason)
	{
		return SecureComputeControlledEmissionResult::Denied(decision, request->Path, reason);
	}
}
