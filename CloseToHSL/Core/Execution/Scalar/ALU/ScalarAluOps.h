#pragma once

#include <cmath>
#include <limits>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Canonical static ALU computation helper.
	///
	/// Purpose: single source of truth for all scalar integer / FP-light operations.
	/// Both <see cref="Pipeline.MicroOps.ScalarALUMicroOp"/> (MicroOp-path) and
	/// the pipeline execute-stage scalar path delegate here directly, eliminating
	/// the duplicated switch-case that previously existed in both places.
	/// </summary>
	class ScalarAluOps final
	{
		/// <summary>
		/// Mask applied to shift-amount operands to keep them in the valid
		/// 64-bit range [0, 63] (6 bits).
		/// </summary>
	private:
		static constexpr int ShiftAmountMask = 0x3F;
		static constexpr int WordShiftAmountMask = 0x1F;

	public:
		static unsigned long long SignExtendWord(unsigned int word);

		static unsigned long long SignExtendWordFromScalar(unsigned long long op1);

		static unsigned long long ZeroExtendWordFromScalar(unsigned long long op1);

		static unsigned long long SignExtendByteFromScalar(unsigned long long op1);

		static unsigned long long SignExtendHalfFromScalar(unsigned long long op1);

		static unsigned long long ZeroExtendHalfFromScalar(unsigned long long op1);

		static unsigned long long ConditionalZeroIfEqualZero(unsigned long long op1, unsigned long long op2);

		static unsigned long long ConditionalZeroIfNotEqualZero(unsigned long long op1, unsigned long long op2);

		static unsigned long long CountLeadingZeros64(unsigned long long op1);

		static unsigned long long CountTrailingZeros64(unsigned long long op1);

		static unsigned long long CountPopulation64(unsigned long long op1);

		static unsigned long long RotateLeft64(unsigned long long op1, unsigned long long op2);

		static unsigned long long RotateRight64(unsigned long long op1, unsigned long long op2);

		static unsigned long long AndNot64(unsigned long long op1, unsigned long long op2);

		static unsigned long long OrNot64(unsigned long long op1, unsigned long long op2);

		static unsigned long long Xnor64(unsigned long long op1, unsigned long long op2);

		static unsigned long long BitSet64(unsigned long long op1, unsigned long long op2);

		static unsigned long long BitClear64(unsigned long long op1, unsigned long long op2);

		static unsigned long long BitInvert64(unsigned long long op1, unsigned long long op2);

		static unsigned long long BitExtract64(unsigned long long op1, unsigned long long op2);

		static unsigned long long MinSigned64(unsigned long long op1, unsigned long long op2);

		static unsigned long long MaxSigned64(unsigned long long op1, unsigned long long op2);

		static unsigned long long MinUnsigned64(unsigned long long op1, unsigned long long op2);

		static unsigned long long MaxUnsigned64(unsigned long long op1, unsigned long long op2);

		static unsigned long long ReverseBytes64(unsigned long long op1);

		static unsigned long long ReverseBitsInEachByte64(unsigned long long op1);

		static unsigned long long ShiftLeftOneAdd(unsigned long long op1, unsigned long long op2);

		static unsigned long long ShiftLeftTwoAdd(unsigned long long op1, unsigned long long op2);

		static unsigned long long ShiftLeftThreeAdd(unsigned long long op1, unsigned long long op2);

		static unsigned long long AddUnsignedWord(unsigned long long op1, unsigned long long op2);

		static unsigned long long ShiftLeftOneAddUnsignedWord(unsigned long long op1, unsigned long long op2);

		static unsigned long long ShiftLeftTwoAddUnsignedWord(unsigned long long op1, unsigned long long op2);

		static unsigned long long ShiftLeftThreeAddUnsignedWord(unsigned long long op1, unsigned long long op2);

		static unsigned long long ShiftLeftUnsignedWordImmediate(unsigned long long op1, unsigned long long immediate);

		static unsigned long long CarryLessMultiplyLow64(unsigned long long op1, unsigned long long op2);

		static unsigned long long CarryLessMultiplyHigh64(unsigned long long op1, unsigned long long op2);

		static unsigned long long CarryLessMultiplyReverse64(unsigned long long op1, unsigned long long op2);

	private:
		static unsigned long long CarryLessMultiplyWindow64(unsigned long long op1, unsigned long long op2, int startBit);

	public:
		static unsigned long long AddWordImmediate(unsigned long long op1, long long signedImmediate);

		static unsigned long long AddWord(unsigned long long op1, unsigned long long op2);

		static unsigned long long SubWord(unsigned long long op1, unsigned long long op2);

		static unsigned long long MultiplyWord(unsigned long long op1, unsigned long long op2);

		static unsigned long long DivideWord(unsigned long long op1, unsigned long long op2);

		static unsigned long long DivideUnsignedWord(unsigned long long op1, unsigned long long op2);

		static unsigned long long RemainderWord(unsigned long long op1, unsigned long long op2);

		static unsigned long long RemainderUnsignedWord(unsigned long long op1, unsigned long long op2);

		static unsigned long long ShiftLeftWord(unsigned long long op1, unsigned long long op2);

		static unsigned long long ShiftLeftWordImmediate(unsigned long long op1, long long signedImmediate);

		static unsigned long long ShiftRightLogicalWordImmediate(unsigned long long op1, long long signedImmediate);

		static unsigned long long ShiftRightArithmeticWordImmediate(unsigned long long op1, long long signedImmediate);

		static unsigned long long ShiftRightLogicalWord(unsigned long long op1, unsigned long long op2);

		static unsigned long long ShiftRightArithmeticWord(unsigned long long op1, unsigned long long op2);

		/// <summary>
		/// Compute the result of a scalar ALU operation.
		/// </summary>
		/// <param name="opCode">Instruction opcode.</param>
		/// <param name="op1">First (source-1) register operand.</param>
		/// <param name="op2">Second operand or register-register source-2 operand.</param>
		/// <param name="immediate">Canonical runtime immediate payload.</param>
		/// <param name="instructionPc">Instruction PC for PC-relative forms such as AUIPC.</param>
		/// <returns>64-bit unsigned result.</returns>
		static unsigned long long Compute(unsigned int opCode, unsigned long long op1, unsigned long long op2, unsigned long long immediate, unsigned long long instructionPc = 0);
	};
}
