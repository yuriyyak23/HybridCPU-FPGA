#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitCount
{
	class CtzInstruction final : public std::enable_shared_from_this<CtzInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"CTZ";
		static constexpr int XLen = 64;

		static unsigned short getOpcode();

		static bool getWritesScalarRegister();

		static bool getHasSideEffects();

		static unsigned long long Execute(unsigned long long source);
	};
}
