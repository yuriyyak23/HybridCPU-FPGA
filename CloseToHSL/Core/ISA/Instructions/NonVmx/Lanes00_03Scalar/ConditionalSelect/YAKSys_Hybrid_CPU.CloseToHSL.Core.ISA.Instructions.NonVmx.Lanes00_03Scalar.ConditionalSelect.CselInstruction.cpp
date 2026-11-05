#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.ConditionalSelect.CselInstruction.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::ConditionalSelect
{

	unsigned long long CselInstruction::EvaluateXLen64(unsigned long long trueValue, unsigned long long falseValue, unsigned long long condition)
	{
		return condition != 0 ? trueValue : falseValue;
	}

	bool CselInstruction::RetireWritesDestination(int destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<CselInstruction::ScalarSelectGoldenVector> CselInstruction::GetLocalGoldenVectors()
	{
		return std::vector<ScalarSelectGoldenVector> { {0x1111'1111'1111'1111ULL, 0x2222'2222'2222'2222ULL, 0x0000'0000'0000'0000ULL, 0x2222'2222'2222'2222ULL}, {0x1111'1111'1111'1111ULL, 0x2222'2222'2222'2222ULL, 0x0000'0000'0000'0001ULL, 0x1111'1111'1111'1111ULL}, {0x0000'0000'0000'0000ULL, 0xFFFF'FFFF'FFFF'FFFFULL, 0x8000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 0x0000'0000'0000'0000ULL, 0xFFFF'FFFF'FFFF'FFFFULL, 0xFFFF'FFFF'FFFF'FFFFULL}, };
	}

	CselInstruction::ScalarSelectGoldenVector::ScalarSelectGoldenVector(unsigned long long trueValue, unsigned long long falseValue, unsigned long long condition, unsigned long long result)
		: TrueValue(trueValue), FalseValue(falseValue), Condition(condition), Result(result)
	{
	}

	unsigned long long CselInstruction::ScalarSelectGoldenVector::getTrueValue() const
	{
		return TrueValue;
	}

	unsigned long long CselInstruction::ScalarSelectGoldenVector::getFalseValue() const
	{
		return FalseValue;
	}

	unsigned long long CselInstruction::ScalarSelectGoldenVector::getCondition() const
	{
		return Condition;
	}

	unsigned long long CselInstruction::ScalarSelectGoldenVector::getResult() const
	{
		return Result;
	}
}
