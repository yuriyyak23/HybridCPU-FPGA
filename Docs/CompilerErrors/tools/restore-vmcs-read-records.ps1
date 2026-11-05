$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Frontend\Projection\VmcsRead\VmcsReadOnlyValueProjectionService.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include <string>',"#include `"../../../Generated/VmcsProjection/VmcsFieldProjectionSchema.h`"`r`n#include `"../../../../../Runtime/Services/RuntimeBoundaryAdmissionService.h`"`r`n#include `"../../../../../System/Vmcs/V2/VmcsV2FieldModel.h`"`r`n#include <string>`r`n#include <utility>")
$i=$s.IndexOf("// C# TO C++ CONVERTER WARNING: Nullable reference types"); $end=$s.IndexOf('class VmcsReadOnlyValueProjectionService final',$i); if($i -lt 0 -or $end -lt 0){throw 'anchor missing'}
$def=@'
class ExecutionDomainDescriptor; class MemoryDomainDescriptor; class ExecutionDomainReadOnlyStateView; class MemoryDomainReadOnlyTranslationView;
struct VmcsReadOnlyValueProjectionRequest {
    const unsigned short FieldId; const std::shared_ptr<RuntimeBoundaryAdmissionResult> RuntimeAdmission; const std::shared_ptr<EvidencePolicyDescriptor> EvidencePolicy; const bool DescriptorValidated;
    const std::shared_ptr<ExecutionDomainDescriptor> Execution; const std::shared_ptr<MemoryDomainDescriptor> Memory; const std::shared_ptr<CompletionRecord> Completion;
    const std::shared_ptr<PrivilegedExecutionStateDescriptor> PrivilegedExecutionState; const unsigned long long RuntimeDomainTag, RuntimeAddressSpaceTag; const PrivilegedExecutionStateEpoch CurrentPrivilegedExecutionStateEpoch; const bool PrivilegedExecutionStateConformanceProven;
    VmcsReadOnlyValueProjectionRequest(unsigned short field, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<EvidencePolicyDescriptor> evidence, bool descriptorValidated, std::shared_ptr<ExecutionDomainDescriptor> execution, std::shared_ptr<MemoryDomainDescriptor> memory, std::shared_ptr<CompletionRecord> completion, std::shared_ptr<PrivilegedExecutionStateDescriptor> privileged = nullptr, unsigned long long domain = 0, unsigned long long addressSpace = 0, PrivilegedExecutionStateEpoch epoch = {}, bool conformance = false)
      : FieldId(field), RuntimeAdmission(std::move(admission)), EvidencePolicy(std::move(evidence)), DescriptorValidated(descriptorValidated), Execution(std::move(execution)), Memory(std::move(memory)), Completion(std::move(completion)), PrivilegedExecutionState(std::move(privileged)), RuntimeDomainTag(domain), RuntimeAddressSpaceTag(addressSpace), CurrentPrivilegedExecutionStateEpoch(epoch), PrivilegedExecutionStateConformanceProven(conformance) {}
};
struct VmcsReadOnlyValueProjectionResult {
    const VmcsReadOnlyValueProjectionDecision Decision; const VmcsField Field; const std::shared_ptr<VmcsFieldProjectionSchemaEntry> SchemaEntry; const std::shared_ptr<VmcsFieldAliasResult> AliasAccess; const std::shared_ptr<VmcsV2ValidationResult> Validation; const long long Value; const std::wstring Reason;
    std::shared_ptr<PrivilegedExecutionStateProjectionResult> PrivilegedProjection;
    VmcsReadOnlyValueProjectionResult(VmcsReadOnlyValueProjectionDecision decision, VmcsField field, std::shared_ptr<VmcsFieldProjectionSchemaEntry> schema, std::shared_ptr<VmcsFieldAliasResult> alias, std::shared_ptr<VmcsV2ValidationResult> validation, long long value, std::wstring reason) : Decision(decision), Field(field), SchemaEntry(std::move(schema)), AliasAccess(std::move(alias)), Validation(std::move(validation)), Value(value), Reason(std::move(reason)) {}
    bool isProjected() const { return Decision == VmcsReadOnlyValueProjectionDecision::ReadOnlyValueProjected && Validation && Validation->Succeeded; }
};
'@
$s=$s.Substring(0,$i)+$def+"`r`n`t"+$s.Substring($end); [IO.File]::WriteAllText($p,$s)