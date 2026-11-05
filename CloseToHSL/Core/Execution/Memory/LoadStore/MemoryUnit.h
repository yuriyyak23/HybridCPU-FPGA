#pragma once

namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps { class InstructionIR; }
#include "../../../State/ICpuState.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <format>
#include <memory>


using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::Core::Memory
{
	/// <summary>
	/// Byte-addressable memory bus used by <see cref="MemoryUnit"/> for typed
	/// load/store operations. Decouples the execution-level memory unit from
	/// the concrete memory subsystem.
	/// </summary>
	class IMemoryBus
	{
	public:
		virtual std::vector<unsigned char> Read(unsigned long long address, int length) = 0;

		virtual void Write(unsigned long long address, std::vector<unsigned char> &data) = 0;
	};

	/// <summary>
	/// Thrown when a memory access violates the ISA v4 alignment policy.
	/// </summary>
	class MemoryAlignmentException final : public std::runtime_error, public std::enable_shared_from_this<MemoryAlignmentException>
	{
	private:
		const unsigned long long Address;
		const int RequiredAlignment;
		const std::wstring InstructionMnemonic;

	public:
		unsigned long long getAddress() const;

		int getRequiredAlignment() const;

		std::wstring getInstructionMnemonic() const;

		MemoryAlignmentException(unsigned long long address, int requiredAlignment, const std::wstring &instructionMnemonic);
	};

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct ResolvedMemoryAccess
	class ResolvedMemoryAccess
	{
	private:
		const unsigned long long EffectiveAddress;
		const bool HasRegisterWrite;
		const unsigned short RegisterDestination;
		const unsigned long long RegisterWriteValue;
		const bool HasStoreCommit;
		const unsigned long long StoreData;
		const unsigned char AccessSize;

		ResolvedMemoryAccess(unsigned long long effectiveAddress, bool hasRegisterWrite, unsigned short registerDestination, unsigned long long registerWriteValue, bool hasStoreCommit, unsigned long long storeData, unsigned char accessSize);

	public:
		unsigned long long getEffectiveAddress() const;

		bool getHasRegisterWrite() const;

		unsigned short getRegisterDestination() const;

		unsigned long long getRegisterWriteValue() const;

		bool getHasStoreCommit() const;

		unsigned long long getStoreData() const;

		unsigned char getAccessSize() const;

		static ResolvedMemoryAccess NoArchitecturalWrite(unsigned long long effectiveAddress);

		static ResolvedMemoryAccess RegisterWrite(unsigned long long effectiveAddress, unsigned short registerDestination, unsigned long long registerWriteValue);

		static ResolvedMemoryAccess StoreCommit(unsigned long long effectiveAddress, unsigned long long storeData, unsigned char accessSize);

		ResolvedMemoryAccess() : EffectiveAddress(0), HasRegisterWrite(false), RegisterDestination(0), RegisterWriteValue(0), HasStoreCommit(false), StoreData(0), AccessSize(0) {}
	};

	/// <summary>
	/// ISA v4 memory execution unit for typed scalar loads/stores plus the retained
	/// absolute legacy Load/Store contour.
	/// </summary>
	class MemoryUnit final : public std::enable_shared_from_this<MemoryUnit>
	{
	private:
		const std::shared_ptr<IMemoryBus> _bus;
		const bool _trapOnMisalign;

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public MemoryUnit(IMemoryBus bus, bool trapOnMisalign = true)
		MemoryUnit(std::shared_ptr<IMemoryBus> bus, bool trapOnMisalign = true);

		/// <summary>
		/// Internal semantic-compatibility execution surface for typed loads/stores.
		/// For loads, writes the result directly to <paramref name="state"/> and
		/// for stores mutates the backing bus immediately, so this method is not
		/// the authoritative retire contract for memory-side follow-through.
		/// Callers that need an authoritative retire-window publication shape should
		/// resolve through <c>ExecutionDispatcherV4.CaptureRetireWindowPublications(...)</c>.
		/// Returns the effective address for pipeline tracking.
		/// </summary>
		unsigned long long Execute(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state);

		/// <summary>
		/// Internal semantic-compatibility execution surface for typed loads/stores scoped
		/// to virtual thread <paramref name="vtId"/>.
		/// </summary>
		unsigned long long Execute(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId);

		/// <summary>
		/// Resolve a typed scalar memory instruction without mutating the
		/// architectural state or the backing bus.
		/// </summary>
		ResolvedMemoryAccess ResolveArchitecturalAccess(std::shared_ptr<InstructionIR> instr, unsigned long long rs1Value, unsigned long long rs2Value);

	private:
		inline static unsigned long long ResolveEffectiveAddress(std::shared_ptr<InstructionIR> instr, unsigned long long rs1Value);

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private static (ulong Rs1Value, ulong Rs2Value) ResolveOperandValues(InstructionIR instr, ICanonicalCpuState state, byte vtId)
//		{
//			ulong rs1Value = instr.HasAbsoluteAddressing ? 0UL : unchecked((ulong)state.ReadRegister(vtId, instr.Rs1));
//			ulong rs2Value = RequiresStoreSourceValue(instr) ? unchecked((ulong)state.ReadRegister(vtId, instr.Rs2)) : 0UL;
//			return (rs1Value, rs2Value);
//		}

		inline static bool RequiresStoreSourceValue(std::shared_ptr<InstructionIR> instr);

		inline void CheckAlignment(unsigned long long address, int alignment, const std::wstring &mnemonic);

		inline static std::vector<unsigned char> GetScalarStoreBytes(unsigned long long storeData, unsigned char accessSize);
	};
}
