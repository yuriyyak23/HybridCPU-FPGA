#include "RuntimeCapabilityIds.Virtualization.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VirtualizationCapabilityAllocation> VirtualizationCapabilityAllocationRegistry::getPhase38Probe()
	{
		return Phase38Probe;
	}
}
