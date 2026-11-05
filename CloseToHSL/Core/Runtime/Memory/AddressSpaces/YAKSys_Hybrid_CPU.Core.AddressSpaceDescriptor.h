#pragma once

#include "AddressSpaceDescriptor.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{
	class AddressSpaceDescriptor final : public std::enable_shared_from_this<AddressSpaceDescriptor>
	{
	private:
		const std::shared_ptr<YAKSys_Hybrid_CPU::Memory::AddressSpaceId> AddressSpaceId;
		const AddressSpaceAuthority Authority;
		const unsigned long long GuestPhysicalBase;
		const unsigned long long SizeBytes;
		const unsigned long long Generation;
		const bool CompatibilityProjectionEnabled;

	public:
		AddressSpaceDescriptor();

		AddressSpaceDescriptor(std::shared_ptr<YAKSys_Hybrid_CPU::Memory::AddressSpaceId> addressSpaceId, AddressSpaceAuthority authority, unsigned long long guestPhysicalBase, unsigned long long sizeBytes, unsigned long long generation, bool compatibilityProjectionEnabled);

		std::shared_ptr<YAKSys_Hybrid_CPU::Memory::AddressSpaceId> getAddressSpaceId() const;

		AddressSpaceAuthority getAuthority() const;

		unsigned long long getGuestPhysicalBase() const;

		unsigned long long getSizeBytes() const;

		unsigned long long getGeneration() const;

		bool getCompatibilityProjectionEnabled() const;

		bool isRuntimeAuthoritative() const;

		bool getHasBoundedRange() const;

		bool AllowsRange(unsigned long long guestPhysicalAddress, unsigned long long sizeBytes);

		std::shared_ptr<AddressSpaceDescriptor> WithCompatibilityProjection(bool enabled);
	};
}
