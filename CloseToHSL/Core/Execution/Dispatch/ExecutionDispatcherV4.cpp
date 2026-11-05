#include "ExecutionDispatcherV4.h"

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
using CpuCore = YAKSys_Hybrid_CPU::Processor::CPU_Core;

namespace YAKSys_Hybrid_CPU::Core::Execution
{

	unsigned long long ExecutionResult::getValue() const
	{
		return Value;
	}

	void ExecutionResult::setValue(unsigned long long value)
	{
		Value = value;
	}

	bool ExecutionResult::getPcRedirected() const
	{
		return PcRedirected;
	}

	void ExecutionResult::setPcRedirected(bool value)
	{
		PcRedirected = value;
	}

	unsigned long long ExecutionResult::getNewPc() const
	{
		return NewPc;
	}

	void ExecutionResult::setNewPc(unsigned long long value)
	{
		NewPc = value;
	}

	bool ExecutionResult::getTrapRaised() const
	{
		return TrapRaised;
	}

	void ExecutionResult::setTrapRaised(bool value)
	{
		TrapRaised = value;
	}

	bool ExecutionResult::getVmxFaulted() const
	{
		return VmxFaulted;
	}

	void ExecutionResult::setVmxFaulted(bool value)
	{
		VmxFaulted = value;
	}

	ExecutionResult ExecutionResult::Ok(unsigned long long value)
	{
		ExecutionResult temp_var = ExecutionResult();
		temp_var.setValue(value);
		return temp_var;
	}

	ExecutionResult ExecutionResult::Redirect(unsigned long long newPc, unsigned long long rdValue)
	{
		ExecutionResult temp_var = ExecutionResult();
		temp_var.setValue(rdValue);
		temp_var.setPcRedirected(true);
		temp_var.setNewPc(newPc);
		return temp_var;
	}

	ExecutionResult ExecutionResult::Trap()
	{
		ExecutionResult temp_var = ExecutionResult();
		temp_var.setTrapRaised(true);
		return temp_var;
	}

	ExecutionResult ExecutionResult::VmxFault()
	{
		ExecutionResult temp_var = ExecutionResult();
		temp_var.setVmxFaulted(true);
		return temp_var;
	}
}
