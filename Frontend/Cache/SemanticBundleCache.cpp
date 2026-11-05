#include "SemanticBundleCache.h"
#include <algorithm>
#include <chrono>
#define NOMINMAX
#include <windows.h>
#include <bcrypt.h>
#include <string>
#pragma comment(lib, "bcrypt.lib")
#include <stdexcept>
#include <utility>
#include "../Replay/HotCompatibility.h"
#include "../Semantic/CompactCodec.h"

namespace HybridCpu::ShadowFrontend
{

	std::shared_ptr<TelemetrySnapshot> SemanticBundleCache::getTelemetry() const
	{
		FreshnessState::RejectCacheLockFromPublication();
		{
		std::scoped_lock<std::recursive_mutex> lock(ingressGate);
			return telemetry->Snapshot();
		}
	}

	bool SemanticBundleCache::getEnabled() const
	{
		FreshnessState::RejectCacheLockFromPublication();
		{
		std::scoped_lock<std::recursive_mutex> lock(ingressGate);
			return enabled;
		}
	}

	void SemanticBundleCache::setEnabled(bool value)
	{
		FreshnessState::RejectCacheLockFromPublication();
		{
		std::scoped_lock<std::recursive_mutex> lock(ingressGate);
			enabled = value;
		}
	}

	int SemanticBundleCache::getByteBudget() const
	{
		return ByteBudget;
	}

	unsigned char SemanticBundleCache::getActiveVirtualThreads() const
	{
		return ActiveVirtualThreads;
	}

	void SemanticBundleCache::setActiveVirtualThreads(unsigned char value)
	{
		ActiveVirtualThreads = value;
	}

	ReplacementPolicy SemanticBundleCache::getPolicy() const
	{
		return Policy;
	}

	void SemanticBundleCache::setPolicy(ReplacementPolicy value)
	{
		Policy = value;
	}

	int SemanticBundleCache::getCapacity() const
	{
		return entries.size();
	}

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
    static void AppendJsonString(std::string& out, const std::wstring& text) {
        constexpr char hex[] = "0123456789ABCDEF"; out.push_back('"');
        for (wchar_t ch : text) {
            unsigned int c = static_cast<unsigned int>(ch);
            if (c == 92) { out.push_back(92); out.push_back(92); continue; }
            char esc = 0;
            switch (c) { case 8: esc = 'b'; break; case 9: esc = 't'; break; case 10: esc = 'n'; break; case 12: esc = 'f'; break; case 13: esc = 'r'; break; }
            if (esc) { out.push_back(92); out.push_back(esc); continue; }
            if (c < 32 || c > 126 || c == 34 || c == 38 || c == 39 || c == 43 || c == 60 || c == 62 || c == 96) {
                out.push_back(92); out.push_back('u'); for (int shift = 12; shift >= 0; shift -= 4) out.push_back(hex[(c >> shift) & 15]);
            } else out.push_back(static_cast<char>(c));
        }
        out.push_back('"');
    }
    static std::string BindingJson(const HotBinding& b) {
        std::string out = "{\"Structural\":"; AppendJsonString(out,b.Structural);
        out += ",\"Certificate\":"; AppendJsonString(out,b.Certificate);
        out += ",\"Owner\":"; AppendJsonString(out,b.Owner);
        out += ",\"Boundary\":"; AppendJsonString(out,b.Boundary);
        out += ",\"Serializing\":" + std::to_string(b.Serializing) + ",\"Phase\":" + std::to_string(b.Phase) + ",\"Cursor\":" + std::to_string(b.Cursor) + "}";
        return out;
    }
    int BindingJsonSize(const HotBinding& b) { return static_cast<int>(BindingJson(b).size()); }
    static std::string TagJson(const LookupTag& tag) {
        const auto& c = *tag.Context;
        std::string out = "{\"Pc\":" + std::to_string(tag.Pc) + ",\"VirtualThread\":" + std::to_string(tag.VirtualThread) + ",\"Context\":{\"ManifestVersion\":";
        AppendJsonString(out,c.ManifestVersion); out += ",\"ManifestHash\":"; AppendJsonString(out,c.ManifestHash);
        out += ",\"Extensions\":"; AppendJsonString(out,c.Extensions); out += ",\"DecoderEpoch\":"; AppendJsonString(out,c.DecoderEpoch);
        out += ",\"DecoderVersion\":"; AppendJsonString(out,c.DecoderVersion); out += ",\"Privilege\":"; AppendJsonString(out,c.Privilege);
        out += ",\"Domain\":"; AppendJsonString(out,c.Domain); out += ",\"AddressSpace\":"; AppendJsonString(out,c.AddressSpace);
        out += ",\"VectorConfiguration\":"; AppendJsonString(out,c.VectorConfiguration);
        out += ",\"ExecutableEpoch\":" + std::to_string(c.ExecutableEpoch) + ",\"CodeEpoch\":" + std::to_string(c.CodeEpoch) + ",\"ReplayEligible\":" + (c.ReplayEligible ? "true" : "false");
        out += ",\"ContextIncarnation\":"; AppendJsonString(out,c.ContextIncarnation); out += ",\"SidebandIdentity\":"; AppendJsonString(out,c.SidebandIdentity);
        out += ",\"IsBound\":"; out += c.isBound() ? "true" : "false";
        out += "},\"Freshness\":{\"Incarnation\":" + std::to_string(tag.Freshness.Incarnation) + ",\"Generation\":" + std::to_string(tag.Freshness.Generation) + "}}";
        return out;
    }
    int BindingJsonSize(const HotBinding& binding);
    static bool SameTag(const LookupTag& a, const LookupTag& b) {
        return a.Pc == b.Pc && a.VirtualThread == b.VirtualThread && a.Freshness == b.Freshness && *a.Context == *b.Context;
    }
    std::shared_ptr<IngressResult> SemanticBundleCache::Request(std::shared_ptr<BundleRequest> request, std::optional<std::shared_ptr<HotCandidate>> candidate, std::optional<std::shared_ptr<HotInputs>> hot, int maxRetries) {
        FreshnessState::RejectCacheLockFromPublication(); request->Validate();
        if (maxRetries < 0 || maxRetries > 16) throw std::out_of_range("maxRetries");
        using Clock = std::chrono::steady_clock;
        auto waitStart = Clock::now(); std::lock_guard lock(ingressGate); auto acquired = Clock::now();
        telemetry->HostIngressWaitTicks += (acquired - waitStart).count();
        struct HoldTimer { FrontendTelemetry& telemetry; Clock::time_point acquired; ~HoldTimer() { telemetry.HostIngressHoldTicks += (Clock::now() - acquired).count(); } } timer{*telemetry, acquired};
        ++telemetry->Requests;
        for (int retry = 0; retry <= maxRetries; ++retry) {
            auto tag = CaptureTag(request); ++telemetry->PermissionChecks;
            auto access = source->CheckAccess(request, tag->Context);
            if (!access->Allowed) {
                std::lock_guard gate(freshness->getGate());
                if (!SameTag(*CaptureTag(request), *tag)) { ++telemetry->Retries; continue; }
                return Fault(request, access);
            }
            std::shared_ptr<DecodeResult> facts; auto outcome = IngressOutcome::Cold;
            std::optional<std::vector<unsigned char>> fill;
            if (enabled && candidate && *candidate && hot && *hot) {
                auto c = *candidate; auto h = *hot; ++telemetry->HotCandidates;
                telemetry->TagGuardBytesRead += TagBytes(c->Tag) + BindingJsonSize(*c->Binding);
                auto reject = h->Current->Cursor != request->Cursor ? std::optional<HotGuard>{HotGuard::Cursor} : HotCompatibility::Reject(c, tag, freshness->getReplayGeneration(), h, [this](HotGuard) { ++telemetry->HotGuardChecks; });
                if (h->ExpectedIdentity) telemetry->HotIdentityBytesRead += IdentityBytes(c->Identity) + IdentityBytes(h->ExpectedIdentity);
                if (!reject) { facts = c->Identity->getResult(); outcome = IngressOutcome::Hot; }
                else ++telemetry->getGuardRejects()[*reject];
            }
            if (!facts && enabled && tag->Context->isBound()) {
                ++telemetry->DbcLookups; int start = SetStart(tag->Pc);
                for (int i = start; i < start + ways; ++i) {
                    if (!entries[i]) continue;
                    auto entry = *entries[i]; int storedTagBytes = entry->Footprint - entry->Encoded->getLength() - 32 - 16;
                    telemetry->TagGuardBytesRead += storedTagBytes;
                    if (!SameTag(*entry->Tag, *tag)) continue;
                    ++telemetry->LookupIntegrityChecks;
                    telemetry->IntegrityBytesRead += entry->Encoded->getLength() + entry->Integrity->getLength() + storedTagBytes;
                    auto digest = EntryIntegrity(entry->Tag, entry->Encoded->getSpan()); auto integrity = entry->Integrity->getSpan();
                    if (digest.size() != integrity.size() || !std::equal(digest.begin(), digest.end(), integrity.begin())) { entries[i] = std::nullopt; ++telemetry->CorruptEntries; continue; }
                    ++telemetry->RestoreAttempts;
                    if (!CompactCodec::TryDecode(entry->Encoded->getSpan(), facts)) { entries[i] = std::nullopt; facts.reset(); ++telemetry->CorruptEntries; break; }
                    telemetry->SemanticPayloadBytesRead += entry->Encoded->getLength();
                    auto updated = std::make_shared<Entry>(*entry); updated->LastUse = Tick(); entries[i] = updated;
                    outcome = IngressOutcome::Warm; break;
                }
            }
            if (!facts) {
                ++telemetry->SemanticMisses; ++telemetry->MissesByVt[request->VirtualThread]; ++telemetry->FetchAttempts;
                auto fetched = source->FetchFullBundle(request, tag->Context);
                if (fetched->Fault) {
                    std::lock_guard gate(freshness->getGate());
                    if (!SameTag(*CaptureTag(request), *tag)) { ++telemetry->Retries; continue; }
                    return Fault(request, std::make_shared<AccessDecision>(AccessDecision{false, fetched->Fault}));
                }
                auto raw = fetched->Raw; if (!raw) throw std::logic_error("Canonical source returned neither raw bytes nor fault.");
                if (raw->getLength() != 256) throw std::logic_error("Canonical source violated full-bundle contract.");
                ++telemetry->RawBundleFetches; telemetry->CanonicalBytesRead += 256; ++telemetry->DecoderCalls;
                facts = decoder->Decode(raw, request, tag->Context);
                if (!facts->getRaw()->Equals(raw)) throw std::logic_error("Canonical decoder raw identity mismatch.");
                if (enabled && tag->Context->isBound()) { fill = CompactCodec::Encode(facts); if (!fill) ++telemetry->UnsupportedFills; }
            }
            std::lock_guard gate(freshness->getGate()); ++telemetry->PermissionChecks;
            access = source->CheckAccess(request, tag->Context);
            if (!SameTag(*CaptureTag(request), *tag) || (outcome == IngressOutcome::Hot && (*candidate)->ReplayGeneration != freshness->getReplayGeneration())) { ++telemetry->Retries; continue; }
            if (!access->Allowed) return Fault(request, access);
            if (fill) Fill(tag, *fill);
            switch (outcome) {
                case IngressOutcome::Cold: ++telemetry->ColdIngress; break;
                case IngressOutcome::Warm: ++telemetry->SemanticHits; ++telemetry->HitsByVt[request->VirtualThread]; ++telemetry->DecodeBypassCount; break;
                case IngressOutcome::Hot: ++telemetry->ValidatedHotHits; ++telemetry->DecodeBypassCount; break;
                default: break;
            }
            if (!facts->isSuccess()) ++telemetry->DecodeFaults;
            return std::make_shared<IngressResult>(IngressResult{request, outcome, facts, std::nullopt});
        }
        ++telemetry->RetryExhausted;
        return std::make_shared<IngressResult>(IngressResult{request, IngressOutcome::RetryExhausted, nullptr, std::nullopt});
    }
	std::shared_ptr<IngressResult> SemanticBundleCache::Fault(std::shared_ptr<BundleRequest> r, std::shared_ptr<AccessDecision> a)
	{
		telemetry->AccessFaults++;
		return std::make_shared<IngressResult>(IngressResult{r, IngressOutcome::AccessFault, nullptr, a->Fault});
	}

	long long SemanticBundleCache::Tick()
	{
		if (clock == std::numeric_limits<long long>::max())
		{
			std::fill(entries.begin(), entries.end(), std::nullopt);
			clock = 0;
		}
		return ++clock;
	}

	int SemanticBundleCache::SetStart(unsigned long long pc)
	{
		return static_cast<int>((pc / 256) % static_cast<unsigned long long>(entries.size() / ways)) * ways;
	}

	int SemanticBundleCache::TagBytes(std::shared_ptr<LookupTag> tag)
	{
		return static_cast<int>(TagJson(*tag).size());
	}

    long long SemanticBundleCache::IdentityBytes(std::shared_ptr<SemanticIdentity> identity) {
        auto result = identity->getResult();
        long long size = result->getRaw()->getLength();
        auto sideband = result->getBundleSideband();
        if (sideband && *sideband) size += (*sideband)->Content->getLength();
        for (const auto& slot : result->getSlots()) {
            size += slot->RawSlot->Content->getLength() + slot->InstructionPayload->Content->getLength()
                  + slot->SlotSideband->Content->getLength() + slot->StaticBinding->Content->getLength()
                  + slot->ScalarLoadAddressPlan->Content->getLength();
        }
        return size;
    }
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
    std::vector<unsigned char> SemanticBundleCache::EntryIntegrity(std::shared_ptr<LookupTag> tag, std::span<const unsigned char> payload) {
        BCRYPT_ALG_HANDLE algorithm = nullptr; BCRYPT_HASH_HANDLE hash = nullptr;
        if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0) throw std::runtime_error("SHA-256 provider unavailable");
        struct Cleanup { BCRYPT_ALG_HANDLE& algorithm; BCRYPT_HASH_HANDLE& hash; ~Cleanup() { if (hash) BCryptDestroyHash(hash); BCryptCloseAlgorithmProvider(algorithm, 0); } } cleanup{algorithm, hash};
        if (BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0) < 0) throw std::runtime_error("SHA-256 initialization failed");
        auto json = TagJson(*tag);
        if (BCryptHashData(hash, reinterpret_cast<PUCHAR>(json.data()), static_cast<ULONG>(json.size()), 0) < 0 ||
            BCryptHashData(hash, const_cast<PUCHAR>(payload.data()), static_cast<ULONG>(payload.size()), 0) < 0) throw std::runtime_error("SHA-256 update failed");
        std::vector<unsigned char> digest(32);
        if (BCryptFinishHash(hash, digest.data(), static_cast<ULONG>(digest.size()), 0) < 0) throw std::runtime_error("SHA-256 finalization failed");
        return digest;
    }
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
