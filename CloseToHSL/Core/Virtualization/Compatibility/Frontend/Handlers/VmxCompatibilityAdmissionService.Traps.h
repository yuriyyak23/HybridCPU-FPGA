#pragma once

#include "VmxCompatibilityAdmissionService.h"
#include "../../../../Runtime/Events/Hypercalls/HypercallBackendAdmissionPolicy.h"
#include "../../../../Runtime/Completion/Routing/TrapCompletionRoutePolicy.h"
#include "../Projection/Events/TrapDecision.h"

// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmxCompatibilityTrapAdmissionDecision : unsigned char
	{
		TrapProjectionDeniedBackend = 0,
		DecodeDenied = 1,
		ProjectionDenied = 2,
		RuntimeAdmissionDenied = 3,
		TrapPolicyDenied = 4
	};

class TrapPolicyDescriptor; class TrapPolicyBitmap;
struct VmxCompatibilityVmCallTrapAdmissionRequest {
    const std::shared_ptr<DomainRuntimeContext> Context; const std::shared_ptr<RootAuthorityDescriptor> RootAuthority; const std::shared_ptr<EvidencePolicyDescriptor> EvidencePolicy; const std::shared_ptr<TrapPolicyDescriptor> TrapPolicy; const std::shared_ptr<TrapPolicyBitmap> TrapBitmap;
    const unsigned char VtId, HypercallLeafRegister, DescriptorRegister; const unsigned short ExecutionDomainTag, AddressSpaceTag; const bool DescriptorValidated, CapabilityValidated, SchedulingValidated, NoEmissionValidated, ProjectionEvidenceValidated, DomainValidated;
    VmxCompatibilityVmCallTrapAdmissionRequest(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<EvidencePolicyDescriptor> evidence, std::shared_ptr<TrapPolicyDescriptor> policy, std::shared_ptr<TrapPolicyBitmap> bitmap, unsigned char vt, unsigned char leaf, unsigned char descriptorRegister, unsigned short domain, unsigned short space, bool descriptorOk, bool capability, bool scheduling, bool noEmission, bool projectionEvidence, bool domainValidated)
      : Context(std::move(context)), RootAuthority(std::move(root)), EvidencePolicy(std::move(evidence)), TrapPolicy(std::move(policy)), TrapBitmap(std::move(bitmap)), VtId(vt), HypercallLeafRegister(leaf), DescriptorRegister(descriptorRegister), ExecutionDomainTag(domain), AddressSpaceTag(space), DescriptorValidated(descriptorOk), CapabilityValidated(capability), SchedulingValidated(scheduling), NoEmissionValidated(noEmission), ProjectionEvidenceValidated(projectionEvidence), DomainValidated(domainValidated) {}
};
struct VmxCompatibilityTrapAdmissionResult {
    const VmxCompatibilityTrapAdmissionDecision Decision; const std::shared_ptr<VmxCompatDecodeResult> Decode; const std::shared_ptr<VmxCompatProjectionResult> Projection; const std::shared_ptr<RuntimeBoundaryAdmissionResult> RuntimeAdmission; const std::shared_ptr<NeutralTrapResult> NeutralResult; const std::shared_ptr<HypercallBackendAdmissionResult> BackendAdmission; const std::shared_ptr<TrapDecision> ProjectedDecision; const std::shared_ptr<TrapCompletionRouteResult> CompletionRoute; const std::shared_ptr<TrapCompletionPublicationFenceResult> PublicationFence; const std::wstring Reason;
    const bool RuntimeAdmissionAllowed, IsAdmittedDeniedTrapProjection;
    VmxCompatibilityTrapAdmissionResult(VmxCompatibilityTrapAdmissionDecision decision, std::shared_ptr<VmxCompatDecodeResult> decode, std::shared_ptr<VmxCompatProjectionResult> projection, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<NeutralTrapResult> neutral, std::shared_ptr<HypercallBackendAdmissionResult> backend, std::shared_ptr<TrapDecision> projected, std::shared_ptr<TrapCompletionRouteResult> route, std::shared_ptr<TrapCompletionPublicationFenceResult> fence, std::wstring reason)
      : Decision(decision), Decode(std::move(decode)), Projection(std::move(projection)), RuntimeAdmission(std::move(admission)), NeutralResult(std::move(neutral)), BackendAdmission(std::move(backend)), ProjectedDecision(std::move(projected)), CompletionRoute(std::move(route)), PublicationFence(std::move(fence)), Reason(std::move(reason)), RuntimeAdmissionAllowed(decision != VmxCompatibilityTrapAdmissionDecision::DecodeDenied && decision != VmxCompatibilityTrapAdmissionDecision::ProjectionDenied && RuntimeAdmission && RuntimeAdmission->IsAllowed), IsAdmittedDeniedTrapProjection(decision == VmxCompatibilityTrapAdmissionDecision::TrapProjectionDeniedBackend && RuntimeAdmission && RuntimeAdmission->IsAllowed && NeutralResult && NeutralResult->ShouldTrap && BackendAdmission && BackendAdmission->DeniesBackendExecution && ProjectedDecision && ProjectedDecision->ShouldExit && CompletionRoute && CompletionRoute->DeniesBackendExecution && PublicationFence && PublicationFence->DeniesBackendExecution && !PublicationFence->CompletionPublicationAllowed && !PublicationFence->RetirePublicationAllowed) {}
};
}