#pragma once

#include "IoVirtualizationBlock.h"
#include "../../../IO/Dma/YAKSys_Hybrid_CPU.Core.DmaWindowDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class IoDomainDescriptor final : public std::enable_shared_from_this<IoDomainDescriptor>
	{
	private:
		const std::shared_ptr<IoVirtualizationBlock> VirtualizationBlock;
		const std::shared_ptr<DmaWindowDescriptor> DmaWindow;
		const bool OwnsDmaAuthority;
		const bool OwnsIommuAuthority;
		const bool CompatibilityProjectionEnabled;

	public:
		IoDomainDescriptor();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public IoDomainDescriptor(IoVirtualizationBlock? virtualizationBlock, System.Nullable<DmaWindowDescriptor> dmaWindow, bool ownsDmaAuthority, bool ownsIommuAuthority, bool compatibilityProjectionEnabled)
		IoDomainDescriptor(std::shared_ptr<IoVirtualizationBlock> virtualizationBlock, std::shared_ptr<DmaWindowDescriptor> dmaWindow, bool ownsDmaAuthority, bool ownsIommuAuthority, bool compatibilityProjectionEnabled);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public IoVirtualizationBlock? VirtualizationBlock {get;}
		std::shared_ptr<IoVirtualizationBlock> getVirtualizationBlock() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DmaWindowDescriptor? DmaWindow {get;}
		std::shared_ptr<DmaWindowDescriptor> getDmaWindow() const;

		bool getOwnsDmaAuthority() const;

		bool getOwnsIommuAuthority() const;

		bool getCompatibilityProjectionEnabled() const;

		bool isAuthoritativeIoStateOwner() const;

		bool getHasVirtualizationBlock() const;

		bool getHasDmaWindow() const;

		bool getHasRequiredIoAuthority() const;

		std::shared_ptr<IoDomainDescriptor> WithCompatibilityProjection(bool enabled);
	};
}
