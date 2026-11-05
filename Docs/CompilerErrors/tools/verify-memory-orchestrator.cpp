#include <memory>
#include <mutex>
#include <map>
#include <unordered_map>
#include <limits>
#include <stdexcept>
#include <cassert>
#include <vector>
class MemoryCycleController { public: std::vector<unsigned long long> edges; bool AdvancePlatformEdge(unsigned long long edge) { edges.push_back(edge); return true; } };
    class MemoryCyclePlatformOrchestrator final {
        struct CoreClockState { unsigned long long EpochOffset = 0, LastLocalCycle = 0; };
        struct DomainState { std::mutex Gate; std::unordered_map<unsigned int, CoreClockState> CoreClocks; unsigned long long LastNormalizedEdge = 0; };
        static inline std::mutex DomainsGate;
        static inline std::map<std::weak_ptr<MemoryCycleController>, std::shared_ptr<DomainState>, std::owner_less<std::weak_ptr<MemoryCycleController>>> Domains;
    public:
        static bool AdvanceCoreObservedPlatformEdge(std::shared_ptr<MemoryCycleController> controller, unsigned int coreId, unsigned long long platformCycle);
    };

    bool MemoryCyclePlatformOrchestrator::AdvanceCoreObservedPlatformEdge(std::shared_ptr<MemoryCycleController> controller, unsigned int coreId, unsigned long long platformCycle) {
        if (!controller) throw std::invalid_argument("controller");
        std::shared_ptr<DomainState> domain;
        {
            std::lock_guard lock(DomainsGate);
            for (auto i = Domains.begin(); i != Domains.end();) { if (i->first.expired()) i = Domains.erase(i); else ++i; }
            std::weak_ptr<MemoryCycleController> key(controller);
            auto [it, inserted] = Domains.try_emplace(key, nullptr);
            if (inserted) it->second = std::make_shared<DomainState>();
            domain = it->second;
        }
        std::lock_guard lock(domain->Gate);
        auto coreClock = domain->CoreClocks[coreId];
        if (platformCycle == coreClock.LastLocalCycle) return false;
        auto epochOffset = coreClock.EpochOffset;
        const auto maximum = std::numeric_limits<unsigned long long>::max();
        if (platformCycle < coreClock.LastLocalCycle) {
            if (domain->LastNormalizedEdge == maximum) throw std::overflow_error("platform cycle overflow");
            auto next = domain->LastNormalizedEdge + 1;
            if (next < platformCycle) throw std::overflow_error("platform cycle underflow");
            epochOffset = next - platformCycle;
        }
        if (epochOffset > maximum - platformCycle) throw std::overflow_error("platform cycle overflow");
        auto normalizedEdge = epochOffset + platformCycle;
        domain->CoreClocks[coreId] = CoreClockState{epochOffset, platformCycle};
        if (normalizedEdge <= domain->LastNormalizedEdge) return false;
        domain->LastNormalizedEdge = normalizedEdge;
        return controller->AdvancePlatformEdge(normalizedEdge);
    }
int main() {
 auto c=std::make_shared<MemoryCycleController>(); using O=MemoryCyclePlatformOrchestrator;
 assert(!O::AdvanceCoreObservedPlatformEdge(c,0,0)); assert(O::AdvanceCoreObservedPlatformEdge(c,0,1));
 assert(!O::AdvanceCoreObservedPlatformEdge(c,1,1)); assert(O::AdvanceCoreObservedPlatformEdge(c,1,2));
 assert(O::AdvanceCoreObservedPlatformEdge(c,0,0)); assert(c->edges.back()==3);
 assert(!O::AdvanceCoreObservedPlatformEdge(c,0,0)); auto other=std::make_shared<MemoryCycleController>(); assert(O::AdvanceCoreObservedPlatformEdge(other,0,1));
 auto m=std::make_shared<MemoryCycleController>(); auto max=std::numeric_limits<unsigned long long>::max(); assert(O::AdvanceCoreObservedPlatformEdge(m,0,max));
 bool fail=false; try { O::AdvanceCoreObservedPlatformEdge(m,0,0); } catch(const std::overflow_error&) { fail=true; } assert(fail);
}
