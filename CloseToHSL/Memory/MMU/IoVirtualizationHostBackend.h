#pragma once

#include "../../Core/Runtime/Domains/Descriptors/IoDomain/IoVirtualizationBlock.h"
#include <memory>

// Description: Host I/O virtualization backend used after I/O-domain descriptor admission has selected a generic backend boundary.
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::CloseToHSL::Memory::MMU
{

	class IoVirtualizationHostBackend final : public IIoVirtualizationBackend
	{
	private:
		static const std::shared_ptr<IoVirtualizationHostBackend> Instance;

	public:
		static std::shared_ptr<IoVirtualizationHostBackend> getInstance();

	private:
		IoVirtualizationHostBackend();

	public:
		unsigned long long getLastIotlbInvalidationEpoch() const override;

		std::shared_ptr<IommuDomainBinding> BindDomain(std::shared_ptr<IommuDomainBinding> binding) override;

		void UnbindDomain(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId) override;

		int InvalidateIotlbAll() override;

		int InvalidateIotlbByIoDomainTag(unsigned short ioDomainTag) override;

		int InvalidateIotlbByIoDomain(unsigned short ioDomainTag, unsigned int domainId) override;

		int InvalidateIotlbByDevice(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId) override;
	};

}

inline const std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::MMU::IoVirtualizationHostBackend> HybridCPU_ISE::CloseToHSL::Memory::MMU::IoVirtualizationHostBackend::Instance = std::shared_ptr<IoVirtualizationHostBackend>(new IoVirtualizationHostBackend());
