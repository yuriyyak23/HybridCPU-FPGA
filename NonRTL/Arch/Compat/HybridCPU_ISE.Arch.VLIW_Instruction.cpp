#include "HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../InvalidOpcodeException.h"
#include "tangible_exception_helper.h"
#include <stdexcept>

using namespace YAKSys_Hybrid_CPU::Arch;


namespace HybridCPU_ISE::Arch
{

	std::wstring VLIW_Instruction::GetReservedWord0ViolationMessage(int slotIndex)
	{
		return slotIndex >= 0 ? std::format(L"Instruction at slot {} has word0[47:40] set. This reserved field must be zero in production decoded streams.", slotIndex) : L"Instruction has word0[47:40] set. This reserved field must be zero in production decoded streams.";
	}

	std::wstring VLIW_Instruction::GetRetiredPolicyGapViolationMessage(int slotIndex)
	{
		return slotIndex >= 0 ? std::format(L"Instruction at slot {} has word3[50] set. This retired legacy scheduling-policy bit must be zero in production decoded streams.", slotIndex) : L"Instruction has word3[50] set. This retired legacy scheduling-policy bit must be zero in production decoded streams.";
	}

	unsigned long long VLIW_Instruction::ValidateWord0ForProductionIngress(unsigned long long rawWord0)
	{
		if ((rawWord0 & ReservedWord0Mask) == 0)
		{
			return rawWord0;
		}

		throw InvalidOpcodeException(GetReservedWord0ViolationMessage());
	}

	unsigned long long VLIW_Instruction::ValidateWord3ForProductionIngress(unsigned long long rawWord3)
	{
		if ((rawWord3 & RetiredPolicyGapMask) == 0)
		{
			return rawWord3;
		}

		throw InvalidOpcodeException(GetRetiredPolicyGapViolationMessage());
	}

	void VLIW_Instruction::ValidateRetiredPolicyGapBitForProductionIngress()
	{
		word0 = ValidateWord0ForProductionIngress(word0);
		word3 = ValidateWord3ForProductionIngress(word3);
	}

	bool VLIW_Instruction::getMaskAgnostic() const
	{
		return ((word0 >> 19) & 0x1ULL) != 0;
	} // Bit 19

	void VLIW_Instruction::setMaskAgnostic(bool value)
	{
		word0 = value ? (word0 | (1ULL << 19)) : (word0 & ~(1ULL << 19));
	}

	bool VLIW_Instruction::getTailAgnostic() const
	{
		return ((word0 >> 20) & 0x1ULL) != 0;
	} // Bit 20

	void VLIW_Instruction::setTailAgnostic(bool value)
	{
		word0 = value ? (word0 | (1ULL << 20)) : (word0 & ~(1ULL << 20));
	}

	bool VLIW_Instruction::getIndexed() const
	{
		return ((word0 >> 21) & 0x1ULL) != 0;
	} // Bit 21

	void VLIW_Instruction::setIndexed(bool value)
	{
		word0 = value ? (word0 | (1ULL << 21)) : (word0 & ~(1ULL << 21));
	}

	bool VLIW_Instruction::getIs2D() const
	{
		return ((word0 >> 22) & 0x1ULL) != 0;
	} // Bit 22

	void VLIW_Instruction::setIs2D(bool value)
	{
		word0 = value ? (word0 | (1ULL << 22)) : (word0 & ~(1ULL << 22));
	}

	bool VLIW_Instruction::getReduction() const
	{
		return ((word0 >> 23) & 0x1ULL) != 0;
	} // Bit 23

	void VLIW_Instruction::setReduction(bool value)
	{
		word0 = value ? (word0 | (1ULL << 23)) : (word0 & ~(1ULL << 23));
	}

	bool VLIW_Instruction::getAcquire() const
	{
		return ((word0 >> 18) & 0x1ULL) != 0;
	} // Bit 18

	void VLIW_Instruction::setAcquire(bool value)
	{
		word0 = value ? (word0 | (1ULL << 18)) : (word0 & ~(1ULL << 18));
	}

	bool VLIW_Instruction::getRelease() const
	{
		return ((word0 >> 17) & 0x1ULL) != 0;
	} // Bit 17

	void VLIW_Instruction::setRelease(bool value)
	{
		word0 = value ? (word0 | (1ULL << 17)) : (word0 & ~(1ULL << 17));
	}

	bool VLIW_Instruction::getSaturating() const
	{
		return ((word0 >> 16) & 0x1ULL) != 0;
	} // Bit 16

	void VLIW_Instruction::setSaturating(bool value)
	{
		word0 = value ? (word0 | (1ULL << 16)) : (word0 & ~(1ULL << 16));
	}

	unsigned long long VLIW_Instruction::getWord0() const
	{
		return word0;
	}

	void VLIW_Instruction::setWord0(unsigned long long value)
	{
		word0 = value;
	}

	unsigned long long VLIW_Instruction::getWord1() const
	{
		return word1;
	}

	void VLIW_Instruction::setWord1(unsigned long long value)
	{
		word1 = value;
	}

	unsigned long long VLIW_Instruction::getWord2() const
	{
		return word2;
	}

	void VLIW_Instruction::setWord2(unsigned long long value)
	{
		word2 = value;
	}

	unsigned long long VLIW_Instruction::getWord3() const
	{
		return word3;
	}

	void VLIW_Instruction::setWord3(unsigned long long value)
	{
		word3 = value;
	}

	unsigned int VLIW_Instruction::getOpCode() const
	{
		return static_cast<unsigned int>((word0 >> 48) & 0xFFFF);
	}

	void VLIW_Instruction::setOpCode(unsigned int value)
	{
		word0 = (word0 & 0x0000FFFFFFFFFFFFULL) | (static_cast<unsigned long long>(value & 0xFFFF) << 48);
	}

	unsigned char VLIW_Instruction::getDataType() const
	{
		return static_cast<unsigned char>((word0 >> 32) & 0xFF);
	}

	void VLIW_Instruction::setDataType(unsigned char value)
	{
		word0 = (word0 & 0xFFFFFF00FFFFFFFFULL) | (static_cast<unsigned long long>(value) << 32);
	}

	DataTypeEnum VLIW_Instruction::getDataTypeValue() const
	{
		return static_cast<DataTypeEnum>(getDataType());
	}

	void VLIW_Instruction::setDataTypeValue(DataTypeEnum value)
	{
		setDataType(static_cast<unsigned char>(value));
	}

	unsigned char VLIW_Instruction::getPredicateMask() const
	{
		return static_cast<unsigned char>((word0 >> 24) & 0xFF);
	}

	void VLIW_Instruction::setPredicateMask(unsigned char value)
	{
		word0 = (word0 & 0xFFFFFFFF00FFFFFFULL) | (static_cast<unsigned long long>(value) << 24);
	}

	unsigned short VLIW_Instruction::getImmediate() const
	{
		return static_cast<unsigned short>(word0 & 0xFFFF);
	} // Bits [15:0] — 16 bits (max 65535)

	void VLIW_Instruction::setImmediate(unsigned short value)
	{
		word0 = (word0 & 0xFFFFFFFFFFFF0000ULL) | (static_cast<unsigned long long>(value) & 0xFFFFULL);
	}

	unsigned char VLIW_Instruction::getReserved() const
	{
		return static_cast<unsigned char>((word0 >> 40) & 0xFF);
	}

	void VLIW_Instruction::setReserved(unsigned char value)
	{
		word0 = (word0 & 0xFFFF00FFFFFFFFFFULL) | (static_cast<unsigned long long>(value) << 40);
	}

	unsigned long long VLIW_Instruction::getDestSrc1Pointer() const
	{
		return word1;
	}

	void VLIW_Instruction::setDestSrc1Pointer(unsigned long long value)
	{
		word1 = value;
	}

	unsigned long long VLIW_Instruction::getSrc2Pointer() const
	{
		return word2;
	}

	void VLIW_Instruction::setSrc2Pointer(unsigned long long value)
	{
		word2 = value;
	}

	unsigned int VLIW_Instruction::getSourceAPointer() const
	{
		return static_cast<unsigned int>(word1 & 0xFFFFFFFFULL);
	}

	void VLIW_Instruction::setSourceAPointer(unsigned int value)
	{
		word1 = (word1 & 0xFFFFFFFF00000000ULL) | value;
	}

	unsigned int VLIW_Instruction::getSourceBPointer() const
	{
		return static_cast<unsigned int>((word1 >> 32) & 0xFFFFFFFFULL);
	}

	void VLIW_Instruction::setSourceBPointer(unsigned int value)
	{
		word1 = (word1 & 0x00000000FFFFFFFFULL) | (static_cast<unsigned long long>(value) << 32);
	}

	unsigned int VLIW_Instruction::getDestinationPointer() const
	{
		return static_cast<unsigned int>(word2 & 0xFFFFFFFFULL);
	}

	void VLIW_Instruction::setDestinationPointer(unsigned int value)
	{
		word2 = (word2 & 0xFFFFFFFF00000000ULL) | value;
	}

	unsigned int VLIW_Instruction::getVectorDataLength() const
	{
		return static_cast<unsigned int>((word2 >> 32) & 0xFFFFFFFFULL);
	}

	void VLIW_Instruction::setVectorDataLength(unsigned int value)
	{
		word2 = (word2 & 0x00000000FFFFFFFFULL) | (static_cast<unsigned long long>(value) << 32);
	}

	unsigned short VLIW_Instruction::getRowStride() const
	{
		return static_cast<unsigned short>((word3 >> 51) & 0x1FFF);
	} // 13 bits (max 8191)

	void VLIW_Instruction::setRowStride(unsigned short value)
	{
		if (value > 0x1FFF)
		{
			throw ArgumentOutOfRangeException(L"RowStride must be <= 8191 (13 bits) (value)");
		}
		word3 = (word3 & 0x0007FFFFFFFFFFFFULL) | (static_cast<unsigned long long>(value & 0x1FFF) << 51);
	}

	unsigned char VLIW_Instruction::getVirtualThreadId() const
	{
		return static_cast<unsigned char>((word3 >> 48) & 0x3);
	} // Bits [49:48] - 2 bits (0-3)

	void VLIW_Instruction::setVirtualThreadId(unsigned char value)
	{
		if (value > 3)
		{
			throw ArgumentOutOfRangeException(L"VirtualThreadId must be 0-3 (value)");
		}
		word3 = (word3 & 0xFFFFCFFFFFFFFFFFULL) | (static_cast<unsigned long long>(value & 0x3) << 48);
	}

	unsigned int VLIW_Instruction::getStreamLength() const
	{
		return static_cast<unsigned int>((word3 >> 16) & 0xFFFFFFFF);
	}

	void VLIW_Instruction::setStreamLength(unsigned int value)
	{
		word3 = (word3 & 0xFFFF00000000FFFFULL) | (static_cast<unsigned long long>(value) << 16);
	}

	unsigned short VLIW_Instruction::getStride() const
	{
		return static_cast<unsigned short>(word3 & 0xFFFF);
	}

	void VLIW_Instruction::setStride(unsigned short value)
	{
		word3 = (word3 & 0xFFFFFFFFFFFF0000ULL) | value;
	}

	void VLIW_Instruction::Clear()
	{
		word0 = 0;
		word1 = 0;
		word2 = 0;
		word3 = 0;
	}

	unsigned long long VLIW_Instruction::PackArchRegs(unsigned char rd, unsigned char rs1, unsigned char rs2)
	{
// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		static ulong PackField(byte regId, string paramName)
	//				{
	//					if (regId == NoArchReg)
	//					{
	//						return NoReg;
	//					}
	//
	//					if (regId > ArchRegId.MaxValue)
	//					{
	//						throw new ArgumentOutOfRangeException(paramName, regId, string.Format("Architectural register id must be in [0, {0}] or NoArchReg.", ArchRegId.MaxValue));
	//					}
	//
	//					return regId;
	//				}

		        auto PackField = [](unsigned char regId, const wchar_t *paramName) -> unsigned long long {
            if (regId == NoArchReg) return NoReg;
            if (regId > 31) throw ArgumentOutOfRangeException(std::format(L"Architectural register id must be in [0, 31] or NoArchReg. ({})", paramName));
            return regId;
        };
        unsigned long long packed = 0;
		packed |= PackField(rd, L"rd");
		packed |= PackField(rs1, L"rs1") << 16;
		packed |= PackField(rs2, L"rs2") << 32;
		return packed;
	}

	bool VLIW_Instruction::TryUnpackArchRegs(unsigned long long packedRegisters, unsigned char &rd, unsigned char &rs1, unsigned char &rs2)
	{
// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		static bool TryDecodeField(ushort encoded, out byte regId)
	//				{
	//					if (encoded == NoReg)
	//					{
	//						regId = NoArchReg;
	//						return true;
	//					}
	//
	//					if (encoded <= ArchRegId.MaxValue)
	//					{
	//						regId = (byte)encoded;
	//						return true;
	//					}
	//
	//					regId = default;
	//					return false;
	//				}

		        auto TryDecodeField = [](unsigned short encoded, unsigned char &regId) -> bool {
            if (encoded == NoReg) { regId = NoArchReg; return true; }
            if (encoded <= 31) { regId = static_cast<unsigned char>(encoded); return true; }
            regId = 0; return false;
        };
        unsigned short rawRd = static_cast<unsigned short>(packedRegisters & 0xFFFF);
		unsigned short rawRs1 = static_cast<unsigned short>((packedRegisters >> 16) & 0xFFFF);
		unsigned short rawRs2 = static_cast<unsigned short>((packedRegisters >> 32) & 0xFFFF);

		if (!TryDecodeField(rawRd, rd) || !TryDecodeField(rawRs1, rs1) || !TryDecodeField(rawRs2, rs2))
		{
			rd = 0;
			rs1 = 0;
			rs2 = 0;
			return false;
		}

		return true;
	}

	unsigned short VLIW_Instruction::getReg1ID() const
	{
		return static_cast<unsigned short>(word1 & 0xFFFF);
	}

	unsigned short VLIW_Instruction::getReg2ID() const
	{
		return static_cast<unsigned short>((word1 >> 16) & 0xFFFF);
	}

	unsigned short VLIW_Instruction::getReg3ID() const
	{
		return static_cast<unsigned short>((word1 >> 32) & 0xFFFF);
	}

    bool VLIW_Instruction::TryReadBytesCore(std::span<const unsigned char> source, int offset)
    {
        if (offset < 0) throw std::out_of_range("offset");
        const auto start = static_cast<std::size_t>(offset);
        if (start > source.size() || source.size() - start < 32) return false;
        auto read = [&](std::size_t at) {
            unsigned long long result = 0;
            for (std::size_t i = 0; i < 8; ++i) result |= static_cast<unsigned long long>(source[at + i]) << (i * 8);
            return result;
        };
        word0 = ValidateWord0ForProductionIngress(read(start));
        word1 = read(start + 8);
        word2 = read(start + 16);
        word3 = ValidateWord3ForProductionIngress(read(start + 24));
        return true;
    }

    bool VLIW_Instruction::TryWriteBytes(std::span<unsigned char> destination)
    {
        if (destination.size() < 32) return false;
        const unsigned long long words[] = {word0, word1, word2, word3};
        for (std::size_t j = 0; j < 4; ++j)
            for (std::size_t i = 0; i < 8; ++i)
                destination[j * 8 + i] = static_cast<unsigned char>(words[j] >> (i * 8));
        return true;
    }

    bool VLIW_Instruction::TryReadBytes(std::span<const unsigned char> source, int offset)
    {
        return TryReadBytesCore(source, offset);
    }
}
