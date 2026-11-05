$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Frontend\Projection'
$p=Join-Path $base 'VmxCompatProjectionService.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#pragma once', "#pragma once`r`n#include `"../../Generated/AliasMaps/CompatAliasMap.h`"`r`n#include <memory>`r`n#include <string>`r`n#include <utility>")
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); if($i -lt 0){throw 'anchor missing'}
$s=$s.Substring(0,$i)+@'
struct VmxCompatProjectionRequest {
    CompatAliasSourceKind SourceKind{}; std::wstring SourceName;
    bool DescriptorValidated{}, EvidenceValidated{}, AttemptsAuthoritativeMutation{};
};
struct VmxCompatProjectionResult {
    VmxCompatProjectionDecision Decision{}; std::shared_ptr<CompatAliasMapEntry> Alias; std::wstring Reason;
    bool getIsAllowed() const { return Decision == VmxCompatProjectionDecision::Allowed; }
    static std::shared_ptr<VmxCompatProjectionResult> Allowed(std::shared_ptr<CompatAliasMapEntry> alias) {
        return std::make_shared<VmxCompatProjectionResult>(VmxCompatProjectionResult{VmxCompatProjectionDecision::Allowed, std::move(alias), L""});
    }
    static std::shared_ptr<VmxCompatProjectionResult> Denied(VmxCompatProjectionDecision decision, std::wstring reason) {
        return std::make_shared<VmxCompatProjectionResult>(VmxCompatProjectionResult{decision, std::make_shared<CompatAliasMapEntry>(), std::move(reason)});
    }
};
}
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.VmxCompatProjectionService.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('#include "../../Generated/AliasMaps/YAKSys_Hybrid_CPU.Core.CompatAliasMap.h"', "#include `"../../Generated/AliasMaps/YAKSys_Hybrid_CPU.Core.CompatAliasMap.h`"`r`n#include `"VmxCompatProjectionService.h`""); [IO.File]::WriteAllText($p,$s)