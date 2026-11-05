#include "Lane7AcceleratorAdmissionPolicy.h"
#include "../YAKSys_Hybrid_CPU.Core.Lane7StateBlock.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal Lane7AcceleratorAdmissionPolicy(Lane7StateBlock lane7)
	Lane7AcceleratorAdmissionPolicy::Lane7AcceleratorAdmissionPolicy(std::shared_ptr<Lane7StateBlock> lane7) : _lane7((lane7 != nullptr) ? lane7 : throw std::invalid_argument("lane7"))
	{
	}

	bool Lane7AcceleratorAdmissionPolicy::TryValidateSubmit(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, unsigned short executionDomainTag, unsigned short addressSpaceTag, std::shared_ptr<Lane7VirtualHandle> &handle, std::shared_ptr<Lane7Fault> &fault)
	{
		return getLane7()->TryValidateGuestSubmit(descriptor, executionDomainTag, addressSpaceTag, handle, fault);
	}

	std::shared_ptr<Lane7PressureSnapshot> Lane7AcceleratorAdmissionPolicy::ObserveSubmitPollPressure(unsigned short ownerVirtualThreadId)
	{
		return getLane7()->ObserveSubmitPollPressure(ownerVirtualThreadId);
	}
}
