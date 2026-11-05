#include "InstructionRegistry.Types.h"

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::Metadata;

namespace YAKSys_Hybrid_CPU::Core
{

	int MicroOpDescriptor::getLatency() const
	{
		return Latency;
	}

	void MicroOpDescriptor::setLatency(int value)
	{
		Latency = value;
	}

	int MicroOpDescriptor::getMemFootprintClass() const
	{
		return MemFootprintClass;
	}

	void MicroOpDescriptor::setMemFootprintClass(int value)
	{
		MemFootprintClass = value;
	}

	std::optional<bool> MicroOpDescriptor::getWritesRegister() const
	{
		return WritesRegister;
	}

	void MicroOpDescriptor::setWritesRegister(const std::optional<bool> &value)
	{
		WritesRegister = value;
	}

	std::optional<bool> MicroOpDescriptor::isMemoryOp() const
	{
		return IsMemoryOp;
	}

	void MicroOpDescriptor::setMemoryOp(const std::optional<bool> &value)
	{
		IsMemoryOp = value;
	}

	MicroOpDescriptor::MicroOpDescriptor()
	{
	}

	MicroOpDescriptor::MicroOpDescriptor(int latency, int memFootprintClass)
	{
		setLatency(latency);
		setMemFootprintClass(memFootprintClass);
	}
}
