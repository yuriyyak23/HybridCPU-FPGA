#include "YAKSys_Hybrid_CPU.Core.IoDomainDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	IoDomainDescriptor::IoDomainDescriptor() : IoDomainDescriptor(nullptr, nullptr, true, true, true)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public IoDomainDescriptor(IoVirtualizationBlock? virtualizationBlock, System.Nullable<DmaWindowDescriptor> dmaWindow, bool ownsDmaAuthority, bool ownsIommuAuthority, bool compatibilityProjectionEnabled)
	IoDomainDescriptor::IoDomainDescriptor(std::shared_ptr<IoVirtualizationBlock> virtualizationBlock, std::shared_ptr<DmaWindowDescriptor> dmaWindow, bool ownsDmaAuthority, bool ownsIommuAuthority, bool compatibilityProjectionEnabled) : VirtualizationBlock(virtualizationBlock), DmaWindow(dmaWindow), OwnsDmaAuthority(ownsDmaAuthority), OwnsIommuAuthority(ownsIommuAuthority), CompatibilityProjectionEnabled(compatibilityProjectionEnabled) {}

	std::shared_ptr<IoVirtualizationBlock> IoDomainDescriptor::getVirtualizationBlock() const
	{
		return VirtualizationBlock;
	}

	std::shared_ptr<DmaWindowDescriptor> IoDomainDescriptor::getDmaWindow() const
	{
		return DmaWindow;
	}

	bool IoDomainDescriptor::getOwnsDmaAuthority() const
	{
		return OwnsDmaAuthority;
	}

	bool IoDomainDescriptor::getOwnsIommuAuthority() const
	{
		return OwnsIommuAuthority;
	}

	bool IoDomainDescriptor::getCompatibilityProjectionEnabled() const
	{
		return CompatibilityProjectionEnabled;
	}

	bool IoDomainDescriptor::isAuthoritativeIoStateOwner() const
	{
		return true;
	}

	bool IoDomainDescriptor::getHasVirtualizationBlock() const
	{
		return getVirtualizationBlock() != nullptr;
	}

	bool IoDomainDescriptor::getHasDmaWindow() const
	{
		return getDmaWindow() != nullptr;
	}

	bool IoDomainDescriptor::getHasRequiredIoAuthority() const
	{
		return getOwnsDmaAuthority() && getOwnsIommuAuthority();
	}

	std::shared_ptr<IoDomainDescriptor> IoDomainDescriptor::WithCompatibilityProjection(bool enabled)
	{
		return std::make_shared<IoDomainDescriptor>(getVirtualizationBlock(), getDmaWindow(), getOwnsDmaAuthority(), getOwnsIommuAuthority(), enabled);
	}
}
