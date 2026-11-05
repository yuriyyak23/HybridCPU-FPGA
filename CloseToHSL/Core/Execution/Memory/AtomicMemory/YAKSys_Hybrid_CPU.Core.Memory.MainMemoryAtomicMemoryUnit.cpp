#include "YAKSys_Hybrid_CPU.Core.Memory.MainMemoryAtomicMemoryUnit.h"
#include "AtomicMemoryUnit.h"
#include "../LoadStore/MemoryUnit.h"
#include "../../../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../Pipeline/MicroOps/Types/MicroOp.Exceptions.h"
#include <iomanip>
#include <sstream>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Memory
{
	namespace
	{
		unsigned long long ReadLittleEndian(std::vector<unsigned char> const& bytes)
		{
			unsigned long long value = 0;
			for (std::size_t index = 0; index < bytes.size(); ++index)
			{
				value |= static_cast<unsigned long long>(bytes[index]) << (index * 8);
			}
			return value;
		}

		std::string FormatAtomicAddressError(char const* operation, unsigned long long address, unsigned char accessSize)
		{
			std::ostringstream message;
			message << operation << " at physical address 0x" << std::uppercase << std::hex << address
				<< std::dec << " for " << static_cast<unsigned int>(accessSize) << " byte(s).";
			return message.str();
		}
	}

	MainMemoryAtomicMemoryUnit::MainMemoryAtomicMemoryUnit(std::shared_ptr<Processor::MainMemoryArea> mainMemory) : _mainMemory(mainMemory)
	{
		ArgumentNullException::ThrowIfNull(mainMemory);
	}

	int MainMemoryAtomicMemoryUnit::LoadReserved32(unsigned long long address)
	{
		return LoadReserved32(address, 0, 0);
	}

	int MainMemoryAtomicMemoryUnit::StoreConditional32(unsigned long long address, int value)
	{
		return StoreConditional32(address, value, 0, 0);
	}

	long long MainMemoryAtomicMemoryUnit::LoadReserved64(unsigned long long address)
	{
		return LoadReserved64(address, 0, 0);
	}

	long long MainMemoryAtomicMemoryUnit::StoreConditional64(unsigned long long address, long long value)
	{
		return StoreConditional64(address, value, 0, 0);
	}

	int MainMemoryAtomicMemoryUnit::AtomicSwap32(unsigned long long address, int value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((int)ApplyWordAtomic(address, unchecked((uint)value), static (_, source) => source));
		return static_cast<int>(ApplyWordAtomic(address, static_cast<unsigned int>(value), [](unsigned int, unsigned int source) { return source; }));
	}

	int MainMemoryAtomicMemoryUnit::AtomicAdd32(unsigned long long address, int value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((int)ApplyWordAtomic(address, unchecked((uint)value), static (previous, source) => unchecked(previous + source)));
		return static_cast<int>(ApplyWordAtomic(address, static_cast<unsigned int>(value), [](unsigned int previous, unsigned int source) { return previous + source; }));
	}

	int MainMemoryAtomicMemoryUnit::AtomicXor32(unsigned long long address, int value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((int)ApplyWordAtomic(address, unchecked((uint)value), static (previous, source) => previous ^ source));
		return static_cast<int>(ApplyWordAtomic(address, static_cast<unsigned int>(value), [](unsigned int previous, unsigned int source) { return previous ^ source; }));
	}

	int MainMemoryAtomicMemoryUnit::AtomicAnd32(unsigned long long address, int value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((int)ApplyWordAtomic(address, unchecked((uint)value), static (previous, source) => previous & source));
		return static_cast<int>(ApplyWordAtomic(address, static_cast<unsigned int>(value), [](unsigned int previous, unsigned int source) { return previous & source; }));
	}

	int MainMemoryAtomicMemoryUnit::AtomicOr32(unsigned long long address, int value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((int)ApplyWordAtomic(address, unchecked((uint)value), static (previous, source) => previous | source));
		return static_cast<int>(ApplyWordAtomic(address, static_cast<unsigned int>(value), [](unsigned int previous, unsigned int source) { return previous | source; }));
	}

	int MainMemoryAtomicMemoryUnit::AtomicMinSigned32(unsigned long long address, int value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((int)ApplyWordAtomic(address, unchecked((uint)value), static (previous, source) => unchecked((uint)Math.Min((int)previous, (int)source))));
		return static_cast<int>(ApplyWordAtomic(address, static_cast<unsigned int>(value), [](unsigned int previous, unsigned int source) { return static_cast<unsigned int>(std::min(static_cast<int>(previous), static_cast<int>(source))); }));
	}

	int MainMemoryAtomicMemoryUnit::AtomicMaxSigned32(unsigned long long address, int value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((int)ApplyWordAtomic(address, unchecked((uint)value), static (previous, source) => unchecked((uint)Math.Max((int)previous, (int)source))));
		return static_cast<int>(ApplyWordAtomic(address, static_cast<unsigned int>(value), [](unsigned int previous, unsigned int source) { return static_cast<unsigned int>(std::max(static_cast<int>(previous), static_cast<int>(source))); }));
	}

	unsigned int MainMemoryAtomicMemoryUnit::AtomicMinUnsigned32(unsigned long long address, unsigned int value)
	{
		return ApplyWordAtomic(address, value, [](unsigned int previous, unsigned int source) { return std::min(previous, source); });
	}

	unsigned int MainMemoryAtomicMemoryUnit::AtomicMaxUnsigned32(unsigned long long address, unsigned int value)
	{
		return ApplyWordAtomic(address, value, [](unsigned int previous, unsigned int source) { return std::max(previous, source); });
	}

	long long MainMemoryAtomicMemoryUnit::AtomicSwap64(unsigned long long address, long long value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((long)ApplyDoublewordAtomic(address, unchecked((ulong)value), static (_, source) => source));
		return static_cast<long long>(ApplyDoublewordAtomic(address, static_cast<unsigned long long>(value), [](unsigned long long, unsigned long long source) { return source; }));
	}

	long long MainMemoryAtomicMemoryUnit::AtomicAdd64(unsigned long long address, long long value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((long)ApplyDoublewordAtomic(address, unchecked((ulong)value), static (previous, source) => unchecked(previous + source)));
		return static_cast<long long>(ApplyDoublewordAtomic(address, static_cast<unsigned long long>(value), [](unsigned long long previous, unsigned long long source) { return previous + source; }));
	}

	long long MainMemoryAtomicMemoryUnit::AtomicXor64(unsigned long long address, long long value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((long)ApplyDoublewordAtomic(address, unchecked((ulong)value), static (previous, source) => previous ^ source));
		return static_cast<long long>(ApplyDoublewordAtomic(address, static_cast<unsigned long long>(value), [](unsigned long long previous, unsigned long long source) { return previous ^ source; }));
	}

	long long MainMemoryAtomicMemoryUnit::AtomicAnd64(unsigned long long address, long long value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((long)ApplyDoublewordAtomic(address, unchecked((ulong)value), static (previous, source) => previous & source));
		return static_cast<long long>(ApplyDoublewordAtomic(address, static_cast<unsigned long long>(value), [](unsigned long long previous, unsigned long long source) { return previous & source; }));
	}

	long long MainMemoryAtomicMemoryUnit::AtomicOr64(unsigned long long address, long long value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((long)ApplyDoublewordAtomic(address, unchecked((ulong)value), static (previous, source) => previous | source));
		return static_cast<long long>(ApplyDoublewordAtomic(address, static_cast<unsigned long long>(value), [](unsigned long long previous, unsigned long long source) { return previous | source; }));
	}

	long long MainMemoryAtomicMemoryUnit::AtomicMin64Signed(unsigned long long address, long long value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((long)ApplyDoublewordAtomic(address, unchecked((ulong)value), static (previous, source) => unchecked((ulong)Math.Min((long)previous, (long)source))));
		return static_cast<long long>(ApplyDoublewordAtomic(address, static_cast<unsigned long long>(value), [](unsigned long long previous, unsigned long long source) { return static_cast<unsigned long long>(std::min(static_cast<long long>(previous), static_cast<long long>(source))); }));
	}

	long long MainMemoryAtomicMemoryUnit::AtomicMax64Signed(unsigned long long address, long long value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((long)ApplyDoublewordAtomic(address, unchecked((ulong)value), static (previous, source) => unchecked((ulong)Math.Max((long)previous, (long)source))));
		return static_cast<long long>(ApplyDoublewordAtomic(address, static_cast<unsigned long long>(value), [](unsigned long long previous, unsigned long long source) { return static_cast<unsigned long long>(std::max(static_cast<long long>(previous), static_cast<long long>(source))); }));
	}

	unsigned long long MainMemoryAtomicMemoryUnit::AtomicMin64Unsigned(unsigned long long address, unsigned long long value)
	{
		return ApplyDoublewordAtomic(address, value, [](unsigned long long previous, unsigned long long source) { return std::min(previous, source); });
	}

	unsigned long long MainMemoryAtomicMemoryUnit::AtomicMax64Unsigned(unsigned long long address, unsigned long long value)
	{
		return ApplyDoublewordAtomic(address, value, [](unsigned long long previous, unsigned long long source) { return std::max(previous, source); });
	}

	YAKSys_Hybrid_CPU::Core::AtomicRetireOutcome MainMemoryAtomicMemoryUnit::ApplyResolvedRetireEffect(AtomicRetireEffect const effect)
	{
		std::vector<unsigned char> rawValue = ReadMemory(effect.getAddress(), effect.getAccessSize());
		unsigned long long previousValue = ReadLittleEndian(rawValue);

		// Retire-time apply resolves reservation and memory visibility here.
		// Architectural register publication is returned to the caller as part of
		// AtomicRetireOutcome and is committed separately through the retire path.
		switch (effect.getOpcode())
		{
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::LR_W:
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::LR_D:
				AtomicReservationRegistry::RegisterReservation(effect.getCoreId(), effect.getVirtualThreadId(), effect.getAddress(), effect.getAccessSize());
				return CreateOutcome(effect, previousValue, false);

			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::SC_W:
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::SC_D:
			{
				bool reservationMatched = AtomicReservationRegistry::ConsumeReservation(effect.getCoreId(), effect.getVirtualThreadId(), effect.getAddress(), effect.getAccessSize());
				if (!reservationMatched)
				{
					return AtomicRetireOutcome::Create(effect, 1, effect.getHasRegisterDestination(), false);
				}

				WriteMemory(effect.getAddress(), effect.getSourceValue(), effect.getAccessSize());
				return AtomicRetireOutcome::Create(effect, 0, effect.getHasRegisterDestination(), true);
			}

			default:
			{
				unsigned long long newValue = ComputeAmoWriteValue(effect.getOpcode(), previousValue, effect.getSourceValue(), effect.getAccessSize());
				WriteMemory(effect.getAddress(), newValue, effect.getAccessSize());
				return CreateOutcome(effect, previousValue, true);
			}
		}
	}

	void MainMemoryAtomicMemoryUnit::NotifyPhysicalWrite(unsigned long long address, int byteCount)
	{
		AtomicReservationRegistry::InvalidateOverlapping(address, byteCount);
	}

	inline AtomicRetireOutcome MainMemoryAtomicMemoryUnit::CreateOutcome(AtomicRetireEffect const effect, unsigned long long previousValue, bool memoryMutated)
	{
		unsigned long long registerValue = effect.getAccessSize() == 4 ? SignExtendWord(previousValue) : previousValue;
		return AtomicRetireOutcome::Create(effect, registerValue, effect.getHasRegisterDestination(), memoryMutated);
	}

	void MainMemoryAtomicMemoryUnit::ValidateAccess(unsigned short opcode, unsigned long long address, unsigned char accessSize)
	{
		if ((address & static_cast<unsigned long long>(accessSize - 1)) != 0)
		{
			throw MemoryAlignmentException(address, accessSize, OpcodeRegistry::GetMnemonicOrHex(opcode));
		}

		unsigned long long mainMemoryLength = static_cast<unsigned long long>(_mainMemory->getLength());
		if (accessSize > mainMemoryLength || address > mainMemoryLength - accessSize)
		{
			throw PageFaultException(address, IsWriteLikeOpcode(opcode));
		}
	}

	unsigned char MainMemoryAtomicMemoryUnit::ResolveAccessSize(unsigned short opcode)
	{
		int temp_var51;
		switch (opcode)
		{
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::LR_W:
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::SC_W:
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOSWAP_W:
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOADD_W:
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOXOR_W:
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOAND_W:
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOOR_W:
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOMIN_W:
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOMAX_W:
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOMINU_W:
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOMAXU_W:
				temp_var51 = 4;
				break;
			default:
				temp_var51 = 8;
				break;
		}
		return temp_var51;
	}

	inline bool MainMemoryAtomicMemoryUnit::IsWriteLikeOpcode(unsigned short opcode)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		return (opcode != YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::LR_W && opcode != YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::LR_D);
	}

	inline int MainMemoryAtomicMemoryUnit::LoadReserved32(unsigned long long address, int coreId, int virtualThreadId)
	{
		ValidateAccess(YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::LR_W, address, 4);
		AtomicReservationRegistry::RegisterReservation(coreId, virtualThreadId, address, 4);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((int)BitConverter.ToUInt32(ReadMemory(address, accessSize: 4), 0));
		return static_cast<int>(static_cast<unsigned int>(ReadLittleEndian(ReadMemory(address, 4))));
	}

	inline int MainMemoryAtomicMemoryUnit::StoreConditional32(unsigned long long address, int value, int coreId, int virtualThreadId)
	{
		ValidateAccess(YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::SC_W, address, 4);
		if (!AtomicReservationRegistry::ConsumeReservation(coreId, virtualThreadId, address, 4))
		{
			return 1;
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: WriteMemory(address, unchecked((uint)value), accessSize: 4);
		WriteMemory(address, static_cast<unsigned int>(value), 4);
		return 0;
	}

	inline long long MainMemoryAtomicMemoryUnit::LoadReserved64(unsigned long long address, int coreId, int virtualThreadId)
	{
		ValidateAccess(YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::LR_D, address, 8);
		AtomicReservationRegistry::RegisterReservation(coreId, virtualThreadId, address, 8);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((long)BitConverter.ToUInt64(ReadMemory(address, accessSize: 8), 0));
		return static_cast<long long>(ReadLittleEndian(ReadMemory(address, 8)));
	}

	inline long long MainMemoryAtomicMemoryUnit::StoreConditional64(unsigned long long address, long long value, int coreId, int virtualThreadId)
	{
		ValidateAccess(YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::SC_D, address, 8);
		if (!AtomicReservationRegistry::ConsumeReservation(coreId, virtualThreadId, address, 8))
		{
			return 1;
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: WriteMemory(address, unchecked((ulong)value), accessSize: 8);
		WriteMemory(address, static_cast<unsigned long long>(value), 8);
		return 0;
	}

	inline unsigned int MainMemoryAtomicMemoryUnit::ApplyWordAtomic(unsigned long long address, unsigned int value, std::function<unsigned int(unsigned int, unsigned int)> operation)
	{
		ValidateAccess(YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOSWAP_W, address, 4);
		unsigned int previousValue = static_cast<unsigned int>(ReadLittleEndian(ReadMemory(address, 4)));
		unsigned int newValue = operation(previousValue, value);
		WriteMemory(address, newValue, 4);
		return previousValue;
	}

	inline unsigned long long MainMemoryAtomicMemoryUnit::ApplyDoublewordAtomic(unsigned long long address, unsigned long long value, std::function<unsigned long long(unsigned long long, unsigned long long)> operation)
	{
		ValidateAccess(YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOSWAP_D, address, 8);
		unsigned long long previousValue = ReadLittleEndian(ReadMemory(address, 8));
		unsigned long long newValue = operation(previousValue, value);
		WriteMemory(address, newValue, 8);
		return previousValue;
	}

	inline std::vector<unsigned char> MainMemoryAtomicMemoryUnit::ReadMemory(unsigned long long address, unsigned char accessSize)
	{
		std::vector<unsigned char> buffer(accessSize);
		if (!_mainMemory->TryReadPhysicalRange(address, buffer))
		{
			throw IOException(FormatAtomicAddressError("Atomic main-memory read failed", address, accessSize));
		}

		return buffer;
	}

	inline void MainMemoryAtomicMemoryUnit::WriteMemory(unsigned long long address, unsigned long long value, unsigned char accessSize)
	{
		if (accessSize != 4 && accessSize != 8)
		{
			throw InvalidOperationException("Unsupported atomic access size.");
		}
		std::vector<unsigned char> bytes(accessSize);
		for (std::size_t index = 0; index < bytes.size(); ++index)
		{
			bytes[index] = static_cast<unsigned char>(value >> (index * 8));
		}
		if (!_mainMemory->TryWritePhysicalRange(address, bytes))
		{
			throw IOException(FormatAtomicAddressError("Atomic main-memory write failed", address, accessSize));
		}
	}

	inline unsigned long long MainMemoryAtomicMemoryUnit::ComputeAmoWriteValue(unsigned short opcode, unsigned long long previousValue, unsigned long long sourceValue, unsigned char accessSize)
	{
		if (accessSize == 4)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: uint previousWord = unchecked((uint)previousValue);
			unsigned int previousWord = static_cast<unsigned int>(previousValue);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: uint sourceWord = unchecked((uint)sourceValue);
			unsigned int sourceWord = static_cast<unsigned int>(sourceValue);
			unsigned int temp_var53;
			switch (opcode)
			{
				case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOSWAP_W:
					temp_var53 = sourceWord;
					break;
				case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOADD_W:
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: temp_var53 = unchecked((uint)(previousWord + sourceWord));
					temp_var53 = static_cast<unsigned int>(previousWord + sourceWord);
					break;
				case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOXOR_W:
					temp_var53 = previousWord ^ sourceWord;
					break;
				case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOAND_W:
					temp_var53 = previousWord & sourceWord;
					break;
				case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOOR_W:
					temp_var53 = previousWord | sourceWord;
					break;
				case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOMIN_W:
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: temp_var53 = unchecked((uint)Math.Min((int)previousWord, (int)sourceWord));
					temp_var53 = static_cast<unsigned int>(std::min(static_cast<int>(previousWord), static_cast<int>(sourceWord)));
					break;
				case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOMAX_W:
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: temp_var53 = unchecked((uint)Math.Max((int)previousWord, (int)sourceWord));
					temp_var53 = static_cast<unsigned int>(std::max(static_cast<int>(previousWord), static_cast<int>(sourceWord)));
					break;
				case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOMINU_W:
					temp_var53 = std::min(previousWord, sourceWord);
					break;
				case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOMAXU_W:
					temp_var53 = std::max(previousWord, sourceWord);
					break;
				default:
					throw InvalidOperationException("Unsupported word atomic opcode.");
					break;
			}
			return temp_var53;
		}

		unsigned long long temp_var54;
		switch (opcode)
		{
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOSWAP_D:
				temp_var54 = sourceValue;
				break;
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOADD_D:
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: temp_var54 = unchecked(previousValue + sourceValue);
				temp_var54 = previousValue + sourceValue;
				break;
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOXOR_D:
				temp_var54 = previousValue ^ sourceValue;
				break;
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOAND_D:
				temp_var54 = previousValue & sourceValue;
				break;
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOOR_D:
				temp_var54 = previousValue | sourceValue;
				break;
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOMIN_D:
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: temp_var54 = unchecked((ulong)Math.Min((long)previousValue, (long)sourceValue));
				temp_var54 = static_cast<unsigned long long>(std::min(static_cast<long long>(previousValue), static_cast<long long>(sourceValue)));
				break;
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOMAX_D:
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: temp_var54 = unchecked((ulong)Math.Max((long)previousValue, (long)sourceValue));
				temp_var54 = static_cast<unsigned long long>(std::max(static_cast<long long>(previousValue), static_cast<long long>(sourceValue)));
				break;
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOMINU_D:
				temp_var54 = std::min(previousValue, sourceValue);
				break;
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::AMOMAXU_D:
				temp_var54 = std::max(previousValue, sourceValue);
				break;
			default:
				throw InvalidOperationException("Unsupported doubleword atomic opcode.");
				break;
		}
		return temp_var54;
	}

	inline unsigned long long MainMemoryAtomicMemoryUnit::SignExtendWord(unsigned long long value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)(long)(int)(uint)value);
		return static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(value))));
	}
}




