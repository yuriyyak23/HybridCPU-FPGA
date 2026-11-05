$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\FrozenAbi\FunctionLeaves'; $p=Join-Path $r 'VmxFunctionLeaf.h'; $s=[IO.File]::ReadAllText($p).Replace('#include <memory>',"#include <memory>`r`n#include <string>`r`n#include <utility>"); $anchor='class VmxFunctionLeafAdmissionPolicy final'; $def=@"
struct VmxFunctionLeafAdmissionRequest final {
    VmxFunctionLeaf Leaf = VmxFunctionLeaf::None;
    bool IsFrozenAbiSurface = false, DescriptorValidated = false, CapabilityValidated = false, Lane7GrantValidated = false, UsesFastPath = false;
};
struct VmxFunctionLeafAdmissionResult final {
    const VmxFunctionLeafAdmissionDecision Decision;
    const std::wstring Reason;
    VmxFunctionLeafAdmissionResult(VmxFunctionLeafAdmissionDecision decision, std::wstring reason) : Decision(decision), Reason(std::move(reason)) {}
    bool isAllowed() const noexcept { return Decision == VmxFunctionLeafAdmissionDecision::Allowed; }
    static const std::shared_ptr<VmxFunctionLeafAdmissionResult> Allowed;
    static std::shared_ptr<VmxFunctionLeafAdmissionResult> Denied(VmxFunctionLeafAdmissionDecision decision, std::wstring reason) { return std::make_shared<VmxFunctionLeafAdmissionResult>(decision, std::move(reason)); }
};
inline const std::shared_ptr<VmxFunctionLeafAdmissionResult> VmxFunctionLeafAdmissionResult::Allowed = std::make_shared<VmxFunctionLeafAdmissionResult>(VmxFunctionLeafAdmissionDecision::Allowed, L"VMFUNC leaf admission allowed.");

"@; if(-not $s.Contains('struct VmxFunctionLeafAdmissionRequest final')) {$s=$s.Replace($anchor,$def+$anchor)}; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'VmxFunctionLeaf.cpp'; $s=[IO.File]::ReadAllText($p).Replace('Validate(request)->IsAllowed','Validate(request)->isAllowed()'); [IO.File]::WriteAllText($p,$s)
