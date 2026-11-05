#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.ByteBitReverse.Brev8Instruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::ByteBitReverse
{

	unsigned short Brev8Instruction::getOpcode()
	{
		return static_cast<unsigned short>(YAKSys_Hybrid_CPU::Processor::CPU_Core::InstructionsEnum::BREV8);
	}

	bool Brev8Instruction::getWritesScalarRegister()
	{
		return true;
	}

	bool Brev8Instruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long Brev8Instruction::Execute(unsigned long long value)
	{
		return EvaluateXLen64(value);
	}

	unsigned long long Brev8Instruction::EvaluateXLen64(unsigned long long value)
	{
		unsigned long long result = 0;

		for (int byteIndex = 0; byteIndex < 8; byteIndex++)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: byte sourceByte = unchecked((byte)(value >> (byteIndex * 8)));
			unsigned char sourceByte = static_cast<unsigned char>(value >> (byteIndex * 8));
			result |= static_cast<unsigned long long>(ReverseBitsInByte(sourceByte)) << (byteIndex * 8);
		}

		return result;
	}

	bool Brev8Instruction::RetireWritesDestination(int destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<Brev8Instruction::ByteBitReverseGoldenVector> Brev8Instruction::GetLocalGoldenVectors()
	{
		return std::vector<ByteBitReverseGoldenVector> { {0x0000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 0xFFFF'FFFF'FFFF'FFFFULL}, {0x0123'4567'89AB'CDEFULL, 0x80C4'A2E6'91D5'B3F7ULL}, {0x8040'2010'0804'0201ULL, 0x0102'0408'1020'4080ULL}, };
	}

	unsigned char Brev8Instruction::ReverseBitsInByte(unsigned char value)
	{
		unsigned int reversed = value;
		reversed = ((reversed & 0x55U) << 1) | ((reversed >> 1) & 0x55U);
		reversed = ((reversed & 0x33U) << 2) | ((reversed >> 2) & 0x33U);
		reversed = ((reversed & 0x0FU) << 4) | ((reversed >> 4) & 0x0FU);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((byte)reversed);
		return static_cast<unsigned char>(reversed);
	}

	Brev8Instruction::ByteBitReverseGoldenVector::ByteBitReverseGoldenVector(unsigned long long value, unsigned long long result)
		: Value(value), Result(result)
	{
	}

	unsigned long long Brev8Instruction::ByteBitReverseGoldenVector::getValue() const
	{
		return Value;
	}

	unsigned long long Brev8Instruction::ByteBitReverseGoldenVector::getResult() const
	{
		return Result;
	}
}
