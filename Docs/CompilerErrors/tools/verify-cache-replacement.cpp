#include "../../Frontend/Cache/SemanticBundleCache.h"
#include <algorithm>
#include <stdexcept>
namespace HybridCpu::ShadowFrontend {
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
