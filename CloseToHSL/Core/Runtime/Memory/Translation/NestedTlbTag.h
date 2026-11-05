#pragma once
#include "../AddressSpaces/AddressSpaceId.h"
#include <memory>
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Memory
{
	/// <summary>
	/// Complete TLB tag for runtime-owned second-stage translations.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedTlbTag(AddressSpaceId AddressSpace, ulong VirtualPageNumber, ulong SecondStageRootIdentity, byte Permissions, byte MemoryType, ulong TranslationEpoch)
//	{
//		public static NestedTlbTag Create(ulong virtualAddress, AddressSpaceId addressSpace, byte permissions, byte memoryType)
//		{
//			return new(addressSpace, virtualAddress >> 12, addressSpace.SecondStageRootIdentity, permissions, memoryType, addressSpace.SecondStageEpoch ^ addressSpace.AddressSpaceTagEpoch ^ addressSpace.AddressSpaceGeneration);
//		}
//
//		public bool MatchesLookup(ulong virtualAddress, AddressSpaceId addressSpace)
//		{
//			return VirtualPageNumber == (virtualAddress >> 12) && AddressSpace == addressSpace && SecondStageRootIdentity == addressSpace.SecondStageRootIdentity;
//		}
//	}
	struct NestedTlbTag final {
		std::shared_ptr<AddressSpaceId> AddressSpace;
		unsigned long long VirtualPageNumber, SecondStageRootIdentity;
		unsigned char Permissions, MemoryType;
		unsigned long long TranslationEpoch;
		static std::shared_ptr<NestedTlbTag> Create(unsigned long long address, std::shared_ptr<AddressSpaceId> space, unsigned char permissions, unsigned char memoryType) {
			if (!space) throw std::invalid_argument("addressSpace");
			return std::make_shared<NestedTlbTag>(NestedTlbTag{std::make_shared<AddressSpaceId>(*space), address >> 12, space->SecondStageRootIdentity, permissions, memoryType, space->SecondStageEpoch ^ space->AddressSpaceTagEpoch ^ space->AddressSpaceGeneration});
		}
		bool MatchesLookup(unsigned long long address, const std::shared_ptr<AddressSpaceId>& space) const {
			if (!space || !AddressSpace) return false;
			const auto& a = *AddressSpace; const auto& b = *space;
			return VirtualPageNumber == (address >> 12) && a.DomainTag == b.DomainTag && a.AddressSpaceTag == b.AddressSpaceTag && a.SecondStageRootIdentity == b.SecondStageRootIdentity && a.SecondStageEpoch == b.SecondStageEpoch && a.AddressSpaceTagEpoch == b.AddressSpaceTagEpoch && a.AddressSpaceGeneration == b.AddressSpaceGeneration && SecondStageRootIdentity == b.SecondStageRootIdentity;
		}
	};
}
