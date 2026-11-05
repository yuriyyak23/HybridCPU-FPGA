$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path

function Read-Source([string]$relative) {
    $path = Join-Path $root $relative
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing source: $path" }
    return [System.IO.File]::ReadAllText($path)
}
function Write-Source([string]$relative, [string]$content) {
    [System.IO.File]::WriteAllText((Join-Path $root $relative), $content, [System.Text.UTF8Encoding]::new($false))
}
function Insert-Once([string]$text, [string]$anchor, [string]$payload, [string]$marker) {
    if ($text.Contains($marker)) { return $text }
    $at = $text.IndexOf($anchor, [StringComparison]::Ordinal)
    if ($at -lt 0) { throw "Insertion anchor not found: $anchor" }
    return $text.Insert($at, $payload)
}

# Restore the private-memory contract used by both migration and manifest policy.
$migrationH = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Migration\SecureMigrationAdmissionPolicy.h'
$m = Read-Source $migrationH
$privateRecord = @'
	struct SecurePrivateMemorySealedPayloadContract final
	{
		bool HasSealedPayload;
		bool HasEncryptedPayload;
		bool HasNeutralKeyOwner;
		bool HasEvidencePolicy;
		bool HasRestoreValidationProof;
		bool ContainsRawSealingKey;

		SecurePrivateMemorySealedPayloadContract(bool sealedPayload, bool encryptedPayload, bool neutralKeyOwner, bool evidencePolicy, bool restoreValidationProof, bool rawSealingKey = false)
			: HasSealedPayload(sealedPayload), HasEncryptedPayload(encryptedPayload), HasNeutralKeyOwner(neutralKeyOwner), HasEvidencePolicy(evidencePolicy), HasRestoreValidationProof(restoreValidationProof), ContainsRawSealingKey(rawSealingKey) {}

		bool IsComplete() const noexcept
		{
			return HasSealedPayload && HasEncryptedPayload && HasNeutralKeyOwner && HasEvidencePolicy && HasRestoreValidationProof && !ContainsRawSealingKey;
		}
	};

'@
$m = Insert-Once $m "// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:`r`n//	public readonly record struct SecurePrivateMemorySealedPayloadContract" $privateRecord 'struct SecurePrivateMemorySealedPayloadContract final'
Write-Source $migrationH $m
$migrationCpp = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Migration\YAKSys_Hybrid_CPU.Core.SecureMigrationAdmissionPolicy.cpp'
$migrationSource = Read-Source $migrationCpp
$migrationSource = $migrationSource.Replace('(!privateMemoryContract ? nullptr : privateMemoryContract.IsComplete) != true', '(!privateMemoryContract.has_value() || !privateMemoryContract.value() || !privateMemoryContract.value()->IsComplete())')
Write-Source $migrationCpp $migrationSource
$migrationPolicyH = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Migration\YAKSys_Hybrid_CPU.Core.SecureMigrationAdmissionPolicy.h'
$mph = Read-Source $migrationPolicyH
$mph = $mph.Replace('std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> &privateMemoryContract = std::nullopt', 'std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> privateMemoryContract = std::nullopt')
$mph = $mph.Replace('AdmitPrivateMemoryPayload(std::shared_ptr<SecureMigrationDescriptor> migration, std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> &privateMemoryContract);', 'AdmitPrivateMemoryPayload(std::shared_ptr<SecureMigrationDescriptor> migration, const std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> &privateMemoryContract);')
Write-Source $migrationPolicyH $mph
$migrationSource = [regex]::Replace($migrationSource, 'SecureMigrationAdmissionPolicy::AdmitCheckpointPayload\(std::shared_ptr<SecureMigrationDescriptor> migration, SecureCheckpointPayloadClass payloadClass, (?:const )?std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>>\s*&?\s*privateMemoryContract\)', 'SecureMigrationAdmissionPolicy::AdmitCheckpointPayload(std::shared_ptr<SecureMigrationDescriptor> migration, SecureCheckpointPayloadClass payloadClass, std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> privateMemoryContract)')
$migrationSource = [regex]::Replace($migrationSource, 'SecureMigrationAdmissionPolicy::AdmitPrivateMemoryPayload\(std::shared_ptr<SecureMigrationDescriptor> migration, (?:const )?std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>>\s*&?\s*privateMemoryContract\)', 'SecureMigrationAdmissionPolicy::AdmitPrivateMemoryPayload(std::shared_ptr<SecureMigrationDescriptor> migration, const std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> &privateMemoryContract)')
Write-Source $migrationCpp $migrationSource

# Restore manifest entry/result records and replace the invalid C# collection projection.
$manifestH = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Checkpoint\SecureOutputManifestClassificationPolicy.h'
$mh = Read-Source $manifestH
if (-not $mh.Contains('#include "../Policies/Migration/SecureMigrationAdmissionPolicy.h"')) {
    $mh = $mh.Replace('#include "SecureCheckpointPayloadPolicy.h"', '#include "SecureCheckpointPayloadPolicy.h"' + "`r`n" + '#include "../Policies/Migration/SecureMigrationAdmissionPolicy.h"')
}
$manifestTypes = @'
	struct SecureOutputManifestEntry final
	{
		SecureOutputManifestEntryKind Kind;
		SecureCheckpointPayloadClass PayloadClass;
		bool OwnerPathClassified;
		bool RestoreValidationProven;

		SecureOutputManifestEntry(SecureOutputManifestEntryKind kind, SecureCheckpointPayloadClass payloadClass, bool ownerPathClassified, bool restoreValidationProven)
			: Kind(kind), PayloadClass(payloadClass), OwnerPathClassified(ownerPathClassified), RestoreValidationProven(restoreValidationProven) {}

		static std::shared_ptr<SecureOutputManifestEntry> RequestState() { return std::make_shared<SecureOutputManifestEntry>(SecureOutputManifestEntryKind::RequestState, SecureCheckpointPayloadClass::SecurePolicyDescriptor, true, true); }
		static std::shared_ptr<SecureOutputManifestEntry> InternalBackendResult() { return std::make_shared<SecureOutputManifestEntry>(SecureOutputManifestEntryKind::InternalBackendResult, SecureCheckpointPayloadClass::SecurePolicyDescriptor, true, true); }
		static std::shared_ptr<SecureOutputManifestEntry> InternalCompletionRecord() { return std::make_shared<SecureOutputManifestEntry>(SecureOutputManifestEntryKind::InternalCompletionRecord, SecureCheckpointPayloadClass::SecurePolicyDescriptor, true, true); }
		static std::shared_ptr<SecureOutputManifestEntry> GuestVisibleOutput() { return std::make_shared<SecureOutputManifestEntry>(SecureOutputManifestEntryKind::GuestVisibleOutput, SecureCheckpointPayloadClass::GuestVisibleState, true, true); }
		static std::shared_ptr<SecureOutputManifestEntry> RetireVisibleState() { return std::make_shared<SecureOutputManifestEntry>(SecureOutputManifestEntryKind::RetireVisibleState, SecureCheckpointPayloadClass::GuestVisibleState, true, true); }
		static std::shared_ptr<SecureOutputManifestEntry> RecomputedAfterRestoreState() { return std::make_shared<SecureOutputManifestEntry>(SecureOutputManifestEntryKind::RecomputedAfterRestoreState, SecureCheckpointPayloadClass::SecurePolicyDescriptor, true, true); }
	};

	struct SecureOutputManifestClassificationResult final
	{
		SecureOutputManifestClassificationDecision Decision;
		SecureOutputManifestEntryKind EntryKind;
		SecureCheckpointPayloadDecision PayloadDecision;
		bool ManifestClassified;
		bool CheckpointPayloadIncluded;
		bool RestoreRevalidationRequired;
		bool CreatesRuntimeAuthority;
		bool CreatesCompletionPublicationAuthority;
		bool CreatesRetirePublicationAuthority;
		std::wstring Reason;
		bool IsDenied;

		SecureOutputManifestClassificationResult(SecureOutputManifestClassificationDecision decision, SecureOutputManifestEntryKind entryKind, SecureCheckpointPayloadDecision payloadDecision, bool classified, bool checkpointIncluded, bool restoreRequired, bool runtimeAuthority, bool completionAuthority, bool retireAuthority, const std::wstring& reason)
			: Decision(decision), EntryKind(entryKind), PayloadDecision(payloadDecision), ManifestClassified(classified), CheckpointPayloadIncluded(checkpointIncluded), RestoreRevalidationRequired(restoreRequired), CreatesRuntimeAuthority(runtimeAuthority), CreatesCompletionPublicationAuthority(completionAuthority), CreatesRetirePublicationAuthority(retireAuthority), Reason(reason), IsDenied(!classified) {}

		bool CreatesAnyRuntimeOrPublicationAuthority() const noexcept { return CreatesRuntimeAuthority || CreatesCompletionPublicationAuthority || CreatesRetirePublicationAuthority; }
		static std::shared_ptr<SecureOutputManifestClassificationResult> Classified(SecureOutputManifestClassificationDecision decision, SecureOutputManifestEntryKind kind, SecureCheckpointPayloadDecision payload, bool checkpointIncluded, bool restoreRequired, const std::wstring& reason)
		{
			return std::make_shared<SecureOutputManifestClassificationResult>(decision, kind, payload, true, checkpointIncluded, restoreRequired, false, false, false, reason);
		}
		static std::shared_ptr<SecureOutputManifestClassificationResult> Denied(SecureOutputManifestClassificationDecision decision, SecureOutputManifestEntryKind kind, SecureCheckpointPayloadDecision payload, const std::wstring& reason)
		{
			return std::make_shared<SecureOutputManifestClassificationResult>(decision, kind, payload, false, false, false, false, false, false, reason);
		}
	};

'@
$mh = Insert-Once $mh "// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:`r`n//	public readonly record struct SecureOutputManifestEntry" $manifestTypes 'struct SecureOutputManifestEntry final'
$oldArrays = "`t`tstatic inline const std::vector<SecureOutputManifestEntryKind> RequiredPositivePathEntries = Array::AsReadOnly(RequiredEntries);`r`n`r`n`t`tstatic inline const std::vector<SecureOutputManifestEntryKind> RequiredEntries = {SecureOutputManifestEntryKind::RequestState, SecureOutputManifestEntryKind::InternalBackendResult, SecureOutputManifestEntryKind::InternalCompletionRecord, SecureOutputManifestEntryKind::GuestVisibleOutput, SecureOutputManifestEntryKind::RetireVisibleState, SecureOutputManifestEntryKind::RecomputedAfterRestoreState};"
$newArrays = "`t`tstatic inline const std::vector<SecureOutputManifestEntryKind> RequiredEntries = {SecureOutputManifestEntryKind::RequestState, SecureOutputManifestEntryKind::InternalBackendResult, SecureOutputManifestEntryKind::InternalCompletionRecord, SecureOutputManifestEntryKind::GuestVisibleOutput, SecureOutputManifestEntryKind::RetireVisibleState, SecureOutputManifestEntryKind::RecomputedAfterRestoreState};`r`n`t`tstatic inline const std::vector<SecureOutputManifestEntryKind> RequiredPositivePathEntries = RequiredEntries;"
$mh = $mh.Replace($oldArrays, $newArrays)
$mh = $mh.Replace('std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> &privateMemoryContract = std::nullopt', 'std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> privateMemoryContract = std::nullopt')
$mh = $mh.Replace('SecureCheckpointPayloadDecision ClassifyPayload(SecureCheckpointPayloadClass payloadClass, std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> &privateMemoryContract);', 'SecureCheckpointPayloadDecision ClassifyPayload(SecureCheckpointPayloadClass payloadClass, const std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> &privateMemoryContract);')
Write-Source $manifestH $mh

$manifestCpp = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Checkpoint\SecureOutputManifestClassificationPolicy.cpp'
$mc = Read-Source $manifestCpp
$mc = $mc.Replace('checkpointPayloadIncluded: ', '')
$mc = $mc.Replace('restoreRevalidationRequired: ', '')
$mc = $mc.Replace('std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> &privateMemoryContract', 'std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> privateMemoryContract')
$mc = [regex]::Replace($mc, 'SecureOutputManifestClassificationPolicy::ClassifyPayload\(SecureCheckpointPayloadClass payloadClass, (?:const )?std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>>\s*&?\s*privateMemoryContract\)', 'SecureOutputManifestClassificationPolicy::ClassifyPayload(SecureCheckpointPayloadClass payloadClass, const std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> &privateMemoryContract)')
$mc = $mc.Replace('(!privateMemoryContract ? nullptr : privateMemoryContract.IsComplete) == true', '(privateMemoryContract.has_value() && privateMemoryContract.value() && privateMemoryContract.value()->IsComplete())')
Write-Source $manifestCpp $mc

# Restore visibility request/result records and translate named arguments to positional C++ arguments.
$debugH = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Debug\SecureDebugAttestationVisibilityPolicy.h'
$dh = Read-Source $debugH
$debugTypes = @'
	struct SecureDebugAttestationVisibilityRequest final
	{
		SecureDebugAttestationQueryKind QueryKind;
		std::shared_ptr<SecureComputeDomainDescriptor> Descriptor;
		std::shared_ptr<DomainMeasurementDescriptor> Measurement;
		std::shared_ptr<EvidencePolicyDescriptor> NeutralEvidencePolicy;
		bool RequestsMigrationPayload = false;
		bool RequestsVmreadValueSource = false;
		bool RequestsActivationEvidence = false;
		bool RequestsBackendOwnerProof = false;
		bool RequestsPrivateMemoryInspection = false;
		bool RequestsCompletionPublication = false;
		bool RequestsRetirePublication = false;
	};

	struct SecureDebugAttestationVisibilityResult final
	{
		SecureDebugAttestationVisibilityDecision Decision;
		SecureEvidenceVisibilityClass VisibilityClass;
		bool GuestVisible, HostOnly, DebugOnly, AttestationOnly;
		bool CreatesRuntimeAuthority, CreatesVmreadAuthority, CreatesMigrationAuthority, CreatesActivationEvidence, CreatesBackendOwnerProof;
		bool CompletionPublicationAuthorized, RetirePublicationAuthorized;
		std::wstring Reason;
		bool IsAllowed;

		SecureDebugAttestationVisibilityResult(SecureDebugAttestationVisibilityDecision decision, SecureEvidenceVisibilityClass visibilityClass, bool guestVisible, bool hostOnly, bool debugOnly, bool attestationOnly, bool runtimeAuthority, bool vmreadAuthority, bool migrationAuthority, bool activationEvidence, bool backendOwnerProof, bool completionPublication, bool retirePublication, const std::wstring& reason)
			: Decision(decision), VisibilityClass(visibilityClass), GuestVisible(guestVisible), HostOnly(hostOnly), DebugOnly(debugOnly), AttestationOnly(attestationOnly), CreatesRuntimeAuthority(runtimeAuthority), CreatesVmreadAuthority(vmreadAuthority), CreatesMigrationAuthority(migrationAuthority), CreatesActivationEvidence(activationEvidence), CreatesBackendOwnerProof(backendOwnerProof), CompletionPublicationAuthorized(completionPublication), RetirePublicationAuthorized(retirePublication), Reason(reason), IsAllowed(decision == SecureDebugAttestationVisibilityDecision::VisibilityClassified) {}

		bool CreatesAnyAuthority() const noexcept { return CreatesRuntimeAuthority || CreatesVmreadAuthority || CreatesMigrationAuthority || CreatesActivationEvidence || CreatesBackendOwnerProof || CompletionPublicationAuthorized || RetirePublicationAuthorized; }
		static std::shared_ptr<SecureDebugAttestationVisibilityResult> Classified(SecureEvidenceVisibilityClass visibilityClass, bool guestVisible, bool hostOnly, bool debugOnly, bool attestationOnly, const std::wstring& reason)
		{
			return std::make_shared<SecureDebugAttestationVisibilityResult>(SecureDebugAttestationVisibilityDecision::VisibilityClassified, visibilityClass, guestVisible, hostOnly, debugOnly, attestationOnly, false, false, false, false, false, false, false, reason);
		}
		static std::shared_ptr<SecureDebugAttestationVisibilityResult> Denied(SecureDebugAttestationVisibilityDecision decision, SecureEvidenceVisibilityClass visibilityClass, const std::wstring& reason)
		{
			return std::make_shared<SecureDebugAttestationVisibilityResult>(decision, visibilityClass, false, false, false, false, false, false, false, false, false, false, false, reason);
		}
	};

'@
$dh = Insert-Once $dh "// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:`r`n// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:`r`n// ORIGINAL LINE: public readonly record struct SecureDebugAttestationVisibilityRequest" $debugTypes 'struct SecureDebugAttestationVisibilityRequest final'
Write-Source $debugH $dh
$debugCpp = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Debug\SecureDebugAttestationVisibilityPolicy.cpp'
$dc = Read-Source $debugCpp
foreach ($name in @('guestVisible','hostOnly','debugOnly','attestationOnly')) { $dc = $dc.Replace($name + ': ', '') }
Write-Source $debugCpp $dc

# Restore release evidence request/results.
$phase21H = 'CloseToHSL\\Core\Virtualization\SecureCompute\Conformance\ReleaseGate\SecureComputePhase21ConformanceEvidencePolicy.h'
$p21h = Read-Source $phase21H
$phase21Types = @'
	struct SecureComputePhase21ConformanceEvidenceRequest final
	{
		bool Phase13RegistryBackedAdmissionCannotExecuteCovered, Phase14CompletionRetireFailClosedCovered, Phase15ManifestCoverageAndManifestOnlyDenialCovered;
		bool Phase16VisibilityCannotBecomeRuntimeEvidenceCovered, Phase17VmxZeroAuthorityCovered, Phase18NestedDesignFenceCovered;
		bool Phase19NoCompilerChangeCovered, Phase20FutureGatedRuntimeActivationCovered, ReleaseEvidenceBoundaryCovered;
		bool PositiveRuntimePathProven = false, RequestsProductionActivation = false, RequestsCompilerSecureEmission = false;
		bool RequestsVmxOwnedSecureComputeAuthority = false, OpensPhase18NestedExecution = false, TreatsManifestOnlyAsExecutionProof = false;
		bool TreatsDebugAttestationAsRuntimeEvidence = false;
	};
	struct SecureComputePhase21ConformanceEvidenceResult final
	{
		SecureComputePhase21ConformanceDecision Decision;
		bool MatrixEvidenceAccepted, FuturePositiveTestsRemainGated, RuntimeExecutionAuthorized, BackendResultAccepted;
		bool CompletionPublicationAuthorized, RetirePublicationAuthorized, VmxAuthorityAuthorized, CompilerEmissionAuthorized;
		bool NestedExecutionAuthorized, ProductionReleaseApproved;
		std::wstring Reason;
		bool CreatesAnyRuntimeAuthority() const noexcept { return RuntimeExecutionAuthorized || BackendResultAccepted || CompletionPublicationAuthorized || RetirePublicationAuthorized || VmxAuthorityAuthorized || CompilerEmissionAuthorized || NestedExecutionAuthorized || ProductionReleaseApproved; }
		static std::shared_ptr<SecureComputePhase21ConformanceEvidenceResult> Accepted(SecureComputePhase21ConformanceDecision decision, bool futurePositiveTestsRemainGated, const std::wstring& reason)
		{
			return std::make_shared<SecureComputePhase21ConformanceEvidenceResult>(SecureComputePhase21ConformanceEvidenceResult{decision, true, futurePositiveTestsRemainGated, false, false, false, false, false, false, false, false, reason});
		}
		static std::shared_ptr<SecureComputePhase21ConformanceEvidenceResult> Denied(SecureComputePhase21ConformanceDecision decision, const std::wstring& reason)
		{
			return std::make_shared<SecureComputePhase21ConformanceEvidenceResult>(SecureComputePhase21ConformanceEvidenceResult{decision, false, true, false, false, false, false, false, false, false, false, reason});
		}
	};

'@
$p21h = Insert-Once $p21h "// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:`r`n//	public readonly record struct SecureComputePhase21ConformanceEvidenceRequest" $phase21Types 'struct SecureComputePhase21ConformanceEvidenceRequest final'
Write-Source $phase21H $p21h
$phase21Cpp = 'CloseToHSL\\Core\Virtualization\SecureCompute\Conformance\ReleaseGate\SecureComputePhase21ConformanceEvidencePolicy.cpp'
$p21c = Read-Source $phase21Cpp
$p21c = $p21c.Replace('futurePositiveTestsRemainGated: ', '')
Write-Source $phase21Cpp $p21c

$phase22H = 'CloseToHSL\\Core\Virtualization\SecureCompute\Conformance\ReleaseGate\SecureComputePhase22LimitedReleaseGatePolicy.h'
$p22h = Read-Source $phase22H
$phase22Types = @'
	struct SecureComputePhase22LimitedReleaseEvidenceRequest final
	{
		bool Phase21MatrixEvidenceAccepted, Phase21FuturePositiveTestsRemainGated, NamedPositiveRuntimePathProven;
		bool OwnerSpecificRfcAdrAccepted, ProductionOwnerCodeExists, OwnerPathReachabilityProven, TypedRequestResultModelExists;
		bool BackendResultOwnerBoundaryProven, CompletionRetirePolicyForNamedPathProven, MigrationManifestRestoreEvidenceComplete;
		bool DebugAttestationVisibilityLimitsProven, VmxProjectionAfterNeutralResultOnly, CompilerNoEmissionDecisionRecorded;
		bool Phase18NestedExecutionExcluded, ProductClaimScopedToNamedPath, BoundedRollbackProcedureReviewed, BackendExecutionAuthorityLocallyProven;
	};
	struct SecureComputePhase22LimitedReleaseGateResult final
	{
		SecureComputePhase22LimitedReleaseDecision Decision;
		bool ReleaseEvidenceAccepted, RuntimeExecutionAuthorized, BackendResultAccepted, CompletionPublicationAuthorized;
		bool RetirePublicationAuthorized, VmxAuthorityAuthorized, CompilerEmissionAuthorized, NestedExecutionAuthorized, ProductionReleaseApproved;
		std::wstring Reason;
		bool CreatesAnyReleaseAuthority() const noexcept { return RuntimeExecutionAuthorized || BackendResultAccepted || CompletionPublicationAuthorized || RetirePublicationAuthorized || VmxAuthorityAuthorized || CompilerEmissionAuthorized || NestedExecutionAuthorized || ProductionReleaseApproved; }
		static std::shared_ptr<SecureComputePhase22LimitedReleaseGateResult> Denied(SecureComputePhase22LimitedReleaseDecision decision, const std::wstring& reason)
		{
			return std::make_shared<SecureComputePhase22LimitedReleaseGateResult>(SecureComputePhase22LimitedReleaseGateResult{decision, false, false, false, false, false, false, false, false, false, reason});
		}
	};

'@
$p22h = Insert-Once $p22h "// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:`r`n//	public readonly record struct SecureComputePhase22LimitedReleaseEvidenceRequest" $phase22Types 'struct SecureComputePhase22LimitedReleaseEvidenceRequest final'
Write-Source $phase22H $p22h

# Consume the manifest/debug records from their defining policy headers.
$activationHPath = Join-Path $root 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Runtime\SecurePositiveRuntimeExecutionActivationPolicy.h'
$activationH = [System.IO.File]::ReadAllText($activationHPath)
if (-not $activationH.Contains('#include "../../Checkpoint/SecureOutputManifestClassificationPolicy.h"')) {
    $activationH = $activationH.Replace('#include "../../Authority/SecureRevocationEpoch.h"', '#include "../../Authority/SecureRevocationEpoch.h"' + "`r`n" + '#include "../../Checkpoint/SecureOutputManifestClassificationPolicy.h"' + "`r`n" + '#include "../Debug/SecureDebugAttestationVisibilityPolicy.h"')
}
foreach ($typeName in @('SecureOutputManifestClassificationResult','SecureDebugAttestationVisibilityResult')) {
    $pattern = '(?s)    struct ' + [regex]::Escape($typeName) + ' final\r?\n.*?\r?\n    \};\r?\n'
    $activationH = [regex]::Replace($activationH, $pattern, '', 1)
}
[System.IO.File]::WriteAllText($activationHPath, $activationH, [System.Text.UTF8Encoding]::new($false))

Write-Output 'Restored manifest, debug-visibility, and Phase 21/22 release-gate records; fixed C# named arguments, optional contract access, and declaration order.'
