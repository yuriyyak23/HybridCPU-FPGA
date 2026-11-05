#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::Rotates
{
	class RolInstruction final : public std::enable_shared_from_this<RolInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"ROL";
		static constexpr int XLen = 64;
		static constexpr int ShiftMask = 0x3F;

		static unsigned short getOpcode();

		static bool getWritesScalarRegister();

		static bool getHasSideEffects();

		static unsigned long long Execute(unsigned long long source, unsigned long long shiftSource);
	};
}
