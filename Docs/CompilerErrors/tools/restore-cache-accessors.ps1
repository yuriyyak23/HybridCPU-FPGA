$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$header=Join-Path $root 'Frontend\\Cache\SemanticBundleCache.h'
$s=Get-Content $header -Raw
$s=$s.Replace('std::mutex ingressGate;', 'mutable std::recursive_mutex ingressGate;')
Set-Content $header $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'Frontend\\Cache\SemanticBundleCache.cpp'
$s=Get-Content $p -Raw
$s=$s.Replace('std::scoped_lock<std::mutex> lock(ingressGate)', 'std::scoped_lock<std::recursive_mutex> lock(ingressGate)')
$from=$s.IndexOf("`tint SemanticBundleCache::getResidentBytes() const")
$to=$s.IndexOf("`tstd::shared_ptr<IngressResult> SemanticBundleCache::Request", $from)
if ($from -lt 0 -or $to -lt 0) { throw 'Cache accessor boundaries not found' }
$body=@"
    int SemanticBundleCache::getResidentBytes() const {
        FreshnessState::RejectCacheLockFromPublication();
        std::lock_guard lock(ingressGate);
        int total = 0;
        for (const auto& entry : entries) if (entry && *entry) total += (*entry)->Footprint;
        return total;
    }
    std::vector<int> SemanticBundleCache::getOccupancy() const {
        FreshnessState::RejectCacheLockFromPublication();
        std::lock_guard lock(ingressGate);
        std::vector<int> result(4);
        for (const auto& entry : entries) if (entry && *entry) { auto vt = (*entry)->Tag->VirtualThread; if (vt >= 0 && vt < 4) ++result[vt]; }
        return result;
    }
    std::shared_ptr<CacheSnapshot> SemanticBundleCache::Snapshot() {
        FreshnessState::RejectCacheLockFromPublication();
        std::lock_guard ingress(ingressGate);
        std::lock_guard state(freshness->getGate());
        return std::make_shared<CacheSnapshot>(CacheSnapshot{enabled, getResidentBytes(), getOccupancy(), telemetry->Snapshot(), Policy, ActiveVirtualThreads, freshness->Snapshot()});
    }
    SemanticBundleCache::SemanticBundleCache(std::shared_ptr<FreshnessState> freshness, std::function<std::shared_ptr<DecodeContext>(std::shared_ptr<BundleRequest>)> contextProvider, std::shared_ptr<ICanonicalSource> source, std::shared_ptr<ICanonicalDecoder> decoder, int capacity, int ways, int byteBudget)
        : ByteBudget(byteBudget), entries(capacity > 0 ? capacity : 0), ways(ways), freshness(std::move(freshness)), contextProvider(std::move(contextProvider)), source(std::move(source)), decoder(std::move(decoder)) {
        if (capacity < ways || ways < 1 || capacity % ways != 0 || byteBudget <= 0) throw std::out_of_range("capacity");
    }
    void SemanticBundleCache::Clear() {
        FreshnessState::RejectCacheLockFromPublication(); std::lock_guard lock(ingressGate);
        std::fill(entries.begin(), entries.end(), std::nullopt);
    }
    void SemanticBundleCache::SetPolicy(ReplacementPolicy policy, unsigned char activeVts) {
        FreshnessState::RejectCacheLockFromPublication();
        if (activeVts == 0 || activeVts > 15) throw std::out_of_range("activeVts");
        std::lock_guard lock(ingressGate); Policy = policy; ActiveVirtualThreads = activeVts;
        std::fill(entries.begin(), entries.end(), std::nullopt);
    }
    void SemanticBundleCache::SetActiveVirtualThreads(unsigned char mask) {
        FreshnessState::RejectCacheLockFromPublication();
        if (mask == 0 || mask > 15) throw std::out_of_range("mask");
        std::lock_guard lock(ingressGate); ActiveVirtualThreads = mask;
    }
    std::shared_ptr<LookupTag> SemanticBundleCache::CaptureTag(std::shared_ptr<BundleRequest> request) {
        request->Validate(); std::lock_guard lock(freshness->getGate());
        return std::make_shared<LookupTag>(LookupTag{request->Pc, request->VirtualThread, contextProvider(request), *freshness->Capture()});
    }

"@
$s=$s.Substring(0,$from)+$body+$s.Substring($to)
$s=$s.Replace('#include "SemanticBundleCache.h"', "#include `"SemanticBundleCache.h`"`r`n#include <algorithm>`r`n#include <stdexcept>`r`n#include <utility>")
Set-Content $p $s -Encoding UTF8 -NoNewline
