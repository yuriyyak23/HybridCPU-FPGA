#pragma once
#include "IOMMU.h"
#include "../../Core/Runtime/IO/Dma/DmaDomainBinding.h"
#include <memory>
#include <tuple>
#include <stdexcept>

using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::CloseToHSL::Memory::MMU
{
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct IotlbTag(ushort IoDomainTag, uint DomainId, uint DeviceId, ulong IoVirtualPageNumber, IOMMUAccessPermissions Permissions, ulong DomainEpoch, ulong MappingEpoch)
//	{
//		public static IotlbTag Create(IommuDomainBinding binding, ulong ioVirtualAddress, IOMMUAccessPermissions permissions, ulong mappingEpoch)
//		{
//			return new(binding.IoDomainTag, binding.DomainId, binding.DeviceId, ioVirtualAddress >> 12, permissions, binding.DomainEpoch, mappingEpoch == 0 ? 1 : mappingEpoch);
//		}
//
//		public bool Matches(IommuDomainBinding binding, ulong ioVirtualAddress, IOMMUAccessPermissions requestedPermissions, ulong mappingEpoch)
//		{
//			return IoDomainTag == binding.IoDomainTag && DomainId == binding.DomainId && DeviceId == binding.DeviceId && IoVirtualPageNumber == (ioVirtualAddress >> 12) && DomainEpoch == binding.DomainEpoch && MappingEpoch == (mappingEpoch == 0 ? 1 : mappingEpoch) && (Permissions & requestedPermissions) == requestedPermissions;
//		}
//	}
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
}
