#include "YAKSys_Hybrid_CPU.Core.SecureIoHypercallAdmissionPolicy.h"
#include "../../../Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "../../Descriptors/Hypercalls/SecureHypercallDescriptor.h"
#include "../../Authority/YAKSys_Hybrid_CPU.Core.SecureAuthorityBounds.h"
#include "../../../../Capabilities/Grants/CapabilityGrant.h"
#include "../../Authority/SecureGrantAuthorityPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureIoHypercallAdmissionPolicy> SecureIoHypercallAdmissionPolicy::getDefault()
	{
		return Default;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureIoHypercallAdmissionResult AdmitIoDma(SecureIoDomainDescriptor? ioPolicy, System.Nullable<SecureMemoryDomainDescriptor> memory, SecureMemoryAccessRequest access, System.Nullable<SecureCompletionPublicationFence> publicationFence, bool requireRetirePublication)
	std::shared_ptr<SecureIoHypercallAdmissionResult> SecureIoHypercallAdmissionPolicy::AdmitIoDma(std::shared_ptr<SecureIoDomainDescriptor> ioPolicy, std::shared_ptr<SecureMemoryDomainDescriptor> memory, std::shared_ptr<SecureMemoryAccessRequest> access, std::shared_ptr<SecureCompletionPublicationFence> publicationFence, bool requireRetirePublication)
	{
		if (ioPolicy == nullptr || !ioPolicy->getNeutralIoOwnerMaterialized())
		{
			return Deny(SecureIoHypercallAdmissionDecision::DeniedMissingIoOwner, L"Secure I/O requires a neutral I/O owner descriptor.");
		}

		std::shared_ptr<SecureMemoryAdmissionResult> memoryResult = _memoryAdmission->Admit(memory, access, ioPolicy);
		if (!memoryResult->IsAllowed)
		{
			return Deny(MapMemoryDecision(memoryResult->Decision), memoryResult->Reason);
		}

		std::shared_ptr<SecureIoHypercallAdmissionResult> completion = AdmitCompletionAndRetire(publicationFence, ioPolicy->getRequireCompletionFence(), requireRetirePublication);
		if (!completion->IsAllowed)
		{
			return completion;
		}

		return SecureIoHypercallAdmissionResult::AllowedIo();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureIoHypercallAdmissionResult AdmitHypercall(SecureHypercallDescriptor? hypercallPolicy, ulong hypercallId, System.Nullable<CapabilityDescriptorSet> capabilities, System.Nullable<EvidencePolicyDescriptor> evidencePolicy, System.Nullable<SecureIoDomainDescriptor> ioPolicy, SecureRevocationEpoch policyEpoch, System.Nullable<SecureCompletionPublicationFence> publicationFence, bool neutralBackendOwnerMaterialized, bool domainValidated, ulong validatedDomainTag = 0)
	std::shared_ptr<SecureIoHypercallAdmissionResult> SecureIoHypercallAdmissionPolicy::AdmitHypercall(std::shared_ptr<SecureHypercallDescriptor> hypercallPolicy, unsigned long long hypercallId, std::shared_ptr<CapabilityDescriptorSet> capabilities, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, std::shared_ptr<SecureIoDomainDescriptor> ioPolicy, std::shared_ptr<SecureRevocationEpoch> policyEpoch, std::shared_ptr<SecureCompletionPublicationFence> publicationFence, bool neutralBackendOwnerMaterialized, bool domainValidated, unsigned long long validatedDomainTag)
	{
		if (policyEpoch == nullptr)
		{
			return Deny(SecureIoHypercallAdmissionDecision::DeniedMissingTypedGrant, L"Secure hypercall admission requires a materialized policy epoch.");
		}

		if (hypercallPolicy == nullptr || !hypercallPolicy->getHasPolicy())
		{
			return Deny(SecureIoHypercallAdmissionDecision::DeniedMissingHypercallPolicy, L"Secure hypercall requires an explicit secure hypercall policy.");
		}

		if (!hypercallPolicy->AllowsHypercallId(hypercallId))
		{
			return Deny(SecureIoHypercallAdmissionDecision::DeniedHypercallId, L"Secure hypercall id is not allowed by policy.");
		}

		for (auto argument : hypercallPolicy->getArguments())
		{
			std::shared_ptr<SecureIoHypercallAdmissionResult> argumentResult = AdmitArgument(argument, ioPolicy, policyEpoch, validatedDomainTag);
			if (!argumentResult->IsAllowed)
			{
				return argumentResult;
			}
		}

		if (hypercallPolicy->getNeutralBackendOwnerRequired() && !neutralBackendOwnerMaterialized)
		{
			return Deny(SecureIoHypercallAdmissionDecision::DeniedMissingNeutralBackendOwner, L"Secure hypercall backend execution requires a neutral runtime backend owner.");
		}

		if (!domainValidated)
		{
			return Deny(SecureIoHypercallAdmissionDecision::DeniedDomainValidation, L"Secure hypercall requires a validated secure runtime domain.");
		}

		std::shared_ptr<SecureIoHypercallAdmissionResult> capability = AdmitTypedGrant(hypercallPolicy->getRequiredGrant(), capabilities, policyEpoch);
		if (!capability->IsAllowed)
		{
			return capability;
		}

		if (hypercallPolicy->getRequireEvidenceApproval() && (evidencePolicy == nullptr || !evidencePolicy->CanExposeToGuest(EvidenceVisibilityClass::GuestArchitecturalState)))
		{
			return Deny(SecureIoHypercallAdmissionDecision::DeniedEvidence, L"Secure hypercall requires neutral evidence policy approval.");
		}

		std::shared_ptr<SecureIoHypercallAdmissionResult> publication = AdmitCompletionAndRetire(publicationFence, hypercallPolicy->getRequireCompletionFence(), hypercallPolicy->getRequireRetirePublicationRule());
		if (!publication->IsAllowed)
		{
			return publication;
		}

		if (!hypercallPolicy->getAllowBackendExecution())
		{
			return SecureIoHypercallAdmissionResult::AllowedAdmittedDenied();
		}

		return Deny(SecureIoHypercallAdmissionDecision::DeniedBackendSuccessClosed, L"Secure hypercall backend success remains closed in this phase.");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private SecureIoHypercallAdmissionResult AdmitArgument(SecureHypercallArgumentDescriptor argument, SecureIoDomainDescriptor? ioPolicy, SecureRevocationEpoch policyEpoch, ulong validatedDomainTag)
	std::shared_ptr<SecureIoHypercallAdmissionResult> SecureIoHypercallAdmissionPolicy::AdmitArgument(std::shared_ptr<SecureHypercallArgumentDescriptor> argument, std::shared_ptr<SecureIoDomainDescriptor> ioPolicy, std::shared_ptr<SecureRevocationEpoch> policyEpoch, unsigned long long validatedDomainTag)
	{
		if (argument->isDeniedRawPrivatePointer())
		{
			return Deny(SecureIoHypercallAdmissionDecision::DeniedRawPrivatePointer, L"Raw private guest pointer arguments are denied for secure hypercalls.");
		}

		if (argument->ArgumentClass == SecureHypercallArgumentClass::OpaqueHandle)
		{
			return argument->Grant.MatchesEpoch(*policyEpoch) ? SecureIoHypercallAdmissionResult::AllowedIo() : Deny(SecureIoHypercallAdmissionDecision::DeniedForgedOpaqueHandle, L"Secure hypercall opaque handles require provenance and current epoch.");
		}

		if (!argument->requiresSharedBuffer())
		{
			return SecureIoHypercallAdmissionResult::AllowedIo();
		}

		if (ioPolicy == nullptr || !ioPolicy->getNeutralIoOwnerMaterialized())
		{
			return Deny(SecureIoHypercallAdmissionDecision::DeniedMissingIoOwner, L"Secure hypercall shared-buffer arguments require a neutral I/O owner.");
		}

		std::shared_ptr<SecureSharedBufferDescriptor> buffer;
		if (!ioPolicy->TryFindCurrentSharedBuffer(argument->SharedBufferId, validatedDomainTag, policyEpoch, buffer))
		{
			return Deny(SecureIoHypercallAdmissionDecision::DeniedSharedBufferBinding, L"Secure hypercall shared-buffer argument requires an explicit descriptor with current owner, lifetime, evidence and buffer-grant epoch.");
		}

		return argument->Grant.MatchesEpoch(*policyEpoch) ? SecureIoHypercallAdmissionResult::AllowedIo() : Deny(SecureIoHypercallAdmissionDecision::DeniedMissingTypedGrant, L"Secure hypercall shared-buffer argument requires a current typed grant.");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private SecureIoHypercallAdmissionResult AdmitTypedGrant(SecureGrantHandle requiredGrant, CapabilityDescriptorSet? capabilities, SecureRevocationEpoch policyEpoch)
	std::shared_ptr<SecureIoHypercallAdmissionResult> SecureIoHypercallAdmissionPolicy::AdmitTypedGrant(std::shared_ptr<SecureGrantHandle> requiredGrant, std::shared_ptr<CapabilityDescriptorSet> capabilities, std::shared_ptr<SecureRevocationEpoch> policyEpoch)
	{
		std::shared_ptr<SecureAuthorityBounds> hypercallBounds = std::make_shared<SecureAuthorityBounds>(false, false, false, false, true, false, false, false);
		std::shared_ptr<SecureGrantAuthorityResult> authority = _grantAuthority->Validate(requiredGrant, SecureGrantMaterializationSource::NeutralRuntimeOwner, hypercallBounds, hypercallBounds, SecureGrantEpochSet::Single(policyEpoch), true, ((capabilities != nullptr) ? capabilities: CapabilityDescriptorSet::getEmpty()), CapabilityGrantScope::DomainGranted, false);

		return authority->IsAllowed ? SecureIoHypercallAdmissionResult::AllowedIo() : Deny(SecureIoHypercallAdmissionDecision::DeniedMissingTypedGrant, authority->Reason);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static SecureIoHypercallAdmissionResult AdmitCompletionAndRetire(SecureCompletionPublicationFence? publicationFence, bool requireCompletionPublication, bool requireRetirePublication)
	std::shared_ptr<SecureIoHypercallAdmissionResult> SecureIoHypercallAdmissionPolicy::AdmitCompletionAndRetire(std::shared_ptr<SecureCompletionPublicationFence> publicationFence, bool requireCompletionPublication, bool requireRetirePublication)
	{
		if (requireCompletionPublication && (publicationFence == nullptr || !publicationFence->getCanPublishCompletion()))
		{
			return Deny(SecureIoHypercallAdmissionDecision::DeniedCompletionFence, L"Secure I/O and hypercall completion publication requires a completion fence.");
		}

		if (requireRetirePublication && (publicationFence == nullptr || !publicationFence->getCanPublishRetire()))
		{
			return Deny(SecureIoHypercallAdmissionDecision::DeniedRetireFence, L"Secure I/O and hypercall retire publication requires an explicit retire fence.");
		}

		return SecureIoHypercallAdmissionResult::AllowedIo();
	}

	SecureIoHypercallAdmissionDecision SecureIoHypercallAdmissionPolicy::MapMemoryDecision(SecureMemoryAdmissionDecision decision)
	{
		switch (decision)
		{
		case SecureMemoryAdmissionDecision::DeniedPrivateDma:
			return SecureIoHypercallAdmissionDecision::DeniedPrivateMemoryAccess;
		case SecureMemoryAdmissionDecision::DeniedDmaRequiresTypedGrant:
			return SecureIoHypercallAdmissionDecision::DeniedMissingTypedGrant;
		default:
			return SecureIoHypercallAdmissionDecision::DeniedSharedBufferBinding;
		}
	}

	std::shared_ptr<SecureIoHypercallAdmissionResult> SecureIoHypercallAdmissionPolicy::Deny(SecureIoHypercallAdmissionDecision decision, const std::wstring &reason)
	{
		return SecureIoHypercallAdmissionResult::Denied(decision, reason);
	}
}
