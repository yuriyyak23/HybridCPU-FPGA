#include "YAKSys_Hybrid_CPU.Core.IotlbInvalidationService.h"
#include "../../Domains/Descriptors/IoDomain/YAKSys_Hybrid_CPU.Core.IoDomainDescriptor.h"
#include "../../Domains/Descriptors/IoDomain/IoVirtualizationBlock.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<IotlbInvalidationResult> IotlbInvalidationService::Invalidate(std::shared_ptr<DomainRuntimeContext> context, IotlbInvalidationScope scope, unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: IoDomainDescriptor? io = context.Io;
		std::shared_ptr<IoDomainDescriptor> io = context != nullptr ? context->getIo() : nullptr;
		if (io == nullptr)
		{
			return Deny(IotlbInvalidationDecision::MissingIoDomain, L"IOTLB invalidation requires an I/O-domain descriptor.");
		}

		if (!io->getHasRequiredIoAuthority())
		{
			return Deny(IotlbInvalidationDecision::RuntimeAuthorityMissing, L"I/O-domain descriptor does not own IOMMU invalidation authority.");
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: IoVirtualizationBlock? virtualizationBlock = io.VirtualizationBlock;
		std::shared_ptr<IoVirtualizationBlock> virtualizationBlock = io->getVirtualizationBlock();
		if (virtualizationBlock == nullptr)
		{
			return Deny(IotlbInvalidationDecision::MissingVirtualizationBlock, L"IOTLB invalidation requires an explicit I/O virtualization block.");
		}

			switch (scope)
		{
		case IotlbInvalidationScope::All:
			return IotlbInvalidationResult::Allowed(virtualizationBlock->InvalidateIotlbAll());
		case IotlbInvalidationScope::IoDomainTag:
			if (ioDomainTag != 0)
			{
				return IotlbInvalidationResult::Allowed(virtualizationBlock->InvalidateIotlbByIoDomainTag(ioDomainTag));
			}
			break;
		case IotlbInvalidationScope::Domain:
			if (ioDomainTag != 0 && domainId != 0)
			{
				return IotlbInvalidationResult::Allowed(virtualizationBlock->InvalidateIotlbByIoDomain(ioDomainTag, domainId));
			}
			break;
		case IotlbInvalidationScope::Device:
			if (ioDomainTag != 0 && domainId != 0 && deviceId != 0)
			{
				return IotlbInvalidationResult::Allowed(virtualizationBlock->InvalidateIotlbByDevice(ioDomainTag, domainId, deviceId));
			}
			break;
		default:
			break;
		}

		return Deny(
			IotlbInvalidationDecision::InvalidDescriptor,
			L"IOTLB invalidation descriptor is missing required domain identifiers.");
	}

	std::shared_ptr<IotlbInvalidationResult> IotlbInvalidationService::Deny(IotlbInvalidationDecision decision, const std::wstring &message)
	{
		return IotlbInvalidationResult::Denied(decision, message);
	}
}
