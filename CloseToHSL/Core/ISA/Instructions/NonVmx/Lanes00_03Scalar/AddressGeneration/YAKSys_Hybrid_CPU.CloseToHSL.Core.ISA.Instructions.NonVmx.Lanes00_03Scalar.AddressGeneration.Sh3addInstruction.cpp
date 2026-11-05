#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.AddressGeneration.Sh3addInstruction.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::AddressGeneration
{

	unsigned short Sh3addInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::SH3ADD);
	}

	bool Sh3addInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool Sh3addInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long Sh3addInstruction::Execute(unsigned long long source, unsigned long long addend)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((source << 3) + addend);
		return (source << 3) + addend;
	}

	unsigned long long Sh3addInstruction::EvaluateXLen64(unsigned long long source, unsigned long long addend)
	{
		return Execute(source, addend);
	}

	bool Sh3addInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<Sh3addInstruction::AddressGenerationGoldenVector> Sh3addInstruction::GetLocalGoldenVectors()
	{
		return std::vector<AddressGenerationGoldenVector> { AddressGenerationGoldenVector(0ULL, 0ULL, 0ULL), AddressGenerationGoldenVector(1ULL, 2ULL, 10ULL), AddressGenerationGoldenVector(0x2000'0000'0000'0000ULL, 5ULL, 5ULL), AddressGenerationGoldenVector(std::numeric_limits<unsigned long long>::max(), 9ULL, 1ULL)};
	}

	Sh3addInstruction::AddressGenerationGoldenVector::AddressGenerationGoldenVector(unsigned long long source, unsigned long long addend, unsigned long long expected)
		: Source(source), Addend(addend), Expected(expected)
	{
	}

	unsigned long long Sh3addInstruction::AddressGenerationGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned long long Sh3addInstruction::AddressGenerationGoldenVector::getAddend() const
	{
		return Addend;
	}

	unsigned long long Sh3addInstruction::AddressGenerationGoldenVector::getExpected() const
	{
		return Expected;
	}
}
