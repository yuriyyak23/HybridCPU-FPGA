#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.ScalarMinMax.MinuInstruction.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::ScalarMinMax
{

	unsigned short MinuInstruction::getOpcode()
	{
		return static_cast<unsigned short>(YAKSys_Hybrid_CPU::Processor::CPU_Core::InstructionsEnum::MINU);
	}

	bool MinuInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool MinuInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long MinuInstruction::Execute(unsigned long long left, unsigned long long right)
	{
		return EvaluateXLen64(left, right);
	}

	unsigned long long MinuInstruction::EvaluateXLen64(unsigned long long left, unsigned long long right)
	{
		return left <= right ? left : right;
	}

	bool MinuInstruction::RetireWritesDestination(int destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<MinuInstruction::ScalarMinMaxGoldenVector> MinuInstruction::GetLocalGoldenVectors()
	{
		return std::vector<ScalarMinMaxGoldenVector> { {0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL}, {0x0000'0000'0000'0001ULL, 0x0000'0000'0000'0002ULL, 0x0000'0000'0000'0001ULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 0x0000'0000'0000'0001ULL, 0x0000'0000'0000'0001ULL}, {0x8000'0000'0000'0000ULL, 0x7FFF'FFFF'FFFF'FFFFULL, 0x7FFF'FFFF'FFFF'FFFFULL}, };
	}

	MinuInstruction::ScalarMinMaxGoldenVector::ScalarMinMaxGoldenVector(unsigned long long left, unsigned long long right, unsigned long long result)
		: Left(left), Right(right), Result(result)
	{
	}

	unsigned long long MinuInstruction::ScalarMinMaxGoldenVector::getLeft() const
	{
		return Left;
	}

	unsigned long long MinuInstruction::ScalarMinMaxGoldenVector::getRight() const
	{
		return Right;
	}

	unsigned long long MinuInstruction::ScalarMinMaxGoldenVector::getResult() const
	{
		return Result;
	}
}
