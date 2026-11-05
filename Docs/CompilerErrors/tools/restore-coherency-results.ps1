$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Memory'
$p=Join-Path $r 'DataCacheRangeResults.h'
if(Test-Path -LiteralPath $p){throw 'Results header already exists'}
$lines=@(
'#pragma once',
'namespace YAKSys_Hybrid_CPU::Core::Memory {',
'struct DataCacheRangeInvalidationResult final {',
'    bool RangeWellFormed = false;',
'    int L1LinesInvalidated = 0, L2LinesInvalidated = 0, AssistResidentLinesInvalidated = 0;',
'    int getTotalLinesInvalidated() const noexcept { return L1LinesInvalidated + L2LinesInvalidated; }',
'    bool getSucceeded() const noexcept { return RangeWellFormed; }',
'    static const DataCacheRangeInvalidationResult Empty;',
'};',
'inline const DataCacheRangeInvalidationResult DataCacheRangeInvalidationResult::Empty{true, 0, 0, 0};',
'struct DataCacheRangeFlushResult final {',
'    bool RangeWellFormed = false, IsNoOpProof = false, RequiresFutureDirtyWriteback = false;',
'    int CleanLinesObserved = 0, L1DirtyLines = 0, L2DirtyLines = 0;',
'    int getDirtyLinesObserved() const noexcept { return L1DirtyLines + L2DirtyLines; }',
'    bool getSucceeded() const noexcept { return RangeWellFormed && !RequiresFutureDirtyWriteback; }',
'    DataCacheRangeFlushResult Merge(const DataCacheRangeFlushResult& other) const noexcept {',
'        return {RangeWellFormed && other.RangeWellFormed, IsNoOpProof && other.IsNoOpProof,',
'            RequiresFutureDirtyWriteback || other.RequiresFutureDirtyWriteback, CleanLinesObserved + other.CleanLinesObserved,',
'            L1DirtyLines + other.L1DirtyLines, L2DirtyLines + other.L2DirtyLines};',
'    }',
'    static const DataCacheRangeFlushResult EmptyNoOpProof;',
'};',
'inline const DataCacheRangeFlushResult DataCacheRangeFlushResult::EmptyNoOpProof{true, true, false, 0, 0, 0};',
'struct MemoryCoherencyObserverResult final {',
'    bool NotificationWellFormed = false;',
'    int DataCacheLinesInvalidated = 0, AssistResidentLinesInvalidated = 0, SrfWindowsInvalidated = 0;',
'    bool getSucceeded() const noexcept { return NotificationWellFormed; }',
'    static const MemoryCoherencyObserverResult Empty;',
'};',
'inline const MemoryCoherencyObserverResult MemoryCoherencyObserverResult::Empty{true, 0, 0, 0};',
'}',
'') -join "`r`n"
[IO.File]::WriteAllText($p,$lines,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'MemoryCoherencyObserver.h'; $s=[IO.File]::ReadAllText($p); $m='#include <vector>'
if(-not $s.Contains($m)){throw 'Expected include missing'}
$s=$s.Replace($m,"#include `"DataCacheRangeResults.h`"`r`n"+$m); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
