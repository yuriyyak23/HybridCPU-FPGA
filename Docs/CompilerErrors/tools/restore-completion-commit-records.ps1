$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Commit\ArchitecturalCompletionCommitOwner.h'; $s=[IO.File]::ReadAllText($p); $a='class ArchitecturalCompletionCommitOwner final'; $def=@"
class ArchitecturalCompletionCommitResult;
class ArchitecturalCompletionBatchPreparationResult;
struct ArchitecturalCompletionProducerPolicy final {
    std::wstring OwnerName; NeutralArchitecturalCompletionClass CompletionClass = NeutralArchitecturalCompletionClass::None;
    bool RequiresReason = false, AllowsQualification = false;
    NeutralFaultAddressSemantic AllowedAddressSemantic = NeutralFaultAddressSemantic::None;
    NeutralFaultAuxiliarySemantic AllowedAuxiliarySemantic = NeutralFaultAuxiliarySemantic::None;
    bool RequiresTranslationProvenance = false, RequiresSecondStageProvenance = false;
};
struct ArchitecturalCompletionReceiptBinding final {
    unsigned long long CompletionIdentity = 0, ProducerOwnerIdentity = 0, ProducerOwnerEpoch = 0, DomainId = 0;
    int ContextId = 0, VirtualThreadId = 0;
    unsigned long long AttemptId = 0, EventId = 0;
    NeutralTranslationProvenance TranslationProvenance;
    NeutralArchitecturalCompletionClass CompletionClass = NeutralArchitecturalCompletionClass::None;
    std::wstring CompletionDigest;
    unsigned long long CanonicalOrderSequence = 0, CommitSequence = 0, RestoreGeneration = 0;
};

"@; if(-not $s.Contains('struct ArchitecturalCompletionProducerPolicy final')) {$s=$s.Replace($a,$def+$a)}; $def=@"
namespace YAKSys_Hybrid_CPU::Core {
struct ArchitecturalCompletionCommitResult final {
    ArchitecturalCompletionCommitDecision Decision;
    std::shared_ptr<ArchitecturalCompletionCommitOwner::ArchitecturalCompletionCommitReceipt> Receipt;
    std::wstring Reason;
    bool isCommitted() const noexcept { return Decision == ArchitecturalCompletionCommitDecision::Committed && Receipt != nullptr; }
};
struct ArchitecturalCompletionBatchPreparationResult final {
    ArchitecturalCompletionCommitDecision Decision;
    std::shared_ptr<ArchitecturalCompletionCommitOwner::CanonicalRetireWindowCommitLease> Lease;
    std::wstring Reason;
    bool isPrepared() const noexcept { return Decision == ArchitecturalCompletionCommitDecision::Committed && Lease != nullptr; }
};
}
"@; if(-not $s.Contains('struct ArchitecturalCompletionCommitResult final')) {$s += "`r`n"+$def}; [IO.File]::WriteAllText($p,$s)
