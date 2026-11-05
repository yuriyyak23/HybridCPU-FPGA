$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Records'
$p=Join-Path $base 'TrapCompletionPublicationFence.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include <string>',"#include `"YAKSys_Hybrid_CPU.Core.CompletionRecord.h`"`r`n#include <string>")
$anchor='class TrapCompletionPublicationFenceResult;'; if(-not $s.Contains($anchor)){throw 'forward anchor missing'}
$def=@'
class NeutralTrapResult;
class TrapCompletionPublicationFenceResult final {
public:
    const TrapCompletionPublicationDecision Decision;
    const std::shared_ptr<NeutralTrapResult> TrapResult;
    const std::shared_ptr<CompletionRecord> Completion;
    const bool RetirePublicationAuthorized;
    const std::wstring Reason;
    const bool CompletionPublicationAllowed, RetirePublicationAllowed, IsDenied, IsCompletionOnly, IsRetireDenied, DeniesBackendExecution;
    TrapCompletionPublicationFenceResult(TrapCompletionPublicationDecision decision, std::shared_ptr<NeutralTrapResult> trap, std::shared_ptr<CompletionRecord> completion, bool retire, std::wstring reason)
        : Decision(decision), TrapResult(std::move(trap)), Completion(std::move(completion)), RetirePublicationAuthorized(retire), Reason(std::move(reason)),
          CompletionPublicationAllowed((decision == TrapCompletionPublicationDecision::Allowed || decision == TrapCompletionPublicationDecision::CompletionPublishedRetireDenied) && Completion && !Completion->isEmpty()),
          RetirePublicationAllowed(decision == TrapCompletionPublicationDecision::Allowed && retire), IsDenied(!CompletionPublicationAllowed),
          IsCompletionOnly(decision == TrapCompletionPublicationDecision::CompletionPublishedRetireDenied && CompletionPublicationAllowed && !RetirePublicationAllowed),
          IsRetireDenied(CompletionPublicationAllowed && !RetirePublicationAllowed), DeniesBackendExecution(decision == TrapCompletionPublicationDecision::DeniedBackendExecution) {}
};
'@
$s=$s.Replace($anchor,$def); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.CompletionRecord.cpp'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('#include "YAKSys_Hybrid_CPU.Core.CompletionRecord.h"', "#include `"YAKSys_Hybrid_CPU.Core.CompletionRecord.h`"`r`n#include `"TrapCompletionPublicationFence.h`"`r`n#include `"../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmxExitQualification.h`""); [IO.File]::WriteAllText($p,$s)