#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.Rotates.RoliInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::Rotates
{

	unsigned short RoliInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::ROLI);
	}

	bool RoliInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool RoliInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long RoliInstruction::Execute(unsigned long long source, unsigned long long immediate6)
	{
		int amount = static_cast<int>(immediate6 & ImmediateMask);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return amount == 0 ? source : unchecked((source << amount) | (source >> (XLen - amount)));
		return amount == 0 ? source : (source << amount) | (source >> (XLen - amount));
	}

	unsigned long long RoliInstruction::EvaluateXLen64(unsigned long long source, unsigned long long immediate6)
	{
		return Execute(source, immediate6);
	}

	bool RoliInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<RoliInstruction::RotateImmediateGoldenVector> RoliInstruction::GetLocalGoldenVectors()
	{
		return std::vector<RotateImmediateGoldenVector> { RotateImmediateGoldenVector(0x0000'0000'0000'0001ULL, 0, 0x0000'0000'0000'0001ULL), RotateImmediateGoldenVector(0x0000'0000'0000'0001ULL, 1, 0x0000'0000'0000'0002ULL), RotateImmediateGoldenVector(0x8000'0000'0000'0000ULL, 1, 0x0000'0000'0000'0001ULL), RotateImmediateGoldenVector(0x0123'4567'89AB'CDEFULL, 4, 0x1234'5678'9ABC'DEF0ULL), RotateImmediateGoldenVector(0x0000'0000'0000'0001ULL, 63, 0x8000'0000'0000'0000ULL)};
	}

	RoliInstruction::RotateImmediateGoldenVector::RotateImmediateGoldenVector(unsigned long long source, unsigned short immediate6, unsigned long long expected)
		: Source(source), Immediate6(immediate6), Expected(expected)
	{
	}

	unsigned long long RoliInstruction::RotateImmediateGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned short RoliInstruction::RotateImmediateGoldenVector::getImmediate6() const
	{
		return Immediate6;
	}

	unsigned long long RoliInstruction::RotateImmediateGoldenVector::getExpected() const
	{
		return Expected;
	}
}
