#pragma once

#include "../../Capabilities/CapabilityBoundaryRequirement.h"
#include "../../Evidence/EvidenceBoundaryRequirement.h"
#include "../../Services/RuntimeBoundaryAdmissionService.h"
#include "../Traps/NeutralTrapResult.h"
#include "NeutralHypercallBackendOwnerDescriptor.h"
#include <utility>
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class HypercallBackendAdmissionDecision : unsigned char
	{
		NotEvaluated = 0,
		DeniedRuntimeAdmission = 1,
		DeniedNoNeutralTrap = 2,
		MissingBackendDescriptor = 3,
		DeniedBackendAuthority = 4,
		DeniedDomainValidation = 5,
		DeniedCapability = 6,
		DeniedEvidence = 7,
		DeniedNeutralBackendOwnerMissing = 8,
		DeniedNeutralBackendOwnerAuthority = 9,
		DeniedNeutralBackendOwnerRfcAdr = 10,
		DeniedNeutralBackendOwnerShape = 11,
		DeniedNeutralBackendOwnerProof = 12
	};

	enum class HypercallBackendAuthority : unsigned char
	{
		Runtime = 0,
		DomainDescriptor = 1,
		CompatibilityProjection = 2
	};


class HypercallBackendDescriptor;
struct HypercallBackendAdmissionRequest {
    const std::shared_ptr<NeutralTrapResult> TrapResult; const std::shared_ptr<RuntimeBoundaryAdmissionResult> RuntimeAdmission; const std::shared_ptr<HypercallBackendDescriptor> BackendDescriptor; const std::shared_ptr<CapabilityDescriptorSet> Capabilities; const std::shared_ptr<EvidencePolicyDescriptor> EvidencePolicy; const bool DomainValidated;
    HypercallBackendAdmissionRequest(std::shared_ptr<NeutralTrapResult> trap, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<HypercallBackendDescriptor> descriptor, std::shared_ptr<CapabilityDescriptorSet> caps, std::shared_ptr<EvidencePolicyDescriptor> evidence, bool domain) : TrapResult(std::move(trap)), RuntimeAdmission(std::move(admission)), BackendDescriptor(std::move(descriptor)), Capabilities(std::move(caps)), EvidencePolicy(std::move(evidence)), DomainValidated(domain) {}
    static std::shared_ptr<HypercallBackendAdmissionRequest> MissingNeutralOwner(std::shared_ptr<NeutralTrapResult> trap, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<CapabilityDescriptorSet> caps, std::shared_ptr<EvidencePolicyDescriptor> evidence, bool domain) { return std::make_shared<HypercallBackendAdmissionRequest>(std::move(trap),std::move(admission),nullptr,std::move(caps),std::move(evidence),domain); }
};
struct HypercallBackendAdmissionResult {
    const HypercallBackendAdmissionDecision Decision; const bool BackendExecutionAuthorized; const std::shared_ptr<CapabilityBoundaryRequirement> CapabilityRequirement; const std::shared_ptr<EvidenceBoundaryRequirement> EvidenceRequirement; const std::wstring Reason; const bool IsAllowed, DeniesBackendExecution;
    HypercallBackendAdmissionResult(HypercallBackendAdmissionDecision decision, bool authorized, std::shared_ptr<CapabilityBoundaryRequirement> capability, std::shared_ptr<EvidenceBoundaryRequirement> evidence, std::wstring reason) : Decision(decision), BackendExecutionAuthorized(authorized), CapabilityRequirement(std::move(capability)), EvidenceRequirement(std::move(evidence)), Reason(std::move(reason)), IsAllowed(authorized), DeniesBackendExecution(!authorized) {}
    static std::shared_ptr<HypercallBackendAdmissionResult> NotEvaluated(std::wstring reason) { return Denied(HypercallBackendAdmissionDecision::NotEvaluated,CapabilityBoundaryRequirement::None,std::make_shared<EvidenceBoundaryRequirement>(EvidenceBoundaryRequirement::None),std::move(reason)); }
    static std::shared_ptr<HypercallBackendAdmissionResult> Denied(HypercallBackendAdmissionDecision decision, std::shared_ptr<CapabilityBoundaryRequirement> capability, std::shared_ptr<EvidenceBoundaryRequirement> evidence, std::wstring reason) { return std::make_shared<HypercallBackendAdmissionResult>(decision,false,std::move(capability),std::move(evidence),std::move(reason)); }
};
	class HypercallBackendAdmissionService final : public std::enable_shared_from_this<HypercallBackendAdmissionService>
	{
	private:
		static inline const std::shared_ptr<HypercallBackendAdmissionService> Default = std::make_shared<HypercallBackendAdmissionService>();

	public:
		static std::shared_ptr<HypercallBackendAdmissionService> getDefault();

		std::shared_ptr<HypercallBackendAdmissionResult> Admit(std::shared_ptr<HypercallBackendAdmissionRequest> request);

	private:
		static std::shared_ptr<HypercallBackendAdmissionResult> Deny(HypercallBackendAdmissionDecision decision, std::shared_ptr<HypercallBackendAdmissionRequest> request, const std::wstring &reason);
	};

}
