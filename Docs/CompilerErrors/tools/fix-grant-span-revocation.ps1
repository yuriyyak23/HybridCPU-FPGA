$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime'
$p=Join-Path $r 'Capabilities\Grants\YAKSys_Hybrid_CPU.Core.CapabilityGrantCollection.h'; $s=[IO.File]::ReadAllText($p).Replace('#include <vector>',"#include <vector>`r`n#include <span>"); $s=$s.Replace('static inline const std::shared_ptr<CapabilityGrantCollection> Empty = std::make_shared<CapabilityGrantCollection>();','static const std::shared_ptr<CapabilityGrantCollection> Empty;').Replace('ReadOnlySpan<std::shared_ptr<CapabilityGrant>>','std::span<const std::shared_ptr<CapabilityGrant>>').Replace('std::vector<std::shared_ptr<CapabilityGrant>> &grants','const std::vector<std::shared_ptr<CapabilityGrant>> &grants'); if(-not $s.Contains('CapabilityGrantCollection::Empty =')) {$s += "`r`ninline const std::shared_ptr<YAKSys_Hybrid_CPU::Core::CapabilityGrantCollection> YAKSys_Hybrid_CPU::Core::CapabilityGrantCollection::Empty = std::make_shared<YAKSys_Hybrid_CPU::Core::CapabilityGrantCollection>();`r`n"}; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'Capabilities\Grants\YAKSys_Hybrid_CPU.Core.CapabilityGrantCollection.cpp'; $s=[IO.File]::ReadAllText($p).Replace('CapabilityGrantCollection(Array::Empty<CapabilityGrant> ())','CapabilityGrantCollection(std::vector<std::shared_ptr<CapabilityGrant>>{})').Replace('std::vector<std::shared_ptr<CapabilityGrant>> &grants','const std::vector<std::shared_ptr<CapabilityGrant>> &grants').Replace('_grants(grants ? grants : Array::Empty<std::shared_ptr<CapabilityGrant>>())','_grants(grants)').Replace('ReadOnlySpan<std::shared_ptr<CapabilityGrant>>','std::span<const std::shared_ptr<CapabilityGrant>>'); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'Domains\SecureCompute\Authority\SecureRevocationEpoch.h'; $s=if(Test-Path -LiteralPath $p){[IO.File]::ReadAllText($p)}else{'#pragma once'}; if(-not $s.Contains('struct SecureRevocationEpoch final')) { $s += @"

#include <memory>
namespace YAKSys_Hybrid_CPU::Core {
struct SecureRevocationEpoch final {
    const unsigned long long Current;
    explicit SecureRevocationEpoch(unsigned long long current = 0) : Current(current) {}
    bool isMaterialized() const noexcept { return Current != 0; }
    bool IsCurrent(unsigned long long candidate) const noexcept { return isMaterialized() && candidate == Current; }
    bool IsStale(unsigned long long candidate) const noexcept { return !IsCurrent(candidate); }
    static const std::shared_ptr<SecureRevocationEpoch> Unmaterialized;
};
inline const std::shared_ptr<SecureRevocationEpoch> SecureRevocationEpoch::Unmaterialized = std::make_shared<SecureRevocationEpoch>();
}
"@ }; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'Domains\SecureCompute\Descriptors\Migration\YAKSys_Hybrid_CPU.Core.SecureMigrationDescriptor.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('#include "../../Authority/SecureRevocationEpoch.h"')) {$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"../../Authority/SecureRevocationEpoch.h`"")}; [IO.File]::WriteAllText($p,$s)
