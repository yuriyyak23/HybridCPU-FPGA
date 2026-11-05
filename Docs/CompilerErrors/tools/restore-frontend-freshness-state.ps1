$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\Frontend\Invalidation'
$p=Join-Path $r 'FreshnessState.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('const std::any Gate = std::any();','mutable std::recursive_mutex Gate;').Replace('std::any getGate() const;','std::recursive_mutex& getGate() const;').Replace('void EnsureUsable();','void EnsureUsable() const;').Replace('std::shared_ptr<ImmutableDictionary<FrontendEvent, long long>> getInvalidations() const;','std::unordered_map<FrontendEvent, long long> getInvalidations() const;')
$m='class FreshnessState final : public std::enable_shared_from_this<FreshnessState>'
$insert=@(
'    struct FreshnessSnapshot final {',
'        FreshnessToken Token;',
'        unsigned long long ReplayGeneration, CommittedOrder;',
'        bool Poisoned;',
'        std::unordered_map<FrontendEvent, long long> Invalidations;',
'    };',
'') -join "`r`n"
if(-not $s.Contains($m)){throw 'State class missing'}
$s=$s.Replace($m,$insert+$m)
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'FreshnessState.cpp'; $lines=@(
'#include "FreshnessState.h"',
'#include <stdexcept>',
'namespace HybridCpu::ShadowFrontend {',
'std::recursive_mutex& FreshnessState::getGate() const { return Gate; }',
'void FreshnessState::RejectCacheLockFromPublication() { if (publicationDepth != 0) throw std::logic_error("Publication callbacks cannot enter cache APIs; lock order is ingress then freshness."); }',
'void FreshnessState::EnsureUsable() const { if (poisoned) throw std::logic_error("Frontend publication failed; this freshness state is permanently poisoned."); }',
'std::shared_ptr<FreshnessToken> FreshnessState::Capture() { std::lock_guard lock(Gate); EnsureUsable(); return std::make_shared<FreshnessToken>(FreshnessToken{incarnation, generation}); }',
'unsigned long long FreshnessState::getReplayGeneration() const { std::lock_guard lock(Gate); EnsureUsable(); return replayGeneration; }',
'std::unordered_map<FrontendEvent, long long> FreshnessState::getInvalidations() const { std::lock_guard lock(Gate); return invalidations; }',
'std::shared_ptr<FreshnessSnapshot> FreshnessState::Snapshot() { std::lock_guard lock(Gate); return std::make_shared<FreshnessSnapshot>(FreshnessSnapshot{{incarnation,generation},replayGeneration,committedOrder,poisoned,invalidations}); }',
'void FreshnessState::Apply(FrontendEvent kind, unsigned long long order, bool committed, std::function<void()> publish) {',
'    if (!committed) return;',
'    std::lock_guard lock(Gate); EnsureUsable();',
'    if (order <= committedOrder) throw std::logic_error("Committed events require strictly ordered publication.");',
'    try {',
'        ++publicationDepth;',
'        try { if (publish) publish(); } catch (...) { --publicationDepth; throw; }',
'        --publicationDepth;',
'        committedOrder = order; ++invalidations[kind];',
'        if (kind != FrontendEvent::Redirect && kind != FrontendEvent::Trap && kind != FrontendEvent::Rollback && kind != FrontendEvent::ReplayPhase && kind != FrontendEvent::Serializing) Advance(generation);',
'        Advance(replayGeneration);',
'    } catch (...) { poisoned = true; throw; }',
'}',
'void FreshnessState::Advance(unsigned long long& value) {',
'    if (value == std::numeric_limits<unsigned long long>::max()) {',
'        if (incarnation == std::numeric_limits<unsigned long long>::max()) throw std::overflow_error("Freshness incarnation overflow.");',
'        ++incarnation; generation = 0; replayGeneration = 0;',
'    } else ++value;',
'}',
'std::shared_ptr<FreshnessState> FreshnessState::OverflowExperiment() { auto state = std::make_shared<FreshnessState>(); state->generation = std::numeric_limits<unsigned long long>::max(); state->replayGeneration = std::numeric_limits<unsigned long long>::max(); return state; }',
'std::shared_ptr<FreshnessState> FreshnessState::ObservationExperiment(std::shared_ptr<FreshnessToken> token, unsigned long long replay, unsigned long long order) {',
'    if (!token || token->Incarnation == 0) throw std::out_of_range("token");',
'    auto state = std::make_shared<FreshnessState>(); state->incarnation = token->Incarnation; state->generation = token->Generation; state->replayGeneration = replay; state->committedOrder = order; return state;',
'}',
'}',
'') -join "`r`n"
[IO.File]::WriteAllText($p,$lines,[Text.UTF8Encoding]::new($false))
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\Frontend\Cache\SemanticBundleCache.cpp'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('std::scoped_lock<std::mutex> lock(freshness->getGate());','std::scoped_lock<std::recursive_mutex> lock(freshness->getGate());'); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
