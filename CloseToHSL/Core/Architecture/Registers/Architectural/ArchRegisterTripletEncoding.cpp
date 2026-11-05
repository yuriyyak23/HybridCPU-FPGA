#include "ArchRegisterTripletEncoding.h"
#include "RegisterIdentity.h"
#include <stdexcept>
#include <format>

namespace YAKSys_Hybrid_CPU::Core::Registers
{
	static unsigned long long PackField(unsigned char regId, const wchar_t *paramName)
	{
		if (regId == ArchRegisterTripletEncoding::NoArchReg) return ArchRegisterTripletEncoding::NoReg;
		if (regId > ArchRegId::MaxValue) throw std::out_of_range("Architectural register id must be in [0, 31] or NoArchReg.");
		return regId;
	}

	static bool TryDecodeField(unsigned short encoded, unsigned char &regId)
	{
		if (encoded == ArchRegisterTripletEncoding::NoReg) { regId = ArchRegisterTripletEncoding::NoArchReg; return true; }
		if (encoded <= ArchRegId::MaxValue) { regId = static_cast<unsigned char>(encoded); return true; }
		regId = ArchRegisterTripletEncoding::NoArchReg;
		return false;
	}

	unsigned long long ArchRegisterTripletEncoding::Pack(unsigned char rd, unsigned char rs1, unsigned char rs2)
	{
// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		static ulong PackField(byte regId, string paramName)
	//			{
	//				if (regId == NoArchReg)
	//				{
	//					return NoReg;
	//				}
	//
	//				if (regId > ArchRegId.MaxValue)
	//				{
	//					throw new ArgumentOutOfRangeException(paramName, regId, string.Format("Architectural register id must be in [0, {0}] or NoArchReg.", ArchRegId.MaxValue));
	//				}
	//
	//				return regId;
	//			}

		return PackField(rd, L"rd") | (PackField(rs1, L"rs1") << 16) | (PackField(rs2, L"rs2") << 32);
	}

	bool ArchRegisterTripletEncoding::TryUnpack(unsigned long long packedRegisters, unsigned char &rd, unsigned char &rs1, unsigned char &rs2)
	{
// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		static bool TryDecodeField(ushort encoded, out byte regId)
	//			{
	//				if (encoded == NoReg)
	//				{
	//					regId = NoArchReg;
	//					return true;
	//				}
	//
	//				if (encoded <= ArchRegId.MaxValue)
	//				{
	//					regId = (byte)encoded;
	//					return true;
	//				}
	//
	//				regId = NoArchReg;
	//				return false;
	//			}

		rd = 0;
		rs1 = 0;
		rs2 = 0;

		return TryDecodeField(static_cast<unsigned short>(packedRegisters & 0xFFFF), rd) && TryDecodeField(static_cast<unsigned short>((packedRegisters >> 16) & 0xFFFF), rs1) && TryDecodeField(static_cast<unsigned short>((packedRegisters >> 32) & 0xFFFF), rs2);
	}
}
