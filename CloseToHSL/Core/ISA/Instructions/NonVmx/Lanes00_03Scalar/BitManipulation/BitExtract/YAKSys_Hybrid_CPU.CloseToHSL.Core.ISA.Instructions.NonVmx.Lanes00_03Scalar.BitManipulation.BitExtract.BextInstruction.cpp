#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.BitExtract.BextInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitExtract
{

	unsigned short BextInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::BEXT);
	}

	bool BextInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool BextInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long BextInstruction::Execute(unsigned long long source, unsigned long long index)
	{
		return (source >> static_cast<int>(index & IndexMask)) & 1ULL;
	}

	unsigned long long BextInstruction::EvaluateXLen64(unsigned long long source, unsigned long long index)
	{
		return Execute(source, index);
	}

	bool BextInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<BextInstruction::BitfieldGoldenVector> BextInstruction::GetLocalGoldenVectors()
	{
		return std::vector<BitfieldGoldenVector> { BitfieldGoldenVector(0x0000'0000'0000'0001ULL, 0, 1ULL), BitfieldGoldenVector(0x0000'0000'0000'0001ULL, 1, 0ULL), BitfieldGoldenVector(0x8000'0000'0000'0000ULL, 63, 1ULL), BitfieldGoldenVector(0x0000'0000'0000'0010ULL, 68, 1ULL), BitfieldGoldenVector(0x0000'0000'0000'0000ULL, 68, 0ULL)};
	}

	BextInstruction::BitfieldGoldenVector::BitfieldGoldenVector(unsigned long long source, unsigned long long index, unsigned long long expected)
		: Source(source), Index(index), Expected(expected)
	{
	}

	unsigned long long BextInstruction::BitfieldGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned long long BextInstruction::BitfieldGoldenVector::getIndex() const
	{
		return Index;
	}

	unsigned long long BextInstruction::BitfieldGoldenVector::getExpected() const
	{
		return Expected;
	}
}
