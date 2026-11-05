#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.BitCount.PopcntInstruction.h"

#include <bit>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitCount
{

	unsigned long long PopcntInstruction::EvaluateXLen64(unsigned long long source)
	{
		return static_cast<unsigned long long>(std::popcount(source));
	}

	bool PopcntInstruction::RetireWritesDestination(int destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<PopcntInstruction::PopulationCountGoldenVector> PopcntInstruction::GetLocalGoldenVectors()
	{
		return std::vector<PopcntInstruction::PopulationCountGoldenVector> { {0x0000'0000'0000'0000ULL, 0ULL}, {0x0000'0000'0000'0001ULL, 1ULL}, {0x8000'0000'0000'0000ULL, 1ULL}, {0x0123'4567'89AB'CDEFULL, 32ULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 64ULL} };
	}

	PopcntInstruction::PopulationCountGoldenVector::PopulationCountGoldenVector(unsigned long long source, unsigned long long result)
		: Source(source), Result(result)
	{
	}

	unsigned long long PopcntInstruction::PopulationCountGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned long long PopcntInstruction::PopulationCountGoldenVector::getResult() const
	{
		return Result;
	}
}
