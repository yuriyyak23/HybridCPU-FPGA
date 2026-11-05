#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.ZeroingSelect.CzeroNezInstruction.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::ZeroingSelect
{

	unsigned short CzeroNezInstruction::getOpcode()
	{
		return static_cast<unsigned short>(YAKSys_Hybrid_CPU::Processor::CPU_Core::InstructionsEnum::CZERO_NEZ);
	}

	bool CzeroNezInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool CzeroNezInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long CzeroNezInstruction::Execute(unsigned long long value, unsigned long long condition)
	{
		return EvaluateXLen64(value, condition);
	}

	unsigned long long CzeroNezInstruction::EvaluateXLen64(unsigned long long value, unsigned long long condition)
	{
		return condition != 0 ? 0ULL : value;
	}

	bool CzeroNezInstruction::RetireWritesDestination(int destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<CzeroNezInstruction::ZeroingSelectGoldenVector> CzeroNezInstruction::GetLocalGoldenVectors()
	{
		return std::vector<ZeroingSelectGoldenVector> { {0x1111'1111'1111'1111ULL, 0x0000'0000'0000'0000ULL, 0x1111'1111'1111'1111ULL}, {0x1111'1111'1111'1111ULL, 0x0000'0000'0000'0001ULL, 0x0000'0000'0000'0000ULL}, {0xFFFF'FFFF'FFFF'FFFFULL, 0x8000'0000'0000'0000ULL, 0x0000'0000'0000'0000ULL}, {0x0000'0000'0000'0000ULL, 0xFFFF'FFFF'FFFF'FFFFULL, 0x0000'0000'0000'0000ULL}, };
	}

	CzeroNezInstruction::ZeroingSelectGoldenVector::ZeroingSelectGoldenVector(unsigned long long value, unsigned long long condition, unsigned long long result)
		: Value(value), Condition(condition), Result(result)
	{
	}

	unsigned long long CzeroNezInstruction::ZeroingSelectGoldenVector::getValue() const
	{
		return Value;
	}

	unsigned long long CzeroNezInstruction::ZeroingSelectGoldenVector::getCondition() const
	{
		return Condition;
	}

	unsigned long long CzeroNezInstruction::ZeroingSelectGoldenVector::getResult() const
	{
		return Result;
	}
}
