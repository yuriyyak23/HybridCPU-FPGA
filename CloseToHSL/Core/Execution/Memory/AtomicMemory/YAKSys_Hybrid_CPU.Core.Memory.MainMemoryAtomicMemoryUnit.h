#pragma once

#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../Memory/IAtomicMemoryUnit.h"
#include "../../../Pipeline/MicroOps/Control/MicroOp.Control.h"
#include <vector>
#include <cmath>
#include <functional>
#include <format>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::Core::Memory
{
		/// <summary>
		/// Resolve validated atomic retire intent without mutating memory.
		/// The returned effect is a retire-side carrier; reservation checks and memory writes
		/// happen only when <see cref="ApplyRetireEffect(IAtomicMemoryUnit, in AtomicRetireEffect)"/>
		/// is invoked on the authoritative retire path.
		/// </summary>

		/// <summary>
		/// Apply a previously resolved atomic retire effect against the bound main-memory surface.
		/// The returned outcome reports retire-time memory mutation and any architectural
		/// destination-register writeback payload as separate facts.
		/// </summary>














	class MainMemoryAtomicMemoryUnit final : public IAtomicMemoryUnit
	{
	private:
		const std::shared_ptr<Processor::MainMemoryArea> _mainMemory;

	public:
		MainMemoryAtomicMemoryUnit(std::shared_ptr<Processor::MainMemoryArea> mainMemory);

		int LoadReserved32(unsigned long long address) override;

		int StoreConditional32(unsigned long long address, int value) override;

		long long LoadReserved64(unsigned long long address) override;

		long long StoreConditional64(unsigned long long address, long long value) override;

		int AtomicSwap32(unsigned long long address, int value) override;

		int AtomicAdd32(unsigned long long address, int value) override;

		int AtomicXor32(unsigned long long address, int value) override;

		int AtomicAnd32(unsigned long long address, int value) override;

		int AtomicOr32(unsigned long long address, int value) override;

		int AtomicMinSigned32(unsigned long long address, int value) override;

		int AtomicMaxSigned32(unsigned long long address, int value) override;

		unsigned int AtomicMinUnsigned32(unsigned long long address, unsigned int value) override;

		unsigned int AtomicMaxUnsigned32(unsigned long long address, unsigned int value) override;

		long long AtomicSwap64(unsigned long long address, long long value) override;

		long long AtomicAdd64(unsigned long long address, long long value) override;

		long long AtomicXor64(unsigned long long address, long long value) override;

		long long AtomicAnd64(unsigned long long address, long long value) override;

		long long AtomicOr64(unsigned long long address, long long value) override;

		long long AtomicMin64Signed(unsigned long long address, long long value) override;

		long long AtomicMax64Signed(unsigned long long address, long long value) override;

		unsigned long long AtomicMin64Unsigned(unsigned long long address, unsigned long long value) override;

		unsigned long long AtomicMax64Unsigned(unsigned long long address, unsigned long long value) override;

		AtomicRetireOutcome ApplyResolvedRetireEffect(AtomicRetireEffect const effect);

		static void NotifyPhysicalWrite(unsigned long long address, int byteCount);

	private:
		inline static AtomicRetireOutcome CreateOutcome(AtomicRetireEffect const effect, unsigned long long previousValue, bool memoryMutated);

	public:
		void ValidateAccess(unsigned short opcode, unsigned long long address, unsigned char accessSize);

		static unsigned char ResolveAccessSize(unsigned short opcode);

	private:
		inline static bool IsWriteLikeOpcode(unsigned short opcode);

		inline int LoadReserved32(unsigned long long address, int coreId, int virtualThreadId);

		inline int StoreConditional32(unsigned long long address, int value, int coreId, int virtualThreadId);

		inline long long LoadReserved64(unsigned long long address, int coreId, int virtualThreadId);

		inline long long StoreConditional64(unsigned long long address, long long value, int coreId, int virtualThreadId);

		inline unsigned int ApplyWordAtomic(unsigned long long address, unsigned int value, std::function<unsigned int(unsigned int, unsigned int)> operation);

		inline unsigned long long ApplyDoublewordAtomic(unsigned long long address, unsigned long long value, std::function<unsigned long long(unsigned long long, unsigned long long)> operation);

		inline std::vector<unsigned char> ReadMemory(unsigned long long address, unsigned char accessSize);

		inline void WriteMemory(unsigned long long address, unsigned long long value, unsigned char accessSize);

		inline static unsigned long long ComputeAmoWriteValue(unsigned short opcode, unsigned long long previousValue, unsigned long long sourceValue, unsigned char accessSize);

		inline static unsigned long long SignExtendWord(unsigned long long value);
	};
}
