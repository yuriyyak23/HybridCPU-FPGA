#include "Lane7AcceleratorHandleNamespace.h"
#include "../YAKSys_Hybrid_CPU.Core.Lane7StateBlock.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal Lane7AcceleratorHandleNamespace(Lane7StateBlock lane7)
	Lane7AcceleratorHandleNamespace::Lane7AcceleratorHandleNamespace(std::shared_ptr<Lane7StateBlock> lane7) : _lane7((lane7 != nullptr) ? lane7 : throw std::invalid_argument("lane7"))
	{
	}

	std::shared_ptr<Lane7VirtualHandle> Lane7AcceleratorHandleNamespace::Allocate(unsigned short ownerVirtualThreadId, AcceleratorDeviceId acceleratorId, Lane7VirtualCapability capabilities)
	{
		return getLane7()->AllocateVirtualHandle(ownerVirtualThreadId, acceleratorId, capabilities);
	}

	bool Lane7AcceleratorHandleNamespace::TryAllocate(unsigned short ownerVirtualThreadId, AcceleratorDeviceId acceleratorId, Lane7VirtualCapability capabilities, std::shared_ptr<Lane7VirtualHandle> &handle, std::shared_ptr<Lane7Fault> &fault)
	{
		return getLane7()->TryAllocateVirtualHandle(ownerVirtualThreadId, acceleratorId, capabilities, handle, fault);
	}

	bool Lane7AcceleratorHandleNamespace::TryResolve(unsigned long long virtualHandle, std::shared_ptr<Lane7VirtualHandle> &handle)
	{
		return getLane7()->TryGetVirtualHandle(virtualHandle, handle);
	}

	bool Lane7AcceleratorHandleNamespace::TryResolveForDescriptor(unsigned short executionDomainTag, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<Lane7VirtualHandle> &handle)
	{
		if (!descriptor) throw std::invalid_argument("descriptor");
		return getLane7()->TryFindVirtualHandle(executionDomainTag, descriptor->OwnerBinding->OwnerVirtualThreadId, descriptor->AcceleratorId, handle);
	}

	bool Lane7AcceleratorHandleNamespace::Release(unsigned long long virtualHandle)
	{
		return getLane7()->TryReleaseVirtualHandle(virtualHandle);
	}
}
