#pragma once

#include <cstdint>
namespace YAKSys_Hybrid_CPU::Memory
{
	/// <summary>
	/// Runtime-owned identity for second-stage translation cache entries.
	/// It is not compatibility frontend state and is not serialized through
	/// frozen field-alias projection vocabulary.
	/// </summary>
    struct AddressSpaceId final {
        std::uint16_t DomainTag = 0, AddressSpaceTag = 0;
        std::uint64_t SecondStageRootIdentity = 0, SecondStageEpoch = 0, AddressSpaceTagEpoch = 0, AddressSpaceGeneration = 0;
        bool MatchesSecondStageRoot(std::uint64_t root) const noexcept { return SecondStageRootIdentity == root; }
        bool MatchesAddressSpaceTag(std::uint16_t tag) const noexcept { return AddressSpaceTag == tag; }
        bool IsCurrent(std::uint64_t rootEpoch, std::uint64_t tagEpoch) const noexcept { return SecondStageEpoch == rootEpoch && AddressSpaceTagEpoch == tagEpoch; }
        std::uint64_t EncodeDescriptor() const noexcept { return (std::uint64_t(DomainTag) << 48) | (std::uint64_t(AddressSpaceTag) << 32) | (SecondStageRootIdentity & 0xffffffffULL); }
        bool operator==(const AddressSpaceId&) const = default;
    };
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AddressSpaceId(ushort DomainTag, ushort AddressSpaceTag, ulong SecondStageRootIdentity, ulong SecondStageEpoch, ulong AddressSpaceTagEpoch, ulong AddressSpaceGeneration)
//	{
//		public bool MatchesSecondStageRoot(ulong secondStageRootIdentity)
//		{
//			return SecondStageRootIdentity == secondStageRootIdentity;
//		}
//
//		public bool MatchesAddressSpaceTag(ushort addressSpaceTag)
//		{
//			return AddressSpaceTag == addressSpaceTag;
//		}
//
//		public bool IsCurrent(ulong secondStageEpoch, ulong addressSpaceTagEpoch)
//		{
//			return SecondStageEpoch == secondStageEpoch && AddressSpaceTagEpoch == addressSpaceTagEpoch;
//		}
//
//		public ulong EncodeDescriptor()
//		{
//			return ((ulong)DomainTag << 48) | ((ulong)AddressSpaceTag << 32) | (SecondStageRootIdentity & 0xFFFF_FFFFUL);
//		}
//	}
}
