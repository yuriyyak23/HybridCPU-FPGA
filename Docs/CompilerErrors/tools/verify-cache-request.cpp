#include "../../Frontend/Cache/SemanticBundleCache.h"
#include "../../Frontend/Semantic/CompactCodec.h"
#include <algorithm>
#include <chrono>
#include <stdexcept>
namespace HybridCpu::ShadowFrontend {
    // The JSON byte count must match System.Text.Json; implementation is a separate porting task.
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

}
