$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\Admission\Memory'; $p=Join-Path $r 'MemoryDomainRuntime.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('struct MemoryDomainRuntimeRequest final')) { $s=$s.Replace('#pragma once',"#pragma once`r`n#include <memory>`r`n#include <string>`r`n#include <utility>"); $i=$s.LastIndexOf('}'); $def=@"
class MemoryDomainDescriptor;
struct MemoryDomainRuntimeRequest final {
    std::shared_ptr<MemoryDomainDescriptor> Descriptor;
    bool RequiresAddressSpace = false, RequiresTranslationPolicy = false, RequiresSecondStageTranslation = false, RequiresDirtyTracking = false;
};
struct MemoryDomainRuntimeResult final {
    const MemoryDomainRuntimeDecision Decision; const std::wstring Reason;
    MemoryDomainRuntimeResult(MemoryDomainRuntimeDecision decision, std::wstring reason) : Decision(decision), Reason(std::move(reason)) {}
    bool isAllowed() const noexcept { return Decision == MemoryDomainRuntimeDecision::Allowed; }
    static const std::shared_ptr<MemoryDomainRuntimeResult> Allowed;
    static std::shared_ptr<MemoryDomainRuntimeResult> Denied(MemoryDomainRuntimeDecision decision, std::wstring reason) { return std::make_shared<MemoryDomainRuntimeResult>(decision, std::move(reason)); }
};
inline const std::shared_ptr<MemoryDomainRuntimeResult> MemoryDomainRuntimeResult::Allowed = std::make_shared<MemoryDomainRuntimeResult>(MemoryDomainRuntimeDecision::Allowed, L"Memory domain runtime admission allowed.");
"@; $s=$s.Insert($i,$def+"`r`n")}; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Core.MemoryDomainRuntime.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('#include "MemoryDomainRuntime.h"')) {$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"MemoryDomainRuntime.h`"")}; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Core.MemoryDomainRuntime.cpp'; $s=[IO.File]::ReadAllText($p).Replace('Validate(request)->IsAllowed','Validate(request)->isAllowed()'); [IO.File]::WriteAllText($p,$s)
