$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Capabilities\CompatibilityControls'; $p=Join-Path $r 'CompatibilityControlPolicyOwner.h'; $s=[IO.File]::ReadAllText($p); $a='class NeutralInterruptRoutingPolicyConsumer final'; $def=@"
struct NeutralInterruptRoutingPolicyResult final {
    const NeutralInterruptRoutingPolicyDecision Decision;
    const std::wstring Reason;
    NeutralInterruptRoutingPolicyResult(NeutralInterruptRoutingPolicyDecision decision, std::wstring reason) : Decision(decision), Reason(std::move(reason)) {}
    bool isAllowed() const noexcept { return Decision == NeutralInterruptRoutingPolicyDecision::Allowed; }
};

"@; if(-not $s.Contains('struct NeutralInterruptRoutingPolicyResult final')) {$s=$s.Replace($a,$def+$a)}; if(-not $s.Contains('#include <utility>')) {$s=$s.Replace('#include <memory>',"#include <memory>`r`n#include <utility>")}; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'CompatibilityControlPolicyOwner.cpp'; $s=[IO.File]::ReadAllText($p).Replace('return new(NeutralInterruptRoutingPolicyDecision::Allowed, L"Current owner-issued neutral event-routing policy admitted interrupt routing.");','return std::make_shared<NeutralInterruptRoutingPolicyResult>(NeutralInterruptRoutingPolicyDecision::Allowed, L"Current owner-issued neutral event-routing policy admitted interrupt routing.");').Replace('return new(decision, reason);','return std::make_shared<NeutralInterruptRoutingPolicyResult>(decision, reason);'); [IO.File]::WriteAllText($p,$s)
