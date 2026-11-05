#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.ScalarMinMax.MinInstruction.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::ScalarMinMax
{

	unsigned short MinInstruction::getOpcode()
	{
		return static_cast<unsigned short>(YAKSys_Hybrid_CPU::Processor::CPU_Core::InstructionsEnum::MIN);
	}

	bool MinInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool MinInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long MinInstruction::Execute(unsigned long long left, unsigned long long right)
	{
		return EvaluateXLen64(left, right);
	}

	unsigned long long MinInstruction::EvaluateXLen64(unsigned long long left, unsigned long long right)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((long)left) <= unchecked((long)right) ? left : right;
		return static_cast<long long>(left) <= static_cast<long long>(right) ? left : right;
	}

	bool MinInstruction::RetireWritesDestination(int destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<MinInstruction::ScalarMinMaxGoldenVector> MinInstruction::GetLocalGoldenVectors()
	{
		return std::vector<ScalarMinMaxGoldenVector> { {0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL}, {0x0000'0000'0000'0001ULL, 0x0000'0000'0000'0002ULL, 0x0000'0000'0000'0001ULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 0x0000'0000'0000'0001ULL, 0xFFFF'FFFF'FFFF'FFFFULL}, {0x8000'0000'0000'0000ULL, 0x7FFF'FFFF'FFFF'FFFFULL, 0x8000'0000'0000'0000ULL}, };
	}

	MinInstruction::ScalarMinMaxGoldenVector::ScalarMinMaxGoldenVector(unsigned long long left, unsigned long long right, unsigned long long result)
		: Left(left), Right(right), Result(result)
	{
	}

	unsigned long long MinInstruction::ScalarMinMaxGoldenVector::getLeft() const
	{
		return Left;
	}

	unsigned long long MinInstruction::ScalarMinMaxGoldenVector::getRight() const
	{
		return Right;
	}

	unsigned long long MinInstruction::ScalarMinMaxGoldenVector::getResult() const
	{
		return Result;
	}
}
