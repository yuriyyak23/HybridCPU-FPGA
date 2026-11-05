#include "ScalarAluOps.h"
#include "UInt64Multiply.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <bit>

// Windows headers define these names as rotate intrinsics; keep the class API visible here.
#ifdef RotateLeft64
#undef RotateLeft64
#endif
#ifdef RotateRight64
#undef RotateRight64
#endif

namespace YAKSys_Hybrid_CPU::Core
{

	unsigned long long ScalarAluOps::SignExtendWord(unsigned int word)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)(long)(int)word);
		return static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(word)));
	}

	unsigned long long ScalarAluOps::SignExtendWordFromScalar(unsigned long long op1)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return SignExtendWord(unchecked((uint)op1));
		return SignExtendWord(static_cast<unsigned int>(op1));
	}

	unsigned long long ScalarAluOps::ZeroExtendWordFromScalar(unsigned long long op1)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((uint)op1);
		return static_cast<unsigned int>(op1);
	}

	unsigned long long ScalarAluOps::SignExtendByteFromScalar(unsigned long long op1)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)(long)(sbyte)(byte)op1);
		return static_cast<unsigned long long>(static_cast<long long>(static_cast<signed char>(static_cast<unsigned char>(op1))));
	}

	unsigned long long ScalarAluOps::SignExtendHalfFromScalar(unsigned long long op1)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)(long)(short)(ushort)op1);
		return static_cast<unsigned long long>(static_cast<long long>(static_cast<short>(static_cast<unsigned short>(op1))));
	}

	unsigned long long ScalarAluOps::ZeroExtendHalfFromScalar(unsigned long long op1)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ushort)op1);
		return static_cast<unsigned short>(op1);
	}

	unsigned long long ScalarAluOps::ConditionalZeroIfEqualZero(unsigned long long op1, unsigned long long op2)
	{
		return op2 == 0 ? 0ULL : op1;
	}

	unsigned long long ScalarAluOps::ConditionalZeroIfNotEqualZero(unsigned long long op1, unsigned long long op2)
	{
		return op2 != 0 ? 0ULL : op1;
	}

	unsigned long long ScalarAluOps::CountLeadingZeros64(unsigned long long op1)
	{
		return static_cast<unsigned long long>(std::countl_zero(op1));
	}

	unsigned long long ScalarAluOps::CountTrailingZeros64(unsigned long long op1)
	{
		return static_cast<unsigned long long>(std::countr_zero(op1));
	}

	unsigned long long ScalarAluOps::CountPopulation64(unsigned long long op1)
	{
		return static_cast<unsigned long long>(std::popcount(op1));
	}

	unsigned long long ScalarAluOps::RotateLeft64(unsigned long long op1, unsigned long long op2)
	{
		int amount = static_cast<int>(op2 & ShiftAmountMask);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return amount == 0 ? op1 : unchecked((op1 << amount) | (op1 >> (64 - amount)));
		return amount == 0 ? op1 : (op1 << amount) | (op1 >> (64 - amount));
	}

	unsigned long long ScalarAluOps::RotateRight64(unsigned long long op1, unsigned long long op2)
	{
		int amount = static_cast<int>(op2 & ShiftAmountMask);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return amount == 0 ? op1 : unchecked((op1 >> amount) | (op1 << (64 - amount)));
		return amount == 0 ? op1 : (op1 >> amount) | (op1 << (64 - amount));
	}

	unsigned long long ScalarAluOps::AndNot64(unsigned long long op1, unsigned long long op2)
	{
		return op1 & ~op2;
	}

	unsigned long long ScalarAluOps::OrNot64(unsigned long long op1, unsigned long long op2)
	{
		return op1 | ~op2;
	}

	unsigned long long ScalarAluOps::Xnor64(unsigned long long op1, unsigned long long op2)
	{
		return ~(op1 ^ op2);
	}

	unsigned long long ScalarAluOps::BitSet64(unsigned long long op1, unsigned long long op2)
	{
		return op1 | (1ULL << static_cast<int>(op2 & ShiftAmountMask));
	}

	unsigned long long ScalarAluOps::BitClear64(unsigned long long op1, unsigned long long op2)
	{
		return op1 & ~(1ULL << static_cast<int>(op2 & ShiftAmountMask));
	}

	unsigned long long ScalarAluOps::BitInvert64(unsigned long long op1, unsigned long long op2)
	{
		return op1 ^ (1ULL << static_cast<int>(op2 & ShiftAmountMask));
	}

	unsigned long long ScalarAluOps::BitExtract64(unsigned long long op1, unsigned long long op2)
	{
		return (op1 >> static_cast<int>(op2 & ShiftAmountMask)) & 1ULL;
	}

	unsigned long long ScalarAluOps::MinSigned64(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((long)op1) <= unchecked((long)op2) ? op1 : op2;
		return static_cast<long long>(op1) <= static_cast<long long>(op2) ? op1 : op2;
	}

	unsigned long long ScalarAluOps::MaxSigned64(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((long)op1) >= unchecked((long)op2) ? op1 : op2;
		return static_cast<long long>(op1) >= static_cast<long long>(op2) ? op1 : op2;
	}

	unsigned long long ScalarAluOps::MinUnsigned64(unsigned long long op1, unsigned long long op2)
	{
		return op1 <= op2 ? op1 : op2;
	}

	unsigned long long ScalarAluOps::MaxUnsigned64(unsigned long long op1, unsigned long long op2)
	{
		return op1 >= op2 ? op1 : op2;
	}

	unsigned long long ScalarAluOps::ReverseBytes64(unsigned long long op1)
	{
		return ((op1 & 0x0000'0000'0000'00FFULL) << 56) | ((op1 & 0x0000'0000'0000'FF00ULL) << 40) | ((op1 & 0x0000'0000'00FF'0000ULL) << 24) | ((op1 & 0x0000'0000'FF00'0000ULL) << 8) | ((op1 & 0x0000'00FF'0000'0000ULL) >> 8) | ((op1 & 0x0000'FF00'0000'0000ULL) >> 24) | ((op1 & 0x00FF'0000'0000'0000ULL) >> 40) | ((op1 & 0xFF00'0000'0000'0000ULL) >> 56);
	}

	unsigned long long ScalarAluOps::ReverseBitsInEachByte64(unsigned long long op1)
	{
		op1 = ((op1 & 0x5555'5555'5555'5555ULL) << 1) | ((op1 >> 1) & 0x5555'5555'5555'5555ULL);
		op1 = ((op1 & 0x3333'3333'3333'3333ULL) << 2) | ((op1 >> 2) & 0x3333'3333'3333'3333ULL);
		op1 = ((op1 & 0x0F0F'0F0F'0F0F'0F0FULL) << 4) | ((op1 >> 4) & 0x0F0F'0F0F'0F0F'0F0FULL);
		return op1;
	}

	unsigned long long ScalarAluOps::ShiftLeftOneAdd(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((op1 << 1) + op2);
		return (op1 << 1) + op2;
	}

	unsigned long long ScalarAluOps::ShiftLeftTwoAdd(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((op1 << 2) + op2);
		return (op1 << 2) + op2;
	}

	unsigned long long ScalarAluOps::ShiftLeftThreeAdd(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((op1 << 3) + op2);
		return (op1 << 3) + op2;
	}

	unsigned long long ScalarAluOps::AddUnsignedWord(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked(ZeroExtendWordFromScalar(op1) + op2);
		return ZeroExtendWordFromScalar(op1) + op2;
	}

	unsigned long long ScalarAluOps::ShiftLeftOneAddUnsignedWord(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ZeroExtendWordFromScalar(op1) << 1) + op2);
		return (ZeroExtendWordFromScalar(op1) << 1) + op2;
	}

	unsigned long long ScalarAluOps::ShiftLeftTwoAddUnsignedWord(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ZeroExtendWordFromScalar(op1) << 2) + op2);
		return (ZeroExtendWordFromScalar(op1) << 2) + op2;
	}

	unsigned long long ScalarAluOps::ShiftLeftThreeAddUnsignedWord(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ZeroExtendWordFromScalar(op1) << 3) + op2);
		return (ZeroExtendWordFromScalar(op1) << 3) + op2;
	}

	unsigned long long ScalarAluOps::ShiftLeftUnsignedWordImmediate(unsigned long long op1, unsigned long long immediate)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked(ZeroExtendWordFromScalar(op1) << (int)(immediate & ShiftAmountMask));
		return ZeroExtendWordFromScalar(op1) << static_cast<int>(immediate & ShiftAmountMask);
	}

	unsigned long long ScalarAluOps::CarryLessMultiplyLow64(unsigned long long op1, unsigned long long op2)
	{
		unsigned long long result = 0ULL;
		unsigned long long multiplicand = op1;
		unsigned long long multiplier = op2;

		for (int bit = 0; bit < 64 && multiplier != 0; bit++)
		{
			if ((multiplier & 1ULL) != 0)
			{
				result ^= multiplicand;
			}

			multiplier >>= 1;
			multiplicand <<= 1;
		}

		return result;
	}

	unsigned long long ScalarAluOps::CarryLessMultiplyHigh64(unsigned long long op1, unsigned long long op2)
	{
		return CarryLessMultiplyWindow64(op1, op2, 64);
	}

	unsigned long long ScalarAluOps::CarryLessMultiplyReverse64(unsigned long long op1, unsigned long long op2)
	{
		return CarryLessMultiplyWindow64(op1, op2, 63);
	}

	unsigned long long ScalarAluOps::CarryLessMultiplyWindow64(unsigned long long op1, unsigned long long op2, int startBit)
	{
		unsigned long long result = 0ULL;

		for (int multiplierBit = 0; multiplierBit < 64; multiplierBit++)
		{
			if (((op2 >> multiplierBit) & 1ULL) == 0)
			{
				continue;
			}

			for (int multiplicandBit = 0; multiplicandBit < 64; multiplicandBit++)
			{
				if (((op1 >> multiplicandBit) & 1ULL) == 0)
				{
					continue;
				}

				int resultBit = multiplierBit + multiplicandBit - startBit;
				if (static_cast<unsigned int>(resultBit) < 64U)
				{
					result ^= 1ULL << resultBit;
				}
			}
		}

		return result;
	}

	unsigned long long ScalarAluOps::AddWordImmediate(unsigned long long op1, long long signedImmediate)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: uint sum = unchecked((uint)op1 + (uint)signedImmediate);
		unsigned int sum = static_cast<unsigned int>(op1) + static_cast<unsigned int>(signedImmediate);
		return SignExtendWord(sum);
	}

	unsigned long long ScalarAluOps::AddWord(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: uint sum = unchecked((uint)op1 + (uint)op2);
		unsigned int sum = static_cast<unsigned int>(op1) + static_cast<unsigned int>(op2);
		return SignExtendWord(sum);
	}

	unsigned long long ScalarAluOps::SubWord(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: uint difference = unchecked((uint)op1 - (uint)op2);
		unsigned int difference = static_cast<unsigned int>(op1) - static_cast<unsigned int>(op2);
		return SignExtendWord(difference);
	}

	unsigned long long ScalarAluOps::MultiplyWord(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: uint product = unchecked((uint)((ulong)(uint)op1 * (uint)op2));
		unsigned int product = static_cast<unsigned int>(static_cast<unsigned long long>(static_cast<unsigned int>(op1)) * static_cast<unsigned int>(op2));
		return SignExtendWord(product);
	}

	unsigned long long ScalarAluOps::DivideWord(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: int dividend = unchecked((int)(uint)op1);
		int dividend = static_cast<int>(static_cast<unsigned int>(op1));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: int divisor = unchecked((int)(uint)op2);
		int divisor = static_cast<int>(static_cast<unsigned int>(op2));

		if (divisor == 0)
		{
			return std::numeric_limits<unsigned long long>::max();
		}

		if (dividend == std::numeric_limits<int>::min() && divisor == -1)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return SignExtendWord(unchecked((uint)int.MinValue));
			return SignExtendWord(static_cast<unsigned int>(std::numeric_limits<int>::min()));
		}

		int quotient = dividend / divisor;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)(long)quotient);
		return static_cast<unsigned long long>(static_cast<long long>(quotient));
	}

	unsigned long long ScalarAluOps::DivideUnsignedWord(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: uint dividend = unchecked((uint)op1);
		unsigned int dividend = static_cast<unsigned int>(op1);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: uint divisor = unchecked((uint)op2);
		unsigned int divisor = static_cast<unsigned int>(op2);

		if (divisor == 0)
		{
			return std::numeric_limits<unsigned long long>::max();
		}

		unsigned int quotient = dividend / divisor;
		return SignExtendWord(quotient);
	}

	unsigned long long ScalarAluOps::RemainderWord(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: int dividend = unchecked((int)(uint)op1);
		int dividend = static_cast<int>(static_cast<unsigned int>(op1));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: int divisor = unchecked((int)(uint)op2);
		int divisor = static_cast<int>(static_cast<unsigned int>(op2));

		if (divisor == 0)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return SignExtendWord(unchecked((uint)dividend));
			return SignExtendWord(static_cast<unsigned int>(dividend));
		}

		if (dividend == std::numeric_limits<int>::min() && divisor == -1)
		{
			return 0;
		}

		int remainder = dividend % divisor;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)(long)remainder);
		return static_cast<unsigned long long>(static_cast<long long>(remainder));
	}

	unsigned long long ScalarAluOps::RemainderUnsignedWord(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: uint dividend = unchecked((uint)op1);
		unsigned int dividend = static_cast<unsigned int>(op1);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: uint divisor = unchecked((uint)op2);
		unsigned int divisor = static_cast<unsigned int>(op2);

		if (divisor == 0)
		{
			return SignExtendWord(dividend);
		}

		unsigned int remainder = dividend % divisor;
		return SignExtendWord(remainder);
	}

	unsigned long long ScalarAluOps::ShiftLeftWord(unsigned long long op1, unsigned long long op2)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: uint shifted = unchecked((uint)op1 << (int)(op2 & WordShiftAmountMask));
		unsigned int shifted = static_cast<unsigned int>(op1) << static_cast<int>(op2 & WordShiftAmountMask);
		return SignExtendWord(shifted);
	}

	unsigned long long ScalarAluOps::ShiftLeftWordImmediate(unsigned long long op1, long long signedImmediate)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: uint shifted = unchecked((uint)op1 << (int)((ulong)signedImmediate & WordShiftAmountMask));
		unsigned int shifted = static_cast<unsigned int>(op1) << static_cast<int>(static_cast<unsigned long long>(signedImmediate) & WordShiftAmountMask);
		return SignExtendWord(shifted);
	}

	unsigned long long ScalarAluOps::ShiftRightLogicalWordImmediate(unsigned long long op1, long long signedImmediate)
	{
		unsigned int shifted = static_cast<unsigned int>(op1) >> static_cast<int>(static_cast<unsigned long long>(signedImmediate) & WordShiftAmountMask);
		return SignExtendWord(shifted);
	}

	unsigned long long ScalarAluOps::ShiftRightArithmeticWordImmediate(unsigned long long op1, long long signedImmediate)
	{
		int shifted = static_cast<int>(static_cast<unsigned int>(op1)) >> static_cast<int>(static_cast<unsigned long long>(signedImmediate) & WordShiftAmountMask);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)(long)shifted);
		return static_cast<unsigned long long>(static_cast<long long>(shifted));
	}

	unsigned long long ScalarAluOps::ShiftRightLogicalWord(unsigned long long op1, unsigned long long op2)
	{
		unsigned int shifted = static_cast<unsigned int>(op1) >> static_cast<int>(op2 & WordShiftAmountMask);
		return SignExtendWord(shifted);
	}

	unsigned long long ScalarAluOps::ShiftRightArithmeticWord(unsigned long long op1, unsigned long long op2)
	{
		int shifted = static_cast<int>(static_cast<unsigned int>(op1)) >> static_cast<int>(op2 & WordShiftAmountMask);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)(long)shifted);
		return static_cast<unsigned long long>(static_cast<long long>(shifted));
	}

	unsigned long long ScalarAluOps::Compute(unsigned int opCode, unsigned long long op1, unsigned long long op2, unsigned long long immediate, unsigned long long instructionPc)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: long signedImmediate = unchecked((long)immediate);
		long long signedImmediate = static_cast<long long>(immediate);

		switch (static_cast<Processor::CPU_Core::InstructionsEnum>(opCode))
		{
			case Processor::CPU_Core::InstructionsEnum::ADD:
				return op1 + op2;
			case Processor::CPU_Core::InstructionsEnum::ADDW:
				return AddWord(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::SUBW:
				return SubWord(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::SLLW:
				return ShiftLeftWord(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::SRLW:
				return ShiftRightLogicalWord(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::SRAW:
				return ShiftRightArithmeticWord(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::SUB:
				return op1 - op2;
			case Processor::CPU_Core::InstructionsEnum::MUL:
				return op1 * op2;
			case Processor::CPU_Core::InstructionsEnum::MULW:
				return MultiplyWord(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::DIVW:
				return DivideWord(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::DIVUW:
				return DivideUnsignedWord(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::REMW:
				return RemainderWord(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::REMUW:
				return RemainderUnsignedWord(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::SEXT_W:
				return SignExtendWordFromScalar(op1);
			case Processor::CPU_Core::InstructionsEnum::ZEXT_W:
				return ZeroExtendWordFromScalar(op1);
			case Processor::CPU_Core::InstructionsEnum::DIV:
				return op2 != 0 ? op1 / op2 : 0;


			case static_cast<Processor::CPU_Core::InstructionsEnum>(45):
				return static_cast<unsigned long long>(std::sqrt(static_cast<double>(op1)));

			case Processor::CPU_Core::InstructionsEnum::MULH:
				return YAKSys_Hybrid_CPU::Execution::Detail::MultiplyHighSigned64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::MULHU:
				return YAKSys_Hybrid_CPU::Execution::Detail::MultiplyHighUnsigned64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::MULHSU:
				return YAKSys_Hybrid_CPU::Execution::Detail::MultiplyHighSignedUnsigned64(op1, op2);

			case Processor::CPU_Core::InstructionsEnum::DIVU:
				return op2 != 0 ? op1 / op2 : std::numeric_limits<unsigned long long>::max();
			case Processor::CPU_Core::InstructionsEnum::REM:
				return op2 != 0 ? static_cast<unsigned long long>(static_cast<long long>(op1) % static_cast<long long>(op2)) : op1;
			case Processor::CPU_Core::InstructionsEnum::REMU:
				return op2 != 0 ? op1 % op2 : op1;

			case Processor::CPU_Core::InstructionsEnum::AND:
				return op1 & op2;
			case Processor::CPU_Core::InstructionsEnum::OR:
				return op1 | op2;
			case Processor::CPU_Core::InstructionsEnum::XOR:
				return op1 ^ op2;
			case Processor::CPU_Core::InstructionsEnum::CZERO_EQZ:
				return ConditionalZeroIfEqualZero(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::CZERO_NEZ:
				return ConditionalZeroIfNotEqualZero(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::CLZ:
				return CountLeadingZeros64(op1);
			case Processor::CPU_Core::InstructionsEnum::CTZ:
				return CountTrailingZeros64(op1);
			case Processor::CPU_Core::InstructionsEnum::CPOP:
				return CountPopulation64(op1);
			case Processor::CPU_Core::InstructionsEnum::SEXT_B:
				return SignExtendByteFromScalar(op1);
			case Processor::CPU_Core::InstructionsEnum::SEXT_H:
				return SignExtendHalfFromScalar(op1);
			case Processor::CPU_Core::InstructionsEnum::ZEXT_H:
				return ZeroExtendHalfFromScalar(op1);
			case Processor::CPU_Core::InstructionsEnum::ROL:
				return RotateLeft64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::ROR:
				return RotateRight64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::ROLI:
				return RotateLeft64(op1, immediate);
			case Processor::CPU_Core::InstructionsEnum::RORI:
				return RotateRight64(op1, immediate);
			case Processor::CPU_Core::InstructionsEnum::BSET:
				return BitSet64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::BCLR:
				return BitClear64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::BINV:
				return BitInvert64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::BEXT:
				return BitExtract64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::BSETI:
				return BitSet64(op1, immediate);
			case Processor::CPU_Core::InstructionsEnum::BCLRI:
				return BitClear64(op1, immediate);
			case Processor::CPU_Core::InstructionsEnum::BINVI:
				return BitInvert64(op1, immediate);
			case Processor::CPU_Core::InstructionsEnum::BEXTI:
				return BitExtract64(op1, immediate);
			case Processor::CPU_Core::InstructionsEnum::ANDN:
				return AndNot64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::ORN:
				return OrNot64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::XNOR:
				return Xnor64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::MIN:
				return MinSigned64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::MAX:
				return MaxSigned64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::MINU:
				return MinUnsigned64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::MAXU:
				return MaxUnsigned64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::REV8:
				return ReverseBytes64(op1);
			case Processor::CPU_Core::InstructionsEnum::BREV8:
				return ReverseBitsInEachByte64(op1);
			case Processor::CPU_Core::InstructionsEnum::SH1ADD:
				return ShiftLeftOneAdd(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::SH2ADD:
				return ShiftLeftTwoAdd(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::SH3ADD:
				return ShiftLeftThreeAdd(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::ADD_UW:
				return AddUnsignedWord(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::SH1ADD_UW:
				return ShiftLeftOneAddUnsignedWord(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::SH2ADD_UW:
				return ShiftLeftTwoAddUnsignedWord(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::SH3ADD_UW:
				return ShiftLeftThreeAddUnsignedWord(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::SLLI_UW:
				return ShiftLeftUnsignedWordImmediate(op1, immediate);
			case Processor::CPU_Core::InstructionsEnum::CLMUL:
				return CarryLessMultiplyLow64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::CLMULH:
				return CarryLessMultiplyHigh64(op1, op2);
			case Processor::CPU_Core::InstructionsEnum::CLMULR:
				return CarryLessMultiplyReverse64(op1, op2);

			case static_cast<Processor::CPU_Core::InstructionsEnum>(52):
				return ~op1;

			case Processor::CPU_Core::InstructionsEnum::SLL:
				return op1 << static_cast<int>(immediate & ShiftAmountMask);
			case Processor::CPU_Core::InstructionsEnum::SRL:
				return op1 >> static_cast<int>(immediate & ShiftAmountMask);
			case Processor::CPU_Core::InstructionsEnum::SRA:
				return static_cast<unsigned long long>(static_cast<long long>(op1) >> static_cast<int>(op2 & ShiftAmountMask));

			case Processor::CPU_Core::InstructionsEnum::SLT:
				return static_cast<long long>(op1) < static_cast<long long>(op2) ? 1ULL : 0ULL;
			case Processor::CPU_Core::InstructionsEnum::SLTU:
				return op1 < op2 ? 1ULL : 0ULL;

			case Processor::CPU_Core::InstructionsEnum::ADDI:
				return op1 + immediate;
			case Processor::CPU_Core::InstructionsEnum::ADDIW:
				return AddWordImmediate(op1, signedImmediate);
			case Processor::CPU_Core::InstructionsEnum::SLLIW:
				return ShiftLeftWordImmediate(op1, signedImmediate);
			case Processor::CPU_Core::InstructionsEnum::SRLIW:
				return ShiftRightLogicalWordImmediate(op1, signedImmediate);
			case Processor::CPU_Core::InstructionsEnum::SRAIW:
				return ShiftRightArithmeticWordImmediate(op1, signedImmediate);
			case Processor::CPU_Core::InstructionsEnum::ANDI:
				return op1 & immediate;
			case Processor::CPU_Core::InstructionsEnum::ORI:
				return op1 | immediate;
			case Processor::CPU_Core::InstructionsEnum::XORI:
				return op1 ^ immediate;
			case Processor::CPU_Core::InstructionsEnum::SLLI:
				return op1 << static_cast<int>(immediate & ShiftAmountMask);
			case Processor::CPU_Core::InstructionsEnum::SRLI:
				return op1 >> static_cast<int>(immediate & ShiftAmountMask);
			case Processor::CPU_Core::InstructionsEnum::SRAI:
				return static_cast<unsigned long long>(static_cast<long long>(op1) >> static_cast<int>(immediate & ShiftAmountMask));
			case Processor::CPU_Core::InstructionsEnum::SLTI:
				return static_cast<long long>(op1) < signedImmediate ? 1ULL : 0ULL;
			case Processor::CPU_Core::InstructionsEnum::SLTIU:
				return op1 < immediate ? 1ULL : 0ULL;
			case Processor::CPU_Core::InstructionsEnum::LUI:
				return static_cast<unsigned long long>(signedImmediate << 12);
			case Processor::CPU_Core::InstructionsEnum::AUIPC:
				return instructionPc + static_cast<unsigned long long>(signedImmediate << 12);



			default:
				return 0;
		}
	}
}
