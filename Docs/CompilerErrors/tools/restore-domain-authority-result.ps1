$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\Authority'
$p=Join-Path $base 'DomainRuntimeAuthority.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#pragma once',"#pragma once`r`n#include <memory>`r`n#include <string>`r`n#include <utility>")
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); if($i -lt 0){throw 'anchor missing'}
$s=$s.Substring(0,$i)+@'
struct DomainRuntimeAuthorityResult {
    const DomainRuntimeAuthorityDecision Decision;
    const std::wstring Message;
    const bool IsAllowed;
    DomainRuntimeAuthorityResult(DomainRuntimeAuthorityDecision decision, std::wstring message)
        : Decision(decision), Message(std::move(message)), IsAllowed(decision == DomainRuntimeAuthorityDecision::Allowed) {}
    static std::shared_ptr<DomainRuntimeAuthorityResult> Denied(DomainRuntimeAuthorityDecision decision, std::wstring message) {
        return std::make_shared<DomainRuntimeAuthorityResult>(decision, std::move(message));
    }
    static const std::shared_ptr<DomainRuntimeAuthorityResult> Allowed;
};
inline const std::shared_ptr<DomainRuntimeAuthorityResult> DomainRuntimeAuthorityResult::Allowed =
    std::make_shared<DomainRuntimeAuthorityResult>(DomainRuntimeAuthorityDecision::Allowed, L"");
}
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.DomainRuntimeAuthority.h'; $s=[IO.File]::ReadAllText($p)
$anchor='class DomainRuntimeAuthority final'; if(-not $s.Contains($anchor)){throw 'class anchor missing'}
$s=$s.Replace($anchor,"struct CapabilityBoundaryRequirement;`r`n`tstruct DomainBoundaryDescriptor;`r`n`t$anchor")
[IO.File]::WriteAllText($p,$s)