#pragma once

#include <cmath>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

	class AcceleratorResourceModel final : public std::enable_shared_from_this<AcceleratorResourceModel>
	{
	private:
		const unsigned int BaseLatencyCycles;
		const unsigned int CyclesPerElement;
		const unsigned int MaxQueueOccupancy;
		const unsigned long long ScratchBytes;
		const unsigned long long MemoryBandwidthBytesPerCycle;

	public:
		AcceleratorResourceModel(unsigned int baseLatencyCycles, unsigned int cyclesPerElement, unsigned int maxQueueOccupancy, unsigned long long scratchBytes, unsigned long long memoryBandwidthBytesPerCycle);

		unsigned int getBaseLatencyCycles() const;

		unsigned int getCyclesPerElement() const;

		unsigned int getMaxQueueOccupancy() const;

		unsigned long long getScratchBytes() const;

		unsigned long long getMemoryBandwidthBytesPerCycle() const;

		unsigned long long EstimateLatencyCycles(unsigned long long elementCount);

		unsigned int EstimateQueueOccupancy(unsigned int commandCount);
	};

}
