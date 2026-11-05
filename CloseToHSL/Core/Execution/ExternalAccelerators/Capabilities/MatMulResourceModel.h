#pragma once

#include "AcceleratorResourceModel.h"
#include "../Descriptors/MatMulDescriptor.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

	class MatMulResourceModel final : public std::enable_shared_from_this<MatMulResourceModel>
	{
	private:
		static inline const std::shared_ptr<MatMulResourceModel> Default = std::make_shared<MatMulResourceModel>(10, 4, 8, 2, 64);
		const unsigned int SetupCycles;
		const unsigned int MacsPerCycle;
		const unsigned int OutputElementsPerWritebackCycle;
		const unsigned int MaxQueueOccupancy;
		const unsigned long long MemoryBandwidthBytesPerCycle;

	public:
		static std::shared_ptr<MatMulResourceModel> getDefault();

		MatMulResourceModel(unsigned int setupCycles, unsigned int macsPerCycle, unsigned int outputElementsPerWritebackCycle, unsigned int maxQueueOccupancy, unsigned long long memoryBandwidthBytesPerCycle);

		unsigned int getSetupCycles() const;

		unsigned int getMacsPerCycle() const;

		unsigned int getOutputElementsPerWritebackCycle() const;

		unsigned int getMaxQueueOccupancy() const;

		unsigned long long getMemoryBandwidthBytesPerCycle() const;

		unsigned long long EstimateLatencyCycles(std::shared_ptr<MatMulDescriptor> descriptor);

		unsigned long long EstimateScratchBytes(std::shared_ptr<MatMulDescriptor> descriptor);

		std::shared_ptr<AcceleratorResourceModel> ToCapabilityResourceModel();

	private:
		static unsigned long long DivideRoundUp(unsigned long long value, unsigned long long divisor);
	};

}
