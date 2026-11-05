#pragma once

#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <functional>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::Core::Execution
{
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ExecutionResult
	class ExecutionResult
	{
	private:
		unsigned long long Value = 0;
		bool PcRedirected = false;
		unsigned long long NewPc = 0;
		bool TrapRaised = false;
		bool VmxFaulted = false;

	public:
		unsigned long long getValue() const;
		void setValue(unsigned long long value);
		bool getPcRedirected() const;
		void setPcRedirected(bool value);
		unsigned long long getNewPc() const;
		void setNewPc(unsigned long long value);
		bool getTrapRaised() const;
		void setTrapRaised(bool value);
		bool getVmxFaulted() const;
		void setVmxFaulted(bool value);

		static ExecutionResult Ok(unsigned long long value = 0);
		static ExecutionResult Redirect(unsigned long long newPc, unsigned long long rdValue = 0);
		static ExecutionResult Trap();
		static ExecutionResult VmxFault();
	};

	class IAtomicMemoryBus
	{
	public:
		virtual unsigned long long AtomicRMW64(unsigned long long address, std::function<unsigned long long(unsigned long long)> modify) = 0;
	};

}
