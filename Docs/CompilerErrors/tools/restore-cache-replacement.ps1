$ErrorActionPreference='Stop'
$p=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'Frontend\\Cache\SemanticBundleCache.cpp'
$s=Get-Content $p -Raw
$a=$s.IndexOf("`tint SemanticBundleCache::Victim("); $b=$s.IndexOf("`tvoid SemanticBundleCache::Fill(", $a)
if ($a -lt 0 -or $b -lt 0) { throw 'Replacement methods not found' }
$body=@"
    int SemanticBundleCache::Victim(std::vector<int>& indices, int requestor) {
        std::vector<int> candidates = indices;
        for (int i : candidates) if (!entries[i]) return i;
        auto occupancy = getOccupancy();
        int active = 0; for (int i = 0; i < 4; ++i) if (ActiveVirtualThreads & (1 << i)) ++active;
        int quota = static_cast<int>(entries.size()) / active;
        if (Policy == ReplacementPolicy::QuotaBorrow) {
            std::vector<int> preferred;
            for (int i : candidates) {
                int vt = (*entries[i])->Tag->VirtualThread;
                if (!(ActiveVirtualThreads & (1 << vt)) || occupancy[vt] > quota) preferred.push_back(i);
            }
            if (!preferred.empty()) candidates = std::move(preferred);
            else {
                std::vector<int> own;
                for (int i : candidates) if ((*entries[i])->Tag->VirtualThread == requestor) own.push_back(i);
                if (!own.empty()) candidates = std::move(own);
            }
        }
        return *std::min_element(candidates.begin(), candidates.end(), [this](int a, int b) {
            auto left = (*entries[a])->LastUse, right = (*entries[b])->LastUse;
            return left < right || (left == right && a < b);
        });
    }
    void SemanticBundleCache::Evict(int index) {
        if (entries[index]) ++telemetry->EvictionsByVt[(*entries[index])->Tag->VirtualThread];
        entries[index] = std::nullopt;
    }

"@
$s=$s.Substring(0,$a)+$body+$s.Substring($b)
$a=$s.IndexOf("`tbool SemanticBundleCache::CorruptForExperiment("); if($a -lt 0){throw 'Corruption methods not found'}
$body=@"
    bool SemanticBundleCache::CorruptForExperiment(unsigned long long pc) {
        FreshnessState::RejectCacheLockFromPublication(); std::lock_guard lock(ingressGate);
        for (auto& item : entries) if (item && (*item)->Tag->Pc == pc) {
            auto old = *item; auto bad = old->Encoded->Copy();
            if (bad.empty()) throw std::out_of_range("encoded entry is empty");
            bad.back() ^= 0x80;
            item = std::make_shared<Entry>(Entry{old->Tag, std::make_shared<FrozenBytes>(bad), old->Integrity, old->LastUse, old->Footprint});
            return true;
        }
        return false;
    }
    bool SemanticBundleCache::CorruptTagForExperiment(unsigned long long pc) {
        FreshnessState::RejectCacheLockFromPublication(); std::lock_guard lock(ingressGate);
        for (auto& item : entries) if (item && (*item)->Tag->Pc == pc) {
            auto old = *item; auto tag = std::make_shared<LookupTag>(*old->Tag);
            tag->VirtualThread = (tag->VirtualThread + 1) % 4;
            item = std::make_shared<Entry>(Entry{tag, old->Encoded, old->Integrity, old->LastUse, old->Footprint});
            return true;
        }
        return false;
    }
}
"@
$s=$s.Substring(0,$a)+$body
$s=$s.Replace('return new(r, IngressOutcome::AccessFault, nullptr, a->Fault);', 'return std::make_shared<IngressResult>(IngressResult{r, IngressOutcome::AccessFault, nullptr, a->Fault});').Replace('Array::Clear(entries);', 'std::fill(entries.begin(), entries.end(), std::nullopt);')
Set-Content $p $s -Encoding UTF8 -NoNewline
