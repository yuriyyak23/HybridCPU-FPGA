#pragma once

#include "SecureMemoryDomainDescriptor.h"
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureMemoryDomainDescriptor final : public std::enable_shared_from_this<SecureMemoryDomainDescriptor>
	{
	private:
		static inline const std::shared_ptr<SecureMemoryDomainDescriptor> Disabled = std::make_shared<SecureMemoryDomainDescriptor>();
		const unsigned long long DomainTag;
		const unsigned long long AddressSpaceTag;
		const std::shared_ptr<SecureRevocationEpoch> PolicyEpoch;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<SecureMemoryRegionDescriptor> Regions;
		const std::vector<std::shared_ptr<SecureMemoryRegionDescriptor>> Regions;
		const SecureMemoryDmaPolicy DmaPolicy;

	public:
		SecureMemoryDomainDescriptor();

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureMemoryDomainDescriptor(ulong domainTag, ulong addressSpaceTag, SecureRevocationEpoch policyEpoch, IReadOnlyList<SecureMemoryRegionDescriptor> regions) : this(domainTag, addressSpaceTag, policyEpoch, regions, SecureMemoryDmaPolicy.Denied)
		SecureMemoryDomainDescriptor(unsigned long long domainTag, unsigned long long addressSpaceTag, std::shared_ptr<SecureRevocationEpoch> policyEpoch, const std::vector<std::shared_ptr<SecureMemoryRegionDescriptor>> &regions);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureMemoryDomainDescriptor(ulong domainTag, ulong addressSpaceTag, SecureRevocationEpoch policyEpoch, IReadOnlyList<SecureMemoryRegionDescriptor> regions, SecureMemoryDmaPolicy dmaPolicy)
		SecureMemoryDomainDescriptor(unsigned long long domainTag, unsigned long long addressSpaceTag, std::shared_ptr<SecureRevocationEpoch> policyEpoch, const std::vector<std::shared_ptr<SecureMemoryRegionDescriptor>> &regions, SecureMemoryDmaPolicy dmaPolicy);

		static std::shared_ptr<SecureMemoryDomainDescriptor> getDisabled();

		unsigned long long getDomainTag() const;

		unsigned long long getAddressSpaceTag() const;

		std::shared_ptr<SecureRevocationEpoch> getPolicyEpoch() const;

		const std::vector<std::shared_ptr<SecureMemoryRegionDescriptor>> getRegions() const;

		SecureMemoryDmaPolicy getDmaPolicy() const;

		bool isMaterialized() const;

		bool getRequiresStageBPolicyRoute() const;

		bool getHasPrivateMemory() const;

		bool getHasSharedMemory() const;

		bool getHasMeasuredMemory() const;

		bool getHasRuntimeMutableMemory() const;

		bool getAllowsExplicitSharedDma() const;

		bool IsBoundTo(unsigned long long domainTag, unsigned long long addressSpaceTag);

		bool TryFindRegion(unsigned long long address, unsigned long long length, std::shared_ptr<SecureMemoryRegionDescriptor> &region);

	private:
		bool HasRegionClass(SecureMemoryRegionClass regionClass) const;
	};
}
