param(
    [string]$WorkspaceRoot = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
)

$ErrorActionPreference = 'Stop'
$resolvedRoot = [System.IO.Path]::GetFullPath($WorkspaceRoot)
$allowedRoot = [System.IO.Path]::GetFullPath((Split-Path -Parent (Split-Path -Parent $PSScriptRoot)))
if ($resolvedRoot -ne $allowedRoot) {
    throw "WorkspaceRoot must be the project root: $allowedRoot"
}

$logPath = Join-Path $PSScriptRoot ("fix-phase43-45-acceptance-makeshared-{0}.log" -f (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
Start-Transcript -LiteralPath $logPath | Out-Null

function Set-FileText([string]$Path, [string]$Text) {
    $fullPath = [System.IO.Path]::GetFullPath($Path)
    if (-not $fullPath.StartsWith($resolvedRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to write outside workspace: $fullPath"
    }
    [System.IO.File]::WriteAllText($fullPath, $Text, [System.Text.UTF8Encoding]::new($false))
}

function Replace-Required([string]$Text, [string]$Pattern, [string]$Replacement, [string]$Label) {
    $updated = [System.Text.RegularExpressions.Regex]::Replace($Text, $Pattern, $Replacement)
    if ($updated -ceq $Text) {
        throw "Expected source pattern was not found: $Label"
    }
    return $updated
}

try {
    $gov = Join-Path $resolvedRoot 'CloseToHSL\\Core\Runtime\Governance\Virtualization'
    $phase43HeaderPath = Join-Path $gov 'Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2.h'
    $phase43CppPath = Join-Path $gov 'Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2.cpp'
    $phase45CppPath = Join-Path $gov 'Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2.cpp'

    $header = [System.IO.File]::ReadAllText($phase43HeaderPath)
    $header = Replace-Required $header '(?m)^\s*static inline const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Record = CreateRecord\(\);\r?\n' "`t`tstatic const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Record;`r`n" 'Phase43 static record initializer'
    $header = Replace-Required $header '(?m)^\s*static inline const std::vector<unsigned char> CanonicalBytes = VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance\(getRecord\(\)\);\r?\n' "`t`tstatic const std::vector<unsigned char> CanonicalBytes;`r`n" 'Phase43 canonical byte initializer'
    $header = Replace-Required $header '(?m)^\s*static inline const std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> CodeOwnersEvidence = .*;\r?\n' "`t`tstatic const std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> CodeOwnersEvidence;`r`n" 'Phase43 code owners initializer'
    $header = Replace-Required $header '(?m)(static inline const std::wstring RepositoryPrincipal = L"@yaksysdev";\r?\n)' ('$1' + "`t`tstatic inline const std::wstring ArchitectureReviewEvidenceId = L`"PHASE43-ARCHITECTURE-REVIEW-2026-08-11-46FC02C`";`r`n") 'Phase43 architecture review evidence id'
    $header = $header.Replace('#include "VirtualizationDecisionCanonicalEncoderV2.h"', '#include "VirtualizationDecisionCanonicalEncoderV2.h"' + "`r`n" + '#include "VmReadScalarDeliveryDecisionValidatorV2.h"')
    Set-FileText $phase43HeaderPath $header

    $cpp = [System.IO.File]::ReadAllText($phase43CppPath)
    $cpp = $cpp.Replace('#include "VirtualizationDecisionContractsV2.h"', '#include "VirtualizationDecisionContractsV2.h"' + "`r`n" + '#include <stdexcept>')
    $staticDefinitions = @'
	const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::Record = Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::CreateRecord();
	const std::vector<unsigned char> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::CanonicalBytes = VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::Record);
	const std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::CodeOwnersEvidence = std::make_shared<VirtualizationCodeOwnersEvidenceV2>(VirtualizationCodeOwnersEvidenceV2{true, Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::CodeOwnersBlobSha, {
		Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Governance/Virtualization/"),
		Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/Descriptors/ExecutionDomain/"),
		Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/Compatibility/Frontend/Projection/VmcsRead/"),
		Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/Retire/"),
		Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/ExecutionFlow/"),
		Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/MicroOps/Types/"),
		Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Architecture/Registers/"),
		Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/docs/ref2/VirtualizationActivationPlan/")
	}});
'@
    $cpp = Replace-Required $cpp '(?s)(namespace YAKSys_Hybrid_CPU::Core\s*\{\s*)' ('$1' + $staticDefinitions) 'Phase43 static definitions insertion'
    $cpp = Replace-Required $cpp '(?m)^\s*return GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::Validate\(.*\);\s*$' @'
		std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> spec = Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getInstance();
		std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> record = getRecord();
		std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> evidence = std::make_shared<VirtualizationDecisionValidationEvidenceV2>(Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getCanonicalBytes(), getCanonicalBytes(), Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2::getCanonicalBytes(), SpecCommitSha, acceptanceContainingCommitSha, getCodeOwnersEvidence(), std::vector<std::shared_ptr<VirtualizationNamespaceAllocationV2>>{}, std::vector<std::shared_ptr<VirtualizationDecisionRevocationEvidenceV2>>{}, std::vector<std::shared_ptr<VirtualizationDecisionSupersessionEvidenceV2>>{});
		return GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::Validate(spec, record, evidence);
'@ 'Phase43 validation call with non-const reference temporaries'
    $cpp = $cpp.Replace('std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewRoleV2::OwnerReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, VirtualizationDecisionReviewStateV2::Completed, RepositoryPrincipal, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, L"PHASE43-EXECUTION-DOMAIN-OWNER-REVIEW-2026-08-11-46FC02C")', 'std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewEvidenceV2{VirtualizationDecisionReviewRoleV2::OwnerReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, VirtualizationDecisionReviewStateV2::Completed, RepositoryPrincipal, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, L"PHASE43-EXECUTION-DOMAIN-OWNER-REVIEW-2026-08-11-46FC02C"})')
    $cpp = $cpp.Replace('std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, ownerReview->State, ownerReview->Principal, ownerReview->ReviewedDecisionId, ownerReview->ReviewedSpecDigest, ownerReview->ReviewedSpecCommitSha, ArchitectureReviewEvidenceId)', 'std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewEvidenceV2{VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, ownerReview->State, ownerReview->Principal, ownerReview->ReviewedDecisionId, ownerReview->ReviewedSpecDigest, ownerReview->ReviewedSpecCommitSha, ArchitectureReviewEvidenceId})')
    $cpp = $cpp.Replace('std::make_shared<VirtualizationDecisionAcceptanceRecordV2>(GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::SchemaVersion, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, VirtualizationDecisionAcceptanceStateV2::Accepted, RepositoryPrincipal, 1, ownerReview, architectureReview, CodeOwnersBlobSha, nullptr, nullptr, std::wstring(L"0", 64))', 'std::make_shared<VirtualizationDecisionAcceptanceRecordV2>(VirtualizationDecisionAcceptanceRecordV2{GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::SchemaVersion, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, VirtualizationDecisionAcceptanceStateV2::Accepted, RepositoryPrincipal, 1, ownerReview, architectureReview, CodeOwnersBlobSha, nullptr, nullptr, std::wstring(L"0", 64)})')
    $cpp = $cpp.Replace('std::make_shared<VirtualizationCodeOwnersRuleV2>(scope, RepositoryPrincipal)', 'std::make_shared<VirtualizationCodeOwnersRuleV2>(VirtualizationCodeOwnersRuleV2{scope, RepositoryPrincipal})')
    $cpp = $cpp.Replace('throw InvalidOperationException(L"Phase 43 SpecV2 drifted from its immutable subject commit.");', 'throw std::invalid_argument("Phase 43 SpecV2 drifted from its immutable subject commit.");')
    $cpp = $cpp.Replace('throw InvalidOperationException(std::format("Phase 43 AcceptanceRecordV2 digest is {0:s}.", record->AcceptanceDigest));', 'throw std::invalid_argument(std::format("Phase 43 AcceptanceRecordV2 digest is {0:s}.", record->AcceptanceDigest));')
    Set-FileText $phase43CppPath $cpp

    $phase45 = [System.IO.File]::ReadAllText($phase45CppPath)
    $phase45 = Replace-Required $phase45 '(?m)^\s*std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> evidence = std::make_shared<VirtualizationDecisionValidationEvidenceV2>\(.*\);\s*$' @'
		const auto specBytes = Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2::getCanonicalBytes();
		const auto acceptanceBytes = getCanonicalBytes();
		const auto specBytesAtCommit = specBytes;
		const auto codeOwners = getCodeOwnersEvidence();
		const std::vector<std::shared_ptr<VirtualizationNamespaceAllocationV2>> allocations{};
		const std::vector<std::shared_ptr<VirtualizationDecisionRevocationEvidenceV2>> revocations{};
		const std::vector<std::shared_ptr<VirtualizationDecisionSupersessionEvidenceV2>> supersessions{};
		std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> evidence = std::make_shared<VirtualizationDecisionValidationEvidenceV2>(specBytes, acceptanceBytes, specBytesAtCommit, SpecCommitSha, acceptanceContainingCommitSha, codeOwners, allocations, revocations, supersessions);
'@ 'Phase45 make_shared validation evidence'
    Set-FileText $phase45CppPath $phase45

    # Scan all active C++ headers and sources for the known C# empty-collection
    # argument pattern. Convert only that exact constructor-call pattern.
    $activeCppFiles = Get-ChildItem -LiteralPath (Join-Path $resolvedRoot '2') -Recurse -File -Include *.h,*.hpp,*.cpp,*.cc |
        Where-Object { $_.FullName -notmatch '[\\/]Legacy[\\/]|[\\/]obj[\\/]' }
    foreach ($file in $activeCppFiles) {
        $text = [System.IO.File]::ReadAllText($file.FullName)
        $updated = $text.Replace('getCodeOwnersEvidence(), {}, {}, {}', 'getCodeOwnersEvidence(), std::vector<std::shared_ptr<VirtualizationNamespaceAllocationV2>>{}, std::vector<std::shared_ptr<VirtualizationDecisionRevocationEvidenceV2>>{}, std::vector<std::shared_ptr<VirtualizationDecisionSupersessionEvidenceV2>>{}')
        if ($updated -cne $text) { Set-FileText $file.FullName $updated }
    }

    Write-Output "Updated Phase43 acceptance and Phase45 validation evidence. Scanned $($activeCppFiles.Count) active C++ files."
    Write-Output "Log: $logPath"
}
finally {
    Stop-Transcript | Out-Null
}
