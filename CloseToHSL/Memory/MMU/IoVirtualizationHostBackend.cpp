#include "IoVirtualizationHostBackend.h"
#include "../../Core/Virtualization/Compatibility/Adapters/IO/YAKSys_Hybrid_CPU.Memory.IOMMU.h"

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::CloseToHSL::Memory::MMU
{

	std::shared_ptr<IoVirtualizationHostBackend> IoVirtualizationHostBackend::getInstance()
	{
		return Instance;
	}

	IoVirtualizationHostBackend::IoVirtualizationHostBackend()
	{
	}

	unsigned long long IoVirtualizationHostBackend::getLastIotlbInvalidationEpoch() const
	{
		return IOMMU::getIotlbInvalidationEpoch();
	}

	std::shared_ptr<IommuDomainBinding> IoVirtualizationHostBackend::BindDomain(std::shared_ptr<IommuDomainBinding> binding)
	{
		return IOMMU::BindIoDomain(binding);
	}

	void IoVirtualizationHostBackend::UnbindDomain(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId)
	{
		IOMMU::UnbindIoDomain(ioDomainTag, domainId, deviceId);
	}

	int IoVirtualizationHostBackend::InvalidateIotlbAll()
	{
		return IOMMU::InvalidateIotlbAll();
	}

	int IoVirtualizationHostBackend::InvalidateIotlbByIoDomainTag(unsigned short ioDomainTag)
	{
		return IOMMU::InvalidateIotlbByIoDomainTag(ioDomainTag);
	}

	int IoVirtualizationHostBackend::InvalidateIotlbByIoDomain(unsigned short ioDomainTag, unsigned int domainId)
	{
		return IOMMU::InvalidateIotlbByIoDomain(ioDomainTag, domainId);
	}

	int IoVirtualizationHostBackend::InvalidateIotlbByDevice(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId)
	{
		return IOMMU::InvalidateIotlbByDevice(ioDomainTag, domainId, deviceId);
	}
}
