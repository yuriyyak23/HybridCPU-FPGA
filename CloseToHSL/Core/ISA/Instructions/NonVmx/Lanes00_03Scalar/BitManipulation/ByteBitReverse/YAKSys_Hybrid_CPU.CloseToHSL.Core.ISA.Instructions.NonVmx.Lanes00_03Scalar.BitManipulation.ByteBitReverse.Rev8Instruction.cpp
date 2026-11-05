#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.ByteBitReverse.Rev8Instruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::ByteBitReverse
{

	unsigned short Rev8Instruction::getOpcode()
	{
		return static_cast<unsigned short>(YAKSys_Hybrid_CPU::Processor::CPU_Core::InstructionsEnum::REV8);
	}

	bool Rev8Instruction::getWritesScalarRegister()
	{
		return true;
	}

	bool Rev8Instruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long Rev8Instruction::Execute(unsigned long long value)
	{
		return EvaluateXLen64(value);
	}

	unsigned long long Rev8Instruction::EvaluateXLen64(unsigned long long value)
	{
		return ((value & 0x0000'0000'0000'00FFULL) << 56) | ((value & 0x0000'0000'0000'FF00ULL) << 40) | ((value & 0x0000'0000'00FF'0000ULL) << 24) | ((value & 0x0000'0000'FF00'0000ULL) << 8) | ((value & 0x0000'00FF'0000'0000ULL) >> 8) | ((value & 0x0000'FF00'0000'0000ULL) >> 24) | ((value & 0x00FF'0000'0000'0000ULL) >> 40) | ((value & 0xFF00'0000'0000'0000ULL) >> 56);
	}

	bool Rev8Instruction::RetireWritesDestination(int destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<Rev8Instruction::ByteBitReverseGoldenVector> Rev8Instruction::GetLocalGoldenVectors()
	{
		return std::vector<ByteBitReverseGoldenVector> { {0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 0xFFFF'FFFF'FFFF'FFFFULL}, {0x0123'4567'89AB'CDEFULL, 0xEFCD'AB89'6745'2301ULL}, {0x8000'0000'0000'0001ULL, 0x0100'0000'0000'0080ULL}, };
	}

	Rev8Instruction::ByteBitReverseGoldenVector::ByteBitReverseGoldenVector(unsigned long long value, unsigned long long result)
		: Value(value), Result(result)
	{
	}

	unsigned long long Rev8Instruction::ByteBitReverseGoldenVector::getValue() const
	{
		return Value;
	}

	unsigned long long Rev8Instruction::ByteBitReverseGoldenVector::getResult() const
	{
		return Result;
	}
}
