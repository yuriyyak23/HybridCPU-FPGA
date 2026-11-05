$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Events\Hypercalls\DomainHypercallRuntimeExecutor.h'; $s=[IO.File]::ReadAllText($p)
$anchor='class DomainHypercallRuntimeExecutor final'; $decl="class DomainHypercallExecutionResult;`r`n    class DomainHypercallReceiptValidationResult;`r`n    class DomainHypercallCompletionOwner;`r`n    "
if(-not $s.Contains('class DomainHypercallExecutionResult;')) {$s=$s.Replace($anchor,$decl+$anchor)}
$block=@"
namespace YAKSys_Hybrid_CPU::Core {
struct DomainHypercallExecutionResult final {
    const DomainHypercallExecutionDecision Decision;
    const std::shared_ptr<DomainHypercallRuntimeExecutor::ExecutionReceipt> Receipt;
    const std::wstring Reason;
    DomainHypercallExecutionResult(DomainHypercallExecutionDecision decision, std::shared_ptr<DomainHypercallRuntimeExecutor::ExecutionReceipt> receipt, std::wstring reason)
        : Decision(decision), Receipt(std::move(receipt)), Reason(std::move(reason)) {}
    bool isExecuted() const noexcept { return Decision == DomainHypercallExecutionDecision::Executed && Receipt != nullptr; }
    bool completionPublicationAuthorized() const noexcept { return false; }
    bool retirePublicationAuthorized() const noexcept { return false; }
};
struct DomainHypercallReceiptValidationResult final {
    const DomainHypercallReceiptValidationDecision Decision;
    const std::wstring Reason;
    DomainHypercallReceiptValidationResult(DomainHypercallReceiptValidationDecision decision, std::wstring reason)
        : Decision(decision), Reason(std::move(reason)) {}
    bool isValid() const noexcept { return Decision == DomainHypercallReceiptValidationDecision::Valid; }
};
}
"@
if(-not $s.Contains('struct DomainHypercallExecutionResult final')) {$s += "`r`n"+$block}; if(-not $s.Contains('#include <utility>')) {$s=$s.Replace('#include <memory>',"#include <memory>`r`n#include <utility>")}; [IO.File]::WriteAllText($p,$s)
