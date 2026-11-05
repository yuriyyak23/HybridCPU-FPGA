#pragma once

#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class ExecutionDomainDescriptor;
	class MemoryDomainDescriptor;
	class IoDomainDescriptor;
	class CapabilityDescriptorSet;
	class SecureComputeDomainDescriptor;
	class DomainRuntimeContext final : public std::enable_shared_from_this<DomainRuntimeContext>
	{
	private:
		const std::shared_ptr<ExecutionDomainDescriptor> Execution;
		const std::shared_ptr<MemoryDomainDescriptor> Memory;
		const std::shared_ptr<IoDomainDescriptor> Io;
		const std::shared_ptr<CapabilityDescriptorSet> Capabilities;
		const std::shared_ptr<SecureComputeDomainDescriptor> SecureCompute;
		const unsigned long long DomainTag;
		const unsigned long long AddressSpaceTag;

	public:
		DomainRuntimeContext();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DomainRuntimeContext(ExecutionDomainDescriptor? execution, System.Nullable<MemoryDomainDescriptor> memory, System.Nullable<IoDomainDescriptor> io, CapabilityDescriptorSet capabilities) : this(execution, memory, io, capabilities, secureCompute: null, domainTag: 0, addressSpaceTag: 0)
		DomainRuntimeContext(std::shared_ptr<ExecutionDomainDescriptor> execution, std::shared_ptr<MemoryDomainDescriptor> memory, std::shared_ptr<IoDomainDescriptor> io, std::shared_ptr<CapabilityDescriptorSet> capabilities);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DomainRuntimeContext(ExecutionDomainDescriptor? execution, System.Nullable<MemoryDomainDescriptor> memory, System.Nullable<IoDomainDescriptor> io, CapabilityDescriptorSet capabilities, System.Nullable<SecureComputeDomainDescriptor> secureCompute) : this(execution, memory, io, capabilities, secureCompute, domainTag: 0, addressSpaceTag: 0)
		DomainRuntimeContext(std::shared_ptr<ExecutionDomainDescriptor> execution, std::shared_ptr<MemoryDomainDescriptor> memory, std::shared_ptr<IoDomainDescriptor> io, std::shared_ptr<CapabilityDescriptorSet> capabilities, std::shared_ptr<SecureComputeDomainDescriptor> secureCompute);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DomainRuntimeContext(ExecutionDomainDescriptor? execution, System.Nullable<MemoryDomainDescriptor> memory, System.Nullable<IoDomainDescriptor> io, CapabilityDescriptorSet capabilities, System.Nullable<SecureComputeDomainDescriptor> secureCompute, ulong domainTag, ulong addressSpaceTag)
		DomainRuntimeContext(std::shared_ptr<ExecutionDomainDescriptor> execution, std::shared_ptr<MemoryDomainDescriptor> memory, std::shared_ptr<IoDomainDescriptor> io, std::shared_ptr<CapabilityDescriptorSet> capabilities, std::shared_ptr<SecureComputeDomainDescriptor> secureCompute, unsigned long long domainTag, unsigned long long addressSpaceTag);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExecutionDomainDescriptor? Execution {get;}
		std::shared_ptr<ExecutionDomainDescriptor> getExecution() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MemoryDomainDescriptor? Memory {get;}
		std::shared_ptr<MemoryDomainDescriptor> getMemory() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public IoDomainDescriptor? Io {get;}
		std::shared_ptr<IoDomainDescriptor> getIo() const;

		std::shared_ptr<CapabilityDescriptorSet> getCapabilities() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureComputeDomainDescriptor? SecureCompute {get;}
		std::shared_ptr<SecureComputeDomainDescriptor> getSecureCompute() const;

		unsigned long long getDomainTag() const;

		unsigned long long getAddressSpaceTag() const;

		bool getHasExecutionDomain() const;

		bool getHasMemoryDomain() const;

		bool getHasIoDomain() const;

		bool getHasRequiredDomains() const;

		std::shared_ptr<DomainRuntimeContext> WithCapabilities(std::shared_ptr<CapabilityDescriptorSet> capabilities);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DomainRuntimeContext WithSecureCompute(SecureComputeDomainDescriptor? secureCompute)
		std::shared_ptr<DomainRuntimeContext> WithSecureCompute(std::shared_ptr<SecureComputeDomainDescriptor> secureCompute);

		bool IsBoundToDomain(unsigned long long domainTag);

		bool IsBoundToAddressSpace(unsigned long long addressSpaceTag);
	};
}
