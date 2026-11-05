param([string]$WorkspaceRoot = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)))
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath($WorkspaceRoot)
$expected = [IO.Path]::GetFullPath((Split-Path -Parent (Split-Path -Parent $PSScriptRoot)))
if ($root -ne $expected) { throw "Unexpected workspace root: $root" }
$log = Join-Path $PSScriptRoot ("fix-phase41-acceptance-initialization-{0}.log" -f (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
Start-Transcript -LiteralPath $log | Out-Null
try {
    $dir = Join-Path $root 'CloseToHSL\\Core\Runtime\Governance\Virtualization'
    $headerPath = Join-Path $dir 'Phase41VmReadScalarDeliveryDecisionAcceptanceV2.h'
    $cppPath = Join-Path $dir 'Phase41VmReadScalarDeliveryDecisionAcceptanceV2.cpp'
    $header = [IO.File]::ReadAllText($headerPath)
    $header = [regex]::Replace($header, '(?m)^\s*static inline const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Record = CreateRecord\(\);\r?\n', "`t`tstatic const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Record;`r`n")
    $header = [regex]::Replace($header, '(?m)^\s*static inline const std::vector<unsigned char> CanonicalBytes = VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance\(getRecord\(\)\);\r?\n', "`t`tstatic const std::vector<unsigned char> CanonicalBytes;`r`n")
    $header = [regex]::Replace($header, '(?m)^\s*static inline const std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> CodeOwnersEvidence = .*;\r?\n', "`t`tstatic const std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> CodeOwnersEvidence;`r`n")
    if ($header -match 'static inline const .*Record = CreateRecord|static inline const .*CanonicalBytes =|static inline const .*CodeOwnersEvidence =') { throw 'Phase41 inline initializer remains in header.' }
    [IO.File]::WriteAllText($headerPath, $header, [Text.UTF8Encoding]::new($false))

    $cpp = [IO.File]::ReadAllText($cppPath)
    if ($cpp -notmatch 'Phase41VmReadScalarDeliveryDecisionAcceptanceV2::Record =') {
        $cpp = $cpp.Replace('#include "VirtualizationDecisionContractsV2.h"', '#include "VirtualizationDecisionContractsV2.h"' + "`r`n" + '#include <stdexcept>')
        $defs = @'
	const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2> Phase41VmReadScalarDeliveryDecisionAcceptanceV2::Record = Phase41VmReadScalarDeliveryDecisionAcceptanceV2::CreateRecord();
	const std::vector<unsigned char> Phase41VmReadScalarDeliveryDecisionAcceptanceV2::CanonicalBytes = VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(Phase41VmReadScalarDeliveryDecisionAcceptanceV2::Record);
	const std::shared_ptr<VirtualizationCodeOwnersEvidenceV2> Phase41VmReadScalarDeliveryDecisionAcceptanceV2::CodeOwnersEvidence = std::make_shared<VirtualizationCodeOwnersEvidenceV2>(VirtualizationCodeOwnersEvidenceV2{true, Phase41VmReadScalarDeliveryDecisionAcceptanceV2::CodeOwnersBlobSha, {
		Phase41VmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Governance/Virtualization/"),
		Phase41VmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/SecureCompute/Descriptors/ExecutionState/"),
		Phase41VmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Runtime/Domains/SecureCompute/Policies/ExecutionState/"),
		Phase41VmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/SecureCompute/Compatibility/Projection/"),
		Phase41VmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Virtualization/Compatibility/Frontend/Projection/VmcsRead/"),
		Phase41VmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/Retire/"),
		Phase41VmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/ExecutionFlow/"),
		Phase41VmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Pipeline/MicroOps/Types/"),
		Phase41VmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/CloseToHSL/Core/Architecture/Registers/"),
		Phase41VmReadScalarDeliveryDecisionAcceptanceV2::Rule(L"/HybridCPU_ISE/docs/ref2/VirtualizationActivationPlan/")
	}});

'@
        $cpp = [regex]::Replace($cpp, '(?s)(namespace YAKSys_Hybrid_CPU::Core\s*\{\s*)', ('$1' + $defs), 1)
    }
    $cpp = [regex]::Replace($cpp, '(?m)^\s*return VmReadScalarDeliveryDecisionValidatorV2::Validate\(.*\);\s*$', @'
		std::optional<std::shared_ptr<VirtualizationDecisionSpecV2>> spec = Phase41VmReadScalarDeliveryDecisionSpecV2::getInstance();
		std::optional<std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>> record = getRecord();
		const auto specBytes = Phase41VmReadScalarDeliveryDecisionSpecV2::getCanonicalBytes();
		const auto acceptanceBytes = getCanonicalBytes();
		const auto codeOwners = getCodeOwnersEvidence();
		const std::vector<std::shared_ptr<VirtualizationNamespaceAllocationV2>> allocations{};
		const std::vector<std::shared_ptr<VirtualizationDecisionRevocationEvidenceV2>> revocations{};
		const std::vector<std::shared_ptr<VirtualizationDecisionSupersessionEvidenceV2>> supersessions{};
		std::optional<std::shared_ptr<VirtualizationDecisionValidationEvidenceV2>> evidence = std::make_shared<VirtualizationDecisionValidationEvidenceV2>(specBytes, acceptanceBytes, specBytes, SpecCommitSha, acceptanceContainingCommitSha, codeOwners, allocations, revocations, supersessions);
		return VmReadScalarDeliveryDecisionValidatorV2::Validate(spec, record, evidence);
'@, 1)
    $cpp = $cpp.Replace('std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewRoleV2::OwnerReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, VirtualizationDecisionReviewStateV2::Completed, RepositoryPrincipal, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, OwnerReviewEvidenceId)', 'std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewEvidenceV2{VirtualizationDecisionReviewRoleV2::OwnerReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::NeutralRuntimeOwner, VirtualizationDecisionReviewStateV2::Completed, RepositoryPrincipal, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, OwnerReviewEvidenceId})')
    $cpp = $cpp.Replace('std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, ownerReview->State, ownerReview->Principal, ownerReview->ReviewedDecisionId, ownerReview->ReviewedSpecDigest, ownerReview->ReviewedSpecCommitSha, ArchitectureReviewEvidenceId)', 'std::make_shared<VirtualizationDecisionReviewEvidenceV2>(VirtualizationDecisionReviewEvidenceV2{VirtualizationDecisionReviewRoleV2::ArchitectureReviewRole, VirtualizationDecisionReviewAuthorityPlaneV2::Architecture, ownerReview->State, ownerReview->Principal, ownerReview->ReviewedDecisionId, ownerReview->ReviewedSpecDigest, ownerReview->ReviewedSpecCommitSha, ArchitectureReviewEvidenceId})')
    $cpp = $cpp.Replace('std::make_shared<VirtualizationDecisionAcceptanceRecordV2>(VmReadScalarDeliveryDecisionValidatorV2::SchemaVersion, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, VirtualizationDecisionAcceptanceStateV2::Accepted, RepositoryPrincipal, 1, ownerReview, architectureReview, CodeOwnersBlobSha, nullptr, nullptr, std::wstring(L"0", 64))', 'std::make_shared<VirtualizationDecisionAcceptanceRecordV2>(VirtualizationDecisionAcceptanceRecordV2{VmReadScalarDeliveryDecisionValidatorV2::SchemaVersion, spec->DecisionId, ExpectedSpecDigest, SpecCommitSha, VirtualizationDecisionAcceptanceStateV2::Accepted, RepositoryPrincipal, 1, ownerReview, architectureReview, CodeOwnersBlobSha, nullptr, nullptr, std::wstring(L"0", 64)})')
    $cpp = $cpp.Replace('std::make_shared<VirtualizationCodeOwnersRuleV2>(scope, RepositoryPrincipal)', 'std::make_shared<VirtualizationCodeOwnersRuleV2>(VirtualizationCodeOwnersRuleV2{scope, RepositoryPrincipal})')
    $cpp = $cpp.Replace('throw InvalidOperationException(L"The scalar-delivery SpecV2 digest drifted from the earlier commit.");', 'throw std::invalid_argument("The scalar-delivery SpecV2 digest drifted from the earlier commit.");')
    $cpp = $cpp.Replace('throw InvalidOperationException(L"The scalar-delivery AcceptanceRecordV2 canonical digest drifted.");', 'throw std::invalid_argument("The scalar-delivery AcceptanceRecordV2 canonical digest drifted.");')
    [IO.File]::WriteAllText($cppPath, $cpp, [Text.UTF8Encoding]::new($false))
    Write-Output 'Updated Phase41 acceptance initialization and validation arguments.'
    Write-Output "Log: $log"
} finally { Stop-Transcript | Out-Null }
