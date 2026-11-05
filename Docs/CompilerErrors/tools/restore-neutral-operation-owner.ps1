$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Events\Hypercalls';$manifest=Join-Path $root 'VirtualizationOperationDecisionManifest.h';$type=Join-Path $root 'VirtualizationOperationDecisionValidationResult.h';$owner=Join-Path $root 'DisabledNeutralVirtualizationOperationOwner.h';$cpp=Join-Path $root 'DisabledNeutralVirtualizationOperationOwner.cpp'
if(Test-Path $type){throw 'Type file exists'};$s=[IO.File]::ReadAllText($manifest)
$start=$s.IndexOf("`tenum class VirtualizationOperationDecisionValidationDecision");$end=$s.IndexOf("`r`n`t};",$start);if($start -lt 0 -or $end -lt 0){throw 'Enum boundaries missing'};$block=$s.Substring($start,$end+6-$start);$s=$s.Remove($start,$end+6-$start).Replace('#pragma once',"#pragma once`r`n#include `"VirtualizationOperationDecisionValidationResult.h`"")
$validation=@"
#pragma once
#include <string>
#include <utility>
namespace YAKSys_Hybrid_CPU::Core {
$block
struct VirtualizationOperationDecisionValidationResult final {
    VirtualizationOperationDecisionValidationDecision Decision;
    std::wstring Reason;
    bool IsStructurallyValidGovernanceEvidence() const noexcept { return Decision == VirtualizationOperationDecisionValidationDecision::ValidGovernanceArtifactOnly; }
    bool BackendExecutionAuthorized() const noexcept { return false; }
    bool CompletionPublicationAuthorized() const noexcept { return false; }
    bool RetirePublicationAuthorized() const noexcept { return false; }
    bool operator==(const VirtualizationOperationDecisionValidationResult&) const = default;
};
}
"@
$oh=[IO.File]::ReadAllText($owner);$oh=$oh.Replace('#include <memory>',"#include <memory>`r`n#include <string>`r`n#include `"VirtualizationOperationDecisionValidationResult.h`"")
$marker='class INeutralVirtualizationOperationOwner';$pos=$oh.IndexOf($marker);if($pos -lt 0){throw 'Owner interface missing'}
$records=@"
struct NeutralVirtualizationOperationOwnerRequest final {
    std::wstring OperationName;
    VirtualizationOperationDecisionValidationResult DecisionValidation;
    bool operator==(const NeutralVirtualizationOperationOwnerRequest&) const = default;
};
struct NeutralVirtualizationOperationOwnerResult final {
    NeutralVirtualizationOperationOwnerDecision Decision;
    std::wstring Reason;
    bool BackendExecutionAuthorized() const noexcept { return false; }
    bool CompletionPublicationAuthorized() const noexcept { return false; }
    bool RetirePublicationAuthorized() const noexcept { return false; }
    bool operator==(const NeutralVirtualizationOperationOwnerResult&) const = default;
};

`t
"@
$oh=$oh.Insert($pos,$records).Replace('std::make_shared<DisabledNeutralVirtualizationOperationOwner>()','std::shared_ptr<DisabledNeutralVirtualizationOperationOwner>(new DisabledNeutralVirtualizationOperationOwner())')
$cs=[IO.File]::ReadAllText($cpp);$old='return new(request->DecisionValidation.IsStructurallyValidGovernanceEvidence ? NeutralVirtualizationOperationOwnerDecision::DeniedOwnerInterfaceDisabled : NeutralVirtualizationOperationOwnerDecision::DeniedDecisionArtifactMissing, L"The neutral operation-owner interface is disabled; D2 evidence cannot execute an operation.");';$new='return std::make_shared<NeutralVirtualizationOperationOwnerResult>(request->DecisionValidation.IsStructurallyValidGovernanceEvidence() ? NeutralVirtualizationOperationOwnerDecision::DeniedOwnerInterfaceDisabled : NeutralVirtualizationOperationOwnerDecision::DeniedDecisionArtifactMissing, L"The neutral operation-owner interface is disabled; D2 evidence cannot execute an operation.");';if(-not $cs.Contains($old)){throw 'Resolve body missing'};$cs=$cs.Replace($old,$new)
[IO.File]::WriteAllText($type,$validation,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($manifest,$s,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($owner,$oh,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($cpp,$cs,[Text.UTF8Encoding]::new($false))
