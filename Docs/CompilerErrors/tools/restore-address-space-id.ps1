$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Memory\AddressSpaces'
$p=Join-Path $root 'AddressSpaceId.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('struct AddressSpaceId final')) { $a='namespace YAKSys_Hybrid_CPU::Memory'; $s=$s.Replace($a,"#include <cstdint>`r`n$a"); $anchor='// C# TO C++ CONVERTER TASK:'; $def=@"
    struct AddressSpaceId final {
        std::uint16_t DomainTag = 0, AddressSpaceTag = 0;
        std::uint64_t SecondStageRootIdentity = 0, SecondStageEpoch = 0, AddressSpaceTagEpoch = 0, AddressSpaceGeneration = 0;
        bool MatchesSecondStageRoot(std::uint64_t root) const noexcept { return SecondStageRootIdentity == root; }
        bool MatchesAddressSpaceTag(std::uint16_t tag) const noexcept { return AddressSpaceTag == tag; }
        bool IsCurrent(std::uint64_t rootEpoch, std::uint64_t tagEpoch) const noexcept { return SecondStageEpoch == rootEpoch && AddressSpaceTagEpoch == tagEpoch; }
        std::uint64_t EncodeDescriptor() const noexcept { return (std::uint64_t(DomainTag) << 48) | (std::uint64_t(AddressSpaceTag) << 32) | (SecondStageRootIdentity & 0xffffffffULL); }
        bool operator==(const AddressSpaceId&) const = default;
    };
"@; $s=$s.Replace($anchor,$def+"`r`n"+$anchor) }; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'AddressSpaceDescriptor.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('#include "AddressSpaceId.h"')) {$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"AddressSpaceId.h`"")}; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'YAKSys_Hybrid_CPU.Core.AddressSpaceDescriptor.cpp'; $s=[IO.File]::ReadAllText($p); $pat='(?s)(AddressSpaceDescriptor::AddressSpaceDescriptor\(std::shared_ptr<AddressSpaceId> addressSpaceId, AddressSpaceAuthority authority, unsigned long long guestPhysicalBase, unsigned long long sizeBytes, unsigned long long generation, bool compatibilityProjectionEnabled\))\s*\{\s*AddressSpaceId = addressSpaceId;\s*Authority = authority;\s*GuestPhysicalBase = guestPhysicalBase;\s*SizeBytes = sizeBytes;\s*Generation = generation;\s*CompatibilityProjectionEnabled = compatibilityProjectionEnabled;\s*\}'; $s=[regex]::Replace($s,$pat,'$1 : AddressSpaceId(addressSpaceId), Authority(authority), GuestPhysicalBase(guestPhysicalBase), SizeBytes(sizeBytes), Generation(generation), CompatibilityProjectionEnabled(compatibilityProjectionEnabled) {}'); [IO.File]::WriteAllText($p,$s)
foreach($n in @('YAKSys_Hybrid_CPU.Core.AddressSpaceDescriptor.h','YAKSys_Hybrid_CPU.Core.AddressSpaceDescriptor.cpp')) { $p=Join-Path $root $n; $s=[IO.File]::ReadAllText($p).Replace('std::shared_ptr<AddressSpaceId>','std::shared_ptr<YAKSys_Hybrid_CPU::Memory::AddressSpaceId>'); [IO.File]::WriteAllText($p,$s) }
