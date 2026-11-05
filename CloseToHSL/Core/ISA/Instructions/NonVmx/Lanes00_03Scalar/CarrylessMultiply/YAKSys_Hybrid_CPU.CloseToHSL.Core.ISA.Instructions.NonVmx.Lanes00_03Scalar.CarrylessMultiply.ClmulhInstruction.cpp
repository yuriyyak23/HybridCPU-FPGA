#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.CarrylessMultiply.ClmulhInstruction.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::CarrylessMultiply
{

	unsigned short ClmulhInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::CLMULH);
	}

	bool ClmulhInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool ClmulhInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long ClmulhInstruction::Execute(unsigned long long multiplicand, unsigned long long multiplier)
	{
		return CarryLessMultiplyWindow64(multiplicand, multiplier, ProductWindowStartBit);
	}

	unsigned long long ClmulhInstruction::EvaluateXLen64(unsigned long long multiplicand, unsigned long long multiplier)
	{
		return Execute(multiplicand, multiplier);
	}

	bool ClmulhInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<ClmulhInstruction::CarryLessGoldenVector> ClmulhInstruction::GetLocalGoldenVectors()
	{
		return std::vector<CarryLessGoldenVector> { CarryLessGoldenVector(0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL), CarryLessGoldenVector(0xFFFF'FFFF'FFFF'FFFFULL, 0xFFFF'FFFF'FFFF'FFFFULL, 0x5555'5555'5555'5555ULL), CarryLessGoldenVector(0x8000'0000'0000'0000ULL, 0x0000'0000'0000'0002ULL, 0x0000'0000'0000'0001ULL), CarryLessGoldenVector(0x1234'5678'9ABC'DEF0ULL, 0x0FED'CBA9'8765'4321ULL, 0x00E0'38D8'6888'50B0ULL)};
	}

	unsigned long long ClmulhInstruction::CarryLessMultiplyWindow64(unsigned long long multiplicand, unsigned long long multiplier, int startBit)
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

	ClmulhInstruction::CarryLessGoldenVector::CarryLessGoldenVector(unsigned long long multiplicand, unsigned long long multiplier, unsigned long long expected)
		: Multiplicand(multiplicand), Multiplier(multiplier), Expected(expected)
	{
	}

	unsigned long long ClmulhInstruction::CarryLessGoldenVector::getMultiplicand() const
	{
		return Multiplicand;
	}

	unsigned long long ClmulhInstruction::CarryLessGoldenVector::getMultiplier() const
	{
		return Multiplier;
	}

	unsigned long long ClmulhInstruction::CarryLessGoldenVector::getExpected() const
	{
		return Expected;
	}
}
