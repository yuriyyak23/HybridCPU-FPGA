$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Commit\ArchitecturalCompletionCommitOwner.h'; $s=[IO.File]::ReadAllText($p)
$pat='(?m)^\s*//\s*private sealed record LiveReceipt\(ArchitecturalCompletionCommitReceipt Receipt, string Seal\);'; $def=@"
        struct LiveReceipt final {
            const std::shared_ptr<ArchitecturalCompletionCommitReceipt> Receipt;
            const std::wstring Seal;
            LiveReceipt(std::shared_ptr<ArchitecturalCompletionCommitReceipt> receipt, std::wstring seal) : Receipt(std::move(receipt)), Seal(std::move(seal)) {}
        };
"@; if(-not $s.Contains('struct LiveReceipt final')) {if(-not [regex]::IsMatch($s,$pat)){throw 'receipt anchor missing'}; $s=[regex]::Replace($s,$pat,$def)}
$pat='(?m)^\s*//\s*private readonly record struct CompletionIdentityKey\(ulong ProducerOwnerIdentity, ulong ProducerOwnerEpoch, ulong DomainId, int ContextId, int VirtualThreadId, ulong AttemptId, ulong EventId, ulong MemoryOperationId\);'; $def=@"
        struct CompletionIdentityKey final {
            unsigned long long ProducerOwnerIdentity = 0, ProducerOwnerEpoch = 0, DomainId = 0;
            int ContextId = 0, VirtualThreadId = 0;
            unsigned long long AttemptId = 0, EventId = 0, MemoryOperationId = 0;
            bool operator==(const CompletionIdentityKey&) const = default;
        };
"@; if(-not $s.Contains('struct CompletionIdentityKey final')) {if(-not [regex]::IsMatch($s,$pat)){throw 'key anchor missing'}; $s=[regex]::Replace($s,$pat,$def)}; if(-not $s.Contains('#include <utility>')) {$s=$s.Replace('#include <memory>',"#include <memory>`r`n#include <utility>")}; [IO.File]::WriteAllText($p,$s)
