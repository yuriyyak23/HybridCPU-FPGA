#include "SecureHypercallBackendContractAdmissionPolicy.h"
#include "../../Descriptors/Hypercalls/SecureHypercallBackendOwnerAbiRegistry.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureHypercallBackendContractAdmissionPolicy> SecureHypercallBackendContractAdmissionPolicy::getDefault()
	{
		return Default;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureHypercallBackendContractAdmissionResult Admit(SecureHypercallBackendContractDescriptor? contract, SecureHypercallBackendContractRequest request)
	std::shared_ptr<SecureHypercallBackendContractAdmissionResult> SecureHypercallBackendContractAdmissionPolicy::Admit(std::shared_ptr<SecureHypercallBackendContractDescriptor> contract, std::shared_ptr<SecureHypercallBackendContractRequest> request)
	{
		if (contract == nullptr || !contract->isMaterialized())
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedUnresolvedContract, L"Secure hypercall contract requires an exact decoded leaf, service ID, owner ID, owner epoch and version.");
		}

		if ((request->TransportOpcode == nullptr || request->TransportOpcode->Value != SecureHypercallBackendOwnerAbiRegistry::getTransportOpcode()->Value))
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedTransportOpcodeMismatch, L"Secure hypercall backend owner contract requires the reviewed transport opcode.");
		}

		if ((request->ServiceId == nullptr || request->ServiceId->Value != contract->getServiceId()->Value))
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedUnknownServiceId, L"Secure hypercall service ID is not present in the accepted owner contract.");
		}

		if ((request->DecodedLeaf == nullptr || request->DecodedLeaf->Value != contract->getDecodedLeaf()->Value))
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedDecodedLeafMismatch, L"Decoded transport leaf does not match the separately typed contract leaf.");
		}

		if ((request->ContractVersion == nullptr || request->ContractVersion->Major != contract->getVersion()->Major || request->ContractVersion->Minor != contract->getVersion()->Minor))
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedUnsupportedContractVersion, L"Secure hypercall contract version is unsupported.");
		}

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (!request->Owner || !request->Owner.value() || !request->Owner.value()->Materialized || !request->Owner.value()->HasIdentity || !request->Owner.value()->IsNeutralSource)
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedMissingOwner, L"Secure hypercall contract requires a materialized neutral backend owner descriptor.");
		}

		if (request->Owner.value()->OwnerId != contract->getOwnerId()->Value)
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedWrongOwner, L"Secure hypercall backend owner does not match the accepted service contract.");
		}

		if (!request->Owner.value()->Epoch || !request->CurrentEpoch || !request->Owner.value()->Epoch->Equals(contract->getOwnerEpoch()) || !request->Owner.value()->Epoch->Equals(request->CurrentEpoch))
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedOwnerEpochMismatch, L"Secure hypercall backend owner epoch is stale or does not match the contract epoch.");
		}

		if ((!contract->getRequiredGrant() || request->PresentedGrant != *contract->getRequiredGrant()) || (!request->CurrentEpoch || !request->PresentedGrant.MatchesEpoch(*request->CurrentEpoch)))
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedMissingGrant, L"Secure hypercall contract requires the exact current typed grant.");
		}

		if (!request->EvidenceValidated)
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedMissingEvidence, L"Secure hypercall contract requires validated neutral evidence.");
		}

		if (!request->EvidenceEpoch || !request->CurrentEpoch || !request->EvidenceEpoch->Equals(request->CurrentEpoch))
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedStaleEvidence, L"Secure hypercall evidence epoch is stale.");
		}

		for (std::shared_ptr<SecureHypercallBackendArgument> argument : request->Arguments)
		{
			std::shared_ptr<SecureHypercallBackendContractAdmissionResult> argumentResult = AdmitArgument(argument, request);
			if (!argumentResult->IsProofOnly)
			{
				return argumentResult;
			}
		}

		if (request->IsReplay && (contract->getReplayPolicy() == SecureHypercallReplayPolicy::DenyReplay || !request->IdempotentRetry || !request->ReplayTokenMatches))
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedReplayViolation, L"Secure hypercall replay is denied unless the contract permits an idempotent retry with a matching replay token.");
		}

		if (request->CancellationRequested)
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedCancelledBeforeExecution, L"Secure hypercall cancellation is accepted only as a pre-execution denial.");
		}

		return SecureHypercallBackendContractAdmissionResult::AllowedProofOnly();
	}

	std::shared_ptr<SecureHypercallBackendContractAdmissionResult> SecureHypercallBackendContractAdmissionPolicy::AdmitArgument(std::shared_ptr<SecureHypercallBackendArgument> argument, std::shared_ptr<SecureHypercallBackendContractRequest> request)
	{
		if (argument->IsRawPointer)
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedRawPointerRepresentation, L"Raw host or guest pointer representation is forbidden by the secure hypercall contract.");
		}

		if (argument->IsOpaqueHandle)
		{
			return argument->Value != 0 && request->CurrentEpoch && argument->Grant.MatchesEpoch(*request->CurrentEpoch) ? SecureHypercallBackendContractAdmissionResult::AllowedProofOnly() : Deny(SecureHypercallBackendContractDecision::DeniedInvalidOpaqueHandle, L"Opaque handles require a nonzero value plus current runtime provenance.");
		}

		if (!argument->IsSharedBuffer)
		{
			return SecureHypercallBackendContractAdmissionResult::AllowedProofOnly();
		}

		std::shared_ptr<SecureSharedBufferDescriptor> buffer;
		if (!request->IoPolicy || !request->IoPolicy->TryFindCurrentSharedBuffer(argument->Value, request->ValidatedDomainTag, request->CurrentEpoch, buffer) || argument->Length == 0 || !buffer || argument->Length > buffer->Length || !request->CurrentEpoch || !argument->Grant.MatchesEpoch(*request->CurrentEpoch))
		{
			return Deny(SecureHypercallBackendContractDecision::DeniedInvalidSharedBuffer, L"Shared-buffer arguments require current owner, lifetime, evidence, bounds and typed-grant validation.");
		}

		return SecureHypercallBackendContractAdmissionResult::AllowedProofOnly();
	}

	std::shared_ptr<SecureHypercallBackendContractAdmissionResult> SecureHypercallBackendContractAdmissionPolicy::Deny(SecureHypercallBackendContractDecision decision, const std::wstring &reason)
	{
		return SecureHypercallBackendContractAdmissionResult::Denied(decision, reason);
	}
}
