#pragma once

#include "../Decode/VmxCompatDecodeBoundary.h"
#include "../Projection/VmxCompatProjectionService.h"
#include "../Projection/VmcsRead/VmcsReadOnlyValueProjectionService.h"
#include <memory>
#include <string>
#include <utility>

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmxCompatibilityVmReadAdmissionDecision : unsigned char
	{
		ScalarProjectionAllowed = 0,
		ReadOnlyProjectionDenied = 1,
		DecodeDenied = 2,
		ProjectionDenied = 3,
		RuntimeAdmissionDenied = 4,
		MissingDescriptor = 5,
		ReadOnlyValueProjected = 6
	};

class DomainRuntimeContext; class RootAuthorityDescriptor; class EvidencePolicyDescriptor;
namespace Vmcs::V2 { class VmcsV2Descriptor; }
struct VmxCompatibilityVmReadAdmissionRequest {
    const std::shared_ptr<DomainRuntimeContext> Context; const std::shared_ptr<RootAuthorityDescriptor> RootAuthority; const std::shared_ptr<EvidencePolicyDescriptor> EvidencePolicy; const std::shared_ptr<Vmcs::V2::VmcsV2Descriptor> Descriptor;
    const unsigned short FieldId; const unsigned char DestinationRegister, FieldSelectorRegister, ReservedRegister;
    const bool DescriptorValidated, CapabilityValidated, SchedulingValidated, NoEmissionValidated, ProjectionEvidenceValidated;
    const std::shared_ptr<CompletionRecord> Completion; const std::shared_ptr<PrivilegedExecutionStateDescriptor> PrivilegedExecutionState; const PrivilegedExecutionStateEpoch CurrentPrivilegedExecutionStateEpoch; const bool PrivilegedExecutionStateConformanceProven;
    VmxCompatibilityVmReadAdmissionRequest(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<EvidencePolicyDescriptor> evidence, std::shared_ptr<Vmcs::V2::VmcsV2Descriptor> descriptor, unsigned short field, unsigned char destination, unsigned char selector, unsigned char reserved, bool descriptorOk, bool capability, bool scheduling, bool noEmission, bool projectionEvidence, std::shared_ptr<CompletionRecord> completion = nullptr, std::shared_ptr<PrivilegedExecutionStateDescriptor> privileged = nullptr, PrivilegedExecutionStateEpoch epoch = {}, bool conformance = false)
      : Context(std::move(context)), RootAuthority(std::move(root)), EvidencePolicy(std::move(evidence)), Descriptor(std::move(descriptor)), FieldId(field), DestinationRegister(destination), FieldSelectorRegister(selector), ReservedRegister(reserved), DescriptorValidated(descriptorOk), CapabilityValidated(capability), SchedulingValidated(scheduling), NoEmissionValidated(noEmission), ProjectionEvidenceValidated(projectionEvidence), Completion(std::move(completion)), PrivilegedExecutionState(std::move(privileged)), CurrentPrivilegedExecutionStateEpoch(epoch), PrivilegedExecutionStateConformanceProven(conformance) {}
};
struct VmxCompatibilityVmReadAdmissionResult {
    const VmxCompatibilityVmReadAdmissionDecision Decision; const std::shared_ptr<VmxCompatDecodeResult> Decode; const std::shared_ptr<VmxCompatProjectionResult> Projection; const std::shared_ptr<RuntimeBoundaryAdmissionResult> RuntimeAdmission; const std::shared_ptr<Vmcs::V2::VmcsV2ValidationResult> VmcsValidation; const std::shared_ptr<VmcsReadOnlyValueProjectionResult> ValueProjection; const long long Value; const std::wstring Reason;
    const bool RuntimeAdmissionAllowed, IsScalarProjectionAllowed, IsReadOnlyValueProjected, IsReadOnlyProjectionDenied;
    VmxCompatibilityVmReadAdmissionResult(VmxCompatibilityVmReadAdmissionDecision decision, std::shared_ptr<VmxCompatDecodeResult> decode, std::shared_ptr<VmxCompatProjectionResult> projection, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<Vmcs::V2::VmcsV2ValidationResult> validation, std::shared_ptr<VmcsReadOnlyValueProjectionResult> valueProjection, long long value, std::wstring reason)
      : Decision(decision), Decode(std::move(decode)), Projection(std::move(projection)), RuntimeAdmission(std::move(admission)), VmcsValidation(std::move(validation)), ValueProjection(std::move(valueProjection)), Value(value), Reason(std::move(reason)), RuntimeAdmissionAllowed(decision != VmxCompatibilityVmReadAdmissionDecision::DecodeDenied && decision != VmxCompatibilityVmReadAdmissionDecision::ProjectionDenied && RuntimeAdmission && RuntimeAdmission->IsAllowed), IsScalarProjectionAllowed(decision == VmxCompatibilityVmReadAdmissionDecision::ScalarProjectionAllowed), IsReadOnlyValueProjected(decision == VmxCompatibilityVmReadAdmissionDecision::ReadOnlyValueProjected && ValueProjection && ValueProjection->isProjected()), IsReadOnlyProjectionDenied(decision == VmxCompatibilityVmReadAdmissionDecision::ReadOnlyProjectionDenied && RuntimeAdmission && RuntimeAdmission->IsAllowed && VmcsValidation && VmcsValidation->Code == Vmcs::V2::VmcsV2ValidationCode::AccessDenied && (!ValueProjection || !ValueProjection->isProjected())) {}
};
}