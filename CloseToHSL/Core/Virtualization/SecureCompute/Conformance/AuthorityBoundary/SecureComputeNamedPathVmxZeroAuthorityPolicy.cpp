#include "SecureComputeNamedPathVmxZeroAuthorityPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityPolicy> SecureComputeNamedPathVmxZeroAuthorityPolicy::getFailClosed()
	{
		return FailClosed;
	}

	std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityResult> SecureComputeNamedPathVmxZeroAuthorityPolicy::Classify(std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityRequest> request)
	{
		std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityResult> denied = DenyAuthorityShortcut(request);
		if (!denied->IsAllowed())
		{
			return denied;
		}

		if (request->RequestsCompatibilityProjection && !request->HasNeutralRuntimeResult)
		{
			return SecureComputeNamedPathVmxZeroAuthorityResult::Denied(SecureComputeNamedPathVmxZeroAuthorityDecision::DeniedProjectionWithoutNeutralResult, request->Path, L"Compatibility projection requires a neutral runtime result and remains zero-authority.");
		}

		return SecureComputeNamedPathVmxZeroAuthorityResult::Classified(request->Path, request->RequestsCompatibilityProjection, L"Named SecureCompute path remains VMX zero-authority.");
	}

	std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityResult> SecureComputeNamedPathVmxZeroAuthorityPolicy::DenyAuthorityShortcut(std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityRequest> request)
	{
		if (request->AttemptsVmxActivation)
		{
			return Deny(SecureComputeNamedPathVmxZeroAuthorityDecision::DeniedVmxActivation, request, L"VMX cannot activate SecureCompute.");
		}

		if (request->AttemptsVmxCapsGrant)
		{
			return Deny(SecureComputeNamedPathVmxZeroAuthorityDecision::DeniedVmxCapsAuthority, request, L"Capability projection cannot grant SecureCompute authority.");
		}

		if (request->AttemptsVmcsStateStore)
		{
			return Deny(SecureComputeNamedPathVmxZeroAuthorityDecision::DeniedVmcsStateStore, request, L"Compatibility state cannot store SecureCompute state.");
		}

		if (request->AttemptsActiveVmcsPointerIdentity)
		{
			return Deny(SecureComputeNamedPathVmxZeroAuthorityDecision::DeniedActiveVmcsPointerIdentity, request, L"Active compatibility pointer identity cannot be SecureCompute domain identity.");
		}

		if (request->AttemptsVmreadSecureStateAuthority)
		{
			return Deny(SecureComputeNamedPathVmxZeroAuthorityDecision::DeniedVmreadSecureStateAuthority, request, L"Read projection cannot read SecureCompute state as authority.");
		}

		if (request->AttemptsVmwriteSecureStateMutation)
		{
			return Deny(SecureComputeNamedPathVmxZeroAuthorityDecision::DeniedVmwriteSecureStateMutation, request, L"Write projection cannot mutate SecureCompute state.");
		}

		if (request->AttemptsVmcsCheckpointAuthority)
		{
			return Deny(SecureComputeNamedPathVmxZeroAuthorityDecision::DeniedVmcsCheckpointAuthority, request, L"Compatibility metadata cannot be SecureCompute checkpoint authority.");
		}

		if (request->AttemptsCompletionPublication)
		{
			return Deny(SecureComputeNamedPathVmxZeroAuthorityDecision::DeniedCompletionPublication, request, L"Compatibility projection cannot publish SecureCompute completion.");
		}

		if (request->AttemptsRetirePublication)
		{
			return Deny(SecureComputeNamedPathVmxZeroAuthorityDecision::DeniedRetirePublication, request, L"Compatibility projection cannot publish SecureCompute retire effects.");
		}

		return SecureComputeNamedPathVmxZeroAuthorityResult::Classified(request->Path, false, L"");
	}

	std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityResult> SecureComputeNamedPathVmxZeroAuthorityPolicy::Deny(SecureComputeNamedPathVmxZeroAuthorityDecision decision, std::shared_ptr<SecureComputeNamedPathVmxZeroAuthorityRequest> request, const std::wstring &reason)
	{
		return SecureComputeNamedPathVmxZeroAuthorityResult::Denied(decision, request->Path, reason);
	}
}



