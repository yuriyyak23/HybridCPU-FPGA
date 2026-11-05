#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::Extension
{
	class SextBInstruction final : public std::enable_shared_from_this<SextBInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"SEXT.B";
		static constexpr int SourceBits = 8;
		static constexpr int XLen = 64;

		static unsigned short getOpcode();

		static bool getWritesScalarRegister();

		static bool getHasSideEffects();

		static unsigned long long Execute(unsigned long long source);
	};
}
