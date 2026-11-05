$ErrorActionPreference = 'Stop'
$root = 'C:\Users\Yuriy\Desktop\HybridCPU_FPGA'
$changes = [System.Collections.Generic.List[string]]::new()
function Replace-Once([string]$relativePath, [string]$old, [string]$new) {
  $path = Join-Path $root $relativePath
  $content = [IO.File]::ReadAllText($path)
  $old = [regex]::Replace($old, "\r?\n", "`r`n")
  $new = [regex]::Replace($new, "\r?\n", "`r`n")
  $old = $old.Replace('\t', [string][char]9)
  $new = $new.Replace('\t', [string][char]9)
  if ($content.Contains($new)) { $changes.Add("already applied: $relativePath"); return }
  if (-not $content.Contains($old)) { throw "Expected source text not found in $relativePath" }
  [IO.File]::WriteAllText($path, $content.Replace($old, $new), [Text.UTF8Encoding]::new($false))
  $changes.Add("updated: $relativePath")
}

function Remove-Once([string]$relativePath, [string]$text) {
  $path = Join-Path $root $relativePath
  $content = [IO.File]::ReadAllText($path)
  if (-not $content.Contains($text)) { $changes.Add("already removed: $relativePath"); return }
  [IO.File]::WriteAllText($path, $content.Replace($text, ''), [Text.UTF8Encoding]::new($false))
  $changes.Add("removed: $relativePath")
}
Remove-Once 'CloseToHSL\\Core\Virtualization\Conformance\AbiFreeze\CompatAbiFreezeContract.h' "using namespace YAKSys_Hybrid_CPU::Arch;`r`n`r`n"
Replace-Once 'CloseToHSL\\Core\Virtualization\Conformance\AbiFreeze\YAKSys_Hybrid_CPU.Core.CompatAbiFreezeContract.h' "#include <memory>`r`n`r`nusing namespace YAKSys_Hybrid_CPU::Arch;" "#include <memory>`r`n#include <span>`r`n`r`n"
Replace-Once 'CloseToHSL\\Core\Virtualization\Conformance\AbiFreeze\YAKSys_Hybrid_CPU.Core.CompatAbiFreezeContract.h' 'static ReadOnlySpan<unsigned short> getFrozenCsrAliases();' 'static std::span<const unsigned short> getFrozenCsrAliases();'
Replace-Once 'CloseToHSL\\Core\Virtualization\Conformance\AbiFreeze\YAKSys_Hybrid_CPU.Core.CompatAbiFreezeContract.cpp' '#include "../../Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"' "#include `"../../Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h`"`r`n#include <array>`r`n#include <span>"
Replace-Once 'CloseToHSL\\Core\Virtualization\Conformance\AbiFreeze\YAKSys_Hybrid_CPU.Core.CompatAbiFreezeContract.cpp' '#include <span>' "#include <span>`r`n`r`nusing namespace YAKSys_Hybrid_CPU::Arch;"
Replace-Once 'CloseToHSL\\Core\Virtualization\Conformance\AbiFreeze\YAKSys_Hybrid_CPU.Core.CompatAbiFreezeContract.cpp' 'ReadOnlySpan<unsigned short> CompatAbiFreezeContract::getFrozenCsrAliases()' 'std::span<const unsigned short> CompatAbiFreezeContract::getFrozenCsrAliases()'
Replace-Once 'CloseToHSL\\Core\Virtualization\Conformance\AbiFreeze\YAKSys_Hybrid_CPU.Core.CompatAbiFreezeContract.cpp' 'return [CsrAddresses::VmxEnable, CsrAddresses::VmxCaps, CsrAddresses::VmxControl, CsrAddresses::VmxExitReason, CsrAddresses::VmxExitQual,];' "static constexpr std::array<unsigned short, 5> aliases{ CsrAddresses::VmxEnable, CsrAddresses::VmxCaps, CsrAddresses::VmxControl, CsrAddresses::VmxExitReason, CsrAddresses::VmxExitQual };`r`n`t`treturn aliases;"
Remove-Once 'CloseToHSL\\Core\Virtualization\Conformance\AbiFreeze\YAKSys_Hybrid_CPU.Core.CompatAbiFreezeContract.cpp' "// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:`r`n"
$vmcsSwitch = @'
		switch (static_cast<VmcsField>(fieldId))
		{
		case VmcsField::GuestPc: case VmcsField::GuestSp: case VmcsField::GuestFlags: case VmcsField::GuestCr0: case VmcsField::GuestCr3: case VmcsField::GuestCr4:
		case VmcsField::HostPc: case VmcsField::HostSp: case VmcsField::HostFlags: case VmcsField::HostCr0: case VmcsField::HostCr3:
		case VmcsField::PinBasedControls: case VmcsField::ProcBasedControls: case VmcsField::ExitControls: case VmcsField::EntryControls:
		case VmcsField::EptPointer: case VmcsField::Vpid: case VmcsField::SecondaryProcControls: case VmcsField::Cr3TargetCount:
		case VmcsField::ExitReason: case VmcsField::ExitQualification: case VmcsField::GuestPhysicalAddress: case VmcsField::EptViolationQualification:
			return true;
		default:
			return false;
		}
'@
$vmcsSwitch = [regex]::Replace($vmcsSwitch, "\r?\n", "`r`n").Replace('\t', [string][char]9)
$vmcsPath = Join-Path $root 'CloseToHSL\\Core\Virtualization\Conformance\AbiFreeze\YAKSys_Hybrid_CPU.Core.CompatAbiFreezeContract.cpp'
$vmcsContent = [IO.File]::ReadAllText($vmcsPath)
$vmcsStart = $vmcsContent.IndexOf('switch (static_cast<VmcsField>(fieldId))')
if ($vmcsStart -ge 0) {
  $vmcsLineStart = $vmcsContent.LastIndexOf("`n", $vmcsStart) + 1
  $vmcsLineEnd = $vmcsContent.IndexOf("`n", $vmcsStart)
  if ($vmcsLineEnd -lt 0) { throw 'Could not locate the malformed VMCS switch line end.' }
  $vmcsContent = $vmcsContent.Substring(0, $vmcsLineStart) + $vmcsSwitch.TrimEnd() + "`r`n" + $vmcsContent.Substring($vmcsLineEnd + 1)
  [IO.File]::WriteAllText($vmcsPath, $vmcsContent, [Text.UTF8Encoding]::new($false))
  $changes.Add('updated: AbiFreeze VMCS field membership switch')
} elseif ($vmcsContent.Contains('return Enum::IsDefined(typeof(VmcsField), fieldId);')) {
  $vmcsContent = $vmcsContent.Replace('return Enum::IsDefined(typeof(VmcsField), fieldId);', $vmcsSwitch.Trim())
  [IO.File]::WriteAllText($vmcsPath, $vmcsContent, [Text.UTF8Encoding]::new($false))
  $changes.Add('updated: AbiFreeze VMCS field membership switch')
} else {
  $changes.Add('already applied: AbiFreeze VMCS field membership switch')
}
Replace-Once 'NonRTL\\Arch\InstructionEncoder.h' '#include "../../Arch/DataTypeEnum.h"' "#include `"../../Arch/DataTypeEnum.h`"`r`n#include `"../../CloseToHSL/Core/Architecture/Registers/Architectural/RegisterIdentity.h`""
Replace-Once 'NonRTL\\Arch\InstructionEncoder.cpp' 'throw ArgumentOutOfRangeException(paramName, regId, std::format("Architectural register id must be in [0, {0:s}].", ArchRegId::MaxValue));' 'throw ArgumentOutOfRangeException(paramName, regId, std::wstring(L"Architectural register id must be in [0, ") + std::to_wstring(ArchRegId::MaxValue) + L"].");'
Replace-Once 'NonRTL\\Arch\InstructionEncoder.cpp' 'throw InvalidOperationException(L"EncodeDotProduct rejected a separate destination pointer for the current VDOT* ABI. " + L"Runtime dot-product execution is limited to the 1D scalar-footprint contour where destination aliases source1; " + L"separate destination/accumulator transport must fail closed until a complete executable ABI is implemented.");' 'throw InvalidOperationException(std::wstring(L"EncodeDotProduct rejected a separate destination pointer for the current VDOT* ABI. ") + L"Runtime dot-product execution is limited to the 1D scalar-footprint contour where destination aliases source1; " + L"separate destination/accumulator transport must fail closed until a complete executable ABI is implemented.");'
Replace-Once 'CloseToHSL\\Core\Runtime\Events\VmRead\VmReadScalarDeliveryCanonicalComposition.h' "class IVmReadScalarResultReceiptOwner`r`n`t{" "class IVmReadScalarResultReceiptOwner : public std::enable_shared_from_this<IVmReadScalarResultReceiptOwner>`r`n`t{"
Replace-Once 'CloseToHSL\\Core\Runtime\Governance\Virtualization\Phase64PinBasedControlsVmReadProjectionD2.h' '#include <memory>' "#include <memory>`r`n#include <optional>`r`n#include <utility>"
$oldModel = @'
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct PinBasedControlsProjectionResult(PinBasedControlsProjectionDecision Decision, System.Nullable<ulong> Value, string Reason)
//	{
//		internal bool IsGranted
//		{
//			get
//			{
//				return Decision == PinBasedControlsProjectionDecision.Granted;
//			}
//		}
//	}
'@
$newModel = @'
struct PinBasedControlsProjectionResult final
{
    const PinBasedControlsProjectionDecision Decision;
    const std::optional<unsigned long long> Value;
    const std::wstring Reason;
    const bool IsGranted;
    PinBasedControlsProjectionResult(PinBasedControlsProjectionDecision decision, std::optional<unsigned long long> value, std::wstring reason)
        : Decision(decision), Value(std::move(value)), Reason(std::move(reason)), IsGranted(decision == PinBasedControlsProjectionDecision::Granted) {}
};
'@
Replace-Once 'CloseToHSL\\Core\Runtime\Governance\Virtualization\Phase64PinBasedControlsVmReadProjectionD2.h' $oldModel $newModel
Replace-Once 'CloseToHSL\\Core\Runtime\Governance\Virtualization\Phase64PinBasedControlsVmReadProjectionD2.cpp' 'return new(PinBasedControlsProjectionDecision::Granted, value, L"Current neutral policy mapped exactly to the frozen HybridCPU VMX8 field.");' 'return std::make_shared<PinBasedControlsProjectionResult>(PinBasedControlsProjectionDecision::Granted, value, L"Current neutral policy mapped exactly to the frozen HybridCPU VMX8 field.");'
Replace-Once 'CloseToHSL\\Core\Runtime\Governance\Virtualization\Phase64PinBasedControlsVmReadProjectionD2.cpp' 'return new(decision, nullptr, reason);' 'return std::make_shared<PinBasedControlsProjectionResult>(decision, std::nullopt, reason);'
Replace-Once 'CloseToHSL\\Core\Runtime\Events\VmRead\PinBasedControlsVmReadScalarDeliveryCanonicalComposition.cpp' 'carrier->getRd() == 0 or > 31' 'carrier->getRd() == 0 || carrier->getRd() > 31'
Replace-Once 'CloseToHSL\\Core\Runtime\Events\VmRead\PinBasedControlsVmReadScalarDeliveryCanonicalComposition.cpp' 'if (!projection->IsGranted || !projection->Value->HasValue)' 'if (!projection->IsGranted || !projection->Value.has_value())'
Replace-Once 'CloseToHSL\\Core\Runtime\Events\VmRead\PinBasedControlsVmReadScalarDeliveryCanonicalComposition.cpp' 'projection->Value->Value' 'projection->Value.value()'
Replace-Once 'CloseToHSL\\Core\Runtime\Events\VmRead\PinBasedControlsVmReadScalarDeliveryCanonicalComposition.cpp' 'return new(VmReadScalarDeliveryDecision::Prepared, receipt, L"Exact owner-issued PinBasedControls projection captured for compatibility-only scalar delivery.");' 'return std::make_shared<VmReadScalarDeliveryResult>(VmReadScalarDeliveryDecision::Prepared, receipt, L"Exact owner-issued PinBasedControls projection captured for compatibility-only scalar delivery.");'
Replace-Once 'CloseToHSL\\Core\Runtime\Events\VmRead\PinBasedControlsVmReadScalarDeliveryCanonicalComposition.cpp' 'return new(decision, nullptr, reason);' 'return std::make_shared<VmReadScalarDeliveryResult>(decision, nullptr, reason);'
$oldValidation = @'
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++:
		return isEnabled() && receipt->getDecisionId() == DecisionId && receipt->getProfileGeneration() == _generation && receipt->getCompatibilityControlCapture() is { } capture && this->ReferenceEquals(receipt->getSourceOwner(), _policyOwner) && _policyOwner->IsCurrent(capture) && receipt->getSourceEpoch() == capture::Identity::PolicyGeneration && receipt->getDomainTag() == capture::Identity::DomainIdentity && receipt->getField() == VmcsField::PinBasedControls && receipt->getAttemptId() != 0 && receipt->getBundleIdentity() != 0 && (!currentRestoreGeneration || receipt->getRestoreGeneration() == currentRestoreGeneration.value());
'@
$newValidation = @'
		if (!receipt || !isEnabled() || receipt->getDecisionId() != DecisionId || receipt->getProfileGeneration() != _generation)
		{
			return false;
		}

		const auto capture = receipt->getCompatibilityControlCapture();
		const auto sourceOwner = receipt->getSourceOwner();
		const auto* typedSourceOwner = std::any_cast<std::shared_ptr<CompatibilityControlPolicyOwner>>(&sourceOwner);
		if (!capture || typedSourceOwner == nullptr || *typedSourceOwner != _policyOwner || !_policyOwner->IsCurrent(capture))
		{
			return false;
		}

		const auto identity = capture->getIdentity();
		return identity && receipt->getSourceEpoch() == identity->PolicyGeneration && receipt->getDomainTag() == identity->DomainIdentity && receipt->getField() == VmcsField::PinBasedControls && receipt->getAttemptId() != 0 && receipt->getBundleIdentity() != 0 && (!currentRestoreGeneration || receipt->getRestoreGeneration() == currentRestoreGeneration.value());
'@
$validationPath = Join-Path $root 'CloseToHSL\\Core\Runtime\Events\VmRead\PinBasedControlsVmReadScalarDeliveryCanonicalComposition.cpp'
$validationContent = [IO.File]::ReadAllText($validationPath)
$validationStart = $validationContent.IndexOf('bool PinBasedControlsVmReadScalarDeliveryCanonicalComposition::Validate(')
$validationEnd = $validationContent.IndexOf("`r`n`t}`r`n`r`n`tvoid PinBasedControlsVmReadScalarDeliveryCanonicalComposition::AdvanceGeneration()", $validationStart)
if ($validationStart -lt 0 -or $validationEnd -lt 0) { throw 'Could not locate Validate method boundaries.' }
$brace = $validationContent.IndexOf('{', $validationStart)
$validationContent = $validationContent.Substring(0, $brace + 1) + "`r`n" + $newValidation.TrimEnd() + $validationContent.Substring($validationEnd)
[IO.File]::WriteAllText($validationPath, $validationContent, [Text.UTF8Encoding]::new($false))
$changes.Add('updated: 2\CloseToHSL\Core\Runtime\Events\VmRead\PinBasedControlsVmReadScalarDeliveryCanonicalComposition.cpp Validate')
# Use nested namespace syntax for compatibility with the active parser diagnostics.
Replace-Once 'CloseToHSL\\Core\Pipeline\Safety\SafetyVerifier.VirtualizationAdmission.h' "namespace YAKSys_Hybrid_CPU::Core`r`n{" "namespace YAKSys_Hybrid_CPU { namespace Core`r`n{"
Replace-Once 'CloseToHSL\\Core\Pipeline\Safety\SafetyVerifier.VirtualizationAdmission.h' "`r`n}`r`n" "`r`n}}`r`n"
Replace-Once 'CloseToHSL\\Core\Pipeline\Safety\SafetyVerifier.VirtualizationAdmission.cpp' "namespace YAKSys_Hybrid_CPU::Core`r`n{" "namespace YAKSys_Hybrid_CPU { namespace Core`r`n{"
Replace-Once 'CloseToHSL\\Core\Pipeline\Safety\SafetyVerifier.VirtualizationAdmission.cpp' "`r`n}`r`n" "`r`n}}`r`n"
$changes | Set-Content -LiteralPath (Join-Path $root 'CompilerErrors\error-pool-8-apply.log') -Encoding utf8
