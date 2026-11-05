#pragma once


#include "../../../../Execution/ExternalAccelerators/Descriptors/AcceleratorCommandDescriptor.h"
#include "../Lane7StateBlock.h"
#include <stdexcept>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core { class Lane7StateBlock; class LaneCompletionDescriptor; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators
{

	class Lane7AcceleratorHandleNamespace final : public std::enable_shared_from_this<Lane7AcceleratorHandleNamespace>
	{
	private:
		const std::weak_ptr<Lane7StateBlock> _lane7;
		std::shared_ptr<Lane7StateBlock> getLane7() const { auto lane = _lane7.lock(); if (!lane) throw std::runtime_error("Lane7 state no longer exists"); return lane; }

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal Lane7AcceleratorHandleNamespace(Lane7StateBlock lane7)
		Lane7AcceleratorHandleNamespace(std::shared_ptr<Lane7StateBlock> lane7);

		std::shared_ptr<Lane7VirtualHandle> Allocate(unsigned short ownerVirtualThreadId, AcceleratorDeviceId acceleratorId, Lane7VirtualCapability capabilities);

		bool TryAllocate(unsigned short ownerVirtualThreadId, AcceleratorDeviceId acceleratorId, Lane7VirtualCapability capabilities, std::shared_ptr<Lane7VirtualHandle> &handle, std::shared_ptr<Lane7Fault> &fault);

		bool TryResolve(unsigned long long virtualHandle, std::shared_ptr<Lane7VirtualHandle> &handle);

		bool TryResolveForDescriptor(unsigned short executionDomainTag, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<Lane7VirtualHandle> &handle);

		bool Release(unsigned long long virtualHandle);
	};

}
