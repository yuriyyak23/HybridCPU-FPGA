$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Services\RuntimeBoundaryAdmissionService.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"../Domains/DomainBoundaryDescriptor.h`"`r`n#include `"../Capabilities/CapabilityBoundaryRequirement.h`"`r`n#include `"../Evidence/EvidenceBoundaryRequirement.h`"`r`n#include `"../Domains/Authority/DomainRuntimeAuthority.h`"`r`n#include `"../Domains/SecureCompute/Policies/Admission/SecureDomainAdmissionPolicy.h`"`r`n#include <memory>`r`n#include <string>`r`n#include <utility>")
$i=$s.IndexOf('/// <summary>'); if($i -lt 0){throw 'anchor missing'}
$s=$s.Substring(0,$i)+@'
class DomainRuntimeContext; class RootAuthorityDescriptor; class EvidencePolicyDescriptor; class DomainRuntimeOperation;
class SecureComputeDomainDescriptor; class DomainMeasurementDescriptor; class SecureMemoryDomainDescriptor; struct SecureMemoryAccessRequest;
struct RuntimeBoundaryAdmissionRequest {
    std::shared_ptr<DomainRuntimeContext> Context; std::shared_ptr<RootAuthorityDescriptor> RootAuthority; std::shared_ptr<EvidencePolicyDescriptor> EvidencePolicy;
    std::shared_ptr<DomainRuntimeOperation> Operation; std::shared_ptr<DomainBoundaryDescriptor> DomainBoundary; std::shared_ptr<CapabilityBoundaryRequirement> CapabilityRequirement;
    EvidenceBoundaryRequirement EvidenceRequirement; std::shared_ptr<SecureComputeDomainDescriptor> SecureDescriptor;
    SecureDomainOperationClass SecureOperationClass = SecureDomainOperationClass::Ordinary;
    std::shared_ptr<DomainMeasurementDescriptor> SecureMeasurement; std::shared_ptr<SecureMemoryDomainDescriptor> SecureMemory; std::shared_ptr<SecureMemoryAccessRequest> SecureMemoryAccess;
    RuntimeBoundaryAdmissionRequest(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<EvidencePolicyDescriptor> evidence, std::shared_ptr<DomainRuntimeOperation> operation, std::shared_ptr<DomainBoundaryDescriptor> domainBoundary, std::shared_ptr<CapabilityBoundaryRequirement> capability, EvidenceBoundaryRequirement evidenceRequirement, std::shared_ptr<SecureComputeDomainDescriptor> secureDescriptor = nullptr, SecureDomainOperationClass secureOperation = SecureDomainOperationClass::Ordinary, std::shared_ptr<DomainMeasurementDescriptor> measurement = nullptr, std::shared_ptr<SecureMemoryDomainDescriptor> memory = nullptr, std::shared_ptr<SecureMemoryAccessRequest> memoryAccess = nullptr)
        : Context(std::move(context)), RootAuthority(std::move(root)), EvidencePolicy(std::move(evidence)), Operation(std::move(operation)), DomainBoundary(std::move(domainBoundary)), CapabilityRequirement(std::move(capability)), EvidenceRequirement(evidenceRequirement), SecureDescriptor(std::move(secureDescriptor)), SecureOperationClass(secureOperation), SecureMeasurement(std::move(measurement)), SecureMemory(std::move(memory)), SecureMemoryAccess(std::move(memoryAccess)) {}
};
struct RuntimeBoundaryAdmissionResult {
    const RuntimeBoundaryAdmissionDecision Decision; const std::wstring Message; const std::shared_ptr<DomainRuntimeAuthorityResult> AuthorityResult; const bool IsAllowed;
    RuntimeBoundaryAdmissionResult(RuntimeBoundaryAdmissionDecision decision, std::wstring message, std::shared_ptr<DomainRuntimeAuthorityResult> authority) : Decision(decision), Message(std::move(message)), AuthorityResult(std::move(authority)), IsAllowed(decision == RuntimeBoundaryAdmissionDecision::Allowed) {}
    static std::shared_ptr<RuntimeBoundaryAdmissionResult> Allowed(std::shared_ptr<DomainRuntimeAuthorityResult> authority) { return std::make_shared<RuntimeBoundaryAdmissionResult>(RuntimeBoundaryAdmissionDecision::Allowed, L"", std::move(authority)); }
    static std::shared_ptr<RuntimeBoundaryAdmissionResult> Denied(RuntimeBoundaryAdmissionDecision decision, std::wstring message, std::shared_ptr<DomainRuntimeAuthorityResult> authority = nullptr) { if (!authority) authority = std::make_shared<DomainRuntimeAuthorityResult>(DomainRuntimeAuthorityDecision::Allowed, L""); return std::make_shared<RuntimeBoundaryAdmissionResult>(decision, std::move(message), std::move(authority)); }
};
}
'@
[IO.File]::WriteAllText($p,$s)
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Services\YAKSys_Hybrid_CPU.Core.RuntimeBoundaryAdmissionService.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('request->DomainBoundary.IsSatisfiedBy(context)','request->DomainBoundary->IsSatisfiedBy(context)').Replace('request->CapabilityRequirement.IsSatisfiedBy(context->getCapabilities())','request->CapabilityRequirement->IsSatisfiedBy(context->getCapabilities())').Replace('request->Operation.IsNoStateExecution','request->Operation->isNoStateExecution()')
$s=$s.Replace('request->SecureMemory is {IsMaterialized: true} secureMemory && !secureMemory::IsBoundTo(secureDescriptor->getDomainTag(), context->getAddressSpaceTag())','request->SecureMemory && request->SecureMemory->isMaterialized() && !request->SecureMemory->IsBoundTo(secureDescriptor->getDomainTag(), context->getAddressSpaceTag())')
$s=$s.Replace('request->SecureMemoryAccess is { } secureMemoryAccess','request->SecureMemoryAccess != nullptr').Replace('_secureMemoryAdmission->Admit(request->SecureMemory, secureMemoryAccess,','_secureMemoryAdmission->Admit(request->SecureMemory, request->SecureMemoryAccess,')
[IO.File]::WriteAllText($p,$s)