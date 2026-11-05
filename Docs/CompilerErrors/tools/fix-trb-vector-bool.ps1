$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Core\Execution\StreamEngine\BurstIO\TransactionReorderBuffer.h'; $s=Get-Content $p -Raw
if (!$s.Contains('FindNextIssuable(const std::vector<bool>& bankBusy)')) { $s=$s.Replace('int FindNextIssuable(std::span<const bool> bankBusy);', "int FindNextIssuable(std::span<const bool> bankBusy);`r`n        int FindNextIssuable(const std::vector<bool>& bankBusy);`r`n    private:`r`n        template<class Flags> int FindNextIssuableImpl(const Flags& bankBusy);`r`n    public:") }
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Execution\StreamEngine\BurstIO\TransactionReorderBuffer.cpp'; $s=Get-Content $p -Raw
$old='int TransactionReorderBuffer::FindNextIssuable(std::span<const bool> bankBusy)'; $new=@"
    int TransactionReorderBuffer::FindNextIssuable(std::span<const bool> bankBusy) { return FindNextIssuableImpl(bankBusy); }
    int TransactionReorderBuffer::FindNextIssuable(const std::vector<bool>& bankBusy) { return FindNextIssuableImpl(bankBusy); }
    template<class Flags>
    int TransactionReorderBuffer::FindNextIssuableImpl(const Flags& bankBusy)
"@
if ($s.Contains($old)) { $s=$s.Replace($old,$new) }
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Memory\Subsystem\YAKSys_Hybrid_CPU.Memory.MemorySubsystem.cpp'; $s=Get-Content $p -Raw
$s=$s.Replace('ReadOnlySpan<bool> bankBusySpan = bankOccupied.AsSpan();', 'const auto& bankBusySpan = bankOccupied;')
Set-Content $p $s -Encoding UTF8 -NoNewline
