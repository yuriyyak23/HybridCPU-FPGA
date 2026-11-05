$ErrorActionPreference = 'Stop'
$root = 'C:\Users\Yuriy\Desktop\HybridCPU_FPGA'
$changes = [System.Collections.Generic.List[string]]::new()
function Replace-Once([string]$relativePath, [string]$old, [string]$new) {
  $path = Join-Path $root $relativePath
  $content = [IO.File]::ReadAllText($path)
  $old = [regex]::Replace($old, "\r?\n", "`r`n").Replace('\t', [string][char]9)
  $new = [regex]::Replace($new, "\r?\n", "`r`n").Replace('\t', [string][char]9)
  $old = $old.Replace('`r`n', "`r`n")
  $new = $new.Replace('`r`n', "`r`n")
  $old = $old.Replace('`t', [string][char]9)
  $new = $new.Replace('`t', [string][char]9)
  $canonicalNew = [regex]::Replace($new, '(?m)^#include ([.][^\r\n"<]+)$', '#include "$1"')
  if (($new.Length -gt 0 -and $content.Contains($new)) -or ($canonicalNew.Length -gt 0 -and $content.Contains($canonicalNew))) { $changes.Add("already applied: $relativePath"); return }
  if ($new.Length -eq 0 -and -not $content.Contains($old)) { $changes.Add("already removed: $relativePath"); return }
  if (-not $content.Contains($old)) { throw "Expected source text not found in $relativePath; old fragment: $old" }
  [IO.File]::WriteAllText($path, $content.Replace($old, $new), [Text.UTF8Encoding]::new($false))
  $changes.Add("updated: $relativePath")
}
function Remove-Text([string]$relativePath, [string]$text) {
  $path = Join-Path $root $relativePath
  $content = [IO.File]::ReadAllText($path)
  $text = [regex]::Replace($text, "\r?\n", "`r`n").Replace('`t', [string][char]9)
  if (-not $content.Contains($text)) { return }
  [IO.File]::WriteAllText($path, $content.Replace($text, ''), [Text.UTF8Encoding]::new($false))
  $changes.Add("removed text from $relativePath")
}

Replace-Once 'tangible_\stringbuilder.h' "#include <memory>" "#include <memory>`r`n#include <codecvt>`r`n#include <locale>"
$oldAppendLine = @'
\tstd::shared_ptr<StringBuilder> appendLine(const std::wstring &toAppend)
\t{
\t\tprivateString += toAppend + L"\r\n";
\t\treturn shared_from_this();
\t}
'@
$newAppendLine = @'
\tstd::shared_ptr<StringBuilder> appendLine(const std::wstring &toAppend)
\t{
\t\tprivateString += toAppend + L"\r\n";
\t\treturn shared_from_this();
\t}

\tstd::shared_ptr<StringBuilder> appendLine(const std::string &toAppend)
\t{
\t\tprivateString += std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(toAppend) + L"\r\n";
\t\treturn shared_from_this();
\t}
'@
Replace-Once 'tangible_\stringbuilder.h' $oldAppendLine $newAppendLine
Replace-Once 'NonRTL\\Processor\Performance\YAKSys_Hybrid_CPU.PerformanceReport.cpp' '#include "YAKSys_Hybrid_CPU.PerformanceReport.h"' "#include `"YAKSys_Hybrid_CPU.PerformanceReport.h`"`r`n#include `"../../../tangible_exception_helper.h`"`r`n#include <algorithm>`r`n#include <iomanip>`r`n#include <sstream>"
Replace-Once 'NonRTL\\Processor\Performance\YAKSys_Hybrid_CPU.PerformanceReport.cpp' 'std::vector<std::shared_ptr<BurstTrace>> PerformanceReport::getBurstTraces() const' 'std::vector<std::shared_ptr<PerformanceReport::BurstTrace>> PerformanceReport::getBurstTraces() const'
Replace-Once 'NonRTL\\Processor\Performance\YAKSys_Hybrid_CPU.PerformanceReport.cpp' "return getBurstTraces() is {Count: > 0};" 'return !getBurstTraces().empty();'
Replace-Once 'NonRTL\\Processor\Performance\YAKSys_Hybrid_CPU.PerformanceReport.cpp' 'throw InvalidOperationException(std::format("Cumulative diagnostic metric ''{0}'' regressed from {1} to {2}; ", metricName, baseline, current) + L"an exact interval cannot be reported because this compatibility field has no availability carrier.");' 'throw InvalidOperationException(metricName + L" cumulative diagnostic metric regressed from " + std::to_wstring(baseline) + L" to " + std::to_wstring(current) + L"; an exact interval cannot be reported because this compatibility field has no availability carrier.");'
Replace-Once 'NonRTL\\Processor\Performance\YAKSys_Hybrid_CPU.PerformanceReport.cpp' 'std::static_pointer_cast<PerformanceReport>(MemberwiseClone())' 'std::make_shared<PerformanceReport>(*this)'
Replace-Once 'NonRTL\\Processor\Performance\YAKSys_Hybrid_CPU.PerformanceReport.cpp' 'std::wstring::Equals(getMemoryCycleTelemetrySchemaVersion(), baseline->getMemoryCycleTelemetrySchemaVersion(), StringComparison::Ordinal)' '(getMemoryCycleTelemetrySchemaVersion() == baseline->getMemoryCycleTelemetrySchemaVersion())'
Replace-Once 'NonRTL\\Processor\Performance\YAKSys_Hybrid_CPU.PerformanceReport.cpp' '(getOracleCounterexampleDominantCategory() != L"") ? getOracleCounterexampleDominantCategory() : "none"' '(getOracleCounterexampleDominantCategory() != L"") ? getOracleCounterexampleDominantCategory() : L"none"'
Replace-Once 'NonRTL\\Processor\Performance\YAKSys_Hybrid_CPU.PerformanceReport.cpp' 'return std::format("{0:.2F} {1}", len, sizes[order]);' '`t`tstd::wostringstream formatted;`r`n`t`tformatted << std::fixed << std::setprecision(2) << len << L" " << sizes[order];`r`n`t`treturn formatted.str();'
$performancePath = Join-Path $root 'NonRTL\\Processor\Performance\YAKSys_Hybrid_CPU.PerformanceReport.cpp'
$performanceText = [IO.File]::ReadAllText($performancePath)
$phase2Start = $performanceText.IndexOf('bool PerformanceReport::getHasPhase2PolicyTelemetry() const')
$phase2End = $performanceText.IndexOf("`r`n`t}`r`n", $phase2Start)
if ($phase2Start -lt 0 -or $phase2End -lt 0) { throw 'Could not locate getHasPhase2PolicyTelemetry method.' }
$phase2Method = @'
bool PerformanceReport::getHasPhase2PolicyTelemetry() const
{
	const auto perVtInjections = getPerVtInjections();
	return getFairnessStarvationEvents() > 0 || getBankPressureAvoidanceCount() > 0 || getSpeculationBudgetExhaustionEvents() > 0 || getPeakConcurrentSpeculativeOps() > 0 || std::any_of(perVtInjections.begin(), perVtInjections.end(), [](long long value) { return value > 0; });
'@
$phase2Method = [regex]::Replace($phase2Method, "\r?\n", "`r`n").Replace('\t', [string][char]9)
$performanceText = $performanceText.Substring(0, $phase2Start) + $phase2Method + $performanceText.Substring($phase2End)
[IO.File]::WriteAllText($performancePath, $performanceText, [Text.UTF8Encoding]::new($false))
$changes.Add('updated: PerformanceReport::getHasPhase2PolicyTelemetry')

function Add-Model([string]$relativePath, [string]$model) {
  $path = Join-Path $root $relativePath
  $content = [IO.File]::ReadAllText($path)
  $typeName = [regex]::Match($model, '(?:struct|class)\s+(\w+)').Groups[1].Value
  if ([regex]::IsMatch($content, "(?m)^\s*(?:struct|class)\s+$([regex]::Escape($typeName))\b")) { $changes.Add("already applied: $typeName"); return }
  $model = [regex]::Replace($model, "\r?\n", "`r`n").Replace('\t', [string][char]9)
  $close = $content.LastIndexOf('}')
  if ($close -lt 0) { throw "Could not find namespace close in $relativePath" }
  $content = $content.Substring(0, $close) + $model + "`r`n" + $content.Substring($close)
  [IO.File]::WriteAllText($path, $content, [Text.UTF8Encoding]::new($false))
  $changes.Add("added: $typeName in $relativePath")
}

# Restore the C# record models as explicit C++ value/request types.
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\LaneBinding\VirtualizationLaneBindingPolicy.h' '#pragma once' "#pragma once`r`n#include ../../../Pipeline/Scheduling/SlotPlacementMetadata.h"
Add-Model 'CloseToHSL\\Core\Virtualization\CompilerBoundary\LaneBinding\VirtualizationLaneBindingPolicy.h' @'
struct VirtualizationLaneBindingRequest final
{
    SlotPlacementMetadata Placement{};
    bool DescriptorValidated = false;
    bool IsCompatibilityFrontend = false;
    bool IsLaneResourceOwnedByDescriptor = false;
    VirtualizationLaneBindingRequest(SlotPlacementMetadata placement, bool descriptorValidated, bool compatibilityFrontend, bool descriptorOwnsLane)
        : Placement(placement), DescriptorValidated(descriptorValidated), IsCompatibilityFrontend(compatibilityFrontend), IsLaneResourceOwnedByDescriptor(descriptorOwnsLane) {}
};
'@
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Scheduling\VirtualizationSystemSingletonSchedulingRule.h' '#pragma once' "#pragma once`r`n#include ../../../Pipeline/Scheduling/SlotPlacementMetadata.h"
Add-Model 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Scheduling\VirtualizationSystemSingletonSchedulingRule.h' @'
struct VirtualizationSystemSingletonSchedulingRequest final
{
    SlotPlacementMetadata Placement{};
    bool DescriptorValidated = false;
    unsigned char SystemSingletonCount = 0;
    VirtualizationSystemSingletonSchedulingRequest(SlotPlacementMetadata placement, bool descriptorValidated, unsigned char singletonCount)
        : Placement(placement), DescriptorValidated(descriptorValidated), SystemSingletonCount(singletonCount) {}
};
'@
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Bundling\VirtualizationBundleLegalityRule.h' '#pragma once' "#pragma once`r`n#include <memory>`r`n#include <string>`r`n#include <utility>`r`n#include ../../../Runtime/Domains/Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.BundleLegalityDescriptor.h`r`n#include ../LaneBinding/VirtualizationLaneBindingPolicy.h`r`n#include ../Scheduling/VirtualizationSystemSingletonSchedulingRule.h"
Add-Model 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Bundling\VirtualizationBundleLegalityRule.h' @'
struct VirtualizationBundleLegalityRequest final
{
    std::shared_ptr<BundleLegalityDescriptor> Descriptor;
    VirtualizationLaneBindingDecision LaneBindingDecision{};
    VirtualizationSchedulingDecision SchedulingDecision{};
    bool DescriptorValidated = false;
    bool RuntimeValidated = false;
    bool UsesCompilerEvidenceAsAuthority = false;
    bool RequiresCompatibilityProjection = false;
};
struct VirtualizationBundleLegalityResult final
{
    VirtualizationBundleLegalityDecision Decision{};
    std::wstring Reason;
    bool IsAllowed = false;
    VirtualizationBundleLegalityResult(VirtualizationBundleLegalityDecision decision, std::wstring reason)
        : Decision(decision), Reason(std::move(reason)), IsAllowed(decision == VirtualizationBundleLegalityDecision::Allowed) {}
    static std::shared_ptr<VirtualizationBundleLegalityResult> Allowed() { return std::make_shared<VirtualizationBundleLegalityResult>(VirtualizationBundleLegalityDecision::Allowed, L"Bundle legality is runtime-authorized and validated."); }
    static std::shared_ptr<VirtualizationBundleLegalityResult> Denied(VirtualizationBundleLegalityDecision decision, std::wstring reason) { return std::make_shared<VirtualizationBundleLegalityResult>(decision, std::move(reason)); }
};
'@
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\IR\VirtualizationCompilerIntent.h' '#pragma once' "#pragma once`r`n#include <memory>`r`n#include <string>`r`n#include <utility>"
Add-Model 'CloseToHSL\\Core\Virtualization\CompilerBoundary\IR\VirtualizationCompilerIntent.h' @'
struct VirtualizationCompilerIntentRequest final
{
    VirtualizationCompilerIntentKind IntentKind{};
    bool IsCompatibilityFrontend = false;
    bool DescriptorValidated = false;
    bool EmitsHostOwnedEvidence = false;
    bool EmitsNativeLaneToken = false;
    bool AttemptsDirectSubstrateMutation = false;
};
struct VirtualizationCompilerIntentResult final
{
    VirtualizationCompilerIntentDecision Decision{};
    std::wstring Reason;
    bool IsAllowed = false;
    VirtualizationCompilerIntentResult(VirtualizationCompilerIntentDecision decision, std::wstring reason)
        : Decision(decision), Reason(std::move(reason)), IsAllowed(decision == VirtualizationCompilerIntentDecision::Allowed) {}
    static std::shared_ptr<VirtualizationCompilerIntentResult> Allowed() { return std::make_shared<VirtualizationCompilerIntentResult>(VirtualizationCompilerIntentDecision::Allowed, L""); }
    static std::shared_ptr<VirtualizationCompilerIntentResult> Denied(VirtualizationCompilerIntentDecision decision, std::wstring reason) { return std::make_shared<VirtualizationCompilerIntentResult>(decision, std::move(reason)); }
};
'@
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\NoEmission\VirtualizationNoEmissionRegressionGate.h' '#pragma once' "#pragma once`r`n#include <memory>"
Add-Model 'CloseToHSL\\Core\Virtualization\CompilerBoundary\NoEmission\VirtualizationNoEmissionRegressionGate.h' @'
struct VirtualizationEmissionRequest final
{
    bool IsCompatibilityFrontend = false;
    bool IsGeneratedProjection = false;
    bool DescriptorValidated = false;
    bool EmitsHostOwnedEvidence = false;
    bool EmitsNativeLaneToken = false;
};
'@
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Lowering\VirtualizationLoweringBoundary.h' '#pragma once' "#pragma once`r`n#include <memory>`r`n#include <string>`r`n#include <utility>`r`n#include ../IR/VirtualizationCompilerIntent.h`r`n#include ../NoEmission/VirtualizationNoEmissionRegressionGate.h"
Add-Model 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Lowering\VirtualizationLoweringBoundary.h' @'
struct VirtualizationLoweringRequest final
{
    std::shared_ptr<VirtualizationCompilerIntentResult> IntentResult;
    VirtualizationEmissionDecision EmissionDecision{};
    bool DescriptorValidated = false;
    bool CapabilityValidated = false;
    bool RuntimeValidated = false;
    bool EmitsDirectVmxHandler = false;
};
struct VirtualizationLoweringResult final
{
    VirtualizationLoweringDecision Decision{};
    std::wstring Reason;
    bool IsAllowed = false;
    VirtualizationLoweringResult(VirtualizationLoweringDecision decision, std::wstring reason)
        : Decision(decision), Reason(std::move(reason)), IsAllowed(decision == VirtualizationLoweringDecision::Allowed) {}
    static std::shared_ptr<VirtualizationLoweringResult> Allowed() { return std::make_shared<VirtualizationLoweringResult>(VirtualizationLoweringDecision::Allowed, L""); }
    static std::shared_ptr<VirtualizationLoweringResult> Denied(VirtualizationLoweringDecision decision, std::wstring reason) { return std::make_shared<VirtualizationLoweringResult>(decision, std::move(reason)); }
};
'@
Replace-Once 'CloseToHSL\\Core\Virtualization\Conformance\NoEmission\VirtualizationNoEmissionContract.h' '#pragma once' "#pragma once`r`n#include <string>`r`n#include <utility>`r`n#include ../../CompilerBoundary/NoEmission/VirtualizationNoEmissionRegressionGate.h"
Add-Model 'CloseToHSL\\Core\Virtualization\Conformance\NoEmission\VirtualizationNoEmissionContract.h' @'
struct VirtualizationNoEmissionContractRequest final
{
    VirtualizationEmissionRequest Emission{};
    bool GoldenArtifactCaptured = false;
};
struct VirtualizationNoEmissionContractResult final
{
    VirtualizationNoEmissionContractDecision Decision{};
    std::wstring Reason;
    bool IsAllowed = false;
    VirtualizationNoEmissionContractResult(VirtualizationNoEmissionContractDecision decision, std::wstring reason)
        : Decision(decision), Reason(std::move(reason)), IsAllowed(decision == VirtualizationNoEmissionContractDecision::Allowed) {}
    static std::shared_ptr<VirtualizationNoEmissionContractResult> Denied(VirtualizationNoEmissionContractDecision decision, std::wstring reason) { return std::make_shared<VirtualizationNoEmissionContractResult>(decision, std::move(reason)); }
};
'@
Replace-Once 'CloseToHSL\\Core\Virtualization\Conformance\HostEvidenceNonLeak\VmxFrontendExposureConformanceContract.h' '#pragma once' "#pragma once`r`n#include <memory>`r`n#include ../../../Runtime/Migration/Format/MigrationDescriptor.h"
Add-Model 'CloseToHSL\\Core\Virtualization\Conformance\HostEvidenceNonLeak\VmxFrontendExposureConformanceContract.h' @'
struct VmxFrontendExposureRequest final
{
    EvidenceVisibilityClass EvidenceClass{};
    MigrationPayloadClass PayloadClass{};
    bool IsGuestVisible = false;
    bool ExposesNativeToken = false;
    bool ExposesBackendHandle = false;
    bool ExposesSchedulerEvidence = false;
    bool ExposesMigrationOnlyState = false;
    bool ExposesLanePrivateState = false;
    bool IsGeneratedProjection = false;
    bool HasAccessPolicy = false;
};
'@
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Handlers\VmxCompatFrontend.h' '#pragma once' "#pragma once`r`n#include <memory>`r`n#include <string>`r`n#include <utility>`r`n#include VmxCompatFrontend.h"
Add-Model 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Handlers\VmxCompatFrontend.h' @'
struct VmxCompatFrontendAdmissionRequest final
{
    bool DecodeBoundaryValidated = false;
    bool ProjectionBoundaryValidated = false;
    bool RetireBoundaryValidated = false;
    bool DescriptorValidated = false;
    bool CapabilityValidated = false;
    bool RuntimeAuthorityValidated = false;
    bool AttemptsDirectVmcsAuthority = false;
};
struct VmxCompatFrontendAdmissionResult final
{
    VmxCompatFrontendAdmissionDecision Decision{};
    std::wstring Reason;
    bool IsAllowed = false;
    VmxCompatFrontendAdmissionResult(VmxCompatFrontendAdmissionDecision decision, std::wstring reason)
        : Decision(decision), Reason(std::move(reason)), IsAllowed(decision == VmxCompatFrontendAdmissionDecision::Allowed) {}
    static std::shared_ptr<VmxCompatFrontendAdmissionResult> Allowed() { return std::make_shared<VmxCompatFrontendAdmissionResult>(VmxCompatFrontendAdmissionDecision::Allowed, L"VMX compatibility frontend admission allowed."); }
    static std::shared_ptr<VmxCompatFrontendAdmissionResult> Denied(VmxCompatFrontendAdmissionDecision decision, std::wstring reason) { return std::make_shared<VmxCompatFrontendAdmissionResult>(decision, std::move(reason)); }
};
'@
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Retire\VmxCompatRetireBoundary.h' '#pragma once' "#pragma once`r`n#include <memory>`r`n#include <string>`r`n#include <utility>`r`n#include VmxRetireModel.h"
Add-Model 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Retire\VmxCompatRetireBoundary.h' @'
struct VmxCompatRetireResult final
{
    VmxCompatRetireDecision Decision{};
    std::wstring Reason;
    bool IsAllowed = false;
    VmxCompatRetireResult(VmxCompatRetireDecision decision, std::wstring reason)
        : Decision(decision), Reason(std::move(reason)), IsAllowed(decision == VmxCompatRetireDecision::Allowed) {}
    static std::shared_ptr<VmxCompatRetireResult> Allowed() { return std::make_shared<VmxCompatRetireResult>(VmxCompatRetireDecision::Allowed, L""); }
    static std::shared_ptr<VmxCompatRetireResult> Denied(VmxCompatRetireDecision decision, std::wstring reason) { return std::make_shared<VmxCompatRetireResult>(decision, std::move(reason)); }
};
struct VmxCompatRetireRequest final
{
    VmxRetireEffect RetireEffect{};
    bool DescriptorValidated = false;
    bool EvidenceValidated = false;
    bool CompletionRouteValidated = false;
};
'@
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\Generated\VmcsProjection\VmcsV2DescriptorProjection.h' '#pragma once' "#pragma once`r`n#include <memory>`r`n#include <string>`r`n#include <utility>`r`n#include VmcsV2DescriptorProjection.h"
Add-Model 'CloseToHSL\\Core\Virtualization\Compatibility\Generated\VmcsProjection\VmcsV2DescriptorProjection.h' @'
struct VmcsV2DescriptorProjectionRequest final
{
    bool GeneratedAliasMapDeclared = false;
    bool DescriptorValidated = false;
    bool EvidenceValidated = false;
    bool ReadOnlyProjection = false;
    bool AttemptsAuthoritativeMutation = false;
};
struct VmcsV2DescriptorProjectionResult final
{
    VmcsV2DescriptorProjectionDecision Decision{};
    unsigned int RevisionId = 0;
    unsigned int SizeBytes = 0;
    std::wstring Baseline;
    std::wstring Reason;
    bool IsAllowed = false;
    VmcsV2DescriptorProjectionResult(VmcsV2DescriptorProjectionDecision decision, unsigned int revision, unsigned int size, std::wstring baseline, std::wstring reason)
        : Decision(decision), RevisionId(revision), SizeBytes(size), Baseline(std::move(baseline)), Reason(std::move(reason)), IsAllowed(decision == VmcsV2DescriptorProjectionDecision::Allowed) {}
    static std::shared_ptr<VmcsV2DescriptorProjectionResult> Allowed() { return std::make_shared<VmcsV2DescriptorProjectionResult>(VmcsV2DescriptorProjectionDecision::Allowed, VmcsV2Header::CurrentRevisionId, VmcsV2Header::CurrentSizeBytes, VmcsV2Header::CompatibilityBaseline, L""); }
    static std::shared_ptr<VmcsV2DescriptorProjectionResult> Denied(VmcsV2DescriptorProjectionDecision decision, std::wstring reason) { return std::make_shared<VmcsV2DescriptorProjectionResult>(decision, 0, 0, L"", std::move(reason)); }
};
'@
Replace-Once 'CloseToHSL\\Core\Virtualization\Conformance\GoldenArtifacts\VirtualizationGoldenArtifactManifest.h' '#pragma once' "#pragma once`r`n#include <string>`r`n#include <memory>`r`n#include <utility>`r`n#include <span>"
Add-Model 'CloseToHSL\\Core\Virtualization\Conformance\GoldenArtifacts\VirtualizationGoldenArtifactManifest.h' @'
struct VirtualizationGoldenArtifact final
{
    VirtualizationGoldenArtifactKind Kind{};
    std::wstring Name;
    bool IsHardArchitecturalGate = false;
    bool RequiresGeneratedSource = false;
    VirtualizationGoldenArtifact(VirtualizationGoldenArtifactKind kind, std::wstring name, bool hardGate, bool generatedSource)
        : Kind(kind), Name(std::move(name)), IsHardArchitecturalGate(hardGate), RequiresGeneratedSource(generatedSource) {}
};
'@
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\FrozenAbi\CsrAliases\VmxCsrAliasSet.h' '#pragma once' "#pragma once`r`n#include <string>`r`n#include <utility>`r`n#include <span>`r`n#include ../../../../../../Arch/CsrAddresses.h"
Add-Model 'CloseToHSL\\Core\Virtualization\Compatibility\FrozenAbi\CsrAliases\VmxCsrAliasSet.h' @'
struct VmxCsrAlias final
{
    unsigned short Address = 0;
    VmxCsrAliasKind Kind{};
    std::wstring Name;
    bool IsReadOnlyProjection = false;
    VmxCsrAlias(unsigned short address, VmxCsrAliasKind kind, std::wstring name, bool readOnly)
        : Address(address), Kind(kind), Name(std::move(name)), IsReadOnlyProjection(readOnly) {}
};
'@
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\FrozenAbi\Opcodes\VmxOpcodeAliasSet.h' '#pragma once' "#pragma once`r`n#include <string>`r`n#include <utility>`r`n#include <span>"
Add-Model 'CloseToHSL\\Core\Virtualization\Compatibility\FrozenAbi\Opcodes\VmxOpcodeAliasSet.h' @'
struct VmxOpcodeAlias final
{
    unsigned short Opcode = 0;
    VmxOpcodeAliasKind Kind{};
    std::wstring Name;
    VmxOpcodeAlias(unsigned short opcode, VmxOpcodeAliasKind kind, std::wstring name)
        : Opcode(opcode), Kind(kind), Name(std::move(name)) {}
};
'@
Add-Model 'CloseToHSL\\Core\Runtime\Nested\Validation\NestedValidationResult.h' @'
struct NestedValidationResult final
{
    bool Succeeded = false;
    NestedValidationCode Code = NestedValidationCode::Success;
    std::wstring Message;
    NestedValidationResult(bool succeeded, NestedValidationCode code, std::wstring message)
        : Succeeded(succeeded), Code(code), Message(std::move(message)) {}
    static std::shared_ptr<NestedValidationResult> Success() { return std::make_shared<NestedValidationResult>(true, NestedValidationCode::Success, L""); }
    static std::shared_ptr<NestedValidationResult> Fail(NestedValidationCode code, std::wstring message) { return std::make_shared<NestedValidationResult>(false, code, std::move(message)); }
};
'@
Replace-Once 'CloseToHSL\\Core\Runtime\Nested\Validation\NestedValidationResult.h' '#pragma once' "#pragma once`r`n#include <memory>`r`n#include <string>`r`n#include <utility>"
Add-Model 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Projection\VmxFrontendResultMapper.h' @'
struct VmxFrontendResult final
{
    VmxFrontendResultKind Kind{};
    VmxCompletionKind CompletionKind{};
    VmFailCode FailCode{};
    VmAbortCode AbortCode{};
    std::wstring Reason;
    bool Succeeded = false;
    VmxFrontendResult(VmxFrontendResultKind kind, VmxCompletionKind completion, VmFailCode fail, VmAbortCode abort, std::wstring reason)
        : Kind(kind), CompletionKind(completion), FailCode(fail), AbortCode(abort), Reason(std::move(reason)), Succeeded(kind == VmxFrontendResultKind::Success) {}
    static VmxFrontendResult Success() { return { VmxFrontendResultKind::Success, VmxCompletionKind::Success, VmFailCode::None, VmAbortCode::None, L"" }; }
    static VmxFrontendResult FailValid(VmFailCode code, std::wstring reason) { return { VmxFrontendResultKind::VmFailValid, VmxCompletionKind::VmFailValid, code, VmAbortCode::None, std::move(reason) }; }
    static VmxFrontendResult FailInvalid(VmFailCode code, std::wstring reason) { return { VmxFrontendResultKind::VmFailInvalid, VmxCompletionKind::VmFailInvalid, code, VmAbortCode::None, std::move(reason) }; }
};
'@

function Fix-Include([string]$relativePath, [string]$includePath) {
  $path = Join-Path $root $relativePath
  $content = [IO.File]::ReadAllText($path)
  $bad = '#include ' + $includePath
  $good = '#include ' + [char]34 + $includePath + [char]34
  if ($content.Contains($good)) {
    if ($content.Contains($bad)) {
      $content = $content.Replace($bad + "`r`n", '').Replace($bad, '')
      [IO.File]::WriteAllText($path, $content, [Text.UTF8Encoding]::new($false))
      $changes.Add("removed duplicate unquoted include $includePath from $relativePath")
    }
    return
  }
  if ($content.Contains($bad)) {
    $content = $content.Replace($bad, $good)
  } else {
    $pragma = '#pragma once'
    if (-not $content.Contains($pragma)) { throw "Expected include $includePath missing in $relativePath" }
    $content = $content.Replace($pragma, $pragma + "`r`n" + $good)
  }
  [IO.File]::WriteAllText($path, $content, [Text.UTF8Encoding]::new($false))
  $changes.Add("quoted include $includePath in $relativePath")
}
Fix-Include 'CloseToHSL\\Core\Virtualization\CompilerBoundary\LaneBinding\VirtualizationLaneBindingPolicy.h' '../../../Pipeline/Scheduling/SlotPlacementMetadata.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Scheduling\VirtualizationSystemSingletonSchedulingRule.h' '../../../Pipeline/Scheduling/SlotPlacementMetadata.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Bundling\VirtualizationBundleLegalityRule.h' '../../../Runtime/Domains/Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.BundleLegalityDescriptor.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Bundling\VirtualizationBundleLegalityRule.h' '../LaneBinding/VirtualizationLaneBindingPolicy.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Bundling\VirtualizationBundleLegalityRule.h' '../Scheduling/VirtualizationSystemSingletonSchedulingRule.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Lowering\VirtualizationLoweringBoundary.h' '../IR/VirtualizationCompilerIntent.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Lowering\VirtualizationLoweringBoundary.h' '../NoEmission/VirtualizationNoEmissionRegressionGate.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\Conformance\NoEmission\VirtualizationNoEmissionContract.h' '../../CompilerBoundary/NoEmission/VirtualizationNoEmissionRegressionGate.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\Conformance\HostEvidenceNonLeak\VmxFrontendExposureConformanceContract.h' '../../../Runtime/Migration/Format/MigrationDescriptor.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\Conformance\HostEvidenceNonLeak\VmxFrontendExposureConformanceContract.h' '../../../Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Handlers\VmxCompatFrontend.h' 'VmxCompatFrontend.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Retire\VmxCompatRetireBoundary.h' 'VmxRetireModel.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\Compatibility\Generated\VmcsProjection\VmcsV2DescriptorProjection.h' 'VmcsV2DescriptorProjection.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\Compatibility\Generated\VmcsProjection\VmcsV2DescriptorProjection.h' 'VmcsV2Header.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\Compatibility\FrozenAbi\CsrAliases\VmxCsrAliasSet.h' '../../../../../../Arch/CsrAddresses.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Projection\VmxFrontendResultMapper.h' '../../../../Runtime/Nested/Validation/NestedValidationResult.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Projection\VmxFrontendResultMapper.h' 'VmxCompletion.h'
Fix-Include 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Projection\VmxFrontendResultMapper.h' '../Retire/VmxRetireModel.h'
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Projection\VmxFrontendResultMapper.h' '#pragma once' "#pragma once`r`n#include <string>`r`n#include <utility>"

function Add-ClassHeaderInclude([string]$relativePath, [string]$includePath) {
  $path = Join-Path $root $relativePath
  $content = [IO.File]::ReadAllText($path)
  $line = '#include ' + [char]34 + $includePath + [char]34
  if ($content.Contains($line)) { return }
  $pragma = '#pragma once'
  if (-not $content.Contains($pragma)) { throw "#pragma once missing from $relativePath" }
  [IO.File]::WriteAllText($path, $content.Replace($pragma, $pragma + "`r`n" + $line), [Text.UTF8Encoding]::new($false))
  $changes.Add("included $includePath in $relativePath")
}
Add-ClassHeaderInclude 'CloseToHSL\\Core\Virtualization\CompilerBoundary\LaneBinding\YAKSys_Hybrid_CPU.Core.VirtualizationLaneBindingPolicy.h' 'VirtualizationLaneBindingPolicy.h'
Add-ClassHeaderInclude 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Scheduling\YAKSys_Hybrid_CPU.Core.VirtualizationSystemSingletonSchedulingRule.h' 'VirtualizationSystemSingletonSchedulingRule.h'
Add-ClassHeaderInclude 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Bundling\YAKSys_Hybrid_CPU.Core.VirtualizationBundleLegalityRule.h' 'VirtualizationBundleLegalityRule.h'
Add-ClassHeaderInclude 'CloseToHSL\\Core\Virtualization\CompilerBoundary\IR\YAKSys_Hybrid_CPU.Core.VirtualizationCompilerIntent.h' 'VirtualizationCompilerIntent.h'
Add-ClassHeaderInclude 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Lowering\YAKSys_Hybrid_CPU.Core.VirtualizationLoweringBoundary.h' 'VirtualizationLoweringBoundary.h'
Add-ClassHeaderInclude 'CloseToHSL\\Core\Virtualization\CompilerBoundary\NoEmission\YAKSys_Hybrid_CPU.Core.VirtualizationNoEmissionRegressionGate.h' 'VirtualizationNoEmissionRegressionGate.h'
Add-ClassHeaderInclude 'CloseToHSL\\Core\Virtualization\Conformance\NoEmission\YAKSys_Hybrid_CPU.Core.VirtualizationNoEmissionContract.h' 'VirtualizationNoEmissionContract.h'
Add-ClassHeaderInclude 'CloseToHSL\\Core\Virtualization\Conformance\HostEvidenceNonLeak\YAKSys_Hybrid_CPU.Core.VmxFrontendExposureConformanceContract.h' 'VmxFrontendExposureConformanceContract.h'
Add-ClassHeaderInclude 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Handlers\YAKSys_Hybrid_CPU.Core.VmxCompatFrontend.h' 'VmxCompatFrontend.h'
Add-ClassHeaderInclude 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Projection\YAKSys_Hybrid_CPU.Core.VmxFrontendResultMapper.h' '../Projection/VmxFrontendResultMapper.h'
Add-ClassHeaderInclude 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Retire\YAKSys_Hybrid_CPU.Core.VmxCompatRetireBoundary.h' 'VmxCompatRetireBoundary.h'
Add-ClassHeaderInclude 'CloseToHSL\\Core\Virtualization\Compatibility\Generated\VmcsProjection\YAKSys_Hybrid_CPU.Core.VmcsV2DescriptorProjection.h' 'VmcsV2DescriptorProjection.h'
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\LaneBinding\YAKSys_Hybrid_CPU.Core.VirtualizationLaneBindingPolicy.h' "`tprivate:`r`n`t`tstatic inline const SlotPlacementMetadata CompatibilityFrontendPlacement = SlotPlacementMetadata() {RequiredSlotClass = SlotClass::SystemSingleton, PinningKind = SlotPinningKind::HardPinned, PinnedLaneId = Lane7Id};`r`n`t`tstatic inline const SlotPlacementMetadata Lane6DescriptorPlacement = SlotPlacementMetadata() {RequiredSlotClass = SlotClass::DmaStreamClass, PinningKind = SlotPinningKind::HardPinned, PinnedLaneId = Lane6Id};`r`n`r`n`tpublic:`r`n`t`tstatic constexpr unsigned char Lane6Id = 6;`r`n`t`tstatic constexpr unsigned char Lane7Id = 7;" "`tpublic:`r`n`t`tstatic constexpr unsigned char Lane6Id = 6;`r`n`t`tstatic constexpr unsigned char Lane7Id = 7;`r`n`r`n`tprivate:`r`n`t`tstatic inline const SlotPlacementMetadata CompatibilityFrontendPlacement = [] { SlotPlacementMetadata placement; placement.RequiredSlotClass = SlotClass::SystemSingleton; placement.PinningKind = SlotPinningKind::HardPinned; placement.PinnedLaneId = Lane7Id; return placement; }();`r`n`t`tstatic inline const SlotPlacementMetadata Lane6DescriptorPlacement = [] { SlotPlacementMetadata placement; placement.RequiredSlotClass = SlotClass::DmaStreamClass; placement.PinningKind = SlotPinningKind::HardPinned; placement.PinnedLaneId = Lane6Id; return placement; }();`r`n`r`n`tpublic:"
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Bundling\YAKSys_Hybrid_CPU.Core.VirtualizationBundleLegalityRule.cpp' 'request->Descriptor.IsRuntimeAuthoritative' 'request->Descriptor->isRuntimeAuthoritative()'
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Bundling\YAKSys_Hybrid_CPU.Core.VirtualizationBundleLegalityRule.cpp' 'request->Descriptor.RequiresRuntimeValidation' 'request->Descriptor->getRequiresRuntimeValidation()'
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Bundling\YAKSys_Hybrid_CPU.Core.VirtualizationBundleLegalityRule.cpp' 'request->Descriptor.CanUseCompilerEvidenceAsAuthority' 'request->Descriptor->getCanUseCompilerEvidenceAsAuthority()'
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Bundling\YAKSys_Hybrid_CPU.Core.VirtualizationBundleLegalityRule.cpp' 'request->Descriptor.CanProjectToCompatibilityFrontend' 'request->Descriptor->getCanProjectToCompatibilityFrontend()'
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\IR\YAKSys_Hybrid_CPU.Core.VirtualizationCompilerIntent.cpp' 'return VirtualizationCompilerIntentResult::Allowed;' 'return VirtualizationCompilerIntentResult::Allowed();'
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Lowering\YAKSys_Hybrid_CPU.Core.VirtualizationLoweringBoundary.cpp' 'request->IntentResult.IsAllowed' 'request->IntentResult->IsAllowed'
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Lowering\YAKSys_Hybrid_CPU.Core.VirtualizationLoweringBoundary.cpp' 'return VirtualizationLoweringResult::Allowed;' 'return VirtualizationLoweringResult::Allowed();'
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Handlers\YAKSys_Hybrid_CPU.Core.VmxCompatFrontend.cpp' 'return VmxCompatFrontendAdmissionResult::Allowed;' 'return VmxCompatFrontendAdmissionResult::Allowed();'
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Retire\YAKSys_Hybrid_CPU.Core.VmxCompatRetireBoundary.cpp' 'request->RetireEffect.IsValid' 'request->RetireEffect.isValid()'
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Retire\YAKSys_Hybrid_CPU.Core.VmxCompatRetireBoundary.cpp' 'request->RetireEffect.IsFaulted' 'request->RetireEffect.isFaulted()'
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Retire\YAKSys_Hybrid_CPU.Core.VmxCompatRetireBoundary.cpp' 'request->RetireEffect.CompletionKind' 'request->RetireEffect.getCompletionKind()'
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Retire\YAKSys_Hybrid_CPU.Core.VmxCompatRetireBoundary.cpp' 'return VmxCompatRetireResult::Allowed;' 'return VmxCompatRetireResult::Allowed();'
Replace-Once 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Bundling\VirtualizationBundleLegalityRule.h' '#include "../../Runtime/Domains/Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.BundleLegalityDescriptor.h"' '#include "../../../Runtime/Domains/Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.BundleLegalityDescriptor.h"'
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\Generated\CsrProjection\YAKSys_Hybrid_CPU.Core.VmxCapsProjection.cpp' 'VmxCapsProjection::VmxCapsProjection(std::shared_ptr<CapabilityPublicationPolicy> publicationPolicy, std::shared_ptr<CapabilityDescriptorSetSchema> schema, bool compatibilityWriteFenceEnabled)`r`n`t{' 'VmxCapsProjection::VmxCapsProjection(std::shared_ptr<CapabilityPublicationPolicy> publicationPolicy, std::shared_ptr<CapabilityDescriptorSetSchema> schema, bool compatibilityWriteFenceEnabled)`r`n`t	: PublicationPolicy((publicationPolicy != nullptr) ? publicationPolicy : CapabilityPublicationPolicy::getFailClosed()), Schema((schema != nullptr) ? schema : std::make_shared<CapabilityDescriptorSetSchema>()), CompatibilityWriteFenceEnabled(compatibilityWriteFenceEnabled)`r`n`t{'
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\Generated\CsrProjection\YAKSys_Hybrid_CPU.Core.VmxCapsProjection.cpp' "`t`tPublicationPolicy = (publicationPolicy != nullptr) ? publicationPolicy : CapabilityPublicationPolicy::getFailClosed();`r`n" ''
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\Generated\CsrProjection\YAKSys_Hybrid_CPU.Core.VmxCapsProjection.cpp' "`t`tSchema = (schema != nullptr) ? schema : std::make_shared<CapabilityDescriptorSetSchema>();`r`n" ''
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\Generated\CsrProjection\YAKSys_Hybrid_CPU.Core.VmxCapsProjection.cpp' "`t`tCompatibilityWriteFenceEnabled = compatibilityWriteFenceEnabled;`r`n" ''
Replace-Once 'CloseToHSL\\Processor\Core\YAKSys_Hybrid_CPU.PodController.cpp' 'std::vector<AssistOwnerSnapshot> PodController::CreateEmptyAssistOwnerSnapshots()' 'std::vector<PodController::AssistOwnerSnapshot> PodController::CreateEmptyAssistOwnerSnapshots()'
Replace-Once 'CloseToHSL\\Processor\Core\YAKSys_Hybrid_CPU.PodController.cpp' 'PodController::PodController(int podX, int podY, std::shared_ptr<MicroOpScheduler> scheduler) : _scheduler(scheduler)`r`n`t{' 'PodController::PodController(int podX, int podY, std::shared_ptr<MicroOpScheduler> scheduler) : PodX(podX), PodY(podY), L2Cache(std::make_shared<SharedPodCache>()), _scheduler(scheduler)`r`n`t{'
Replace-Once 'CloseToHSL\\Processor\Core\YAKSys_Hybrid_CPU.PodController.cpp' '(pods != nullptr) ? pods : Processor::Pods' '(pods != nullptr) ? pods : std::make_shared<std::vector<std::shared_ptr<PodController>>>(Processor::Pods)'
Remove-Text 'CloseToHSL\\Processor\Core\YAKSys_Hybrid_CPU.PodController.cpp' "`r`n`t`tPodX = podX;`r`n`t`tPodY = podY;`r`n`t`tL2Cache = std::make_shared<SharedPodCache>();"
Replace-Once 'CloseToHSL\\Core\Execution\Vector\ALU\YAKSys_Hybrid_CPU.Execution.VectorALU.cpp' 'return __builtin_bswap64(value);' 'return ((value & 0x00000000000000FFULL) << 56) | ((value & 0x000000000000FF00ULL) << 40) | ((value & 0x0000000000FF0000ULL) << 24) | ((value & 0x00000000FF000000ULL) << 8) | ((value & 0x000000FF00000000ULL) >> 8) | ((value & 0x0000FF0000000000ULL) >> 24) | ((value & 0x00FF000000000000ULL) >> 40) | ((value & 0xFF00000000000000ULL) >> 56);'
Replace-Once 'CloseToHSL\\Core\Virtualization\Conformance\GeneratedParity\YAKSys_Hybrid_CPU.Core.VmxCapsBitSchemaConformanceContract.cpp' 'StringHelper::isEmptyOrWhiteSpace(entry->TypedGrantSource) || !entry->TypedGrantSource->Contains(L"TypedGrant", System::StringComparison::Ordinal) && !entry->TypedGrantSource->Contains(L"TypedGrants", System::StringComparison::Ordinal)' '(StringHelper::isEmptyOrWhiteSpace(entry->TypedGrantSource) || (entry->TypedGrantSource.find(L"TypedGrant") == std::wstring::npos && entry->TypedGrantSource.find(L"TypedGrants") == std::wstring::npos))'
Replace-Once 'CloseToHSL\\Core\Virtualization\Conformance\GeneratedParity\YAKSys_Hybrid_CPU.Core.VmxCapsBitSchemaConformanceContract.cpp' 'entry->TypedGrantSource->StartsWith(L"CapabilityDescriptorSet.", System::StringComparison::Ordinal)' 'entry->TypedGrantSource.rfind(L"CapabilityDescriptorSet.", 0) == 0'
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\FrozenAbi\CsrAliases\YAKSys_Hybrid_CPU.Core.VmxCsrAliasSet.h' 'static ReadOnlySpan<std::shared_ptr<VmxCsrAlias>> getFrozenAliases();' 'static std::span<const std::shared_ptr<VmxCsrAlias>> getFrozenAliases();'
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\FrozenAbi\CsrAliases\YAKSys_Hybrid_CPU.Core.VmxCsrAliasSet.cpp' 'ReadOnlySpan<std::shared_ptr<VmxCsrAlias>> VmxCsrAliasSet::getFrozenAliases()' 'std::span<const std::shared_ptr<VmxCsrAlias>> VmxCsrAliasSet::getFrozenAliases()'
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\FrozenAbi\Opcodes\YAKSys_Hybrid_CPU.Core.VmxOpcodeAliasSet.h' 'static ReadOnlySpan<std::shared_ptr<VmxOpcodeAlias>> getFrozenAliases();' 'static std::span<const std::shared_ptr<VmxOpcodeAlias>> getFrozenAliases();'
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\FrozenAbi\Opcodes\YAKSys_Hybrid_CPU.Core.VmxOpcodeAliasSet.cpp' 'ReadOnlySpan<std::shared_ptr<VmxOpcodeAlias>> VmxOpcodeAliasSet::getFrozenAliases()' 'std::span<const std::shared_ptr<VmxOpcodeAlias>> VmxOpcodeAliasSet::getFrozenAliases()'
Replace-Once 'CloseToHSL\\Core\Virtualization\Conformance\GoldenArtifacts\YAKSys_Hybrid_CPU.Core.VirtualizationGoldenArtifactManifest.h' 'static ReadOnlySpan<std::shared_ptr<VirtualizationGoldenArtifact>> getArtifacts();' 'static std::span<const std::shared_ptr<VirtualizationGoldenArtifact>> getArtifacts();'
Replace-Once 'CloseToHSL\\Core\Virtualization\Conformance\GoldenArtifacts\YAKSys_Hybrid_CPU.Core.VirtualizationGoldenArtifactManifest.cpp' 'ReadOnlySpan<std::shared_ptr<VirtualizationGoldenArtifact>> VirtualizationGoldenArtifactManifest::getArtifacts()' 'std::span<const std::shared_ptr<VirtualizationGoldenArtifact>> VirtualizationGoldenArtifactManifest::getArtifacts()'
Replace-Once 'CloseToHSL\\Core\Virtualization\Conformance\GoldenArtifacts\YAKSys_Hybrid_CPU.Core.VirtualizationGoldenArtifactManifest.h' '#include <memory>' "#include <memory>`r`n#include <span>"
Replace-Once 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Projection\YAKSys_Hybrid_CPU.Core.VmxFrontendResultMapper.h' 'std::shared_ptr<VmxFrontendResult> FromNestedValidation(std::shared_ptr<NestedValidationResult> validation);' 'VmxFrontendResult FromNestedValidation(std::shared_ptr<NestedValidationResult> validation);'
$mapperPath = Join-Path $root 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Projection\YAKSys_Hybrid_CPU.Core.VmxFrontendResultMapper.cpp'
$mapperText = [IO.File]::ReadAllText($mapperPath)
$mapperSignature = 'std::shared_ptr<VmxFrontendResult> VmxFrontendResultMapper::FromNestedValidation(std::shared_ptr<NestedValidationResult> validation)'
if ($mapperText.Contains($mapperSignature)) {
  $mapperStart = $mapperText.IndexOf($mapperSignature)
  $mapperBrace = $mapperText.IndexOf('{', $mapperStart)
  $mapperClose = $mapperText.IndexOf("`r`n`t}", $mapperBrace)
  if ($mapperBrace -lt 0 -or $mapperClose -lt 0) { throw 'Could not locate VmxFrontendResultMapper body.' }
  $mapperText = $mapperText.Substring(0, $mapperStart) + 'VmxFrontendResult VmxFrontendResultMapper::FromNestedValidation(std::shared_ptr<NestedValidationResult> validation)' + $mapperText.Substring($mapperStart + $mapperSignature.Length)
  $mapperBrace = $mapperText.IndexOf('{', $mapperStart)
  $mapperClose = $mapperText.IndexOf("`r`n`t}", $mapperBrace)
  $mapperBody = @'
		if (!validation)
		{
			return VmxFrontendResult::FailInvalid(VmFailCode::UnknownVmcsField, L"Nested validation result is required.");
		}
		if (validation->Succeeded)
		{
			return VmxFrontendResult::Success();
		}
		switch (validation->Code)
		{
		case NestedValidationCode::ProjectionDenied:
			return VmxFrontendResult::FailValid(VmFailCode::VmcsFieldAccessDenied, validation->Message);
		case NestedValidationCode::CompatibilityProjectionFailed:
			return VmxFrontendResult::FailInvalid(VmFailCode::GuestGprPersistenceIncomplete, validation->Message);
		default:
			return VmxFrontendResult::FailInvalid(VmFailCode::UnknownVmcsField, validation->Message);
		}
'@
  $mapperBody = [regex]::Replace($mapperBody, "\r?\n", "`r`n").Replace('\t', [string][char]9).Replace('`t', [string][char]9)
  $mapperText = $mapperText.Substring(0, $mapperBrace + 1) + "`r`n" + $mapperBody + $mapperText.Substring($mapperClose)
  [IO.File]::WriteAllText($mapperPath, $mapperText, [Text.UTF8Encoding]::new($false))
  $changes.Add('updated: VmxFrontendResultMapper implementation')
}
$parityFile = 'CloseToHSL\\Core\Virtualization\Conformance\GeneratedParity\YAKSys_Hybrid_CPU.Core.VmcsFieldProjectionSchemaConformanceContract.cpp'
Replace-Once $parityFile 'if (!Enum::IsDefined(typeof(VmcsFieldProjectionOwner), entry->Owner))' 'if (static_cast<unsigned char>(entry->Owner) > static_cast<unsigned char>(VmcsFieldProjectionOwner::CompatibilityControlDescriptor))'
Replace-Once $parityFile 'if (!Enum::IsDefined(typeof(VmcsFieldProjectionAccessPolicy), entry->AccessPolicy) || entry->AccessPolicy == VmcsFieldProjectionAccessPolicy::Denied)' 'if (static_cast<unsigned char>(entry->AccessPolicy) > static_cast<unsigned char>(VmcsFieldProjectionAccessPolicy::ReadWrite) || entry->AccessPolicy == VmcsFieldProjectionAccessPolicy::Denied)'
Replace-Once $parityFile 'if (!Enum::IsDefined(typeof(VmcsFieldProjectionMigrationPolicy), entry->MigrationPolicy) || entry->MigrationPolicy == VmcsFieldProjectionMigrationPolicy::None)' 'if (static_cast<unsigned char>(entry->MigrationPolicy) > static_cast<unsigned char>(VmcsFieldProjectionMigrationPolicy::ProjectionOnly) || entry->MigrationPolicy == VmcsFieldProjectionMigrationPolicy::None)'
$oldVmcsFieldLoop = @'
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
		for (VmcsField field : Enum::GetValues(typeof(VmcsField))->Cast<VmcsField>())
		{
			if (!std::find(seen.begin(), seen.end(), field) != seen.end())
			{
				return VmcsFieldProjectionSchemaConformanceViolation::MissingFrozenField;
			}
		}
'@
$newVmcsFieldLoop = @'
		static constexpr std::array<VmcsField, 23> frozenFields = { VmcsField::GuestPc, VmcsField::GuestSp, VmcsField::GuestFlags, VmcsField::GuestCr0, VmcsField::GuestCr3, VmcsField::GuestCr4, VmcsField::HostPc, VmcsField::HostSp, VmcsField::HostFlags, VmcsField::HostCr0, VmcsField::HostCr3, VmcsField::PinBasedControls, VmcsField::ProcBasedControls, VmcsField::ExitControls, VmcsField::EntryControls, VmcsField::EptPointer, VmcsField::Vpid, VmcsField::SecondaryProcControls, VmcsField::Cr3TargetCount, VmcsField::ExitReason, VmcsField::ExitQualification, VmcsField::GuestPhysicalAddress, VmcsField::EptViolationQualification };
		for (VmcsField field : frozenFields)
		{
			if (std::find(seen.begin(), seen.end(), field) == seen.end())
			{
				return VmcsFieldProjectionSchemaConformanceViolation::MissingFrozenField;
			}
		}
'@
Replace-Once $parityFile $oldVmcsFieldLoop $newVmcsFieldLoop
Replace-Once $parityFile '#include "YAKSys_Hybrid_CPU.Core.VmcsFieldProjectionSchemaConformanceContract.h"' "#include `"YAKSys_Hybrid_CPU.Core.VmcsFieldProjectionSchemaConformanceContract.h`"`r`n#include <array>`r`n#include <algorithm>`r`n#include <unordered_set>"
$modelHeaders = [regex]::Matches([IO.File]::ReadAllText($PSCommandPath), "(?m)^Add-Model '([^']+)'") | ForEach-Object { $_.Groups[1].Value } | Sort-Object -Unique
foreach ($relativePath in $modelHeaders) {
  $path = Join-Path $root $relativePath
  $lines = [IO.File]::ReadAllLines($path)
  $seenIncludes = [Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
  $normalized = [Collections.Generic.List[string]]::new()
  foreach ($line in $lines) {
    if ($relativePath -eq 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Retire\VmxCompatRetireBoundary.h' -and $line.Trim() -eq '#include "VmxCompatRetireBoundary.h"') { continue }
    if ($relativePath -eq 'CloseToHSL\\Core\Virtualization\CompilerBoundary\Bundling\VirtualizationBundleLegalityRule.h' -and $line.Trim() -eq '#include "../../Runtime/Domains/Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.BundleLegalityDescriptor.h"') { continue }
    if ($line -match '^\s*#include\s+') {
      if (-not $seenIncludes.Add($line.Trim())) { continue }
    }
    $normalized.Add($line)
  }
  if ($normalized.Count -ne $lines.Length) {
    [IO.File]::WriteAllLines($path, $normalized, [Text.UTF8Encoding]::new($false))
    $changes.Add("deduplicated includes in $relativePath")
  }
}
Add-Content -LiteralPath (Join-Path $root 'CompilerErrors\error-pool-9-apply.log') -Value "`r`n--- rerun $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss') ---" -Encoding utf8
$changes | Add-Content -LiteralPath (Join-Path $root 'CompilerErrors\error-pool-9-apply.log') -Encoding utf8
