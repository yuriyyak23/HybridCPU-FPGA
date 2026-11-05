#include "YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "../../Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"

namespace YAKSys_Hybrid_CPU::Core
{

	DomainRuntimeContext::DomainRuntimeContext() : DomainRuntimeContext(nullptr, nullptr, nullptr, CapabilityDescriptorSet::getEmpty(), nullptr, 0, 0)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DomainRuntimeContext(ExecutionDomainDescriptor? execution, System.Nullable<MemoryDomainDescriptor> memory, System.Nullable<IoDomainDescriptor> io, CapabilityDescriptorSet capabilities) : this(execution, memory, io, capabilities, secureCompute: null, domainTag: 0, addressSpaceTag: 0)
	DomainRuntimeContext::DomainRuntimeContext(std::shared_ptr<ExecutionDomainDescriptor> execution, std::shared_ptr<MemoryDomainDescriptor> memory, std::shared_ptr<IoDomainDescriptor> io, std::shared_ptr<CapabilityDescriptorSet> capabilities) : DomainRuntimeContext(execution, memory, io, capabilities, nullptr, 0, 0)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DomainRuntimeContext(ExecutionDomainDescriptor? execution, System.Nullable<MemoryDomainDescriptor> memory, System.Nullable<IoDomainDescriptor> io, CapabilityDescriptorSet capabilities, System.Nullable<SecureComputeDomainDescriptor> secureCompute) : this(execution, memory, io, capabilities, secureCompute, domainTag: 0, addressSpaceTag: 0)
	DomainRuntimeContext::DomainRuntimeContext(std::shared_ptr<ExecutionDomainDescriptor> execution, std::shared_ptr<MemoryDomainDescriptor> memory, std::shared_ptr<IoDomainDescriptor> io, std::shared_ptr<CapabilityDescriptorSet> capabilities, std::shared_ptr<SecureComputeDomainDescriptor> secureCompute) : DomainRuntimeContext(execution, memory, io, capabilities, secureCompute, 0, 0)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DomainRuntimeContext(ExecutionDomainDescriptor? execution, System.Nullable<MemoryDomainDescriptor> memory, System.Nullable<IoDomainDescriptor> io, CapabilityDescriptorSet capabilities, System.Nullable<SecureComputeDomainDescriptor> secureCompute, ulong domainTag, ulong addressSpaceTag)
	DomainRuntimeContext::DomainRuntimeContext(std::shared_ptr<ExecutionDomainDescriptor> execution, std::shared_ptr<MemoryDomainDescriptor> memory, std::shared_ptr<IoDomainDescriptor> io, std::shared_ptr<CapabilityDescriptorSet> capabilities, std::shared_ptr<SecureComputeDomainDescriptor> secureCompute, unsigned long long domainTag, unsigned long long addressSpaceTag) : Execution(execution), Memory(memory), Io(io), Capabilities(capabilities), SecureCompute(secureCompute), DomainTag(domainTag), AddressSpaceTag(addressSpaceTag) {}

	std::shared_ptr<ExecutionDomainDescriptor> DomainRuntimeContext::getExecution() const
	{
		return Execution;
	}

	std::shared_ptr<MemoryDomainDescriptor> DomainRuntimeContext::getMemory() const
	{
		return Memory;
	}

	std::shared_ptr<IoDomainDescriptor> DomainRuntimeContext::getIo() const
	{
		return Io;
	}

	std::shared_ptr<CapabilityDescriptorSet> DomainRuntimeContext::getCapabilities() const
	{
		return Capabilities;
	}

	std::shared_ptr<SecureComputeDomainDescriptor> DomainRuntimeContext::getSecureCompute() const
	{
		return SecureCompute;
	}

	unsigned long long DomainRuntimeContext::getDomainTag() const
	{
		return DomainTag;
	}

	unsigned long long DomainRuntimeContext::getAddressSpaceTag() const
	{
		return AddressSpaceTag;
	}

	bool DomainRuntimeContext::getHasExecutionDomain() const
	{
		return getExecution() != nullptr;
	}

	bool DomainRuntimeContext::getHasMemoryDomain() const
	{
		return getMemory() != nullptr;
	}

	bool DomainRuntimeContext::getHasIoDomain() const
	{
		return getIo() != nullptr;
	}

	bool DomainRuntimeContext::getHasRequiredDomains() const
	{
		return getHasExecutionDomain() && getHasMemoryDomain() && getHasIoDomain();
	}

	std::shared_ptr<DomainRuntimeContext> DomainRuntimeContext::WithCapabilities(std::shared_ptr<CapabilityDescriptorSet> capabilities)
	{
		return std::make_shared<DomainRuntimeContext>(getExecution(), getMemory(), getIo(), capabilities, getSecureCompute(), getDomainTag(), getAddressSpaceTag());
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DomainRuntimeContext WithSecureCompute(SecureComputeDomainDescriptor? secureCompute)
	std::shared_ptr<DomainRuntimeContext> DomainRuntimeContext::WithSecureCompute(std::shared_ptr<SecureComputeDomainDescriptor> secureCompute)
	{
		return std::make_shared<DomainRuntimeContext>(getExecution(), getMemory(), getIo(), getCapabilities(), secureCompute, getDomainTag(), getAddressSpaceTag());
	}

	bool DomainRuntimeContext::IsBoundToDomain(unsigned long long domainTag)
	{
		return getDomainTag() != 0 && getDomainTag() == domainTag;
	}

	bool DomainRuntimeContext::IsBoundToAddressSpace(unsigned long long addressSpaceTag)
	{
		return getAddressSpaceTag() != 0 && getAddressSpaceTag() == addressSpaceTag;
	}
}
