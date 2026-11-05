#include "HypercallBackendAdmissionPolicy.h"
#include "YAKSys_Hybrid_CPU.Core.HypercallBackendDescriptor.h"
#include "../../Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<HypercallBackendAdmissionService> HypercallBackendAdmissionService::getDefault()
	{
		return Default;
	}

	std::shared_ptr<HypercallBackendAdmissionResult> HypercallBackendAdmissionService::Admit(std::shared_ptr<HypercallBackendAdmissionRequest> request)
	{
		if (!request->RuntimeAdmission->IsAllowed)
		{
			return Deny(HypercallBackendAdmissionDecision::DeniedRuntimeAdmission, request, L"Hypercall backend admission requires runtime boundary admission.");
		}

		if (!request->TrapResult->ShouldTrap)
		{
			return Deny(HypercallBackendAdmissionDecision::DeniedNoNeutralTrap, request, L"Hypercall backend admission requires a neutral trap result.");
		}

		if (request->BackendDescriptor == nullptr)
		{
			return Deny(HypercallBackendAdmissionDecision::MissingBackendDescriptor, request, L"Hypercall backend execution remains denied: no neutral runtime backend descriptor is materialized.");
		}

		std::shared_ptr<HypercallBackendDescriptor> descriptor = request->BackendDescriptor;
		if (!descriptor->isRuntimeAuthoritative())
		{
			return Deny(HypercallBackendAdmissionDecision::DeniedBackendAuthority, request, L"Compatibility projection cannot own hypercall backend execution.");
		}

		if (descriptor->getRequiresValidatedDomain() && !request->DomainValidated)
		{
			return Deny(HypercallBackendAdmissionDecision::DeniedDomainValidation, request, L"Hypercall backend admission requires a validated runtime domain.");
		}

		if (!descriptor->getCapabilityRequirement()->IsSatisfiedBy((request->Capabilities != nullptr) ? request->Capabilities : CapabilityDescriptorSet::getEmpty()))
		{
			return Deny(HypercallBackendAdmissionDecision::DeniedCapability, request, L"Hypercall backend admission requires a typed runtime capability grant.");
		}

		if (!descriptor->getEvidenceRequirement()->IsSatisfiedBy(request->EvidencePolicy))
		{
			return Deny(HypercallBackendAdmissionDecision::DeniedEvidence, request, L"Hypercall backend admission requires neutral evidence policy approval.");
		}

		if (!descriptor->getNeutralBackendOwnerMaterialized())
		{
			return Deny(HypercallBackendAdmissionDecision::DeniedNeutralBackendOwnerMissing, request, L"Hypercall backend execution remains denied: neutral backend owner semantics are not materialized.");
		}

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		auto owner = descriptor->getNeutralBackendOwner();
		if (!owner || !owner->isMaterialized())
		{
			return Deny(HypercallBackendAdmissionDecision::DeniedNeutralBackendOwnerMissing, request, L"Hypercall backend execution remains denied: no neutral backend owner descriptor is materialized.");
		}

		if (!owner->isNeutralRuntimeOwner())
		{
			return Deny(HypercallBackendAdmissionDecision::DeniedNeutralBackendOwnerAuthority, request, L"Hypercall backend execution remains denied: compatibility projection cannot source neutral backend owner authority.");
		}

		if (!owner->isCandidateDraft())
		{
			return Deny(HypercallBackendAdmissionDecision::DeniedNeutralBackendOwnerRfcAdr, request, L"Hypercall backend execution remains denied: owner-specific RFC/ADR packet is missing or not the VMCALL no-state draft.");
		}

		if (!owner->hasNoStateCandidateShape())
		{
			return Deny(HypercallBackendAdmissionDecision::DeniedNeutralBackendOwnerShape, request, L"Hypercall backend execution remains denied: owner descriptor is not the no-state, no-payload, domain-local draft shape.");
		}

		if (!owner->NegativeTestsPresent)
		{
			return Deny(HypercallBackendAdmissionDecision::DeniedNeutralBackendOwnerProof, request, L"Hypercall backend execution remains denied: owner-specific negative conformance proof is missing.");
		}

		return Deny(HypercallBackendAdmissionDecision::DeniedNeutralBackendOwnerRfcAdr, request, L"Hypercall backend execution remains denied: RFC-HV-VMCALL-NO-STATE-OWNER-0001 is draft only and has no accepted owner semantics.");
	}

	std::shared_ptr<HypercallBackendAdmissionResult> HypercallBackendAdmissionService::Deny(HypercallBackendAdmissionDecision decision, std::shared_ptr<HypercallBackendAdmissionRequest> request, const std::wstring &reason)
	{
		return HypercallBackendAdmissionResult::Denied(decision, request->BackendDescriptor ? request->BackendDescriptor->getCapabilityRequirement() : CapabilityBoundaryRequirement::None, request->BackendDescriptor ? request->BackendDescriptor->getEvidenceRequirement() : std::make_shared<EvidenceBoundaryRequirement>(EvidenceBoundaryRequirement::None), reason);
	}
}
