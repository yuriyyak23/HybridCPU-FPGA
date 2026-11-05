$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\SecureCompute\Descriptors\Memory'
$p=Join-Path $base 'SecureMemoryDomainDescriptor.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('#pragma once',"#pragma once`r`n#include `"../../Authority/SecureRevocationEpoch.h`"")
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); if($i -lt 0){throw 'anchor missing'}
$s=$s.Substring(0,$i)+@'
struct SecureMemoryRegionDescriptor final {
    const SecureMemoryRegionClass RegionClass;
    const unsigned long long Start, Length;
    const SecureMemoryHostVisibility HostVisibility;
    const unsigned long long PolicyEpoch;
    const SecureRuntimeMutableDirtyPolicy RuntimeDirtyPolicy;
    const SecureRuntimeMutableMigrationClass RuntimeMigrationClass;
    const bool IsEmpty, IsPrivate, IsShared, IsMeasured, IsRuntimeMutable, IsHostReadable, HasRuntimeMutableClassification;
    SecureMemoryRegionDescriptor(SecureMemoryRegionClass regionClass = SecureMemoryRegionClass::Private, unsigned long long start = 0, unsigned long long length = 0, SecureMemoryHostVisibility visibility = SecureMemoryHostVisibility::Denied, unsigned long long epoch = 0, SecureRuntimeMutableDirtyPolicy dirty = SecureRuntimeMutableDirtyPolicy::None, SecureRuntimeMutableMigrationClass migration = SecureRuntimeMutableMigrationClass::Unclassified)
        : RegionClass(regionClass), Start(start), Length(length), HostVisibility(visibility), PolicyEpoch(epoch), RuntimeDirtyPolicy(dirty), RuntimeMigrationClass(migration),
          IsEmpty(length == 0), IsPrivate(regionClass == SecureMemoryRegionClass::Private), IsShared(regionClass == SecureMemoryRegionClass::Shared), IsMeasured(regionClass == SecureMemoryRegionClass::Measured), IsRuntimeMutable(regionClass == SecureMemoryRegionClass::RuntimeMutable), IsHostReadable(visibility == SecureMemoryHostVisibility::ExplicitShared),
          HasRuntimeMutableClassification(regionClass != SecureMemoryRegionClass::RuntimeMutable || (dirty != SecureRuntimeMutableDirtyPolicy::None && migration != SecureRuntimeMutableMigrationClass::Unclassified)) {}
    bool Contains(unsigned long long address, unsigned long long length) const {
        if (IsEmpty || length == 0 || address < Start) return false;
        const auto offset = address - Start; return offset < Length && length <= Length - offset;
    }
    bool IsCurrentFor(std::shared_ptr<SecureRevocationEpoch> epoch) const { return epoch && epoch->IsCurrent(PolicyEpoch); }
};
}
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.SecureMemoryDomainDescriptor.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('System::Array::Empty<SecureMemoryRegionDescriptor> ()','std::vector<std::shared_ptr<SecureMemoryRegionDescriptor>>{}')
$pat='(SecureMemoryDomainDescriptor::SecureMemoryDomainDescriptor\(unsigned long long domainTag, unsigned long long addressSpaceTag, std::shared_ptr<SecureRevocationEpoch> policyEpoch, const std::vector<std::shared_ptr<SecureMemoryRegionDescriptor>> &regions, SecureMemoryDmaPolicy dmaPolicy\))\s*\{\s*DomainTag = domainTag;\s*AddressSpaceTag = addressSpaceTag;\s*PolicyEpoch = policyEpoch;\s*Regions = regions;\s*DmaPolicy = dmaPolicy;\s*\}'
if(-not [regex]::IsMatch($s,$pat)){throw 'constructor missing'}; $s=[regex]::Replace($s,$pat,'$1 : DomainTag(domainTag), AddressSpaceTag(addressSpaceTag), PolicyEpoch(policyEpoch), Regions(regions), DmaPolicy(dmaPolicy) {}')
$s=$s.Replace('getPolicyEpoch()->IsMaterialized','getPolicyEpoch()->isMaterialized()'); [IO.File]::WriteAllText($p,$s)
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\Admission\Memory\MemoryDomainRuntime.CpuSecondStage.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('using namespace YAKSys_Hybrid_CPU::Memory;',''); [IO.File]::WriteAllText($p,$s)