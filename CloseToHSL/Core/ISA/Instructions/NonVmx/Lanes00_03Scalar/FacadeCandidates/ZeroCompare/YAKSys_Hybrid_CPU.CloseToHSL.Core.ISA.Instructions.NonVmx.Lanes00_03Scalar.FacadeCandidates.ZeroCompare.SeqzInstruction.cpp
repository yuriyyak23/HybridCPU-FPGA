#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.FacadeCandidates.ZeroCompare.SeqzInstruction.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::FacadeCandidates::ZeroCompare
{

	unsigned long long SeqzInstruction::EvaluateHardwareCandidateXLen64(unsigned long long value)
	{
		return value == 0 ? 1ULL : 0ULL;
	}

	bool SeqzInstruction::WouldRetireWriteDestinationIfHardwareSelected(int destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<SeqzInstruction::ZeroCompareFacadeGoldenVector> SeqzInstruction::GetLocalHardwareCandidateGoldenVectors()
	{
		return std::vector<SeqzInstruction::ZeroCompareFacadeGoldenVector> { {0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0001ULL}, {0x0000'0000'0000'0001ULL, 0x0000'0000'0000'0000ULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 0x0000'0000'0000'0000ULL}, {0x8000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL} };
	}

	SeqzInstruction::ZeroCompareFacadeGoldenVector::ZeroCompareFacadeGoldenVector(unsigned long long value, unsigned long long result)
		: Value(value), Result(result)
	{
	}

	unsigned long long SeqzInstruction::ZeroCompareFacadeGoldenVector::getValue() const
	{
		return Value;
	}

	unsigned long long SeqzInstruction::ZeroCompareFacadeGoldenVector::getResult() const
	{
		return Result;
	}
}
