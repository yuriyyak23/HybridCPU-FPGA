$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\Descriptors\ExecutionDomain'
$p=Join-Path $root 'ExecutionDomainReadOnlyStateView.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('class ExecutionDomainReadOnlyStateView final')) { $s += @"

#include <memory>
namespace YAKSys_Hybrid_CPU::Core {
class ExecutionDomainReadOnlyStateView final {
public:
    const unsigned long long GuestPc, GuestSp, GuestFlags;
    const bool HasMaterializedGuestPc, HasMaterializedGuestSp, HasMaterializedGuestFlags;
    const unsigned long long StateEpoch;
    ExecutionDomainReadOnlyStateView(unsigned long long pc = 0, unsigned long long sp = 0, unsigned long long flags = 0, bool hasPc = false, bool hasSp = false, bool hasFlags = false, unsigned long long epoch = 0)
        : GuestPc(pc), GuestSp(sp), GuestFlags(flags), HasMaterializedGuestPc(hasPc), HasMaterializedGuestSp(hasSp), HasMaterializedGuestFlags(hasFlags), StateEpoch(epoch) {}
    static const std::shared_ptr<ExecutionDomainReadOnlyStateView> Unmaterialized;
    static std::shared_ptr<ExecutionDomainReadOnlyStateView> FromGuestPcSpFlags(unsigned long long pc, unsigned long long sp, unsigned long long flags, unsigned long long epoch = 0) { return std::make_shared<ExecutionDomainReadOnlyStateView>(pc, sp, flags, true, true, true, epoch); }
    bool getHasAnyMaterializedGuestArchitecturalState() const noexcept { return HasMaterializedGuestPc || HasMaterializedGuestSp || HasMaterializedGuestFlags; }
    bool getHasCompleteGuestPcSpFlags() const noexcept { return HasMaterializedGuestPc && HasMaterializedGuestSp && HasMaterializedGuestFlags; }
    bool isMaterialized() const noexcept { return getHasAnyMaterializedGuestArchitecturalState(); }
};
inline const std::shared_ptr<ExecutionDomainReadOnlyStateView> ExecutionDomainReadOnlyStateView::Unmaterialized = std::make_shared<ExecutionDomainReadOnlyStateView>();
}
"@ }; [IO.File]::WriteAllText($p,$s)
foreach($name in @('YAKSys_Hybrid_CPU.Core.ExecutionDomainDescriptor.h','YAKSys_Hybrid_CPU.Core.ExecutionDomainDescriptor.cpp')) { $p=Join-Path $root $name; $s=[IO.File]::ReadAllText($p).Replace('std::optional<std::shared_ptr<ExecutionDomainReadOnlyStateView>> &readOnlyState = std::nullopt','std::shared_ptr<ExecutionDomainReadOnlyStateView> readOnlyState = nullptr').Replace('std::optional<std::shared_ptr<ExecutionDomainReadOnlyStateView>> &readOnlyState','std::shared_ptr<ExecutionDomainReadOnlyStateView> readOnlyState'); if($name.EndsWith('.h') -and -not $s.Contains('#include "ExecutionDomainReadOnlyStateView.h"')) {$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"ExecutionDomainReadOnlyStateView.h`"")}; [IO.File]::WriteAllText($p,$s) }
