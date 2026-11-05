#include "YAKSys_Hybrid_CPU.Core.AddressSpaceDescriptor.h"

using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{

	AddressSpaceDescriptor::AddressSpaceDescriptor() : AddressSpaceDescriptor(nullptr, AddressSpaceAuthority::RuntimeOwned, 0, 0, 0, false)
	{
	}

	AddressSpaceDescriptor::AddressSpaceDescriptor(std::shared_ptr<YAKSys_Hybrid_CPU::Memory::AddressSpaceId> addressSpaceId, AddressSpaceAuthority authority, unsigned long long guestPhysicalBase, unsigned long long sizeBytes, unsigned long long generation, bool compatibilityProjectionEnabled) : AddressSpaceId(addressSpaceId), Authority(authority), GuestPhysicalBase(guestPhysicalBase), SizeBytes(sizeBytes), Generation(generation), CompatibilityProjectionEnabled(compatibilityProjectionEnabled) {}

	std::shared_ptr<YAKSys_Hybrid_CPU::Memory::AddressSpaceId> AddressSpaceDescriptor::getAddressSpaceId() const
	{
		return AddressSpaceId;
	}

	AddressSpaceAuthority AddressSpaceDescriptor::getAuthority() const
	{
		return Authority;
	}

	unsigned long long AddressSpaceDescriptor::getGuestPhysicalBase() const
	{
		return GuestPhysicalBase;
	}

	unsigned long long AddressSpaceDescriptor::getSizeBytes() const
	{
		return SizeBytes;
	}

	unsigned long long AddressSpaceDescriptor::getGeneration() const
	{
		return Generation;
	}

	bool AddressSpaceDescriptor::getCompatibilityProjectionEnabled() const
	{
		return CompatibilityProjectionEnabled;
	}

	bool AddressSpaceDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == AddressSpaceAuthority::RuntimeOwned;
	}

	bool AddressSpaceDescriptor::getHasBoundedRange() const
	{
		return getSizeBytes() != 0;
	}

	bool AddressSpaceDescriptor::AllowsRange(unsigned long long guestPhysicalAddress, unsigned long long sizeBytes)
	{
		if (!isRuntimeAuthoritative() || !getHasBoundedRange() || sizeBytes == 0)
		{
			return false;
		}

		unsigned long long end = guestPhysicalAddress + sizeBytes - 1;
		unsigned long long domainEnd = getGuestPhysicalBase() + getSizeBytes() - 1;
		if (end < guestPhysicalAddress || domainEnd < getGuestPhysicalBase())
		{
			return false;
		}

		return guestPhysicalAddress >= getGuestPhysicalBase() && end <= domainEnd;
	}

	std::shared_ptr<AddressSpaceDescriptor> AddressSpaceDescriptor::WithCompatibilityProjection(bool enabled)
	{
		return std::make_shared<AddressSpaceDescriptor>(getAddressSpaceId(), getAuthority(), getGuestPhysicalBase(), getSizeBytes(), getGeneration(), enabled);
	}
}
