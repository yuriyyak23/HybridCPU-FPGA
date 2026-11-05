$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\Admission\Execution'; $p=Join-Path $r 'ExecutionDomainRuntime.h'; $s=[IO.File]::ReadAllText($p).Replace('#pragma once',"#pragma once`r`n#include <memory>`r`n#include <string>`r`n#include <utility>"); $pos=$s.LastIndexOf('}'); if(-not $s.Contains('struct ExecutionDomainRuntimeRequest final')) { $def=@"
class ExecutionDomainDescriptor;
struct ExecutionDomainRuntimeRequest final {
    std::shared_ptr<ExecutionDomainDescriptor> Descriptor;
    bool RequiresBundleLegality = false, RequiresSchedulingBudget = false, RequiresCompatibilityProjection = false;
};
struct ExecutionDomainRuntimeResult final {
    const ExecutionDomainRuntimeDecision Decision; const std::wstring Reason;
    ExecutionDomainRuntimeResult(ExecutionDomainRuntimeDecision decision, std::wstring reason) : Decision(decision), Reason(std::move(reason)) {}
    bool isAllowed() const noexcept { return Decision == ExecutionDomainRuntimeDecision::Allowed; }
    static const std::shared_ptr<ExecutionDomainRuntimeResult> Allowed;
    static std::shared_ptr<ExecutionDomainRuntimeResult> Denied(ExecutionDomainRuntimeDecision decision, std::wstring reason) { return std::make_shared<ExecutionDomainRuntimeResult>(decision, std::move(reason)); }
};
inline const std::shared_ptr<ExecutionDomainRuntimeResult> ExecutionDomainRuntimeResult::Allowed = std::make_shared<ExecutionDomainRuntimeResult>(ExecutionDomainRuntimeDecision::Allowed, L"Execution domain runtime admission allowed.");
"@; $s=$s.Insert($pos,$def+"`r`n") }; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'ExecutionDomainRuntime.StateEpoch.h'; $s=[IO.File]::ReadAllText($p).Replace('const unsigned long long Value;','const unsigned long long Value = 0;'); $anchor='enum class ExecutionDomainSourceCaptureDecision'; $def=@"
class ExecutionDomainDescriptor;
class ExecutionDomainSourceCaptureResult;
struct ExecutionDomainSourceBindResult final {
    ExecutionDomainSourceBindDecision Decision; std::shared_ptr<ExecutionDomainDescriptor> Descriptor; ExecutionDomainStateEpoch Epoch; std::wstring Reason;
    bool isBound() const noexcept { return Decision == ExecutionDomainSourceBindDecision::Bound && Descriptor != nullptr && Epoch.isMaterialized(); }
};

"@; if(-not $s.Contains('struct ExecutionDomainSourceBindResult final')) {$s=$s.Replace($anchor,$def+$anchor)}; if(-not $s.Contains('#include <memory>')) {$s=$s.Replace('#include <string>',"#include <string>`r`n#include <memory>")}; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Core.ExecutionDomainRuntime.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('#include "ExecutionDomainRuntime.h"')) {$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"ExecutionDomainRuntime.h`"")}; [IO.File]::WriteAllText($p,$s)
