#include "MemoryUnit.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../../../NonRTL/Arch/InvalidOpcodeException.h"
#include "../../../Pipeline/MicroOps/IR/InstructionIR.h"

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Memory
{

	unsigned long long MemoryAlignmentException::getAddress() const
	{
		return Address;
	}

	int MemoryAlignmentException::getRequiredAlignment() const
	{
		return RequiredAlignment;
	}

	std::wstring MemoryAlignmentException::getInstructionMnemonic() const
	{
		return InstructionMnemonic;
	}

	MemoryAlignmentException::MemoryAlignmentException(unsigned long long address, int requiredAlignment, const std::wstring &instructionMnemonic)
		: std::runtime_error("Misaligned memory access"), Address(address), RequiredAlignment(requiredAlignment), InstructionMnemonic(instructionMnemonic)
	{
	}

	ResolvedMemoryAccess::ResolvedMemoryAccess(unsigned long long effectiveAddress, bool hasRegisterWrite, unsigned short registerDestination, unsigned long long registerWriteValue, bool hasStoreCommit, unsigned long long storeData, unsigned char accessSize)
		: EffectiveAddress(effectiveAddress), HasRegisterWrite(hasRegisterWrite), RegisterDestination(registerDestination), RegisterWriteValue(registerWriteValue), HasStoreCommit(hasStoreCommit), StoreData(storeData), AccessSize(accessSize)
	{
	}

	unsigned long long ResolvedMemoryAccess::getEffectiveAddress() const
	{
		return EffectiveAddress;
	}

	bool ResolvedMemoryAccess::getHasRegisterWrite() const
	{
		return HasRegisterWrite;
	}

	unsigned short ResolvedMemoryAccess::getRegisterDestination() const
	{
		return RegisterDestination;
	}

	unsigned long long ResolvedMemoryAccess::getRegisterWriteValue() const
	{
		return RegisterWriteValue;
	}

	bool ResolvedMemoryAccess::getHasStoreCommit() const
	{
		return HasStoreCommit;
	}

	unsigned long long ResolvedMemoryAccess::getStoreData() const
	{
		return StoreData;
	}

	unsigned char ResolvedMemoryAccess::getAccessSize() const
	{
		return AccessSize;
	}

	ResolvedMemoryAccess ResolvedMemoryAccess::NoArchitecturalWrite(unsigned long long effectiveAddress)
	{
		return ResolvedMemoryAccess(effectiveAddress, false, 0, 0, false, 0, 0);
	}

	ResolvedMemoryAccess ResolvedMemoryAccess::RegisterWrite(unsigned long long effectiveAddress, unsigned short registerDestination, unsigned long long registerWriteValue)
	{
		return ResolvedMemoryAccess(effectiveAddress, registerDestination != 0, registerDestination, registerWriteValue, false, 0, 0);
	}

	ResolvedMemoryAccess ResolvedMemoryAccess::StoreCommit(unsigned long long effectiveAddress, unsigned long long storeData, unsigned char accessSize)
	{
		return ResolvedMemoryAccess(effectiveAddress, false, 0, 0, true, storeData, accessSize);
	}

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public MemoryUnit(IMemoryBus bus, bool trapOnMisalign = true)
	MemoryUnit::MemoryUnit(std::shared_ptr<IMemoryBus> bus, bool trapOnMisalign) : _bus((bus != nullptr) ? bus : throw std::invalid_argument("bus")), _trapOnMisalign(trapOnMisalign)
	{
	}

	unsigned long long MemoryUnit::Execute(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state)
	{
		return Execute(instr, state, 0);
	}

	unsigned long long MemoryUnit::Execute(std::shared_ptr<InstructionIR> instr, std::shared_ptr<ICanonicalCpuState> state, unsigned char vtId)
	{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
		unsigned long long rs1 = instr->HasAbsoluteAddressing ? 0ULL : static_cast<unsigned long long>(state->ReadRegister(vtId, instr->Rs1));
		unsigned long long rs2 = RequiresStoreSourceValue(instr) ? static_cast<unsigned long long>(state->ReadRegister(vtId, instr->Rs2)) : 0ULL;
		ResolvedMemoryAccess resolved = ResolveArchitecturalAccess(instr, rs1, rs2);

		if (resolved.getHasRegisterWrite())
		{
			state->WriteRegister(vtId, resolved.getRegisterDestination(), resolved.getRegisterWriteValue());
		}

		if (resolved.getHasStoreCommit())
		{
			std::vector<unsigned char> storeBytes = GetScalarStoreBytes(resolved.getStoreData(), resolved.getAccessSize());
			_bus->Write(resolved.getEffectiveAddress(), storeBytes);
		}

		return resolved.getEffectiveAddress();
	}

	ResolvedMemoryAccess MemoryUnit::ResolveArchitecturalAccess(std::shared_ptr<InstructionIR> instr, unsigned long long rs1Value, unsigned long long rs2Value)
	{
		unsigned short opcode = instr->CanonicalOpcode->Value;
		unsigned long long effectiveAddress = ResolveEffectiveAddress(instr, rs1Value);

		switch (opcode)
		{
			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::LB:
			{
					std::vector<unsigned char> raw = _bus->Read(effectiveAddress, 1);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ulong value = unchecked((ulong)(long)(sbyte)raw[0]);
					unsigned long long value = static_cast<unsigned long long>(static_cast<long long>(static_cast<signed char>(raw[0])));
					return instr->Rd != 0 ? ResolvedMemoryAccess::RegisterWrite(effectiveAddress, instr->Rd, value) : ResolvedMemoryAccess::NoArchitecturalWrite(effectiveAddress);
			}

			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::LBU:
			{
					std::vector<unsigned char> raw = _bus->Read(effectiveAddress, 1);
					unsigned long long value = raw[0];
					return instr->Rd != 0 ? ResolvedMemoryAccess::RegisterWrite(effectiveAddress, instr->Rd, value) : ResolvedMemoryAccess::NoArchitecturalWrite(effectiveAddress);
			}

			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::LH:
			{
					CheckAlignment(effectiveAddress, 2, L"LH");
					std::vector<unsigned char> raw = _bus->Read(effectiveAddress, 2);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ulong value = unchecked((ulong)(long)(short)(raw[0] | (raw[1] << 8)));
					unsigned long long value = static_cast<unsigned long long>(static_cast<long long>(static_cast<short>(raw[0] | (raw[1] << 8))));
					return instr->Rd != 0 ? ResolvedMemoryAccess::RegisterWrite(effectiveAddress, instr->Rd, value) : ResolvedMemoryAccess::NoArchitecturalWrite(effectiveAddress);
			}

			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::LHU:
			{
					CheckAlignment(effectiveAddress, 2, L"LHU");
					std::vector<unsigned char> raw = _bus->Read(effectiveAddress, 2);
					unsigned long long value = static_cast<unsigned short>(raw[0] | (raw[1] << 8));
					return instr->Rd != 0 ? ResolvedMemoryAccess::RegisterWrite(effectiveAddress, instr->Rd, value) : ResolvedMemoryAccess::NoArchitecturalWrite(effectiveAddress);
			}

			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::LW:
			{
					CheckAlignment(effectiveAddress, 4, L"LW");
					std::vector<unsigned char> raw = _bus->Read(effectiveAddress, 4);
					int word = raw[0] | (raw[1] << 8) | (raw[2] << 16) | (raw[3] << 24);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ulong value = unchecked((ulong)(long)word);
					unsigned long long value = static_cast<unsigned long long>(static_cast<long long>(word));
					return instr->Rd != 0 ? ResolvedMemoryAccess::RegisterWrite(effectiveAddress, instr->Rd, value) : ResolvedMemoryAccess::NoArchitecturalWrite(effectiveAddress);
			}

			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::LWU:
			{
					CheckAlignment(effectiveAddress, 4, L"LWU");
					std::vector<unsigned char> raw = _bus->Read(effectiveAddress, 4);
					unsigned long long value = static_cast<unsigned int>(raw[0] | (raw[1] << 8) | (raw[2] << 16) | (raw[3] << 24));
					return instr->Rd != 0 ? ResolvedMemoryAccess::RegisterWrite(effectiveAddress, instr->Rd, value) : ResolvedMemoryAccess::NoArchitecturalWrite(effectiveAddress);
			}

			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::LD:
			{
					CheckAlignment(effectiveAddress, 8, L"LD");
					std::vector<unsigned char> raw = _bus->Read(effectiveAddress, 8);
					unsigned long long value = 0;
					for (std::size_t index = 0; index < raw.size(); ++index)
					{
						value |= static_cast<unsigned long long>(raw[index]) << (index * 8);
					}
					return instr->Rd != 0 ? ResolvedMemoryAccess::RegisterWrite(effectiveAddress, instr->Rd, value) : ResolvedMemoryAccess::NoArchitecturalWrite(effectiveAddress);
			}

			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::SB:
				return ResolvedMemoryAccess::StoreCommit(effectiveAddress, rs2Value, 1);

			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::SH:
				CheckAlignment(effectiveAddress, 2, L"SH");
				return ResolvedMemoryAccess::StoreCommit(effectiveAddress, rs2Value, 2);

			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::SW:
				CheckAlignment(effectiveAddress, 4, L"SW");
				return ResolvedMemoryAccess::StoreCommit(effectiveAddress, rs2Value, 4);

			case YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::SD:
				CheckAlignment(effectiveAddress, 8, L"SD");
				return ResolvedMemoryAccess::StoreCommit(effectiveAddress, rs2Value, 8);



			default:
				throw InvalidOpcodeException(L"Invalid opcode in MemoryUnit.");
		}
	}

	inline unsigned long long MemoryUnit::ResolveEffectiveAddress(std::shared_ptr<InstructionIR> instr, unsigned long long rs1Value)
	{
		return instr->HasAbsoluteAddressing ? static_cast<unsigned long long>(instr->Imm) : static_cast<unsigned long long>(static_cast<long long>(rs1Value) + instr->Imm);
	}

	inline bool MemoryUnit::RequiresStoreSourceValue(std::shared_ptr<InstructionIR> instr)
	{
		unsigned short opcode = instr->CanonicalOpcode->Value;
		return opcode == YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::SB
			|| opcode == YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::SH
			|| opcode == YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::SW
			|| opcode == YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::SD;
	}

	inline void MemoryUnit::CheckAlignment(unsigned long long address, int alignment, const std::wstring &mnemonic)
	{
		if (_trapOnMisalign && (address & static_cast<unsigned long long>(alignment - 1)) != 0)
		{
			throw MemoryAlignmentException(address, alignment, mnemonic);
		}
	}

	inline std::vector<unsigned char> MemoryUnit::GetScalarStoreBytes(unsigned long long storeData, unsigned char accessSize)
	{
		switch (accessSize)
		{
		case 1:
			return { static_cast<unsigned char>(storeData & 0xFF) };
		case 2:
			return { static_cast<unsigned char>(storeData & 0xFF), static_cast<unsigned char>((storeData >> 8) & 0xFF) };
		case 4:
			return { static_cast<unsigned char>(storeData & 0xFF), static_cast<unsigned char>((storeData >> 8) & 0xFF),
				static_cast<unsigned char>((storeData >> 16) & 0xFF), static_cast<unsigned char>((storeData >> 24) & 0xFF) };
		case 8:
		{
			std::vector<unsigned char> bytes(8);
			for (unsigned int i = 0; i < bytes.size(); ++i)
				bytes[i] = static_cast<unsigned char>((storeData >> (i * 8)) & 0xFF);
			return bytes;
		}
		default:
			throw InvalidOperationException(std::format(L"Unsupported scalar memory access size {}.", static_cast<unsigned int>(accessSize)));
		}
	}
}
