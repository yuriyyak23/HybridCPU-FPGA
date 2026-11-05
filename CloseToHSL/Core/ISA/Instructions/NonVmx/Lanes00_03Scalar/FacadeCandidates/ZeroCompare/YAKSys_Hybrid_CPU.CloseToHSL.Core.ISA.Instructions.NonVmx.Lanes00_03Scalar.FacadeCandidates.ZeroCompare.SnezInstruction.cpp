#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.FacadeCandidates.ZeroCompare.SnezInstruction.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::FacadeCandidates::ZeroCompare
{

	unsigned long long SnezInstruction::EvaluateHardwareCandidateXLen64(unsigned long long value)
	{
		return value != 0 ? 1ULL : 0ULL;
	}

	bool SnezInstruction::WouldRetireWriteDestinationIfHardwareSelected(int destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<SnezInstruction::ZeroCompareFacadeGoldenVector> SnezInstruction::GetLocalHardwareCandidateGoldenVectors()
	{
		return std::vector<SnezInstruction::ZeroCompareFacadeGoldenVector> { {0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL}, {0x0000'0000'0000'0001ULL, 0x0000'0000'0000'0001ULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 0x0000'0000'0000'0001ULL}, {0x8000'0000'0000'0000ULL, 0x0000'0000'0000'0001ULL} };
	}

	SnezInstruction::ZeroCompareFacadeGoldenVector::ZeroCompareFacadeGoldenVector(unsigned long long value, unsigned long long result)
		: Value(value), Result(result)
	{
	}

	unsigned long long SnezInstruction::ZeroCompareFacadeGoldenVector::getValue() const
	{
		return Value;
	}

	unsigned long long SnezInstruction::ZeroCompareFacadeGoldenVector::getResult() const
	{
		return Result;
	}
}
