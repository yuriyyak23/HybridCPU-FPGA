$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Records'
$p=Join-Path $r 'DomainHypercallCompletionOwner.h'; $s=[IO.File]::ReadAllText($p).Replace('#include <mutex>',"#include <mutex>`r`n#include <map>`r`n#include <stdexcept>")
$old='const std::shared_ptr<ConditionalWeakTable<std::shared_ptr<CompletionPublicationToken>, std::shared_ptr<LivePublication>>> _live = std::make_shared<ConditionalWeakTable<std::shared_ptr<CompletionPublicationToken>, std::shared_ptr<LivePublication>>>()'
$new=@"
class LivePublicationTable final {
            std::mutex mutex_;
            std::map<std::weak_ptr<CompletionPublicationToken>, std::shared_ptr<LivePublication>, std::owner_less<std::weak_ptr<CompletionPublicationToken>>> entries_;
        public:
            void Add(const std::shared_ptr<CompletionPublicationToken>& key, std::shared_ptr<LivePublication> value) {
                std::lock_guard<std::mutex> lock(mutex_);
                for (auto it = entries_.begin(); it != entries_.end();) { if (it->first.expired()) it = entries_.erase(it); else ++it; }
                if (!entries_.emplace(key, std::move(value)).second) throw std::invalid_argument("duplicate publication");
            }
            bool TryGetValue(const std::shared_ptr<CompletionPublicationToken>& key, std::shared_ptr<LivePublication>& value) {
                std::lock_guard<std::mutex> lock(mutex_); auto it = entries_.find(key);
                if (it == entries_.end()) { value.reset(); return false; } value = it->second; return true;
            }
            bool Remove(const std::shared_ptr<CompletionPublicationToken>& key) { std::lock_guard<std::mutex> lock(mutex_); return entries_.erase(key) != 0; }
        };
        const std::shared_ptr<LivePublicationTable> _live = std::make_shared<LivePublicationTable>()
"@
if(-not $s.Contains($old)){throw 'table declaration not found'}; $s=$s.Replace($old,$new); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'DomainHypercallCompletionOwner.cpp'; $s=[IO.File]::ReadAllText($p).Replace('std::optional<std::shared_ptr<LivePublication>> published;', 'std::shared_ptr<LivePublication> published;').Replace('published.Completion != completion','published->Completion != completion').Replace('std::optional<std::shared_ptr<LivePublication>> publication;', 'std::shared_ptr<LivePublication> publication;').Replace('publication.LifecycleGate::TryBeginTransition','publication->LifecycleGate->TryBeginTransition').Replace('_liveTokens.Remove(token);','_liveTokens.erase(token);'); [IO.File]::WriteAllText($p,$s)
