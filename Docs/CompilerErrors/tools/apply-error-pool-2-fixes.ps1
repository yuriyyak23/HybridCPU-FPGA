$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path

function Set-TextIfChanged([string]$path, [string]$text) {
    $full = Join-Path $root $path
    if (-not (Test-Path -LiteralPath $full)) { throw "Missing expected source: $full" }
    $old = [System.IO.File]::ReadAllText($full)
    if ($old -cne $text) { [System.IO.File]::WriteAllText($full, $text, [System.Text.UTF8Encoding]::new($false)) }
}

# StreamIngressWarmTelemetry has immutable counters: initialize them in declaration order.
$telemetryHeader = 'NonRTL\\Memory\Registers\StreamRegisterFile.Telemetry.h'
$h = [System.IO.File]::ReadAllText((Join-Path $root $telemetryHeader))
$h = $h -replace '(?m)^using namespace YAKSys_Hybrid_CPU::Core;\r?\n\r?\n', ''
Set-TextIfChanged $telemetryHeader $h

$telemetryCpp = 'NonRTL\\Memory\Registers\StreamRegisterFile.Telemetry.cpp'
$cppPath = Join-Path $root $telemetryCpp
$cpp = [System.IO.File]::ReadAllText($cppPath)
$cpp = $cpp -replace '(?m)^using namespace YAKSys_Hybrid_CPU::Core;\r?\n\r?\n', ''
$ctor = '(?s)(\tStreamIngressWarmTelemetry::StreamIngressWarmTelemetry\(unsigned long long foregroundWarmAttempts, unsigned long long foregroundWarmSuccesses, unsigned long long foregroundWarmReuseHits, unsigned long long foregroundBypassHits, unsigned long long assistWarmAttempts, unsigned long long assistWarmSuccesses, unsigned long long assistWarmReuseHits, unsigned long long assistBypassHits, unsigned long long translationRejects, unsigned long long backendRejects, unsigned long long assistResidentBudgetRejects, unsigned long long assistLoadingBudgetRejects, unsigned long long assistNoVictimRejects\))\s*\{.*?\n\t\}'
$initializer = @'
$1
		: ForegroundWarmAttempts(foregroundWarmAttempts),
		  ForegroundWarmSuccesses(foregroundWarmSuccesses),
		  ForegroundWarmReuseHits(foregroundWarmReuseHits),
		  ForegroundBypassHits(foregroundBypassHits),
		  AssistWarmAttempts(assistWarmAttempts),
		  AssistWarmSuccesses(assistWarmSuccesses),
		  AssistWarmReuseHits(assistWarmReuseHits),
		  AssistBypassHits(assistBypassHits),
		  TranslationRejects(translationRejects),
		  BackendRejects(backendRejects),
		  AssistResidentBudgetRejects(assistResidentBudgetRejects),
		  AssistLoadingBudgetRejects(assistLoadingBudgetRejects),
		  AssistNoVictimRejects(assistNoVictimRejects)
	{}
'@
$cpp = [regex]::Replace($cpp, $ctor, $initializer, 1)
if ($cpp -match 'ForegroundWarmAttempts = foregroundWarmAttempts') { throw 'Telemetry constructor was not converted to an initializer list.' }
Set-TextIfChanged $telemetryCpp $cpp

# Restore the C# records as immutable-value C++ records consumed by the activation classifier.
$activationHeader = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Runtime\SecurePositiveRuntimeExecutionActivationPolicy.h'
$ahPath = Join-Path $root $activationHeader
$ah = [System.IO.File]::ReadAllText($ahPath)
if (-not $ah.Contains('#include "../../Authority/SecureRevocationEpoch.h"')) {
    $ah = $ah.Replace('#include <memory>', '#include <memory>' + "`r`n" + '#include "../../Authority/SecureRevocationEpoch.h"')
}
if (-not $ah.Contains('struct SecurePositiveRuntimeExecutionActivationRequest final')) {
    $marker = "// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:`r`n//	public readonly record struct SecurePositiveRuntimeExecutionActivationRequest"
    $types = @'
    struct SecureBackendOwnerDescriptor final
    {
        bool Materialized = false;
        bool HasIdentity = false;
        bool IsNeutralSource = false;
        bool HasProofChain = false;
        bool NegativeTestsPresent = false;
    };

    struct SecureHypercallBackendContractAdmissionResult final { bool IsProofOnly = false; };
    struct SecureBackendOwnerAdmissionResult final { bool IsAllowed = false; };
    struct SecureIoHypercallAdmissionResult final { bool IsAdmittedDenied = false; bool IsPolicyAdmissionOnly = false; };
    struct SecureCompletionRetirePublicationResult final { bool CompletionPublished = false; bool RetirePublished = false; };
    struct SecureOutputManifestClassificationResult final { bool ManifestClassified = false; };
    struct SecureDebugAttestationVisibilityResult final { bool IsAllowed = false; };
    struct SecureComputeNamedPathVmxZeroAuthorityResult final { bool IsAllowed = false; };
    struct SecureNestedDomainAdmissionResult final { bool IsAllowed = false; };
    struct SecureComputeControlledEmissionResult final { bool IsNoEmissionPreserved = false; };

    struct SecurePositiveRuntimeExecutionActivationRequest final
    {
        SecurePositiveRuntimePathCandidate Candidate = SecurePositiveRuntimePathCandidate::None;
        std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> RuntimeAuthorityOwner;
        bool BackendResultOwnerBoundaryProven = false;
        bool OwnerPathReachabilityProven = false;
        bool NeutralBackendResultProduced = false;
        bool OutputManifestCoverageComplete = false;
        bool RestoreRulesClassified = false;
        bool CompilerNoEmissionDecisionRecorded = false;
        bool Phase18NestedExecutionExcluded = true;
    };

    struct SecurePositiveRuntimeExecutionActivationResult final
    {
        SecurePositiveRuntimeExecutionActivationDecision Decision = SecurePositiveRuntimeExecutionActivationDecision::FutureGatedNoNamedPositivePath;
        SecurePositiveRuntimePathCandidate Candidate = SecurePositiveRuntimePathCandidate::None;
        bool RuntimeExecutionAuthorized = false;
        bool BackendResultAccepted = false;
        bool CompletionPublicationAuthorized = false;
        bool RetirePublicationAuthorized = false;
        bool VmxAuthorityAuthorized = false;
        bool CompilerEmissionAuthorized = false;
        bool NestedExecutionAuthorized = false;
        bool ProductionReleaseApproved = false;
        std::wstring Reason;

        bool IsFutureGated() const noexcept
        {
            return !RuntimeExecutionAuthorized && !BackendResultAccepted && !CompletionPublicationAuthorized &&
                !RetirePublicationAuthorized && !VmxAuthorityAuthorized && !CompilerEmissionAuthorized &&
                !NestedExecutionAuthorized && !ProductionReleaseApproved;
        }

        static std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> FutureGated(
            SecurePositiveRuntimeExecutionActivationDecision decision,
            SecurePositiveRuntimePathCandidate candidate,
            const std::wstring& reason)
        {
            auto result = std::make_shared<SecurePositiveRuntimeExecutionActivationResult>();
            result->Decision = decision;
            result->Candidate = candidate;
            result->Reason = reason;
            return result;
        }
    };

'@
    if (-not $ah.Contains($marker)) { throw 'Activation request record anchor was not found.' }
    $ah = $ah.Replace($marker, $types + $marker)
}
Set-TextIfChanged $activationHeader $ah

$activationCpp = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Runtime\SecurePositiveRuntimeExecutionActivationPolicy.cpp'
$acPath = Join-Path $root $activationCpp
$ac = [System.IO.File]::ReadAllText($acPath)
$ac = $ac.Replace('return admission->Decision == SecureBackendOwnerAdmissionDecision::AllowedProofOnlyNoExecution ?', 'return admission->IsAllowed ?')
$ac = $ac.Replace('return compilerDecision->Decision == SecureComputeControlledEmissionDecision::NoEmissionPreserved ?', 'return compilerDecision->IsNoEmissionPreserved ?')
$ac = $ac.Replace('owner.value()->HasIdentity && owner.value()->IsNeutralSource && owner.value()->HasProofChain', 'owner.value()->HasIdentity() && owner.value()->IsNeutralSource() && owner.value()->HasProofChain()')
$ac = $ac.Replace('admission->IsProofOnly ?', 'admission->IsProofOnly() ?')
$ac = $ac.Replace('admission->IsAllowed ?', 'admission->IsAllowed() ?')
$ac = $ac.Replace('admission->IsAdmittedDenied ?', 'admission->IsAdmittedDenied() ?')
$ac = $ac.Replace('admission->IsPolicyAdmissionOnly ?', 'admission->IsPolicyAdmissionOnly() ?')
$ac = $ac.Replace('visibility->IsAllowed ?', 'visibility->IsAllowed() ?')
$ac = $ac.Replace('zeroAuthority->IsAllowed ?', 'zeroAuthority->IsAllowed() ?')
$ac = $ac.Replace('nested->IsAllowed ?', 'nested->IsAllowed() ?')
$ac = $ac.Replace('compilerDecision->IsNoEmissionPreserved ?', 'compilerDecision->IsNoEmissionPreserved() ?')
$oldPattern = @'
		return owner is { } value && value::Materialized && value::HasIdentity && value::IsNeutralSource && value::HasProofChain && value::NegativeTestsPresent;
'@
$newPattern = @'
		return owner.has_value() && owner.value() && owner.value()->Materialized && owner.value()->HasIdentity() && owner.value()->IsNeutralSource() && owner.value()->HasProofChain() && owner.value()->NegativeTestsPresent;
'@
$ac = $ac.Replace($oldPattern, $newPattern)
if ($ac.Contains('owner is { } value') -or $ac.Contains('value::Materialized')) { throw 'C# pattern syntax remains in activation policy.' }
Set-TextIfChanged $activationCpp $ac

# Replace the temporary converter placeholders with the complete C# record payloads.
$ah = [System.IO.File]::ReadAllText($ahPath)
$typesStart = $ah.IndexOf('    struct SecureBackendOwnerDescriptor final', [StringComparison]::Ordinal)
$typesEnd = $ah.IndexOf('// C# TO C++ CONVERTER TASK:', $typesStart, [StringComparison]::Ordinal)
if ($typesStart -lt 0 -or $typesEnd -lt 0) { throw 'Activation record replacement anchors were not found.' }
$types = @'
    enum class SecureBackendOwnerSource : unsigned char;
    enum class SecureBackendOwnerAdmissionDecision : unsigned char;
    enum class SecureHypercallBackendContractDecision : unsigned char;
    enum class SecureIoHypercallAdmissionDecision : unsigned char;
    enum class SecureCompletionRetirePublicationDecision : unsigned char;
    enum class SecurePublicationLadderStep : unsigned char;
    enum class SecureOutputManifestClassificationDecision : unsigned char;
    enum class SecureOutputManifestEntryKind : unsigned char;
    enum class SecureCheckpointPayloadDecision : unsigned char;
    enum class SecureDebugAttestationVisibilityDecision : unsigned char;
    enum class SecureEvidenceVisibilityClass : unsigned char;
    enum class SecureComputeNamedPathVmxZeroAuthorityDecision : unsigned char;
    enum class SecureComputeNamedPositivePath : unsigned char;
    enum class SecureNestedDomainAdmissionDecision : unsigned char;
    enum class SecureComputeControlledEmissionDecision : unsigned char;
    enum class SecureComputeCompilerEmissionPath : unsigned char;

    struct SecureBackendOwnerDescriptor final
    {
        unsigned long long OwnerId = 0;
        SecureBackendOwnerSource Source{};
        unsigned long long PolicyDigest = 0;
        unsigned long long ProofDigest = 0;
        SecureRevocationEpoch Epoch{};
        bool Materialized = false;
        bool GrantProofValidated = false;
        bool EvidenceProofValidated = false;
        bool CompletionFenceValidated = false;
        bool RetireFenceValidated = false;
        bool NegativeTestsPresent = false;
        bool HasIdentity() const noexcept { return OwnerId != 0; }
        bool IsNeutralSource() const noexcept { const auto s = static_cast<unsigned char>(Source); return s >= 1 && s <= 3; }
        bool HasProofChain() const noexcept { return PolicyDigest != 0 && ProofDigest != 0 && GrantProofValidated && EvidenceProofValidated && CompletionFenceValidated && RetireFenceValidated; }
    };
    struct SecureHypercallBackendContractAdmissionResult final
    {
        SecureHypercallBackendContractDecision Decision{};
        bool ContractProofAccepted = false, BackendExecutionAuthorized = false, CompletionPublicationAuthorized = false, RetirePublicationAuthorized = false;
        std::wstring Reason;
        bool IsProofOnly() const noexcept { return static_cast<unsigned char>(Decision) == 0 && ContractProofAccepted && !BackendExecutionAuthorized && !CompletionPublicationAuthorized && !RetirePublicationAuthorized; }
    };
    struct SecureBackendOwnerAdmissionResult final
    {
        SecureBackendOwnerAdmissionDecision Decision{};
        bool ProofChainAccepted = false, BackendExecutionAuthorized = false;
        std::wstring Reason;
        bool IsAllowed() const noexcept { return static_cast<unsigned char>(Decision) == 0; }
    };
    struct SecureIoHypercallAdmissionResult final
    {
        SecureIoHypercallAdmissionDecision Decision{};
        bool BackendExecutionAuthorized = false, CompletionPublicationAuthorized = false, RetirePublicationAuthorized = false;
        std::wstring Reason;
        bool IsAdmittedDenied() const noexcept { return static_cast<unsigned char>(Decision) == 1 && !BackendExecutionAuthorized; }
        bool IsPolicyAdmissionOnly() const noexcept { return static_cast<unsigned char>(Decision) == 0 && !BackendExecutionAuthorized && !CompletionPublicationAuthorized && !RetirePublicationAuthorized; }
    };
    struct SecureCompletionRetirePublicationResult final
    {
        SecureCompletionRetirePublicationDecision Decision{};
        SecurePublicationLadderStep LadderStep{};
        bool CompletionPublished = false, RetirePublished = false;
        std::wstring Reason;
    };
    struct SecureOutputManifestClassificationResult final
    {
        SecureOutputManifestClassificationDecision Decision{};
        SecureOutputManifestEntryKind EntryKind{};
        SecureCheckpointPayloadDecision PayloadDecision{};
        bool ManifestClassified = false, CheckpointPayloadIncluded = false, RestoreRevalidationRequired = false;
        bool CreatesRuntimeAuthority = false, CreatesCompletionPublicationAuthority = false, CreatesRetirePublicationAuthority = false;
        std::wstring Reason;
    };
    struct SecureDebugAttestationVisibilityResult final
    {
        SecureDebugAttestationVisibilityDecision Decision{};
        SecureEvidenceVisibilityClass VisibilityClass{};
        bool GuestVisible = false, HostOnly = false, DebugOnly = false, AttestationOnly = false;
        bool CreatesRuntimeAuthority = false, CreatesVmreadAuthority = false, CreatesMigrationAuthority = false, CreatesActivationEvidence = false, CreatesBackendOwnerProof = false;
        bool CompletionPublicationAuthorized = false, RetirePublicationAuthorized = false;
        std::wstring Reason;
        bool IsAllowed() const noexcept { return static_cast<unsigned char>(Decision) == 0; }
    };
    struct SecureComputeNamedPathVmxZeroAuthorityResult final
    {
        SecureComputeNamedPathVmxZeroAuthorityDecision Decision{};
        SecureComputeNamedPositivePath Path{};
        bool CompatibilityProjectionAllowed = false, VmxActivationAuthorized = false, VmxCapsAuthorityAuthorized = false, VmcsStateStoreAuthorized = false;
        bool ActiveVmcsPointerIdentityAuthorized = false, VmreadSecureStateAuthorityAuthorized = false, VmwriteSecureStateMutationAuthorized = false;
        bool VmcsCheckpointAuthorityAuthorized = false, CompletionPublicationAuthorized = false, RetirePublicationAuthorized = false;
        std::wstring Reason;
        bool IsAllowed() const noexcept { return static_cast<unsigned char>(Decision) == 0; }
    };
    struct SecureNestedDomainAdmissionResult final
    {
        SecureNestedDomainAdmissionDecision Decision{};
        std::wstring Reason;
        bool BackendSuccessAuthorized = false, MutableNestedStateAuthorized = false;
        bool IsAllowed() const noexcept { return static_cast<unsigned char>(Decision) <= 1; }
    };
    struct SecureComputeControlledEmissionResult final
    {
        SecureComputeControlledEmissionDecision Decision{};
        SecureComputeCompilerEmissionPath Path{};
        bool CompilerEmissionAuthorized = false, BackendExecutionAuthorized = false, NewInstructionEncodingAuthorized = false;
        bool NewOperandFormatAuthorized = false, CapabilityAwareLoadStoreFetchAuthorized = false, VmxSecureModeEmissionAuthorized = false;
        std::wstring Reason;
        bool IsNoEmissionPreserved() const noexcept { return static_cast<unsigned char>(Decision) == 0; }
    };
    struct SecurePositiveRuntimeExecutionActivationRequest final
    {
        SecurePositiveRuntimePathCandidate Candidate = SecurePositiveRuntimePathCandidate::None;
        std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> RuntimeAuthorityOwner;
        bool BackendResultOwnerBoundaryProven = false, OwnerPathReachabilityProven = false, NeutralBackendResultProduced = false;
        bool OutputManifestCoverageComplete = false, RestoreRulesClassified = false, CompilerNoEmissionDecisionRecorded = false;
        bool Phase18NestedExecutionExcluded = true;
    };
    struct SecurePositiveRuntimeExecutionActivationResult final
    {
        SecurePositiveRuntimeExecutionActivationDecision Decision = SecurePositiveRuntimeExecutionActivationDecision::FutureGatedNoNamedPositivePath;
        SecurePositiveRuntimePathCandidate Candidate = SecurePositiveRuntimePathCandidate::None;
        bool RuntimeExecutionAuthorized = false, BackendResultAccepted = false, CompletionPublicationAuthorized = false, RetirePublicationAuthorized = false;
        bool VmxAuthorityAuthorized = false, CompilerEmissionAuthorized = false, NestedExecutionAuthorized = false, ProductionReleaseApproved = false;
        std::wstring Reason;
        bool IsFutureGated() const noexcept { return !RuntimeExecutionAuthorized && !BackendResultAccepted && !CompletionPublicationAuthorized && !RetirePublicationAuthorized && !VmxAuthorityAuthorized && !CompilerEmissionAuthorized && !NestedExecutionAuthorized && !ProductionReleaseApproved; }
        static std::shared_ptr<SecurePositiveRuntimeExecutionActivationResult> FutureGated(SecurePositiveRuntimeExecutionActivationDecision decision, SecurePositiveRuntimePathCandidate candidate, const std::wstring& reason)
        {
            auto result = std::make_shared<SecurePositiveRuntimeExecutionActivationResult>();
            result->Decision = decision; result->Candidate = candidate; result->Reason = reason; return result;
        }
    };

'@
$ah = $ah.Substring(0, $typesStart) + $types + $ah.Substring($typesEnd)
Set-TextIfChanged $activationHeader $ah

Write-Output 'Applied activation-record, optional-owner, and immutable telemetry fixes.'
