$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Registers\Retire'; $h=Join-Path $dir 'RetireCoordinator.h'; $c=Join-Path $dir 'RetireCoordinator.cpp'
$hs=[IO.File]::ReadAllText($h); $cs=[IO.File]::ReadAllText($c)
$hs=$hs.Replace('#include <vector>',"#include <vector>`r`n#include <span>")
$hs=$hs.Replace('namespace YAKSys_Hybrid_CPU::Core::Registers::Retire { template<typename ArchContextState>class StandaloneTargets; }','')
$hs=$hs.Replace('ReadOnlySpan<RetireRecord>','std::span<const RetireRecord>')
$start=$hs.IndexOf("`t`tstd::shared_ptr<StandaloneTargets> ReserveTargets();"); $end=$hs.IndexOf("`tprivate:`r`n// C# TO C++ CONVERTER WARNING",$hs.IndexOf("`t`t};",$start)+6)
if($start -lt 0 -or $end -lt 0){throw 'Target block missing'}
$block=@"
        class StandaloneTargets final {
            std::vector<std::shared_ptr<ArchContextState>> contexts_;
            std::vector<std::shared_ptr<Disposable>> leases_;
        public:
            StandaloneTargets(std::vector<std::shared_ptr<ArchContextState>> contexts,
                              std::vector<std::shared_ptr<Disposable>> leases)
                : contexts_(std::move(contexts)), leases_(std::move(leases)) {}
            StandaloneTargets(const StandaloneTargets&) = delete;
            ~StandaloneTargets() {
                for (auto it = leases_.rbegin(); it != leases_.rend(); ++it)
                    if (*it) { try { (*it)->dispose(); } catch (...) {} }
            }
            const std::vector<std::shared_ptr<ArchContextState>>& getContexts() const { return contexts_; }
        };
        std::shared_ptr<StandaloneTargets> ReserveTargets();

"@
$hs=$hs.Remove($start,$end-$start).Insert($start,$block)
$cs=$cs.Replace('ReadOnlySpan<RetireRecord>','std::span<const RetireRecord>')
$start=$cs.IndexOf("`tvoid RetireCoordinator::Retire(RetireRecord const record)"); $end=$cs.IndexOf('// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a ''readonly'' collection', $cs.IndexOf('RetireCoordinator::ReserveTargets()', $start))
if($start -lt 0 -or $end -lt 0){throw 'Retire implementation boundaries missing'}
$impl=@"
    namespace {
        struct RetireAccessGuard {
            std::shared_ptr<Disposable> lease;
            ~RetireAccessGuard() { if (lease) { try { lease->dispose(); } catch (...) {} } }
        };
    }
    void RetireCoordinator::Retire(RetireRecord const record) {
        RetireAccessGuard access{_reserveAccess ? _reserveAccess(CanonicalStateAccessKind::Write) : nullptr};
        ValidateRecord(record);
        auto targets = ReserveTargets();
        ApplyRetireRecord(record, targets->getContexts());
    }
    void RetireCoordinator::Retire(std::span<const RetireRecord> records) {
        RetireAccessGuard access{_reserveAccess ? _reserveAccess(CanonicalStateAccessKind::Write) : nullptr};
        for (const auto& record : records) ValidateRecord(record);
        auto targets = ReserveTargets();
        for (const auto& record : records) ApplyRetireRecord(record, targets->getContexts());
    }
    void RetireCoordinator::Prevalidate(std::span<const RetireRecord> records) {
        RetireAccessGuard access{_reserveAccess ? _reserveAccess(CanonicalStateAccessKind::Read) : nullptr};
        for (const auto& record : records) ValidateRecord(record);
    }
    std::shared_ptr<RetireCoordinator::StandaloneTargets> RetireCoordinator::ReserveTargets() {
        auto snapshot = _archContexts;
        if (_reserveAccess) return std::make_shared<StandaloneTargets>(std::move(snapshot), std::vector<std::shared_ptr<Disposable>>{});
        if (snapshot.size() != static_cast<std::size_t>(getVtCount())) throw std::logic_error("Retire target geometry changed.");
        std::vector<std::shared_ptr<Disposable>> leases;
        leases.reserve(snapshot.size());
        try {
            for (const auto& context : snapshot) if (context) leases.push_back(context->ReserveStandaloneRetirement());
            return std::make_shared<StandaloneTargets>(std::move(snapshot), std::move(leases));
        } catch (...) {
            for (auto it = leases.rbegin(); it != leases.rend(); ++it) if (*it) (*it)->dispose();
            throw;
        }
    }

"@
$cs=$cs.Remove($start,$end-$start).Insert($start,$impl)
[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false))
