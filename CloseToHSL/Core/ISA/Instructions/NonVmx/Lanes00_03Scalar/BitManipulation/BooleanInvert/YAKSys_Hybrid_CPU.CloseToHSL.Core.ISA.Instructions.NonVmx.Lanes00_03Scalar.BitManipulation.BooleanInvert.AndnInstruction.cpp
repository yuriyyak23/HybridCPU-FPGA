#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.BooleanInvert.AndnInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BooleanInvert
{

	unsigned short AndnInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::ANDN);
	}

	bool AndnInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool AndnInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long AndnInstruction::Execute(unsigned long long left, unsigned long long right)
	{
		return left & ~right;
	}

	unsigned long long AndnInstruction::EvaluateXLen64(unsigned long long left, unsigned long long right)
	{
		return left & ~right;
	}

	bool AndnInstruction::RetireWritesDestination(int destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<AndnInstruction::BooleanInvertGoldenVector> AndnInstruction::GetLocalGoldenVectors()
	{
		return std::vector<BooleanInvertGoldenVector> { {0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 0x0000'0000'0000'0000ULL, 0xFFFF'FFFF'FFFF'FFFFULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 0xFFFF'FFFF'FFFF'FFFFULL, 0x0000'0000'0000'0000ULL}, {0xFFFF'0000'FFFF'0000ULL, 0x00FF'00FF'00FF'00FFULL, 0xFF00'0000'FF00'0000ULL}, };
	}

	AndnInstruction::BooleanInvertGoldenVector::BooleanInvertGoldenVector(unsigned long long left, unsigned long long right, unsigned long long result)
		: Left(left), Right(right), Result(result)
	{
	}

	unsigned long long AndnInstruction::BooleanInvertGoldenVector::getLeft() const
	{
		return Left;
	}

	unsigned long long AndnInstruction::BooleanInvertGoldenVector::getRight() const
	{
		return Right;
	}

	unsigned long long AndnInstruction::BooleanInvertGoldenVector::getResult() const
	{
		return Result;
	}
}
