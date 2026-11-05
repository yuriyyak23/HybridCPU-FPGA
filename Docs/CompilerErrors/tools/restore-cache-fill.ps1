$ErrorActionPreference='Stop'
$p=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'Frontend\\Cache\SemanticBundleCache.cpp'
$s=Get-Content $p -Raw
$a=$s.IndexOf("`tvoid SemanticBundleCache::Fill("); $b=$s.IndexOf('    bool SemanticBundleCache::CorruptForExperiment(', $a)
if ($a -lt 0 -or $b -lt 0) { throw 'Fill boundaries missing' }
$body=@"
    void SemanticBundleCache::Fill(std::shared_ptr<LookupTag> tag, std::vector<unsigned char>& encoded) {
        int footprint = TagBytes(tag) + static_cast<int>(encoded.size()) + 32 + 16;
        if (footprint > ByteBudget) { ++telemetry->UnsupportedFills; return; }
        int start = SetStart(tag->Pc);
        std::vector<int> candidates; for (int i = start; i < start + ways; ++i) candidates.push_back(i);
        int index = Victim(candidates, tag->VirtualThread); Evict(index);
        while (getResidentBytes() + footprint > ByteBudget) {
            candidates.clear();
            for (int i = 0; i < static_cast<int>(entries.size()); ++i) if (entries[i]) candidates.push_back(i);
            Evict(Victim(candidates, tag->VirtualThread));
        }
        auto integrity = EntryIntegrity(tag, encoded);
        entries[index] = std::make_shared<Entry>(Entry{tag, std::make_shared<FrozenBytes>(encoded), std::make_shared<FrozenBytes>(integrity), Tick(), footprint});
        ++telemetry->Fills; telemetry->FillBytesWritten += footprint;
    }

"@
$s=$s.Substring(0,$a)+$body+$s.Substring($b)
Set-Content $p $s -Encoding UTF8 -NoNewline
