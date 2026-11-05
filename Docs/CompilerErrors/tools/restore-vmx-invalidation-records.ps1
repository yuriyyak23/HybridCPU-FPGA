$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\FrozenAbi\OperandForms'; $p=Join-Path $r 'VmxInvalidationScope.h'; $s=[IO.File]::ReadAllText($p).Replace('#include <memory>',"#include <memory>`r`n#include <string>`r`n#include <utility>"); $anchor='class VmxInvalidationScopeAdmissionPolicy final'; $def=@"
struct VmxInvalidationScopeAdmissionRequest final {
    VmxInvalidationScope Scope = VmxInvalidationScope::None;
    bool DescriptorValidated = false, CapabilityValidated = false, RuntimeInvalidationValidated = false, TranslationAuthorityValidated = false, AttemptsDirectMmuMutation = false;
};
struct VmxInvalidationScopeAdmissionResult final {
    const VmxInvalidationScopeAdmissionDecision Decision;
    const std::wstring Reason;
    VmxInvalidationScopeAdmissionResult(VmxInvalidationScopeAdmissionDecision decision, std::wstring reason) : Decision(decision), Reason(std::move(reason)) {}
    bool isAllowed() const noexcept { return Decision == VmxInvalidationScopeAdmissionDecision::Allowed; }
    static const std::shared_ptr<VmxInvalidationScopeAdmissionResult> Allowed;
    static std::shared_ptr<VmxInvalidationScopeAdmissionResult> Denied(VmxInvalidationScopeAdmissionDecision decision, std::wstring reason) { return std::make_shared<VmxInvalidationScopeAdmissionResult>(decision, std::move(reason)); }
};
inline const std::shared_ptr<VmxInvalidationScopeAdmissionResult> VmxInvalidationScopeAdmissionResult::Allowed = std::make_shared<VmxInvalidationScopeAdmissionResult>(VmxInvalidationScopeAdmissionDecision::Allowed, L"VMX invalidation scope admission allowed.");

"@; if(-not $s.Contains('struct VmxInvalidationScopeAdmissionRequest final')) {$s=$s.Replace($anchor,$def+$anchor)}; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'VmxInvalidationScope.cpp'; $s=[IO.File]::ReadAllText($p).Replace('Validate(request)->IsAllowed','Validate(request)->isAllowed()'); [IO.File]::WriteAllText($p,$s)
