#include "YAKSys_Hybrid_CPU.Core.SecureMemoryDomainDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureMemoryDomainDescriptor::SecureMemoryDomainDescriptor() : SecureMemoryDomainDescriptor(0, 0, SecureRevocationEpoch::Unmaterialized, std::vector<std::shared_ptr<SecureMemoryRegionDescriptor>>{}, SecureMemoryDmaPolicy::Denied)
	{
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureMemoryDomainDescriptor(ulong domainTag, ulong addressSpaceTag, SecureRevocationEpoch policyEpoch, IReadOnlyList<SecureMemoryRegionDescriptor> regions) : this(domainTag, addressSpaceTag, policyEpoch, regions, SecureMemoryDmaPolicy.Denied)
	SecureMemoryDomainDescriptor::SecureMemoryDomainDescriptor(unsigned long long domainTag, unsigned long long addressSpaceTag, std::shared_ptr<SecureRevocationEpoch> policyEpoch, const std::vector<std::shared_ptr<SecureMemoryRegionDescriptor>> &regions) : SecureMemoryDomainDescriptor(domainTag, addressSpaceTag, policyEpoch, regions, SecureMemoryDmaPolicy::Denied)
	{
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureMemoryDomainDescriptor(ulong domainTag, ulong addressSpaceTag, SecureRevocationEpoch policyEpoch, IReadOnlyList<SecureMemoryRegionDescriptor> regions, SecureMemoryDmaPolicy dmaPolicy)
	SecureMemoryDomainDescriptor::SecureMemoryDomainDescriptor(unsigned long long domainTag, unsigned long long addressSpaceTag, std::shared_ptr<SecureRevocationEpoch> policyEpoch, const std::vector<std::shared_ptr<SecureMemoryRegionDescriptor>> &regions, SecureMemoryDmaPolicy dmaPolicy) : DomainTag(domainTag), AddressSpaceTag(addressSpaceTag), PolicyEpoch(policyEpoch), Regions(regions), DmaPolicy(dmaPolicy) {}

	std::shared_ptr<SecureMemoryDomainDescriptor> SecureMemoryDomainDescriptor::getDisabled()
	{
		return Disabled;
	}

	unsigned long long SecureMemoryDomainDescriptor::getDomainTag() const
	{
		return DomainTag;
	}

	unsigned long long SecureMemoryDomainDescriptor::getAddressSpaceTag() const
	{
		return AddressSpaceTag;
	}

	std::shared_ptr<SecureRevocationEpoch> SecureMemoryDomainDescriptor::getPolicyEpoch() const
	{
		return PolicyEpoch;
	}

	const std::vector<std::shared_ptr<SecureMemoryRegionDescriptor>> SecureMemoryDomainDescriptor::getRegions() const
	{
		return Regions;
	}

	SecureMemoryDmaPolicy SecureMemoryDomainDescriptor::getDmaPolicy() const
	{
		return DmaPolicy;
	}

	bool SecureMemoryDomainDescriptor::isMaterialized() const
	{
		return getDomainTag() != 0 && getAddressSpaceTag() != 0 && getPolicyEpoch()->isMaterialized();
	}

	bool SecureMemoryDomainDescriptor::getRequiresStageBPolicyRoute() const
	{
		return isMaterialized() && getRegions().size() != 0;
	}

	bool SecureMemoryDomainDescriptor::getHasPrivateMemory() const
	{
		for (auto region : getRegions())
		{
			if (region->IsPrivate && !region->IsEmpty)
			{
				return true;
			}
		}

		return false;
	}

	bool SecureMemoryDomainDescriptor::getHasSharedMemory() const
	{
		return HasRegionClass(SecureMemoryRegionClass::Shared);
	}

	bool SecureMemoryDomainDescriptor::getHasMeasuredMemory() const
	{
		return HasRegionClass(SecureMemoryRegionClass::Measured);
	}

	bool SecureMemoryDomainDescriptor::getHasRuntimeMutableMemory() const
	{
		return HasRegionClass(SecureMemoryRegionClass::RuntimeMutable);
	}

	bool SecureMemoryDomainDescriptor::getAllowsExplicitSharedDma() const
	{
		return isMaterialized() && getDmaPolicy() == SecureMemoryDmaPolicy::ExplicitSharedBuffersWithTypedGrant && getHasSharedMemory();
	}

	bool SecureMemoryDomainDescriptor::IsBoundTo(unsigned long long domainTag, unsigned long long addressSpaceTag)
	{
		return getDomainTag() != 0 && getAddressSpaceTag() != 0 && getDomainTag() == domainTag && getAddressSpaceTag() == addressSpaceTag;
	}

	bool SecureMemoryDomainDescriptor::TryFindRegion(unsigned long long address, unsigned long long length, std::shared_ptr<SecureMemoryRegionDescriptor> &region)
	{
		for (auto candidate : getRegions())
		{
			if (candidate->Contains(address, length))
			{
				region = candidate;
				return true;
			}
		}

		region = nullptr;
		return false;
	}

	bool SecureMemoryDomainDescriptor::HasRegionClass(SecureMemoryRegionClass regionClass) const
	{
		for (auto region : getRegions())
		{
			if (region->RegionClass == regionClass && !region->IsEmpty)
			{
				return true;
			}
		}

		return false;
	}
}
