$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime'
$p=Join-Path $root 'Completion\Records\YAKSys_Hybrid_CPU.Core.CompletionRecord.h'; $s=[IO.File]::ReadAllText($p); $anchor='class CompletionRecord final'; if(-not $s.Contains('class VmxExitQualification;')) {$s=$s.Replace($anchor,"class VmxExitQualification;`r`n    class TrapCompletionPublicationFenceResult;`r`n    $anchor")}; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'Migration\Checkpoint\DomainHypercallLifecycleGate.h'; $s=[IO.File]::ReadAllText($p); $anchor='class DomainHypercallLifecycleGate final'; $v=@"
class DomainHypercallExactActivationRequest;
    struct DomainHypercallLifecycleSnapshot final {
        const unsigned long long DomainTag;
        const DomainHypercallLifecycleState State;
        const unsigned long long LifecycleEpoch;
        const int TransitionsInFlight;
        DomainHypercallLifecycleSnapshot(unsigned long long domainTag, DomainHypercallLifecycleState state, unsigned long long epoch, int inFlight)
            : DomainTag(domainTag), State(state), LifecycleEpoch(epoch), TransitionsInFlight(inFlight) {}
        bool acceptsNewE2() const noexcept { return State == DomainHypercallLifecycleState::ActiveExactProfile; }
        bool isTransitionQuiescent() const noexcept { return TransitionsInFlight == 0; }
    };

"@
if(-not $s.Contains('struct DomainHypercallLifecycleSnapshot final')) {$s=$s.Replace($anchor,$v+$anchor)}; [IO.File]::WriteAllText($p,$s)
