#include "AcceleratorResourceModel.h"
#include <limits>
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

	AcceleratorResourceModel::AcceleratorResourceModel(unsigned int baseLatencyCycles, unsigned int cyclesPerElement, unsigned int maxQueueOccupancy, unsigned long long scratchBytes, unsigned long long memoryBandwidthBytesPerCycle)
		: BaseLatencyCycles(baseLatencyCycles), CyclesPerElement(cyclesPerElement), MaxQueueOccupancy(maxQueueOccupancy), ScratchBytes(scratchBytes), MemoryBandwidthBytesPerCycle(memoryBandwidthBytesPerCycle)
	{
		
		
		
		
		
	}

	unsigned int AcceleratorResourceModel::getBaseLatencyCycles() const
	{
		return BaseLatencyCycles;
	}

	unsigned int AcceleratorResourceModel::getCyclesPerElement() const
	{
		return CyclesPerElement;
	}

	unsigned int AcceleratorResourceModel::getMaxQueueOccupancy() const
	{
		return MaxQueueOccupancy;
	}

	unsigned long long AcceleratorResourceModel::getScratchBytes() const
	{
		return ScratchBytes;
	}

	unsigned long long AcceleratorResourceModel::getMemoryBandwidthBytesPerCycle() const
	{
		return MemoryBandwidthBytesPerCycle;
	}

	unsigned long long AcceleratorResourceModel::EstimateLatencyCycles(unsigned long long elementCount)
	{
//		checked
			if (getCyclesPerElement() != 0 && elementCount > (std::numeric_limits<unsigned long long>::max() - getBaseLatencyCycles()) / getCyclesPerElement())
				throw std::overflow_error("Accelerator latency exceeds unsigned 64-bit range");
			return getBaseLatencyCycles() + (elementCount * getCyclesPerElement());
// C# TO C++ CONVERTER TASK: End of the original C# 'checked' block.
	}

	unsigned int AcceleratorResourceModel::EstimateQueueOccupancy(unsigned int commandCount)
	{
		return std::min(commandCount, getMaxQueueOccupancy());
	}
}
