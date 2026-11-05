$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core';$dest=Join-Path $root 'Runtime\Events\Hypercalls\VirtualizationOperationDecisionTypes.h';$manifest=Join-Path $root 'Runtime\Events\Hypercalls\VirtualizationOperationDecisionManifest.h';$admission=Join-Path $root 'Pipeline\Safety\SafetyVerifier.VirtualizationOperationAdmission.h';$verifier=Join-Path $root 'Pipeline\Safety\YAKSys_Hybrid_CPU.Core.SafetyVerifier.cpp';if(Test-Path $dest){throw 'File exists'}
$content=@"
#pragma once
#include "NeutralHypercallBackendOwnerDescriptor.h"
#include <string>
#include <vector>
namespace YAKSys_Hybrid_CPU::Core {
enum class VirtualizationOperationDecisionState : unsigned char { Absent = 0, Draft = 1, Accepted = 2, Withdrawn = 3 };
struct VirtualizationOperationDecisionManifest final {
    unsigned int SchemaVersion = 0;
    std::wstring DecisionId;
    VirtualizationOperationDecisionState State = VirtualizationOperationDecisionState::Absent;
    std::wstring OperationName, DecisionOwner;
    NeutralHypercallBackendOwnerSource OwnerSource = NeutralHypercallBackendOwnerSource::None;
    std::wstring AcceptedCommitSha;
    std::vector<std::wstring> RequiredReviewers;
    std::vector<unsigned long long> ExactNumericLeaves;
    std::wstring ValueSource, CapabilityPolicy, EvidenceClass, MigrationClass, DenialReason;
    bool operator==(const VirtualizationOperationDecisionManifest&) const = default;
};
struct VirtualizationDecisionAttributionEvidence final {
    std::wstring DecisionOwner, AcceptedCommitSha;
    bool CodeOwnersRulePresent = false, CodeOwnersRuleMatched = false, RequiredReviewersApproved = false;
    std::vector<std::wstring> ApprovedReviewers;
    bool CompatibilityFrontendSelfApproved = false;
    bool operator==(const VirtualizationDecisionAttributionEvidence&) const = default;
};
}
"@
$ms=[IO.File]::ReadAllText($manifest);$start=$ms.IndexOf("`tenum class VirtualizationOperationDecisionState : unsigned char");$end=$ms.IndexOf("`r`n`t};",$start);if($start -lt 0 -or $end -lt 0){throw 'State enum missing'};$ms=$ms.Remove($start,$end+6-$start).Replace('#pragma once',"#pragma once`r`n#include `"VirtualizationOperationDecisionTypes.h`"")
$ads=[IO.File]::ReadAllText($admission);$ads=$ads.Replace('#include <memory>',"#include <memory>`r`n#include `"../../Runtime/Events/Hypercalls/VirtualizationOperationDecisionTypes.h`"")
$m="`tstruct VirtualizationOperationAdmissionResult final";$pos=$ads.IndexOf($m);if($pos -lt 0){$m='    struct VirtualizationOperationAdmissionResult final';$pos=$ads.IndexOf($m)};if($pos -lt 0){throw 'Result marker missing'}
$block=@"
    struct VirtualizationOperationAdmissionRequest final {
        VirtualizationOperationDecisionManifest DecisionManifest;
        VirtualizationDecisionAttributionEvidence Attribution;
        bool CanonicalRuntimeLeafCaptured = false, CapabilityGrantIdentityPresent = false;
        bool EvidencePolicyIdentityPresent = false, AddressSpaceIdentityPresent = false, RestoreGenerationPresent = false;
        bool operator==(const VirtualizationOperationAdmissionRequest&) const = default;
    };

"@
$ads=$ads.Insert($pos,$block)
$vs=[IO.File]::ReadAllText($verifier);$old='VirtualizationOperationDecisionManifestValidator::Validate(request->DecisionManifest, request->Attribution)';$new='VirtualizationOperationDecisionManifestValidator::Validate(std::make_shared<VirtualizationOperationDecisionManifest>(request->DecisionManifest), std::make_shared<VirtualizationDecisionAttributionEvidence>(request->Attribution))';if(-not $vs.Contains($old)){throw 'Validation call missing'};$vs=$vs.Replace($old,$new).Replace('if (!d2->IsStructurallyValidGovernanceEvidence)','if (!d2->IsStructurallyValidGovernanceEvidence())')
[IO.File]::WriteAllText($dest,$content,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($manifest,$ms,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($admission,$ads,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($verifier,$vs,[Text.UTF8Encoding]::new($false))
