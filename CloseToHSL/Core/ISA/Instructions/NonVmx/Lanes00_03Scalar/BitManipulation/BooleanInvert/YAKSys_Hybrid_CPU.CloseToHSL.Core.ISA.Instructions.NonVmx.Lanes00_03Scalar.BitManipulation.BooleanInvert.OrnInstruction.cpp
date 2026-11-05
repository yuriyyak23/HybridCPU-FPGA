#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.BooleanInvert.OrnInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BooleanInvert
{

	unsigned short OrnInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::ORN);
	}

	bool OrnInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool OrnInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long OrnInstruction::Execute(unsigned long long left, unsigned long long right)
	{
		return left | ~right;
	}

	unsigned long long OrnInstruction::EvaluateXLen64(unsigned long long left, unsigned long long right)
	{
		return left | ~right;
	}

	bool OrnInstruction::RetireWritesDestination(int destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<OrnInstruction::BooleanInvertGoldenVector> OrnInstruction::GetLocalGoldenVectors()
	{
		return std::vector<BooleanInvertGoldenVector> { {0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL, 0xFFFF'FFFF'FFFF'FFFFULL}, {0x0000'0000'0000'0000ULL, 0xFFFF'FFFF'FFFF'FFFFULL, 0x0000'0000'0000'0000ULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 0xFFFF'FFFF'FFFF'FFFFULL, 0xFFFF'FFFF'FFFF'FFFFULL}, {0x1234'5678'9ABC'DEF0ULL, 0x00FF'00FF'00FF'00FFULL, 0xFF34'FF78'FFBC'FFF0ULL}, };
	}

	OrnInstruction::BooleanInvertGoldenVector::BooleanInvertGoldenVector(unsigned long long left, unsigned long long right, unsigned long long result)
		: Left(left), Right(right), Result(result)
	{
	}

	unsigned long long OrnInstruction::BooleanInvertGoldenVector::getLeft() const
	{
		return Left;
	}

	unsigned long long OrnInstruction::BooleanInvertGoldenVector::getRight() const
	{
		return Right;
	}

	unsigned long long OrnInstruction::BooleanInvertGoldenVector::getResult() const
	{
		return Result;
	}
}
