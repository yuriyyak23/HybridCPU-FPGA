$ErrorActionPreference = 'Stop'
$root = 'C:\Users\Yuriy\Desktop\HybridCPU_FPGA'
Set-Location -LiteralPath $root

function Replace-Once([string]$Path, [string]$Old, [string]$New) {
    $full = Join-Path $root $Path
    $content = [System.IO.File]::ReadAllText($full)
    if ($content.Contains($New)) { return }
    $count = ([regex]::Matches($content, [regex]::Escape($Old))).Count
    if ($count -ne 1) { throw "Expected one match in $Path, found $count. Search text: $Old" }
    $content = $content.Replace($Old, $New)
    [System.IO.File]::WriteAllText($full, $content, [System.Text.UTF8Encoding]::new($false))
}

function Replace-First([string]$Path, [string]$Old, [string]$New) {
    $full = Join-Path $root $Path
    $content = [System.IO.File]::ReadAllText($full)
    if ($content.Contains($New)) { return }
    $index = $content.IndexOf($Old, [StringComparison]::Ordinal)
    if ($index -lt 0) { throw "No match in $Path" }
    $content = $content.Substring(0, $index) + $New + $content.Substring($index + $Old.Length)
    [System.IO.File]::WriteAllText($full, $content, [System.Text.UTF8Encoding]::new($false))
}

function Replace-Block([string]$Path, [string]$Start, [string]$End, [string]$Replacement) {
    $full = Join-Path $root $Path
    $content = [System.IO.File]::ReadAllText($full)
    $startIndex = $content.IndexOf($Start, [StringComparison]::Ordinal)
    if ($startIndex -lt 0) {
        if ($content.Contains($Replacement)) { return }
        throw "Start marker not found in $Path"
    }
    $endIndex = $content.IndexOf($End, $startIndex, [StringComparison]::Ordinal)
    if ($endIndex -lt 0) { throw "End marker not found in $Path. Start marker: $Start" }
    $content = $content.Substring(0, $startIndex) + $Replacement + $content.Substring($endIndex)
    [System.IO.File]::WriteAllText($full, $content, [System.Text.UTF8Encoding]::new($false))
}

$descriptor = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Descriptors\Backend\SecureBackendOwnerDescriptor.h'
Replace-Once $descriptor '#pragma once' "#pragma once`r`n`r`n#include `"../../Authority/SecureRevocationEpoch.h`"`r`n#include <memory>`r`n#include <string>`r`n#include <utility>"
Replace-Block $descriptor '// C# TO C++ CONVERTER TASK: C# ''records'' are not converted by C# to C++ Converter:' "`r`n`r`n}" @'
struct SecureBackendOwnerDescriptor final {
    const unsigned long long OwnerId;
    const SecureBackendOwnerSource Source;
    const unsigned long long PolicyDigest;
    const unsigned long long ProofDigest;
    const std::shared_ptr<SecureRevocationEpoch> Epoch;
    const bool Materialized;
    const bool GrantProofValidated;
    const bool EvidenceProofValidated;
    const bool CompletionFenceValidated;
    const bool RetireFenceValidated;
    const bool NegativeTestsPresent;
    const bool HasIdentity;
    const bool IsNeutralSource;
    const bool HasProofChain;

    SecureBackendOwnerDescriptor(unsigned long long ownerId, SecureBackendOwnerSource source,
        unsigned long long policyDigest, unsigned long long proofDigest,
        std::shared_ptr<SecureRevocationEpoch> epoch, bool materialized,
        bool grantProofValidated, bool evidenceProofValidated,
        bool completionFenceValidated, bool retireFenceValidated,
        bool negativeTestsPresent)
        : OwnerId(ownerId), Source(source), PolicyDigest(policyDigest), ProofDigest(proofDigest),
          Epoch(epoch ? std::move(epoch) : SecureRevocationEpoch::Unmaterialized),
          Materialized(materialized), GrantProofValidated(grantProofValidated),
          EvidenceProofValidated(evidenceProofValidated), CompletionFenceValidated(completionFenceValidated),
          RetireFenceValidated(retireFenceValidated), NegativeTestsPresent(negativeTestsPresent),
          HasIdentity(ownerId != 0),
          IsNeutralSource(source == SecureBackendOwnerSource::NeutralRuntimeService ||
              source == SecureBackendOwnerSource::NeutralDeviceModel ||
              source == SecureBackendOwnerSource::NeutralMigrationService),
          HasProofChain(policyDigest != 0 && proofDigest != 0 && grantProofValidated &&
              evidenceProofValidated && completionFenceValidated && retireFenceValidated) {}

    static const std::shared_ptr<SecureBackendOwnerDescriptor> None;
};

inline const std::shared_ptr<SecureBackendOwnerDescriptor> SecureBackendOwnerDescriptor::None =
    std::make_shared<SecureBackendOwnerDescriptor>(0, SecureBackendOwnerSource::None, 0, 0,
        SecureRevocationEpoch::Unmaterialized, false, false, false, false, false, false);
'@ + "`r`n`r`n}"

$epoch = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Authority\SecureRevocationEpoch.h'
Replace-Once $epoch '    bool IsStale(unsigned long long candidate) const noexcept { return !IsCurrent(candidate); }' "    bool IsStale(unsigned long long candidate) const noexcept { return !IsCurrent(candidate); }`r`n    bool Equals(const std::shared_ptr<SecureRevocationEpoch>& other) const noexcept { return other && Current == other->Current; }"

$ownerPolicy = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Backend\SecureBackendOwnerAdmissionPolicy.h'
Replace-Once $ownerPolicy '#pragma once' "#pragma once`r`n`r`n#include `"../../Descriptors/Backend/SecureBackendOwnerDescriptor.h`"`r`n#include <memory>`r`n#include <optional>`r`n#include <string>`r`n#include <utility>"
Replace-Block $ownerPolicy '// C# TO C++ CONVERTER TASK: C# ''records'' are not converted by C# to C++ Converter:' "`r`n`r`n}" @'
struct SecureBackendOwnerAdmissionRequest final {
    std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> Owner;
    SecureBackendRfcAdrState RfcAdrState = SecureBackendRfcAdrState::Missing;
    std::shared_ptr<SecureRevocationEpoch> CurrentEpoch = SecureRevocationEpoch::Unmaterialized;
    bool RequestsBackendExecution = false;
};

struct SecureBackendOwnerAdmissionResult final {
    SecureBackendOwnerAdmissionDecision Decision;
    bool ProofChainAccepted;
    bool BackendExecutionAuthorized;
    std::wstring Reason;
    bool IsAllowed;

    SecureBackendOwnerAdmissionResult(SecureBackendOwnerAdmissionDecision decision, bool proofChainAccepted,
        bool backendExecutionAuthorized, std::wstring reason)
        : Decision(decision), ProofChainAccepted(proofChainAccepted),
          BackendExecutionAuthorized(backendExecutionAuthorized), Reason(std::move(reason)),
          IsAllowed(decision == SecureBackendOwnerAdmissionDecision::AllowedProofOnlyNoExecution) {}

    static std::shared_ptr<SecureBackendOwnerAdmissionResult> AllowedProofOnly() {
        return std::make_shared<SecureBackendOwnerAdmissionResult>(
            SecureBackendOwnerAdmissionDecision::AllowedProofOnlyNoExecution, true, false,
            L"Neutral backend owner proof chain is accepted only as policy evidence; secure backend execution remains closed.");
    }
    static std::shared_ptr<SecureBackendOwnerAdmissionResult> Denied(
        SecureBackendOwnerAdmissionDecision decision, const std::wstring& reason) {
        return std::make_shared<SecureBackendOwnerAdmissionResult>(decision, false, false, reason);
    }
};
'@ + "`r`n`r`n}"

$ownerCpp = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Backend\YAKSys_Hybrid_CPU.Core.SecureBackendOwnerAdmissionPolicy.cpp'
Replace-Once $ownerCpp 'owner.value().Materialized' 'owner.value()->Materialized'
Replace-Once $ownerCpp 'owner.value().HasIdentity' 'owner.value()->HasIdentity'
Replace-Once $ownerCpp 'owner.value().IsNeutralSource' 'owner.value()->IsNeutralSource'
Replace-Once $ownerCpp 'owner.value().HasProofChain' 'owner.value()->HasProofChain'
Replace-Once $ownerCpp 'owner.value().Epoch->Equals(request->CurrentEpoch)' 'owner.value()->Epoch->Equals(request->CurrentEpoch)'
Replace-Once $ownerCpp 'owner.value().NegativeTestsPresent' 'owner.value()->NegativeTestsPresent'
Replace-Once $ownerCpp 'return SecureBackendOwnerAdmissionResult::AllowedProofOnly;' 'return SecureBackendOwnerAdmissionResult::AllowedProofOnly();'

$hyperPolicy = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Backend\SecureHypercallBackendContractAdmissionPolicy.h'
Replace-Once $hyperPolicy '#pragma once' "#pragma once`r`n`r`n#include `"../../Authority/SecureRevocationEpoch.h`"`r`n#include <memory>`r`n#include <string>`r`n#include <utility>"
Replace-Block $hyperPolicy '// C# TO C++ CONVERTER TASK: C# ''records'' are not converted by C# to C++ Converter:' "`r`n`r`n`tclass SecureHypercallBackendContractAdmissionPolicy" @'
struct SecureHypercallBackendContractAdmissionResult final {
    SecureHypercallBackendContractDecision Decision;
    bool ContractProofAccepted;
    bool BackendExecutionAuthorized;
    bool CompletionPublicationAuthorized;
    bool RetirePublicationAuthorized;
    std::wstring Reason;
    bool IsProofOnly;

    SecureHypercallBackendContractAdmissionResult(SecureHypercallBackendContractDecision decision,
        bool proofAccepted, bool executionAuthorized, bool completionAuthorized,
        bool retireAuthorized, std::wstring reason)
        : Decision(decision), ContractProofAccepted(proofAccepted),
          BackendExecutionAuthorized(executionAuthorized),
          CompletionPublicationAuthorized(completionAuthorized),
          RetirePublicationAuthorized(retireAuthorized), Reason(std::move(reason)),
          IsProofOnly(decision == SecureHypercallBackendContractDecision::AllowedProofOnlyNoExecution &&
              proofAccepted && !executionAuthorized && !completionAuthorized && !retireAuthorized) {}

    static std::shared_ptr<SecureHypercallBackendContractAdmissionResult> AllowedProofOnly() {
        return std::make_shared<SecureHypercallBackendContractAdmissionResult>(
            SecureHypercallBackendContractDecision::AllowedProofOnlyNoExecution, true, false, false, false,
            L"Secure hypercall owner/service contract proof accepted; execution and publication remain closed.");
    }
    static std::shared_ptr<SecureHypercallBackendContractAdmissionResult> Denied(
        SecureHypercallBackendContractDecision decision, const std::wstring& reason) {
        return std::make_shared<SecureHypercallBackendContractAdmissionResult>(decision, false, false, false, false, reason);
    }
};
'@ + "`r`n`r`n`tclass SecureHypercallBackendContractAdmissionPolicy"

$ioPolicy = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Io\SecureIoHypercallAdmissionPolicy.h'
Replace-Once $ioPolicy '#pragma once' "#pragma once`r`n`r`n#include <memory>`r`n#include <string>"
Replace-Once $ioPolicy '#include <string>' "#include <string>`r`n#include <utility>`r`n#include `"../../Authority/SecureRevocationEpoch.h`""
Replace-Block $ioPolicy '// C# TO C++ CONVERTER TASK: C# ''records'' are not converted by C# to C++ Converter:' "`r`n`r`n}" @'
struct SecureIoHypercallAdmissionResult final {
    SecureIoHypercallAdmissionDecision Decision;
    bool BackendExecutionAuthorized;
    bool CompletionPublicationAuthorized;
    bool RetirePublicationAuthorized;
    std::wstring Reason;
    bool IsAllowed;
    bool IsAdmittedDenied;
    bool IsPolicyAdmissionOnly;

    SecureIoHypercallAdmissionResult(SecureIoHypercallAdmissionDecision decision,
        bool executionAuthorized, bool completionAuthorized, bool retireAuthorized, std::wstring reason)
        : Decision(decision), BackendExecutionAuthorized(executionAuthorized),
          CompletionPublicationAuthorized(completionAuthorized), RetirePublicationAuthorized(retireAuthorized),
          Reason(std::move(reason)),
          IsAllowed(decision == SecureIoHypercallAdmissionDecision::AllowedIo || decision == SecureIoHypercallAdmissionDecision::AllowedAdmittedDenied),
          IsAdmittedDenied(decision == SecureIoHypercallAdmissionDecision::AllowedAdmittedDenied && !executionAuthorized),
          IsPolicyAdmissionOnly(decision == SecureIoHypercallAdmissionDecision::AllowedIo && !executionAuthorized && !completionAuthorized && !retireAuthorized) {}

    static std::shared_ptr<SecureIoHypercallAdmissionResult> AllowedIo() {
        return std::make_shared<SecureIoHypercallAdmissionResult>(SecureIoHypercallAdmissionDecision::AllowedIo, false, false, false,
            L"Secure I/O policy admission succeeded; backend execution and publication remain closed.");
    }
    static std::shared_ptr<SecureIoHypercallAdmissionResult> AllowedAdmittedDenied() {
        return std::make_shared<SecureIoHypercallAdmissionResult>(SecureIoHypercallAdmissionDecision::AllowedAdmittedDenied, false, false, false,
            L"Secure hypercall admission is recognized but backend execution remains closed.");
    }
    static std::shared_ptr<SecureIoHypercallAdmissionResult> Denied(SecureIoHypercallAdmissionDecision decision, const std::wstring& reason) {
        return std::make_shared<SecureIoHypercallAdmissionResult>(decision, false, false, false, reason);
    }
};
'@ + "`r`n`r`n}"

$emission = 'CloseToHSL\\Core\Virtualization\SecureCompute\Conformance\NoEmission\SecureComputeControlledEmissionGatePolicy.h'
Replace-Once $emission '#pragma once' "#pragma once`r`n`r`n#include <string>`r`n#include <memory>"
Replace-Once $emission '#include <memory>' "#include <memory>`r`n#include <utility>"
Replace-Block $emission '// C# TO C++ CONVERTER TASK: C# ''records'' are not converted by C# to C++ Converter:' "`r`n`r`n`tclass SecureComputeControlledEmissionGatePolicy" @'
struct SecureComputeControlledEmissionRequest final {
    SecureComputeCompilerEmissionPath Path = SecureComputeCompilerEmissionPath::NoCompilerChange;
    bool RequestsCompilerEmission = false;
    bool HasPositiveNeutralRuntimeOwner = false;
    bool HasControlledEmissionRfc = false;
    bool HasReleaseApproval = false;
    bool BackendExecutionAuthorized = false;
    bool EmitsNewInstructionEncoding = false;
    bool EmitsNewOperandFormat = false;
    bool EmitsCapabilityAwareLoadStoreFetch = false;
    bool EmitsVmxSecureMode = false;
};

struct SecureComputeControlledEmissionResult final {
    SecureComputeControlledEmissionDecision Decision;
    SecureComputeCompilerEmissionPath Path;
    bool CompilerEmissionAuthorized;
    bool BackendExecutionAuthorized;
    bool NewInstructionEncodingAuthorized;
    bool NewOperandFormatAuthorized;
    bool CapabilityAwareLoadStoreFetchAuthorized;
    bool VmxSecureModeEmissionAuthorized;
    std::wstring Reason;
    bool IsAllowed() const noexcept { return Decision == SecureComputeControlledEmissionDecision::NoEmissionPreserved; }
    bool CreatesAnyEmissionAuthority() const noexcept {
        return CompilerEmissionAuthorized || BackendExecutionAuthorized || NewInstructionEncodingAuthorized ||
            NewOperandFormatAuthorized || CapabilityAwareLoadStoreFetchAuthorized || VmxSecureModeEmissionAuthorized;
    }

    SecureComputeControlledEmissionResult(SecureComputeControlledEmissionDecision decision,
        SecureComputeCompilerEmissionPath path, bool compilerAuthorized, bool backendAuthorized,
        bool instructionAuthorized, bool operandAuthorized, bool loadStoreAuthorized,
        bool vmxAuthorized, std::wstring reason)
        : Decision(decision), Path(path), CompilerEmissionAuthorized(compilerAuthorized),
          BackendExecutionAuthorized(backendAuthorized), NewInstructionEncodingAuthorized(instructionAuthorized),
          NewOperandFormatAuthorized(operandAuthorized), CapabilityAwareLoadStoreFetchAuthorized(loadStoreAuthorized),
          VmxSecureModeEmissionAuthorized(vmxAuthorized), Reason(std::move(reason)) {}

    static std::shared_ptr<SecureComputeControlledEmissionResult> NoEmissionPreserved(SecureComputeCompilerEmissionPath path, const std::wstring& reason) {
        return std::make_shared<SecureComputeControlledEmissionResult>(SecureComputeControlledEmissionDecision::NoEmissionPreserved, path, false, false, false, false, false, false, reason);
    }
    static std::shared_ptr<SecureComputeControlledEmissionResult> Denied(SecureComputeControlledEmissionDecision decision, SecureComputeCompilerEmissionPath path, const std::wstring& reason) {
        return std::make_shared<SecureComputeControlledEmissionResult>(decision, path, false, false, false, false, false, false, reason);
    }
};
'@ + "`r`n`r`n`tclass SecureComputeControlledEmissionGatePolicy"

$emissionCpp = 'CloseToHSL\\Core\Virtualization\SecureCompute\Conformance\NoEmission\SecureComputeControlledEmissionGatePolicy.cpp'
Replace-Block $emissionCpp "`t// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:" "`r`n`t}`r`n`r`n`tstd::shared_ptr<SecureComputeControlledEmissionResult> SecureComputeControlledEmissionGatePolicy::Deny(" @'
        switch (violation)
        {
        case SecureComputeNoEmissionViolation::NewInstructionEncoding:
            return Deny(SecureComputeControlledEmissionDecision::DeniedNewInstructionEncoding, request, L"SecureCompute compiler no-emission denies new instruction encodings.");
        case SecureComputeNoEmissionViolation::NewOperandFormat:
            return Deny(SecureComputeControlledEmissionDecision::DeniedNewOperandFormat, request, L"SecureCompute compiler no-emission denies new operand formats.");
        case SecureComputeNoEmissionViolation::CapabilityAwareLoadStoreFetch:
            return Deny(SecureComputeControlledEmissionDecision::DeniedCapabilityAwareLoadStoreFetch, request, L"SecureCompute compiler no-emission denies capability-aware memory instructions.");
        case SecureComputeNoEmissionViolation::VmxSecureModeEmission:
            return Deny(SecureComputeControlledEmissionDecision::DeniedVmxSecureModeEmission, request, L"SecureCompute compiler no-emission denies VMX secure-mode emission.");
        default:
            return SecureComputeControlledEmissionResult::NoEmissionPreserved(request->Path, L"");
        }
'@ + "`r`n`t}`r`n`r`n`tstd::shared_ptr<SecureComputeControlledEmissionResult> SecureComputeControlledEmissionGatePolicy::Deny("
Replace-Once $emissionCpp '!noEmissionViolation->IsAllowed' '!noEmissionViolation->IsAllowed()'

$publication = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Publication\SecureCompletionRetirePublicationAuthorityPolicy.h'
Replace-Once $publication '#include "../../Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"' "#include `"../../Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h`"`r`n#include `"../Descriptors/Backend/SecureBackendOwnerDescriptor.h`"`r`n#include `"../Authority/SecureRevocationEpoch.h`"`r`n#include `"YAKSys_Hybrid_CPU.Core.SecureCompletionPublicationFence.h`"`r`n#include `"../Policies/Backend/SecureBackendOwnerAdmissionPolicy.h`"`r`n#include `"../Policies/Backend/SecureHypercallBackendContractAdmissionPolicy.h`"`r`n#include `"../Policies/Io/SecureIoHypercallAdmissionPolicy.h`""
Replace-Once $publication '#include <memory>' "#include <memory>`r`n#include <utility>"
Replace-Block $publication '// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:' "`r`n`tclass SecureCompletionRetirePublicationAuthorityPolicy" @'
struct SecureCompletionPublicationAuthorityRequest final {
    SecurePublicationPathKind PathKind = SecurePublicationPathKind::None;
    SecurePublicationBackendResultState BackendResultState = SecurePublicationBackendResultState::Missing;
    std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> BackendResultOwner;
    std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> CompletionOwner;
    std::shared_ptr<SecureCompletionPublicationFence> PublicationFence;
    std::shared_ptr<SecureRevocationEpoch> CurrentEpoch = SecureRevocationEpoch::Unmaterialized;
    bool CompletionRecordMaterialized = false;
};

struct SecureCompletionRetirePublicationResult final {
    SecureCompletionRetirePublicationDecision Decision;
    SecurePublicationLadderStep LadderStep;
    bool CompletionPublished;
    bool RetirePublished;
    std::wstring Reason;
    bool IsDenied;
    bool IsCompletionOnly;

    SecureCompletionRetirePublicationResult(SecureCompletionRetirePublicationDecision decision,
        SecurePublicationLadderStep ladderStep, bool completionPublished, bool retirePublished, std::wstring reason)
        : Decision(decision), LadderStep(ladderStep), CompletionPublished(completionPublished),
          RetirePublished(retirePublished), Reason(std::move(reason)),
          IsDenied(!completionPublished && !retirePublished), IsCompletionOnly(completionPublished && !retirePublished) {}
    static std::shared_ptr<SecureCompletionRetirePublicationResult> Denied(
        SecureCompletionRetirePublicationDecision decision, SecurePublicationLadderStep ladderStep, const std::wstring& reason) {
        return std::make_shared<SecureCompletionRetirePublicationResult>(decision, ladderStep, false, false, reason);
    }
    static std::shared_ptr<SecureCompletionRetirePublicationResult> PublishedCompletion(const std::wstring& reason) {
        return std::make_shared<SecureCompletionRetirePublicationResult>(SecureCompletionRetirePublicationDecision::CompletionPublishedRetirePending,
            SecurePublicationLadderStep::CompletionPublication, true, false, reason);
    }
    static std::shared_ptr<SecureCompletionRetirePublicationResult> PublishedRetire() {
        return std::make_shared<SecureCompletionRetirePublicationResult>(SecureCompletionRetirePublicationDecision::RetirePublished,
            SecurePublicationLadderStep::RetirePublication, true, true, L"");
    }
};

struct SecureRetirePublicationAuthorityRequest final {
    SecureCompletionRetirePublicationResult CompletionPublication;
    std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> RetireOwner;
    std::shared_ptr<SecureCompletionPublicationFence> PublicationFence;
    std::shared_ptr<SecureRevocationEpoch> CurrentEpoch = SecureRevocationEpoch::Unmaterialized;
    EvidenceVisibilityClass EvidenceClass = EvidenceVisibilityClass::HostOwnedRuntimeEvidence;
    TrapCompletionMigrationClass MigrationClass = TrapCompletionMigrationClass::Unclassified;
};
'@ + "`r`n`r`n`tclass SecureCompletionRetirePublicationAuthorityPolicy"

$publicationCpp = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Publication\SecureCompletionRetirePublicationAuthorityPolicy.cpp'
Replace-Once $publicationCpp '#include "../Policies/Backend/SecureBackendOwnerAdmissionPolicy.h"' "#include `"../Policies/Backend/SecureBackendOwnerAdmissionPolicy.h`"`r`n#include `"../Policies/Io/SecureIoHypercallAdmissionPolicy.h`"`r`n#include `"../Policies/Backend/SecureHypercallBackendContractAdmissionPolicy.h`""
Replace-Once $publicationCpp 'if ((request->PublicationFence == nullptr ? nullptr : request->PublicationFence.CanPublishCompletion) != true)' 'if (request->PublicationFence == nullptr || !request->PublicationFence->getCanPublishCompletion())'
Replace-Once $publicationCpp 'if ((request->PublicationFence == nullptr ? nullptr : request->PublicationFence.CanPublishRetire) != true)' 'if (request->PublicationFence == nullptr || !request->PublicationFence->getCanPublishRetire())'
Replace-Once $publicationCpp 'owner.value().CompletionFenceValidated' 'owner.value()->CompletionFenceValidated'
Replace-Once $publicationCpp 'owner.value().RetireFenceValidated' 'owner.value()->RetireFenceValidated'
Replace-Once $publicationCpp 'owner.value().Materialized' 'owner.value()->Materialized'
Replace-Once $publicationCpp 'owner.value().HasIdentity' 'owner.value()->HasIdentity'
Replace-Once $publicationCpp 'owner.value().IsNeutralSource' 'owner.value()->IsNeutralSource'
Replace-Once $publicationCpp 'owner.value().Epoch->Equals(currentEpoch)' 'owner.value()->Epoch->Equals(currentEpoch)'
Replace-Once $publicationCpp 'return owner is { } value && value::Materialized && value::HasIdentity && value::IsNeutralSource && value::Epoch->Equals(currentEpoch);' 'return owner && owner.value() && owner.value()->Materialized && owner.value()->HasIdentity && owner.value()->IsNeutralSource && owner.value()->Epoch && owner.value()->Epoch->Equals(currentEpoch);'
Replace-Block $publicationCpp "`t// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:" "`r`n`t}`r`n`r`n`tstd::optional<std::shared_ptr<SecureCompletionRetirePublicationResult>> SecureCompletionRetirePublicationAuthorityPolicy::ValidateCompletionOwner" @'
        switch (pathKind)
        {
        case SecurePublicationPathKind::ProofOnlyBackendOwnerAdmission:
            return Deny(SecureCompletionRetirePublicationDecision::DeniedProofOnlyAdmission, SecurePublicationLadderStep::NoBackendResult, L"Proof-only backend owner admission cannot publish secure completion.");
        case SecurePublicationPathKind::AdmittedDeniedSecureHypercallAdmission:
            return Deny(SecureCompletionRetirePublicationDecision::DeniedAdmittedDeniedAdmission, SecurePublicationLadderStep::NoBackendResult, L"Admitted-denied secure hypercall recognition cannot publish secure completion.");
        case SecurePublicationPathKind::RegistryBackedHypercallContractAdmission:
            return Deny(SecureCompletionRetirePublicationDecision::DeniedRegistryBackedProofOnlyAdmission, SecurePublicationLadderStep::NoBackendResult, L"Registry-backed Phase 13 owner/service admission remains proof-only and cannot publish secure completion.");
        case SecurePublicationPathKind::SecureIoPolicyAdmissionOnly:
            return Deny(SecureCompletionRetirePublicationDecision::DeniedPolicyAdmissionOnly, SecurePublicationLadderStep::NoBackendResult, L"Secure I/O policy admission cannot publish secure completion.");
        case SecurePublicationPathKind::GenericTrapRouteFlags:
            return Deny(SecureCompletionRetirePublicationDecision::DeniedGenericTrapRouteNotSecureAuthority, SecurePublicationLadderStep::NoBackendResult, L"Generic trap route publication flags are not SecureCompute authority without secure owner/path reachability.");
        case SecurePublicationPathKind::VmxCompatibilityProjectionOnly:
            return Deny(SecureCompletionRetirePublicationDecision::DeniedVmxProjectionOnly, SecurePublicationLadderStep::NoBackendResult, L"VMX compatibility projection is zero-authority for SecureCompute completion and retire publication.");
        default:
            return DenyNoBackendResult();
        }
'@ + "`r`n`t}`r`n`r`n`tstd::optional<std::shared_ptr<SecureCompletionRetirePublicationResult>> SecureCompletionRetirePublicationAuthorityPolicy::ValidateCompletionOwner"

$contract = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Descriptors\Hypercalls\SecureHypercallBackendContract.h'
Replace-Once $contract '#include <memory>' "#include `"../../Authority/SecureRevocationEpoch.h`"`r`n#include `"../../Authority/SecureGrantHandle.h`"`r`n#include `"../Backend/SecureBackendOwnerDescriptor.h`"`r`n#include `"../Io/YAKSys_Hybrid_CPU.Core.SecureIoDomainDescriptor.h`"`r`n#include <memory>`r`n#include <optional>`r`n#include <vector>"
Replace-Once $contract '#include <vector>' "#include <vector>`r`n#include <utility>"
Replace-Once $contract 'SecureGrantHandle::None' 'std::make_shared<SecureGrantHandle>()'
Replace-First $contract '// C# TO C++ CONVERTER TASK: C# ''records'' are not converted by C# to C++ Converter:' "struct SecureHypercallTransportOpcode final { unsigned short Value = 0; explicit SecureHypercallTransportOpcode(unsigned short value = 0) : Value(value) {} };`r`n`r`nstruct SecureHypercallDecodedLeaf final { unsigned long long Value = 0; bool IsMaterialized = false; explicit SecureHypercallDecodedLeaf(unsigned long long value = 0) : Value(value), IsMaterialized(value != 0) {} };`r`nstruct SecureComputeServiceId final { unsigned long long Value = 0; bool IsMaterialized = false; explicit SecureComputeServiceId(unsigned long long value = 0) : Value(value), IsMaterialized(value != 0) {} };`r`nstruct SecureBackendOwnerId final { unsigned long long Value = 0; bool IsMaterialized = false; explicit SecureBackendOwnerId(unsigned long long value = 0) : Value(value), IsMaterialized(value != 0) {} };`r`nstruct SecureHypercallContractVersion final { unsigned short Major = 0; unsigned short Minor = 0; bool IsMaterialized = false; SecureHypercallContractVersion(unsigned short major = 0, unsigned short minor = 0) : Major(major), Minor(minor), IsMaterialized(major != 0) {} };`r`n"
Replace-Block $contract '//	public readonly record struct SecureHypercallBackendArgument(byte Index, SecureHypercallArgumentOwnership Ownership, ulong Value, ulong Length, SecureGrantHandle Grant)' "`r`n`r`n`tclass SecureHypercallBackendContractDescriptor" @'
struct SecureHypercallBackendArgument final {
    unsigned char Index = 0;
    SecureHypercallArgumentOwnership Ownership = SecureHypercallArgumentOwnership::GuestImmediate;
    unsigned long long Value = 0;
    unsigned long long Length = 0;
    SecureGrantHandle Grant;
    bool IsRawPointer = false;
    bool IsSharedBuffer = false;
    bool IsOpaqueHandle = false;
    SecureHypercallBackendArgument(unsigned char index = 0, SecureHypercallArgumentOwnership ownership = SecureHypercallArgumentOwnership::GuestImmediate,
        unsigned long long value = 0, unsigned long long length = 0, SecureGrantHandle grant = {})
        : Index(index), Ownership(ownership), Value(value), Length(length), Grant(grant),
          IsRawPointer(ownership == SecureHypercallArgumentOwnership::RawHostPointerDenied),
          IsSharedBuffer(ownership == SecureHypercallArgumentOwnership::ExplicitSharedBuffer),
          IsOpaqueHandle(ownership == SecureHypercallArgumentOwnership::OpaqueRuntimeHandle) {}
};
'@ + "`r`n`r`n`tclass SecureHypercallBackendContractDescriptor"
Replace-Block $contract '// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:' "`r`n`r`n}" @'
struct SecureHypercallBackendContractRequest final {
    std::shared_ptr<SecureHypercallTransportOpcode> TransportOpcode;
    std::shared_ptr<SecureHypercallDecodedLeaf> DecodedLeaf;
    std::shared_ptr<SecureComputeServiceId> ServiceId;
    std::shared_ptr<SecureHypercallContractVersion> ContractVersion;
    std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> Owner;
    std::shared_ptr<SecureRevocationEpoch> CurrentEpoch = SecureRevocationEpoch::Unmaterialized;
    SecureGrantHandle PresentedGrant;
    bool EvidenceValidated = false;
    std::shared_ptr<SecureRevocationEpoch> EvidenceEpoch = SecureRevocationEpoch::Unmaterialized;
    std::shared_ptr<SecureIoDomainDescriptor> IoPolicy;
    unsigned long long ValidatedDomainTag = 0;
    std::vector<std::shared_ptr<SecureHypercallBackendArgument>> Arguments;
    bool IsReplay = false;
    bool IdempotentRetry = false;
    bool ReplayTokenMatches = false;
    bool CancellationRequested = false;
};
'@ + "`r`n`r`n}"

$ioDescriptor = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Descriptors\Io\YAKSys_Hybrid_CPU.Core.SecureIoDomainDescriptor.h'
Replace-Once $ioDescriptor '#include "SecureIoDomainDescriptor.h"' "#include `"SecureIoDomainDescriptor.h`"`r`n#include `"../../Authority/SecureRevocationEpoch.h`""
$grantPolicy = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Authority\SecureGrantAuthorityPolicy.h'
Replace-Once $grantPolicy '#pragma once' "#pragma once`r`n`r`n#include `"SecureRevocationEpoch.h`""
$grantHandleCompat = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Authority\YAKSys_Hybrid_CPU.Core.SecureGrantHandle.h'
Replace-Once $grantHandleCompat '#pragma once' "#pragma once`r`n`r`n#include `"SecureRevocationEpoch.h`"`r`n#include `"SecureGrantHandle.h`""
$derivationRecord = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Authority\YAKSys_Hybrid_CPU.Core.SecurePolicyDerivationRecord.h'
Replace-Once $derivationRecord '#pragma once' "#pragma once`r`n`r`n#include `"SecureRevocationEpoch.h`""

$hyperCpp = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Backend\SecureHypercallBackendContractAdmissionPolicy.cpp'
Replace-Once $hyperCpp 'if (request->Owner is not { } owner || !owner::Materialized || !owner::HasIdentity || !owner::IsNeutralSource)' 'if (!request->Owner || !request->Owner.value() || !request->Owner.value()->Materialized || !request->Owner.value()->HasIdentity || !request->Owner.value()->IsNeutralSource)'
Replace-Once $hyperCpp 'owner::OwnerId' 'request->Owner.value()->OwnerId'
Replace-Once $hyperCpp 'owner::Epoch->Equals(contract->getOwnerEpoch())' 'request->Owner.value()->Epoch->Equals(contract->getOwnerEpoch())'
Replace-Once $hyperCpp 'owner::Epoch->Equals(request->CurrentEpoch)' 'request->Owner.value()->Epoch->Equals(request->CurrentEpoch)'
Replace-Once $hyperCpp 'request->PresentedGrant != contract->getRequiredGrant()' '(!contract->getRequiredGrant() || request->PresentedGrant != *contract->getRequiredGrant())'
Replace-Once $hyperCpp '!request->PresentedGrant.MatchesEpoch(request->CurrentEpoch)' '(!request->CurrentEpoch || !request->PresentedGrant.MatchesEpoch(*request->CurrentEpoch))'
Replace-Once $hyperCpp 'request->TransportOpcode != SecureHypercallBackendOwnerAbiRegistry::getTransportOpcode()' '(request->TransportOpcode == nullptr || request->TransportOpcode->Value != SecureHypercallBackendOwnerAbiRegistry::getTransportOpcode()->Value)'
Replace-Once $hyperCpp 'request->ServiceId != contract->getServiceId()' '(request->ServiceId == nullptr || request->ServiceId->Value != contract->getServiceId()->Value)'
Replace-Once $hyperCpp 'request->DecodedLeaf != contract->getDecodedLeaf()' '(request->DecodedLeaf == nullptr || request->DecodedLeaf->Value != contract->getDecodedLeaf()->Value)'
Replace-Once $hyperCpp 'request->ContractVersion != contract->getVersion()' '(request->ContractVersion == nullptr || request->ContractVersion->Major != contract->getVersion()->Major || request->ContractVersion->Minor != contract->getVersion()->Minor)'
$contractCpp = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Descriptors\Hypercalls\SecureHypercallBackendContract.cpp'
$ctorStart = 'SecureHypercallBackendContractDescriptor::SecureHypercallBackendContractDescriptor('
$ctorEnd = 'std::shared_ptr<SecureHypercallBackendContractDescriptor> SecureHypercallBackendContractDescriptor::getUnresolved()'
$newContractCtor = @'
SecureHypercallBackendContractDescriptor::SecureHypercallBackendContractDescriptor(std::shared_ptr<SecureHypercallDecodedLeaf> decodedLeaf, std::shared_ptr<SecureComputeServiceId> serviceId, std::shared_ptr<SecureBackendOwnerId> ownerId, std::shared_ptr<SecureRevocationEpoch> ownerEpoch, std::shared_ptr<SecureHypercallContractVersion> version, std::shared_ptr<SecureGrantHandle> requiredGrant, SecureHypercallReplayPolicy replayPolicy, SecureHypercallCancellationPolicy cancellationPolicy, SecureHypercallRequestMigrationClass requestMigrationClass, SecureHypercallResultMigrationClass resultMigrationClass)
    : DecodedLeaf(std::move(decodedLeaf)), ServiceId(std::move(serviceId)), OwnerId(std::move(ownerId)),
      OwnerEpoch(ownerEpoch ? std::move(ownerEpoch) : SecureRevocationEpoch::Unmaterialized),
      Version(std::move(version)), RequiredGrant(std::move(requiredGrant)), ReplayPolicy(replayPolicy),
      CancellationPolicy(cancellationPolicy), RequestMigrationClass(requestMigrationClass), ResultMigrationClass(resultMigrationClass)
{
}
'@
Replace-Block $contractCpp $ctorStart $ctorEnd $newContractCtor

$multiplyHeader = 'CloseToHSL\\Core\Execution\Scalar\ALU\UInt64Multiply.h'
$multiplyPath = Join-Path $root $multiplyHeader
if (-not (Test-Path -LiteralPath $multiplyPath)) {
    @'
#pragma once
#include <cstdint>

namespace YAKSys_Hybrid_CPU::Execution::Detail {
    inline std::uint64_t MultiplyHighUnsigned64(std::uint64_t lhs, std::uint64_t rhs) noexcept {
        const std::uint64_t lhsLow = static_cast<std::uint32_t>(lhs);
        const std::uint64_t lhsHigh = lhs >> 32;
        const std::uint64_t rhsLow = static_cast<std::uint32_t>(rhs);
        const std::uint64_t rhsHigh = rhs >> 32;
        std::uint64_t product = lhsLow * rhsLow;
        const std::uint64_t carry = product >> 32;
        product = lhsHigh * rhsLow + carry;
        const std::uint64_t middle = static_cast<std::uint32_t>(product);
        const std::uint64_t high = product >> 32;
        product = lhsLow * rhsHigh + middle;
        return lhsHigh * rhsHigh + high + (product >> 32);
    }

    inline std::uint64_t MultiplyHighSignedUnsigned64(std::uint64_t signedLhs, std::uint64_t unsignedRhs) noexcept {
        const std::uint64_t unsignedHigh = MultiplyHighUnsigned64(signedLhs, unsignedRhs);
        return (signedLhs >> 63) ? unsignedHigh - unsignedRhs : unsignedHigh;
    }

    inline std::uint64_t MultiplyHighSigned64(std::uint64_t signedLhs, std::uint64_t signedRhs) noexcept {
        std::uint64_t high = MultiplyHighUnsigned64(signedLhs, signedRhs);
        if (signedLhs >> 63) high -= signedRhs;
        if (signedRhs >> 63) high -= signedLhs;
        return high;
    }
}
'@ | Set-Content -LiteralPath $multiplyPath -Encoding utf8
}

$scalarCpp = 'CloseToHSL\\Core\Execution\Scalar\ALU\ScalarAluOps.cpp'
Replace-Once $scalarCpp '#include "ScalarAluOps.h"' "#include `"ScalarAluOps.h`"`r`n#include `"UInt64Multiply.h`""
Replace-Once $scalarCpp 'static_cast<unsigned long long>((static_cast<__int128>(static_cast<long long>(op1)) * static_cast<__int128>(static_cast<long long>(op2))) >> 64)' 'YAKSys_Hybrid_CPU::Execution::Detail::MultiplyHighSigned64(op1, op2)'
Replace-Once $scalarCpp 'static_cast<unsigned long long>((static_cast<unsigned __int128>(op1) * static_cast<unsigned __int128>(op2)) >> 64)' 'YAKSys_Hybrid_CPU::Execution::Detail::MultiplyHighUnsigned64(op1, op2)'
Replace-Once $scalarCpp 'static_cast<unsigned long long>((static_cast<__int128>(static_cast<long long>(op1)) * static_cast<__int128>(static_cast<unsigned __int128>(op2))) >> 64)' 'YAKSys_Hybrid_CPU::Execution::Detail::MultiplyHighSignedUnsigned64(op1, op2)'

$dispatcherCpp = 'CloseToHSL\\Core\Execution\YAKSys_Hybrid_CPU.Core.Execution.ExecutionDispatcherV4.cpp'
Replace-Once $dispatcherCpp '#include "YAKSys_Hybrid_CPU.Core.Execution.ExecutionDispatcherV4.h"' "#include `"YAKSys_Hybrid_CPU.Core.Execution.ExecutionDispatcherV4.h`"`r`n#include `"Scalar/ALU/UInt64Multiply.h`""
Replace-Once $dispatcherCpp 'static_cast<unsigned long long>((static_cast<__int128>(static_cast<long long>(rs1)) * static_cast<__int128>(static_cast<long long>(rs2))) >> 64)' 'YAKSys_Hybrid_CPU::Execution::Detail::MultiplyHighSigned64(rs1, rs2)'
Replace-Once $dispatcherCpp 'static_cast<unsigned long long>((static_cast<unsigned __int128>(rs1) * static_cast<unsigned __int128>(rs2)) >> 64)' 'YAKSys_Hybrid_CPU::Execution::Detail::MultiplyHighUnsigned64(rs1, rs2)'
Replace-Once $dispatcherCpp 'static_cast<unsigned long long>((static_cast<__int128>(static_cast<long long>(rs1)) * static_cast<__int128>(static_cast<unsigned __int128>(rs2))) >> 64)' 'YAKSys_Hybrid_CPU::Execution::Detail::MultiplyHighSignedUnsigned64(rs1, rs2)'

$vectorH = 'CloseToHSL\\Core\Execution\Vector\ALU\YAKSys_Hybrid_CPU.Execution.VectorALU.h'
Replace-Once $vectorH 'static __int128 SignedMin(DataTypeEnum dataType);' 'static long long SignedMin(DataTypeEnum dataType);'
Replace-Once $vectorH 'static __int128 SignedMax(DataTypeEnum dataType);' 'static long long SignedMax(DataTypeEnum dataType);'
Replace-Once $vectorH 'static unsigned __int128 UnsignedMax(DataTypeEnum dataType);' 'static unsigned long long UnsignedMax(DataTypeEnum dataType);'
$vectorCpp = 'CloseToHSL\\Core\Execution\Vector\ALU\YAKSys_Hybrid_CPU.Execution.VectorALU.cpp'
Replace-Block $vectorCpp "`t	__int128 result = static_cast<__int128>(x) + static_cast<__int128>(y);" "`r`n`t}`r`n`r`n`tunsigned long long VectorALU::SaturatingAddUnsignedForElementWidth" @'
        const long long min = SignedMin(dataType);
        const long long max = SignedMax(dataType);
        if ((y > 0 && x > max - y) || (y < 0 && x < min - y))
        {
            TrackOverflow(core);
            return y > 0 ? max : min;
        }
        return x + y;
'@ + "`r`n`t}`r`n`r`n`tunsigned long long VectorALU::SaturatingAddUnsignedForElementWidth"
Replace-Block $vectorCpp "`t	unsigned __int128 result = static_cast<unsigned __int128>(x) + static_cast<unsigned __int128>(y);" "`r`n`t}`r`n`r`n`t__int128 VectorALU::SignedMin" @'
        const unsigned long long max = UnsignedMax(dataType);
        if (x > max || y > max - x)
        {
            TrackOverflow(core);
            return max;
        }
        return x + y;
'@ + "`r`n`t}`r`n`r`n`t__int128 VectorALU::SignedMin"
Replace-Once $vectorCpp '__int128 VectorALU::SignedMin' 'long long VectorALU::SignedMin'
Replace-Once $vectorCpp '__int128 VectorALU::SignedMax' 'long long VectorALU::SignedMax'
Replace-Once $vectorCpp 'unsigned __int128 VectorALU::UnsignedMax' 'unsigned long long VectorALU::UnsignedMax'
Replace-Block $vectorCpp "`t	// Use 128-bit intermediate to detect overflow`r`n`t	__int128 result = static_cast<__int128>(x) * static_cast<__int128>(y);`r`n`t	if (result > std::numeric_limits<long long>::max() || result < std::numeric_limits<long long>::min())`r`n`t	{`r`n`t		TrackOverflow(core);`r`n`t		return result > std::numeric_limits<long long>::max() ? std::numeric_limits<long long>::max() : std::numeric_limits<long long>::min();`r`n`t	}`r`n`t	return static_cast<long long>(result);" "`r`n`t}`r`n`r`n`tunsigned long long VectorALU::SaturatingAddUnsigned" @'
        const long long max = std::numeric_limits<long long>::max();
        const long long min = std::numeric_limits<long long>::min();
        bool overflow = false;
        if (x > 0) overflow = y > 0 ? x > max / y : (y < 0 && y < min / x);
        else if (x < 0) overflow = y > 0 ? x < min / y : (y < 0 && x < max / y);
        if (overflow)
        {
            TrackOverflow(core);
            return ((x < 0) != (y < 0)) ? min : max;
        }
        return x * y;
'@ + "`r`n`t}`r`n`r`n`tunsigned long long VectorALU::SaturatingAddUnsigned"
Replace-Block $vectorCpp "`t	// Use 128-bit intermediate to detect overflow`r`n`t	unsigned __int128 result = static_cast<unsigned __int128>(x) * static_cast<unsigned __int128>(y);`r`n`t	if (result > std::numeric_limits<unsigned long long>::max())`r`n`t	{`r`n`t		TrackOverflow(core);`r`n`t		return std::numeric_limits<unsigned long long>::max();`r`n`t	}`r`n`t	return static_cast<unsigned long long>(result);" "`r`n`t}`r`n`r`n`tinline int VectorALU::CountLeadingZeros" @'
        if (y != 0 && x > std::numeric_limits<unsigned long long>::max() / y)
        {
            TrackOverflow(core);
            return std::numeric_limits<unsigned long long>::max();
        }
        return x * y;
'@ + "`r`n`t}`r`n`r`n`tinline int VectorALU::CountLeadingZeros"

Write-Output 'Pool 6 source fixes applied. No build or compilation commands were run.'
