#include "YAKSys_Hybrid_CPU.Core.IoDomainRuntime.h"


namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<IoDomainRuntimeResult> IoDomainRuntime::Validate(std::shared_ptr<IoDomainRuntimeRequest> request)
	{
		if (request == nullptr || request->Descriptor == nullptr)
		{
			return IoDomainRuntimeResult::Denied(IoDomainRuntimeDecision::MissingDescriptor, L"I/O domain runtime requires an I/O-domain descriptor.");
		}

		if (!request->Descriptor->isAuthoritativeIoStateOwner())
		{
			return IoDomainRuntimeResult::Denied(IoDomainRuntimeDecision::DescriptorAuthorityDenied, L"I/O state authority must belong to the I/O-domain descriptor.");
		}

		if (request->RequiresDmaAuthority && !request->Descriptor->getOwnsDmaAuthority())
		{
			return IoDomainRuntimeResult::Denied(IoDomainRuntimeDecision::DmaAuthorityDenied, L"DMA authority is not owned by the I/O-domain descriptor.");
		}

		if (request->RequiresIommuAuthority && !request->Descriptor->getOwnsIommuAuthority())
		{
			return IoDomainRuntimeResult::Denied(IoDomainRuntimeDecision::IommuAuthorityDenied, L"IOMMU authority is not owned by the I/O-domain descriptor.");
		}

		if (request->RequiresVirtualizationBlock && !request->Descriptor->getHasVirtualizationBlock())
		{
			return IoDomainRuntimeResult::Denied(IoDomainRuntimeDecision::MissingVirtualizationBlock, L"I/O domain runtime requires an I/O virtualization block.");
		}

		if (request->RequiresDmaWindow && !request->Descriptor->getHasDmaWindow())
		{
			return IoDomainRuntimeResult::Denied(IoDomainRuntimeDecision::MissingDmaWindow, L"I/O domain runtime requires a DMA window descriptor.");
		}

		if (request->RequiresCompatibilityProjection && !request->Descriptor->getCompatibilityProjectionEnabled())
		{
			return IoDomainRuntimeResult::Denied(IoDomainRuntimeDecision::CompatibilityProjectionDenied, L"I/O descriptor denies compatibility projection.");
		}

		return IoDomainRuntimeResult::Allowed;
	}

	bool IoDomainRuntime::CanAdmit(std::shared_ptr<IoDomainRuntimeRequest> request)
	{
		return Validate(request)->isAllowed();
	}
}
