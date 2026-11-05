#pragma once

#include <map>
#include <tuple>
#include "../../../IO/Dma/DmaDomainBinding.h"
#include <stdexcept>
#include <memory>

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute { struct DmaStreamComputeOwnerBinding; }
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{
	class IIoVirtualizationBackend
	{
	public:
		virtual unsigned long long getLastIotlbInvalidationEpoch() const = 0;

		virtual std::shared_ptr<IommuDomainBinding> BindDomain(std::shared_ptr<IommuDomainBinding> binding) = 0;

		virtual void UnbindDomain(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId) = 0;

		virtual int InvalidateIotlbAll() = 0;

		virtual int InvalidateIotlbByIoDomainTag(unsigned short ioDomainTag) = 0;

		virtual int InvalidateIotlbByIoDomain(unsigned short ioDomainTag, unsigned int domainId) = 0;

		virtual int InvalidateIotlbByDevice(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId) = 0;
	};

	class IoVirtualizationBlock final : public std::enable_shared_from_this<IoVirtualizationBlock>
	{
	private:
		unsigned long long OwnershipEpoch = 0;
		unsigned long long PolicyEpoch = 0;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<(ushort IoDomainTag, ulong DomainTag, uint DeviceId), IommuDomainBinding> _bindings = new();
		std::map<std::tuple<unsigned short, unsigned long long, unsigned int>, std::shared_ptr<IommuDomainBinding>> _bindings;
		const std::shared_ptr<IIoVirtualizationBackend> _backend;

	public:
		IoVirtualizationBlock();

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public IoVirtualizationBlock(IIoVirtualizationBackend backend)
		IoVirtualizationBlock(std::shared_ptr<IIoVirtualizationBackend> backend);

		unsigned long long getOwnershipEpoch() const;
	private:
		void setOwnershipEpoch(unsigned long long value);

	public:
		unsigned long long getPolicyEpoch() const;
	private:
		void setPolicyEpoch(unsigned long long value);

	public:
		unsigned long long getLastIotlbInvalidationEpoch() const;

		std::shared_ptr<IommuDomainBinding> BindDomain(std::shared_ptr<IommuDomainBinding> binding);

		bool UnbindDomain(unsigned short ioDomainTag, unsigned long long domainTag, unsigned int deviceId);

		bool TryResolveBinding(unsigned short ioDomainTag, unsigned long long domainTag, unsigned int deviceId, std::shared_ptr<IommuDomainBinding> &binding);

		bool TryResolveDescriptorBinding(unsigned short ioDomainTag, std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding, std::shared_ptr<IommuDomainBinding> &binding);

		int InvalidateIotlbAll();

		int InvalidateIotlbByIoDomainTag(unsigned short ioDomainTag);

		int InvalidateIotlbByIoDomain(unsigned short ioDomainTag, unsigned int domainId);

		int InvalidateIotlbByDevice(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId);

	private:
		void AdvanceOwnershipEpoch();

		void AdvancePolicyEpoch();
	};
}
