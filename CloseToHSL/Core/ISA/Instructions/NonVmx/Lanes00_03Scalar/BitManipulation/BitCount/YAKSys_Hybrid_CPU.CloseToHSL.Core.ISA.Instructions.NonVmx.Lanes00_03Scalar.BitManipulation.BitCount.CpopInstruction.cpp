#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.BitCount.CpopInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <bit>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitCount
{

	unsigned short CpopInstruction::getOpcode()
	{
		return static_cast<unsigned short>(YAKSys_Hybrid_CPU::Processor::CPU_Core::InstructionsEnum::CPOP);
	}

	bool CpopInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool CpopInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long CpopInstruction::Execute(unsigned long long source)
	{
		return EvaluateXLen64(source);
	}

	unsigned long long CpopInstruction::EvaluateXLen64(unsigned long long source)
	{
		return static_cast<unsigned long long>(std::popcount(source));
	}

	bool CpopInstruction::RetireWritesDestination(int destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<CpopInstruction::PopulationCountGoldenVector> CpopInstruction::GetLocalGoldenVectors()
	{
		return std::vector<CpopInstruction::PopulationCountGoldenVector> { {0x0000'0000'0000'0000ULL, 0ULL}, {0x0000'0000'0000'0001ULL, 1ULL}, {0x8000'0000'0000'0000ULL, 1ULL}, {0x0123'4567'89AB'CDEFULL, 32ULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 64ULL} };
	}

	CpopInstruction::PopulationCountGoldenVector::PopulationCountGoldenVector(unsigned long long source, unsigned long long result)
		: Source(source), Result(result)
	{
	}

	unsigned long long CpopInstruction::PopulationCountGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned long long CpopInstruction::PopulationCountGoldenVector::getResult() const
	{
		return Result;
	}
}
