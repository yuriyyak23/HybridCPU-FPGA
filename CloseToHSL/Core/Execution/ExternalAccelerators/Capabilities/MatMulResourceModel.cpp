#include "MatMulResourceModel.h"
#include <limits>
#include <stdexcept>
#include "../Descriptors/MatMulDescriptor.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{
	namespace {
		unsigned long long CheckedMultiply(unsigned long long a, unsigned long long b) { if (b && a > std::numeric_limits<unsigned long long>::max() / b) throw std::overflow_error("MatMul resource overflow"); return a * b; }
		unsigned long long CheckedAdd(unsigned long long a, unsigned long long b) { if (a > std::numeric_limits<unsigned long long>::max() - b) throw std::overflow_error("MatMul resource overflow"); return a + b; }
	}

	std::shared_ptr<MatMulResourceModel> MatMulResourceModel::getDefault()
	{
		return Default;
	}

	MatMulResourceModel::MatMulResourceModel(unsigned int setupCycles, unsigned int macsPerCycle, unsigned int outputElementsPerWritebackCycle, unsigned int maxQueueOccupancy, unsigned long long memoryBandwidthBytesPerCycle)
		: SetupCycles(setupCycles), MacsPerCycle(macsPerCycle), OutputElementsPerWritebackCycle(outputElementsPerWritebackCycle), MaxQueueOccupancy(maxQueueOccupancy), MemoryBandwidthBytesPerCycle(memoryBandwidthBytesPerCycle)
	{
		if (macsPerCycle == 0)
		{
			throw std::out_of_range("MatMul resource model requires a non-zero MAC throughput.");
		}

		if (outputElementsPerWritebackCycle == 0)
		{
			throw std::out_of_range("MatMul resource model requires non-zero writeback throughput.");
		}
	}

	unsigned int MatMulResourceModel::getSetupCycles() const
	{
		return SetupCycles;
	}

	unsigned int MatMulResourceModel::getMacsPerCycle() const
	{
		return MacsPerCycle;
	}

	unsigned int MatMulResourceModel::getOutputElementsPerWritebackCycle() const
	{
		return OutputElementsPerWritebackCycle;
	}

	unsigned int MatMulResourceModel::getMaxQueueOccupancy() const
	{
		return MaxQueueOccupancy;
	}

	unsigned long long MatMulResourceModel::getMemoryBandwidthBytesPerCycle() const
	{
		return MemoryBandwidthBytesPerCycle;
	}

	unsigned long long MatMulResourceModel::EstimateLatencyCycles(std::shared_ptr<MatMulDescriptor> descriptor)
	{
		if (!descriptor) throw std::invalid_argument("descriptor");
//		checked
			unsigned long long macs = CheckedMultiply(CheckedMultiply(descriptor->M, descriptor->N), descriptor->K);
			unsigned long long outputElements = CheckedMultiply(descriptor->M, descriptor->N);
			unsigned long long computeCycles = DivideRoundUp(macs, getMacsPerCycle());
			unsigned long long writebackCycles = DivideRoundUp(outputElements, getOutputElementsPerWritebackCycle());
			return CheckedAdd(CheckedAdd(getSetupCycles(), computeCycles), writebackCycles);
// C# TO C++ CONVERTER TASK: End of the original C# 'checked' block.
	}

	unsigned long long MatMulResourceModel::EstimateScratchBytes(std::shared_ptr<MatMulDescriptor> descriptor)
	{
		if (!descriptor) throw std::invalid_argument("descriptor");
		unsigned short inputBytes = MatMulDescriptorValidator::DatatypeSizeBytes(descriptor->Datatypes.InputDatatype);
		unsigned short accumulatorBytes = MatMulDescriptorValidator::DatatypeSizeBytes(descriptor->Datatypes.AccumulatorDatatype);
//		checked
			unsigned long long aTile = CheckedMultiply(CheckedMultiply(descriptor->TileM, descriptor->TileK), inputBytes);
			unsigned long long bTile = CheckedMultiply(CheckedMultiply(descriptor->TileK, descriptor->TileN), inputBytes);
			unsigned long long cTile = CheckedMultiply(CheckedMultiply(descriptor->TileM, descriptor->TileN), accumulatorBytes);
			return CheckedAdd(CheckedAdd(aTile, bTile), cTile);
// C# TO C++ CONVERTER TASK: End of the original C# 'checked' block.
	}

	std::shared_ptr<AcceleratorResourceModel> MatMulResourceModel::ToCapabilityResourceModel()
	{
		return std::make_shared<AcceleratorResourceModel>(getSetupCycles(), getMacsPerCycle(), getMaxQueueOccupancy(), 4096, getMemoryBandwidthBytesPerCycle());
	}

	unsigned long long MatMulResourceModel::DivideRoundUp(unsigned long long value, unsigned long long divisor)
	{
		return value == 0 ? 0 : ((value - 1) / divisor) + 1;
	}
}
