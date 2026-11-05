$ErrorActionPreference='Stop'
$p=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'Frontend\\Cache\SemanticBundleCache.cpp'
$s=Get-Content $p -Raw
$a=$s.IndexOf("`tstd::shared_ptr<RetainedGraphAccounting> SemanticBundleCache::RetainedGraph("); $b=$s.IndexOf("`tstd::vector<unsigned char> SemanticBundleCache::EntryIntegrity(",$a)
if ($a -lt 0 -or $b -lt 0) { throw 'RetainedGraph boundaries missing' }
$body=@"
    std::shared_ptr<RetainedGraphAccounting> SemanticBundleCache::RetainedGraph() {
        FreshnessState::RejectCacheLockFromPublication();
        std::lock_guard lock(ingressGate);
        std::unordered_set<const DecodeContext*> contexts;
        int live = 0; long long encodedBytes = 0, integrityBytes = 0;
        for (const auto& item : entries) if (item && *item) {
            const auto& e = **item; ++live; contexts.insert(e.Tag->Context.get());
            encodedBytes += e.Encoded->getLength(); integrityBytes += e.Integrity->getLength();
        }
        // C++ contexts own std::wstring values, so count each distinct field object once.
        std::unordered_set<const std::wstring*> strings;
        for (const auto* c : contexts) {
            const std::wstring* fields[] = {&c->ManifestVersion, &c->ManifestHash, &c->Extensions, &c->DecoderEpoch, &c->DecoderVersion, &c->Privilege, &c->Domain, &c->AddressSpace, &c->VectorConfiguration, &c->ContextIncarnation, &c->SidebandIdentity};
            for (const auto* value : fields) strings.insert(value);
        }
        long long stringBytes = 0; for (const auto* value : strings) stringBytes += 2LL * value->size();
        const auto contextCount = static_cast<int>(contexts.size());
        return std::make_shared<RetainedGraphAccounting>(RetainedGraphAccounting{live, contextCount, static_cast<int>(strings.size()), stringBytes, encodedBytes, integrityBytes, live * 6LL + contextCount * 11LL + static_cast<long long>(entries.size()), getResidentBytes(), L"Reachable cache entry/tag/context/string/FrozenBytes/backing-array inventory; UTF16/byte content and modeled reference slots. CLR headers/alignment and shared provider/telemetry/delegates excluded; not exact heap bytes or SRAM."});
    }

"@
$s=$s.Substring(0,$a)+$body+$s.Substring($b)
Set-Content $p $s -Encoding UTF8 -NoNewline
