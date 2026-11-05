#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.BitSetClearInvert.BclriInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitSetClearInvert
{

	unsigned short BclriInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::BCLRI);
	}

	bool BclriInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool BclriInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long BclriInstruction::Execute(unsigned long long source, unsigned long long immediate6)
	{
		return source & ~(1ULL << static_cast<int>(immediate6 & ImmediateMask));
	}

	unsigned long long BclriInstruction::EvaluateXLen64(unsigned long long source, unsigned long long immediate6)
	{
		return Execute(source, immediate6);
	}

	bool BclriInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<BclriInstruction::BitfieldImmediateGoldenVector> BclriInstruction::GetLocalGoldenVectors()
	{
		return std::vector<BitfieldImmediateGoldenVector> { BitfieldImmediateGoldenVector(0xFFFF'FFFF'FFFF'FFFFULL, 0, 0xFFFF'FFFF'FFFF'FFFEULL), BitfieldImmediateGoldenVector(0xFFFF'FFFF'FFFF'FFFFULL, 63, 0x7FFF'FFFF'FFFF'FFFFULL), BitfieldImmediateGoldenVector(0x0000'0000'0000'0010ULL, 4, 0x0000'0000'0000'0000ULL), BitfieldImmediateGoldenVector(0x0123'4567'89AB'CDFFULL, 4, 0x0123'4567'89AB'CDEFULL)};
	}

	BclriInstruction::BitfieldImmediateGoldenVector::BitfieldImmediateGoldenVector(unsigned long long source, unsigned short immediate6, unsigned long long expected)
		: Source(source), Immediate6(immediate6), Expected(expected)
	{
	}

	unsigned long long BclriInstruction::BitfieldImmediateGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned short BclriInstruction::BitfieldImmediateGoldenVector::getImmediate6() const
	{
		return Immediate6;
	}

	unsigned long long BclriInstruction::BitfieldImmediateGoldenVector::getExpected() const
	{
		return Expected;
	}
}
