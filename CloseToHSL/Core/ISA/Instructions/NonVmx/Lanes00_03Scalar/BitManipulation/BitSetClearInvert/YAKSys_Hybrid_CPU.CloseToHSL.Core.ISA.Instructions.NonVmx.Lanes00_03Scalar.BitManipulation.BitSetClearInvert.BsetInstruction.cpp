#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.BitSetClearInvert.BsetInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitSetClearInvert
{

	unsigned short BsetInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::BSET);
	}

	bool BsetInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool BsetInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long BsetInstruction::Execute(unsigned long long source, unsigned long long index)
	{
		return source | (1ULL << static_cast<int>(index & IndexMask));
	}

	unsigned long long BsetInstruction::EvaluateXLen64(unsigned long long source, unsigned long long index)
	{
		return Execute(source, index);
	}

	bool BsetInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<BsetInstruction::BitfieldGoldenVector> BsetInstruction::GetLocalGoldenVectors()
	{
		return std::vector<BitfieldGoldenVector> { BitfieldGoldenVector(0x0000'0000'0000'0000ULL, 0, 0x0000'0000'0000'0001ULL), BitfieldGoldenVector(0x0000'0000'0000'0000ULL, 63, 0x8000'0000'0000'0000ULL), BitfieldGoldenVector(0x0000'0000'0000'0010ULL, 4, 0x0000'0000'0000'0010ULL), BitfieldGoldenVector(0x0123'4567'89AB'CDEFULL, 68, 0x0123'4567'89AB'CDFFULL)};
	}

	BsetInstruction::BitfieldGoldenVector::BitfieldGoldenVector(unsigned long long source, unsigned long long index, unsigned long long expected)
		: Source(source), Index(index), Expected(expected)
	{
	}

	unsigned long long BsetInstruction::BitfieldGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned long long BsetInstruction::BitfieldGoldenVector::getIndex() const
	{
		return Index;
	}

	unsigned long long BsetInstruction::BitfieldGoldenVector::getExpected() const
	{
		return Expected;
	}
}
