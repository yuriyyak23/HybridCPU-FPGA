$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Records\TrapCompletionPublicationFence.h'
$s=[IO.File]::ReadAllText($p)
$anchor='class TrapCompletionPublicationFence final'
$insert=@"
class TrapCompletionPublicationFenceResult;
    class NeutralTrapResult;
    class TrapCompletionRouteRequest;
    class TrapCompletionRouteResult;
    struct TrapCompletionOwnerPolicyResult final {
        const bool IsAllowed;
        const std::wstring Reason;
        TrapCompletionOwnerPolicyResult(bool isAllowed, std::wstring reason)
            : IsAllowed(isAllowed), Reason(std::move(reason)) {}
    };

"@
if (-not $s.Contains('struct TrapCompletionOwnerPolicyResult final')) { if(-not $s.Contains($anchor)){throw 'anchor missing'}; $s=$s.Replace($anchor,$insert+$anchor) }
$s=$s.Replace('static inline const std::shared_ptr<TrapCompletionPublicationFence> Default = std::make_shared<TrapCompletionPublicationFence>();','static const std::shared_ptr<TrapCompletionPublicationFence> Default;')
if(-not $s.Contains('TrapCompletionPublicationFence::Default =')) { $s += "`r`ninline const std::shared_ptr<YAKSys_Hybrid_CPU::Core::TrapCompletionPublicationFence> YAKSys_Hybrid_CPU::Core::TrapCompletionPublicationFence::Default = std::make_shared<YAKSys_Hybrid_CPU::Core::TrapCompletionPublicationFence>();`r`n" }
if(-not $s.Contains('#include <utility>')) { $s=$s.Replace('#include <memory>',"#include <memory>`r`n#include <utility>") }
[IO.File]::WriteAllText($p,$s)
