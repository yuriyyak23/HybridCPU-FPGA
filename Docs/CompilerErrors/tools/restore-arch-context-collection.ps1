$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Registers\Architectural'
$path=Join-Path $root 'ArchContextState.h'; $cpp=Join-Path $root 'ArchContextState.cpp'; $dest=Join-Path $root 'ArchContextCollection.h'
if(Test-Path $dest){throw 'Destination exists'}
$s=[IO.File]::ReadAllText($path); $start=$s.IndexOf("`tclass ArchContextCollection final :"); if($start -lt 0){throw 'Collection class missing'}
$end=$s.IndexOf("`r`n`t};",$start); if($end -lt 0){throw 'Collection end missing'}; $s=$s.Remove($start,$end+6-$start)
$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"ArchContextCollection.h`"")
$source=[IO.File]::ReadAllText($cpp); $start=$source.IndexOf('// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:', $source.IndexOf('ArchContextState::CommittedRegisterStorage::IEnumerable_GetEnumerator'))
if($start -lt 0){throw 'Collection source block missing'}; $end=$source.LastIndexOf("`r`n}"); $source=$source.Remove($start,$end-$start)
$content=@"
#pragma once
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core::Registers {
class ArchContextState;
class ArchContextCollection final {
public:
    using Context = std::shared_ptr<ArchContextState>;
    using Publish = std::function<void(int, Context)>;
private:
    std::vector<Context> contexts_;
    const Publish publish_;
public:
    explicit ArchContextCollection(const std::vector<Context>& contexts, Publish publish = {})
        : contexts_(contexts), publish_(std::move(publish)) {}
    int getCount() const { return static_cast<int>(contexts_.size()); }
    int getLength() const { return getCount(); }
    std::size_t size() const noexcept { return contexts_.size(); }
    bool empty() const noexcept { return contexts_.empty(); }
    Context operator[](int index) const { return contexts_.at(static_cast<std::size_t>(index)); }
    void setIndexer(int index, Context context) {
        if (!publish_) throw std::logic_error("This context view has no original publication owner.");
        publish_(index, std::move(context));
    }
    void StoreFromCore(int index, Context context) { contexts_.at(static_cast<std::size_t>(index)) = std::move(context); }
    std::vector<Context> Clone() const { return contexts_; }
    auto begin() const noexcept { return contexts_.cbegin(); }
    auto end() const noexcept { return contexts_.cend(); }
};
}
"@
[IO.File]::WriteAllText($dest,$content,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($cpp,$source,[Text.UTF8Encoding]::new($false))
