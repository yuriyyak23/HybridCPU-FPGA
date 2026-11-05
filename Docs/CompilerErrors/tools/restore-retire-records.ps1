$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Records\DomainHypercallRetireOwner.h'; $s=[IO.File]::ReadAllText($p); $anchor='class DomainHypercallRetireOwner final'; $def=@"
class VmxMicroOp;
class DomainHypercallRetireResult;
struct DomainHypercallRetireEligibility final {
    std::shared_ptr<VmxMicroOp> Carrier;
    int VirtualThreadId = 0; unsigned long long DomainTag = 0;
    int SourceSlotId = 0, WorkingSlotId = 0;
    unsigned long long WorkingBundleSequence = 0, OperationAttempt = 0;
    int PhysicalLaneId = 0; unsigned char RetireOrderIndex = 0;
    unsigned long long RetireWindowIdentity = 0, OrderEpoch = 0;
    bool IsCanonicalHead = false, IsSquashed = false, HasWinningException = false;
};

"@; if(-not $s.Contains('struct DomainHypercallRetireEligibility final')) {$s=$s.Replace($anchor,$def+$anchor)}
$pat='(?m)^\s*//\s*private sealed record LiveGrant\(string Digest\);'; if(-not $s.Contains('struct LiveGrant final')) {if(-not [regex]::IsMatch($s,$pat)){throw 'LiveGrant anchor missing'}; $s=[regex]::Replace($s,$pat,"`r`n        struct LiveGrant final { const std::wstring Digest; explicit LiveGrant(std::wstring digest) : Digest(std::move(digest)) {} };")}
$def=@"
namespace YAKSys_Hybrid_CPU::Core {
struct DomainHypercallRetireResult final {
    const DomainHypercallRetireDecision Decision;
    const std::shared_ptr<DomainHypercallRetireOwner::VirtualizationRetireGrant> E6;
    const std::wstring Reason;
    DomainHypercallRetireResult(DomainHypercallRetireDecision decision, std::shared_ptr<DomainHypercallRetireOwner::VirtualizationRetireGrant> grant, std::wstring reason) : Decision(decision), E6(std::move(grant)), Reason(std::move(reason)) {}
    bool isIssued() const noexcept { return Decision == DomainHypercallRetireDecision::Issued && E6 != nullptr; }
};
}
"@; if(-not $s.Contains('struct DomainHypercallRetireResult final')) {$s += "`r`n"+$def}; if(-not $s.Contains('#include <utility>')) {$s=$s.Replace('#include <memory>',"#include <memory>`r`n#include <utility>")}; [IO.File]::WriteAllText($p,$s)
