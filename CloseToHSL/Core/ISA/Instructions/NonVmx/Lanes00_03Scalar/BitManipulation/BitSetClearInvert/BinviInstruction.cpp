#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.BitSetClearInvert.BinviInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitSetClearInvert
{

	unsigned short BinviInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::BINVI);
	}

	bool BinviInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool BinviInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long BinviInstruction::Execute(unsigned long long source, unsigned long long immediate6)
	{
		return source ^ (1ULL << static_cast<int>(immediate6 & ImmediateMask));
	}

	unsigned long long BinviInstruction::EvaluateXLen64(unsigned long long source, unsigned long long immediate6)
	{
		return Execute(source, immediate6);
	}

	bool BinviInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<BinviInstruction::BitfieldImmediateGoldenVector> BinviInstruction::GetLocalGoldenVectors()
	{
		return std::vector<BitfieldImmediateGoldenVector> { BitfieldImmediateGoldenVector(0x0000'0000'0000'0000ULL, 1, 0x0000'0000'0000'0002ULL), BitfieldImmediateGoldenVector(0x0000'0000'0000'0002ULL, 1, 0x0000'0000'0000'0000ULL), BitfieldImmediateGoldenVector(0x0000'0000'0000'0000ULL, 63, 0x8000'0000'0000'0000ULL), BitfieldImmediateGoldenVector(0x0000'0000'0000'0010ULL, 4, 0x0000'0000'0000'0000ULL)};
	}

	BinviInstruction::BitfieldImmediateGoldenVector::BitfieldImmediateGoldenVector(unsigned long long source, unsigned short immediate6, unsigned long long expected)
		: Source(source), Immediate6(immediate6), Expected(expected)
	{
	}

	unsigned long long BinviInstruction::BitfieldImmediateGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned short BinviInstruction::BitfieldImmediateGoldenVector::getImmediate6() const
	{
		return Immediate6;
	}

	unsigned long long BinviInstruction::BitfieldImmediateGoldenVector::getExpected() const
	{
		return Expected;
	}
}
