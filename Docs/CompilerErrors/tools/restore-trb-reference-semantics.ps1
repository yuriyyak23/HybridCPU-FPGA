$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Core\Execution\StreamEngine\BurstIO\TransactionReorderBuffer.h'; $s=Get-Content $p -Raw
$s=$s.Replace('const std::vector<TrbEntry> _entries;', 'std::vector<TrbEntry> _entries;').Replace('inline TrbEntry GetEntry(int index) const;', 'TrbEntry GetEntry(int index) const;')
$s=[regex]::Replace($s,'\s*TransactionReorderBuffer\(\) : _entries\(std::vector<TrbEntry>\(\)\)\s*\{\s*\}', '')
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Execution\StreamEngine\BurstIO\TransactionReorderBuffer.cpp'; $s=Get-Content $p -Raw
$s=$s.Replace('TrbEntry entry = _entries[entryIndex];','TrbEntry& entry = _entries[entryIndex];').Replace('TrbEntry head = _entries[_head];','TrbEntry& head = _entries[_head];').Replace('inline TrbEntry TransactionReorderBuffer::GetEntry','TransactionReorderBuffer::TrbEntry TransactionReorderBuffer::GetEntry')
Set-Content $p $s -Encoding UTF8 -NoNewline
