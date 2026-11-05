#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.BitSetClearInvert.BinvInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitSetClearInvert
{

	unsigned short BinvInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::BINV);
	}

	bool BinvInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool BinvInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long BinvInstruction::Execute(unsigned long long source, unsigned long long index)
	{
		return source ^ (1ULL << static_cast<int>(index & IndexMask));
	}

	unsigned long long BinvInstruction::EvaluateXLen64(unsigned long long source, unsigned long long index)
	{
		return Execute(source, index);
	}

	bool BinvInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<BinvInstruction::BitfieldGoldenVector> BinvInstruction::GetLocalGoldenVectors()
	{
		return std::vector<BitfieldGoldenVector> { BitfieldGoldenVector(0x0000'0000'0000'0000ULL, 1, 0x0000'0000'0000'0002ULL), BitfieldGoldenVector(0x0000'0000'0000'0002ULL, 1, 0x0000'0000'0000'0000ULL), BitfieldGoldenVector(0x0000'0000'0000'0000ULL, 63, 0x8000'0000'0000'0000ULL), BitfieldGoldenVector(0x0000'0000'0000'0010ULL, 68, 0x0000'0000'0000'0000ULL)};
	}

	BinvInstruction::BitfieldGoldenVector::BitfieldGoldenVector(unsigned long long source, unsigned long long index, unsigned long long expected)
		: Source(source), Index(index), Expected(expected)
	{
	}

	unsigned long long BinvInstruction::BitfieldGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned long long BinvInstruction::BitfieldGoldenVector::getIndex() const
	{
		return Index;
	}

	unsigned long long BinvInstruction::BitfieldGoldenVector::getExpected() const
	{
		return Expected;
	}
}
