#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.BitSetClearInvert.BclrInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitSetClearInvert
{

	unsigned short BclrInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::BCLR);
	}

	bool BclrInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool BclrInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long BclrInstruction::Execute(unsigned long long source, unsigned long long index)
	{
		return source & ~(1ULL << static_cast<int>(index & IndexMask));
	}

	unsigned long long BclrInstruction::EvaluateXLen64(unsigned long long source, unsigned long long index)
	{
		return Execute(source, index);
	}

	bool BclrInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<BclrInstruction::BitfieldGoldenVector> BclrInstruction::GetLocalGoldenVectors()
	{
		return std::vector<BitfieldGoldenVector> { BitfieldGoldenVector(0xFFFF'FFFF'FFFF'FFFFULL, 0, 0xFFFF'FFFF'FFFF'FFFEULL), BitfieldGoldenVector(0xFFFF'FFFF'FFFF'FFFFULL, 63, 0x7FFF'FFFF'FFFF'FFFFULL), BitfieldGoldenVector(0x0000'0000'0000'0010ULL, 4, 0x0000'0000'0000'0000ULL), BitfieldGoldenVector(0x0123'4567'89AB'CDFFULL, 68, 0x0123'4567'89AB'CDEFULL)};
	}

	BclrInstruction::BitfieldGoldenVector::BitfieldGoldenVector(unsigned long long source, unsigned long long index, unsigned long long expected)
		: Source(source), Index(index), Expected(expected)
	{
	}

	unsigned long long BclrInstruction::BitfieldGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned long long BclrInstruction::BitfieldGoldenVector::getIndex() const
	{
		return Index;
	}

	unsigned long long BclrInstruction::BitfieldGoldenVector::getExpected() const
	{
		return Expected;
	}
}
