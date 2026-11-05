#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.CarrylessMultiply.ClmulrInstruction.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::CarrylessMultiply
{

	unsigned short ClmulrInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::CLMULR);
	}

	bool ClmulrInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool ClmulrInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long ClmulrInstruction::Execute(unsigned long long multiplicand, unsigned long long multiplier)
	{
		return CarryLessMultiplyWindow64(multiplicand, multiplier, ProductWindowStartBit);
	}

	unsigned long long ClmulrInstruction::EvaluateXLen64(unsigned long long multiplicand, unsigned long long multiplier)
	{
		return Execute(multiplicand, multiplier);
	}

	bool ClmulrInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<ClmulrInstruction::CarryLessGoldenVector> ClmulrInstruction::GetLocalGoldenVectors()
	{
		return std::vector<CarryLessGoldenVector> { CarryLessGoldenVector(0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL), CarryLessGoldenVector(0xFFFF'FFFF'FFFF'FFFFULL, 0xFFFF'FFFF'FFFF'FFFFULL, 0xAAAA'AAAA'AAAA'AAAAULL), CarryLessGoldenVector(0x8000'0000'0000'0000ULL, 0x0000'0000'0000'0002ULL, 0x0000'0000'0000'0002ULL), CarryLessGoldenVector(0x1234'5678'9ABC'DEF0ULL, 0x0FED'CBA9'8765'4321ULL, 0x01C0'71B0'D110'A160ULL)};
	}

	unsigned long long ClmulrInstruction::CarryLessMultiplyWindow64(unsigned long long multiplicand, unsigned long long multiplier, int startBit)
	{
		unsigned long long result = 0ULL;

		for (int multiplierBit = 0; multiplierBit < XLen; multiplierBit++)
		{
			if (((multiplier >> multiplierBit) & 1ULL) == 0)
			{
				continue;
			}

			for (int multiplicandBit = 0; multiplicandBit < XLen; multiplicandBit++)
			{
				if (((multiplicand >> multiplicandBit) & 1ULL) == 0)
				{
					continue;
				}

				int resultBit = multiplierBit + multiplicandBit - startBit;
				if (static_cast<unsigned int>(resultBit) < static_cast<unsigned int>(XLen))
				{
					result ^= 1ULL << resultBit;
				}
			}
		}

		return result;
	}

	ClmulrInstruction::CarryLessGoldenVector::CarryLessGoldenVector(unsigned long long multiplicand, unsigned long long multiplier, unsigned long long expected)
		: Multiplicand(multiplicand), Multiplier(multiplier), Expected(expected)
	{
	}

	unsigned long long ClmulrInstruction::CarryLessGoldenVector::getMultiplicand() const
	{
		return Multiplicand;
	}

	unsigned long long ClmulrInstruction::CarryLessGoldenVector::getMultiplier() const
	{
		return Multiplier;
	}

	unsigned long long ClmulrInstruction::CarryLessGoldenVector::getExpected() const
	{
		return Expected;
	}
}
