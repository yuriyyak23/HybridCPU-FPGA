#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.ScalarMinMax.MaxInstruction.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::ScalarMinMax
{

	unsigned short MaxInstruction::getOpcode()
	{
		return static_cast<unsigned short>(YAKSys_Hybrid_CPU::Processor::CPU_Core::InstructionsEnum::MAX);
	}

	bool MaxInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool MaxInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long MaxInstruction::Execute(unsigned long long left, unsigned long long right)
	{
		return EvaluateXLen64(left, right);
	}

	unsigned long long MaxInstruction::EvaluateXLen64(unsigned long long left, unsigned long long right)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((long)left) >= unchecked((long)right) ? left : right;
		return static_cast<long long>(left) >= static_cast<long long>(right) ? left : right;
	}

	bool MaxInstruction::RetireWritesDestination(int destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<MaxInstruction::ScalarMinMaxGoldenVector> MaxInstruction::GetLocalGoldenVectors()
	{
		return std::vector<ScalarMinMaxGoldenVector> { {0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL}, {0x0000'0000'0000'0001ULL, 0x0000'0000'0000'0002ULL, 0x0000'0000'0000'0002ULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 0x0000'0000'0000'0001ULL, 0x0000'0000'0000'0001ULL}, {0x8000'0000'0000'0000ULL, 0x7FFF'FFFF'FFFF'FFFFULL, 0x7FFF'FFFF'FFFF'FFFFULL}, };
	}

	MaxInstruction::ScalarMinMaxGoldenVector::ScalarMinMaxGoldenVector(unsigned long long left, unsigned long long right, unsigned long long result)
		: Left(left), Right(right), Result(result)
	{
	}

	unsigned long long MaxInstruction::ScalarMinMaxGoldenVector::getLeft() const
	{
		return Left;
	}

	unsigned long long MaxInstruction::ScalarMinMaxGoldenVector::getRight() const
	{
		return Right;
	}

	unsigned long long MaxInstruction::ScalarMinMaxGoldenVector::getResult() const
	{
		return Result;
	}
}
