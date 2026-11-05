#pragma once

#include <limits>

namespace YAKSys_Hybrid_CPU::Core::Registers
{
	/// <summary>
	/// Core-owned architectural register triplet encoding used by runtime helper contours.
	/// Keeps register packing out of legacy CompilerEnv surfaces.
	/// </summary>
	class ArchRegisterTripletEncoding final
	{
	public:
		static constexpr unsigned short NoReg = std::numeric_limits<unsigned short>::max();
		static constexpr unsigned char NoArchReg = std::numeric_limits<unsigned char>::max();

		static unsigned long long Pack(unsigned char rd, unsigned char rs1, unsigned char rs2);

		static bool TryUnpack(unsigned long long packedRegisters, unsigned char &rd, unsigned char &rs1, unsigned char &rs2);
	};
}
