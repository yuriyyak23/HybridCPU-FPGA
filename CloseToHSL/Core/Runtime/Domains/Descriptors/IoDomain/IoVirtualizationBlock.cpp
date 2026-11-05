#include "IoVirtualizationBlock.h"
#include "../../../../../Memory/MMU/IoVirtualizationHostBackend.h"
#include "../../../../Execution/DmaStreamCompute/DmaStreamComputeOwnerBinding.h"

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{

	IoVirtualizationBlock::IoVirtualizationBlock() : IoVirtualizationBlock(IoVirtualizationHostBackend::getInstance())
	{
	}

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public IoVirtualizationBlock(IIoVirtualizationBackend backend)
	IoVirtualizationBlock::IoVirtualizationBlock(std::shared_ptr<IIoVirtualizationBackend> backend) : _backend((backend != nullptr) ? backend : throw std::invalid_argument("backend"))
	{
	}

	unsigned long long IoVirtualizationBlock::getOwnershipEpoch() const
	{
		return OwnershipEpoch;
	}

	void IoVirtualizationBlock::setOwnershipEpoch(unsigned long long value)
	{
		OwnershipEpoch = value;
	}

	unsigned long long IoVirtualizationBlock::getPolicyEpoch() const
	{
		return PolicyEpoch;
	}

	void IoVirtualizationBlock::setPolicyEpoch(unsigned long long value)
	{
		PolicyEpoch = value;
	}

	unsigned long long IoVirtualizationBlock::getLastIotlbInvalidationEpoch() const
	{
		return _backend->getLastIotlbInvalidationEpoch();
	}

	std::shared_ptr<IommuDomainBinding> IoVirtualizationBlock::BindDomain(std::shared_ptr<IommuDomainBinding> binding)
	{
		std::shared_ptr<IommuDomainBinding> effective = _backend->BindDomain(binding);
		if (!effective || !effective->isValid())
		{
			return nullptr;
		}

		_bindings[std::make_tuple(effective->IoDomainTag, effective->DomainTag, effective->DeviceId)] = effective;
		AdvanceOwnershipEpoch();
		return effective;
	}

	bool IoVirtualizationBlock::UnbindDomain(unsigned short ioDomainTag, unsigned long long domainTag, unsigned int deviceId)
    {
        auto it = _bindings.find(std::make_tuple(ioDomainTag, domainTag, deviceId));
        if (it == _bindings.end()) return false;
        auto binding = it->second;
        _bindings.erase(it);
        _backend->UnbindDomain(binding->IoDomainTag, binding->DomainId, binding->DeviceId);
        AdvanceOwnershipEpoch();
        return true;
    }
bool IoVirtualizationBlock::TryResolveBinding(unsigned short ioDomainTag, unsigned long long domainTag, unsigned int deviceId, std::shared_ptr<IommuDomainBinding> &binding)
    {
        auto it = _bindings.find(std::make_tuple(ioDomainTag, domainTag, deviceId));
        if (it == _bindings.end()) { binding.reset(); return false; }
        binding = it->second;
        return true;
    }
bool IoVirtualizationBlock::TryResolveDescriptorBinding(unsigned short ioDomainTag, std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding, std::shared_ptr<IommuDomainBinding> &binding)
	{
		return TryResolveBinding(ioDomainTag, ownerBinding->OwnerDomainTag, ownerBinding->DeviceId, binding);
	}

	int IoVirtualizationBlock::InvalidateIotlbAll()
	{
		int invalidated = _backend->InvalidateIotlbAll();
		if (invalidated != 0)
		{
			AdvancePolicyEpoch();
		}

		return invalidated;
	}

	int IoVirtualizationBlock::InvalidateIotlbByIoDomainTag(unsigned short ioDomainTag)
	{
		int invalidated = _backend->InvalidateIotlbByIoDomainTag(ioDomainTag);
		if (invalidated != 0)
		{
			AdvancePolicyEpoch();
		}

		return invalidated;
	}

	int IoVirtualizationBlock::InvalidateIotlbByIoDomain(unsigned short ioDomainTag, unsigned int domainId)
	{
		int invalidated = _backend->InvalidateIotlbByIoDomain(ioDomainTag, domainId);
		if (invalidated != 0)
		{
			AdvancePolicyEpoch();
		}

		return invalidated;
	}

	int IoVirtualizationBlock::InvalidateIotlbByDevice(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId)
	{
		int invalidated = _backend->InvalidateIotlbByDevice(ioDomainTag, domainId, deviceId);
		if (invalidated != 0)
		{
			AdvancePolicyEpoch();
		}

		return invalidated;
	}

	void IoVirtualizationBlock::AdvanceOwnershipEpoch()
	{
//		unchecked
			setOwnershipEpoch(getOwnershipEpoch() + 1);
			if (getOwnershipEpoch() == 0)
			{
				setOwnershipEpoch(1);
			}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}

	void IoVirtualizationBlock::AdvancePolicyEpoch()
	{
//		unchecked
			setPolicyEpoch(getPolicyEpoch() + 1);
			if (getPolicyEpoch() == 0)
			{
				setPolicyEpoch(1);
			}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}
}
