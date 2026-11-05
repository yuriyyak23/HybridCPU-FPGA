$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p = Join-Path $root 'Frontend\\Contracts\Contracts.h'
$s = Get-Content $p -Raw
if (!$s.Contains('struct RetainedGraphAccounting final')) {
 $marker = "`t// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:"
 $target = "`t// PC/transport serial/cursor are intentionally absent from reusable facts."
 # The record definitions are inserted after IngressOutcome so its type is complete.
 $anchor = "`t// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:"
 $idx = $s.IndexOf('//', $s.IndexOf('enum class IngressOutcome'))
 if ($idx -lt 0) { throw 'IngressOutcome insertion point missing' }
 $records = @"
    struct RetainedGraphAccounting final {
        int Entries, UniqueContexts, UniqueStrings;
        long long StringContentBytes, EncodedContentBytes, IntegrityContentBytes, ReferenceSlots, ModeledEntryBytes;
        std::wstring Scope;
    };
    struct IngressResult final {
        std::shared_ptr<BundleRequest> Placement;
        IngressOutcome Outcome;
        std::shared_ptr<DecodeResult> Facts;
        std::optional<std::wstring> AccessFault;
    };
"@
 $s = $s.Insert($idx, $records + "`r`n")
 Set-Content $p $s -Encoding UTF8 -NoNewline
}
$p = Join-Path $root 'Frontend\\Cache\SemanticBundleCache.h'
$s = Get-Content $p -Raw
if (!$s.Contains('struct CacheSnapshot final')) {
 $marker = "`tclass SemanticBundleCache final"
 $record = @"
    struct CacheSnapshot final {
        bool Enabled;
        int ResidentBytes;
        std::vector<int> Occupancy;
        std::shared_ptr<TelemetrySnapshot> Counters;
        ReplacementPolicy Policy;
        unsigned char ActiveVirtualThreads;
        std::shared_ptr<FreshnessSnapshot> Freshness;
    };
"@
 $s = $s.Replace($marker, $record + "`r`n" + $marker)
}
if (!$s.Contains('struct Entry final')) {
 $s = $s.Replace("`t`tstd::vector<std::optional<std::shared_ptr<Entry>>> entries;", "        struct Entry final { std::shared_ptr<LookupTag> Tag; std::shared_ptr<FrozenBytes> Encoded, Integrity; long long LastUse; int Footprint; };`r`n`t`tstd::vector<std::optional<std::shared_ptr<Entry>>> entries;")
}
if (!$s.Contains('#include <span>')) { $s = $s.Replace('#include <limits>', "#include <limits>`r`n#include <span>") }
$s = $s.Replace('int byteBudget = int::MaxValue', 'int byteBudget = std::numeric_limits<int>::max()').Replace('std::optional<std::shared_ptr<HotCandidate>> &candidate = std::nullopt', 'std::optional<std::shared_ptr<HotCandidate>> candidate = std::nullopt').Replace('std::optional<std::shared_ptr<HotInputs>> &hot = std::nullopt', 'std::optional<std::shared_ptr<HotInputs>> hot = std::nullopt').Replace('ReadOnlySpan<unsigned char> payload', 'std::span<const unsigned char> payload')
Set-Content $p $s -Encoding UTF8 -NoNewline
$p = Join-Path $root 'Frontend\\Cache\SemanticBundleCache.cpp'
$s = Get-Content $p -Raw
$s = $s.Replace('std::optional<std::shared_ptr<HotCandidate>> &candidate', 'std::optional<std::shared_ptr<HotCandidate>> candidate').Replace('std::optional<std::shared_ptr<HotInputs>> &hot', 'std::optional<std::shared_ptr<HotInputs>> hot').Replace('ReadOnlySpan<unsigned char> payload', 'std::span<const unsigned char> payload')
Set-Content $p $s -Encoding UTF8 -NoNewline
