#pragma once
#include "../Lane7StateBlock.h"


#include <stdexcept>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core { class Lane7StateBlock; class LaneCompletionDescriptor; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators
{

	class Lane7AcceleratorAdmissionPolicy final : public std::enable_shared_from_this<Lane7AcceleratorAdmissionPolicy>
	{
	private:
		const std::weak_ptr<Lane7StateBlock> _lane7;
		std::shared_ptr<Lane7StateBlock> getLane7() const { auto lane = _lane7.lock(); if (!lane) throw std::runtime_error("Lane7 state no longer exists"); return lane; }

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal Lane7AcceleratorAdmissionPolicy(Lane7StateBlock lane7)
		Lane7AcceleratorAdmissionPolicy(std::shared_ptr<Lane7StateBlock> lane7);

		bool TryValidateSubmit(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, unsigned short executionDomainTag, unsigned short addressSpaceTag, std::shared_ptr<Lane7VirtualHandle> &handle, std::shared_ptr<Lane7Fault> &fault);

		std::shared_ptr<Lane7PressureSnapshot> ObserveSubmitPollPressure(unsigned short ownerVirtualThreadId);
	};

}
