$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Memory\MMU\IotlbTag.h'; $s=[IO.File]::ReadAllText($p).Replace('#pragma once','#pragma once'+"`r`n"+'#include "IOMMU.h"'+"`r`n"+'#include "../../Core/Runtime/IO/Dma/DmaDomainBinding.h"'+"`r`n"+'#include <memory>'+"`r`n"+'#include <tuple>'); $idx=$s.LastIndexOf("`r`n}"); if($idx -lt 0){throw 'Namespace end missing'}
$block=@"
	struct IotlbTag final {
		unsigned short IoDomainTag = 0;
		unsigned int DomainId = 0, DeviceId = 0;
		unsigned long long IoVirtualPageNumber = 0;
		IOMMUAccessPermissions Permissions = IOMMUAccessPermissions::None;
		unsigned long long DomainEpoch = 0, MappingEpoch = 0;
		static std::shared_ptr<IotlbTag> Create(const std::shared_ptr<IommuDomainBinding>& binding, unsigned long long address, IOMMUAccessPermissions permissions, unsigned long long mappingEpoch) {
			if (!binding) throw std::invalid_argument("binding");
			return std::make_shared<IotlbTag>(IotlbTag{binding->IoDomainTag, binding->DomainId, binding->DeviceId, address >> 12, permissions, binding->DomainEpoch, mappingEpoch ? mappingEpoch : 1});
		}
		bool Matches(const std::shared_ptr<IommuDomainBinding>& binding, unsigned long long address, IOMMUAccessPermissions requested, unsigned long long mappingEpoch) const noexcept {
			return binding && IoDomainTag == binding->IoDomainTag && DomainId == binding->DomainId && DeviceId == binding->DeviceId && IoVirtualPageNumber == (address >> 12) && DomainEpoch == binding->DomainEpoch && MappingEpoch == (mappingEpoch ? mappingEpoch : 1) && (static_cast<int>(Permissions) & static_cast<int>(requested)) == static_cast<int>(requested);
		}
		bool operator==(const IotlbTag&) const = default;
		bool operator<(const IotlbTag& other) const noexcept { return std::tie(IoDomainTag, DomainId, DeviceId, IoVirtualPageNumber, Permissions, DomainEpoch, MappingEpoch) < std::tie(other.IoDomainTag, other.DomainId, other.DeviceId, other.IoVirtualPageNumber, other.Permissions, other.DomainEpoch, other.MappingEpoch); }
	};
"@
$s=$s.Insert($idx,"`r`n"+$block).Replace('#include <tuple>','#include <tuple>'+"`r`n"+'#include <stdexcept>'); [IO.File]::WriteAllText($p,$s)