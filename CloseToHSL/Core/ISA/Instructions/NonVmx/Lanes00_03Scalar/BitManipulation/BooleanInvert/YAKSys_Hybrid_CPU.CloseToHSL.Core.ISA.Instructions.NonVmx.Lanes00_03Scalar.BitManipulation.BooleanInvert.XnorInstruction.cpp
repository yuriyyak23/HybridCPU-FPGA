#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.BooleanInvert.XnorInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BooleanInvert
{

	unsigned short XnorInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::XNOR);
	}

	bool XnorInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool XnorInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long XnorInstruction::Execute(unsigned long long left, unsigned long long right)
	{
		return ~(left ^ right);
	}

	unsigned long long XnorInstruction::EvaluateXLen64(unsigned long long left, unsigned long long right)
	{
		return ~(left ^ right);
	}

	bool XnorInstruction::RetireWritesDestination(int destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<XnorInstruction::BooleanInvertGoldenVector> XnorInstruction::GetLocalGoldenVectors()
	{
		return std::vector<BooleanInvertGoldenVector> { {0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL, 0xFFFF'FFFF'FFFF'FFFFULL}, {0x0000'0000'0000'0000ULL, 0xFFFF'FFFF'FFFF'FFFFULL, 0x0000'0000'0000'0000ULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 0xFFFF'FFFF'FFFF'FFFFULL, 0xFFFF'FFFF'FFFF'FFFFULL}, {0xAAAA'AAAA'5555'5555ULL, 0x0F0F'0F0F'F0F0'F0F0ULL, 0x5A5A'5A5A'5A5A'5A5AULL}, };
	}

	XnorInstruction::BooleanInvertGoldenVector::BooleanInvertGoldenVector(unsigned long long left, unsigned long long right, unsigned long long result)
		: Left(left), Right(right), Result(result)
	{
	}

	unsigned long long XnorInstruction::BooleanInvertGoldenVector::getLeft() const
	{
		return Left;
	}

	unsigned long long XnorInstruction::BooleanInvertGoldenVector::getRight() const
	{
		return Right;
	}

	unsigned long long XnorInstruction::BooleanInvertGoldenVector::getResult() const
	{
		return Result;
	}
}
