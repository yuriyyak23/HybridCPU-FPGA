$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Memory\Timing\MemoryCycleController.h'
$s=Get-Content $p -Raw
if (!$s.Contains('#include <map>')) { $s=$s.Replace('#include <mutex>', "#include <mutex>`r`n#include <map>") }
$a=$s.IndexOf("`tclass MemoryCyclePlatformOrchestrator final"); if ($a -lt 0) { throw 'Orchestrator missing' }
$h=@"
    class MemoryCyclePlatformOrchestrator final {
        struct CoreClockState { unsigned long long EpochOffset = 0, LastLocalCycle = 0; };
        struct DomainState { std::mutex Gate; std::unordered_map<unsigned int, CoreClockState> CoreClocks; unsigned long long LastNormalizedEdge = 0; };
        static inline std::mutex DomainsGate;
        static inline std::map<std::weak_ptr<MemoryCycleController>, std::shared_ptr<DomainState>, std::owner_less<std::weak_ptr<MemoryCycleController>>> Domains;
    public:
        static bool AdvanceCoreObservedPlatformEdge(std::shared_ptr<MemoryCycleController> controller, unsigned int coreId, unsigned long long platformCycle);
    };
}
"@
$s=$s.Substring(0,$a)+$h
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Memory\Timing\MemoryCycleController.cpp'; $s=Get-Content $p -Raw
$a=$s.IndexOf("`tbool MemoryCyclePlatformOrchestrator::AdvanceCoreObservedPlatformEdge("); if($a -lt 0){throw 'Orchestrator implementation missing'}
$cpp=@"
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
}
"@
$s=$s.Substring(0,$a)+$cpp
if (!$s.Contains('#include <limits>')) { $s=$s.Replace('#include "MemoryCycleController.h"', "#include `"MemoryCycleController.h`"`r`n#include <limits>") }
Set-Content $p $s -Encoding UTF8 -NoNewline
