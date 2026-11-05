#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.AddressGeneration.Sh2addInstruction.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::AddressGeneration
{

	unsigned short Sh2addInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::SH2ADD);
	}

	bool Sh2addInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool Sh2addInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long Sh2addInstruction::Execute(unsigned long long source, unsigned long long addend)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((source << 2) + addend);
		return (source << 2) + addend;
	}

	unsigned long long Sh2addInstruction::EvaluateXLen64(unsigned long long source, unsigned long long addend)
	{
		return Execute(source, addend);
	}

	bool Sh2addInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<Sh2addInstruction::AddressGenerationGoldenVector> Sh2addInstruction::GetLocalGoldenVectors()
	{
		return std::vector<AddressGenerationGoldenVector> { AddressGenerationGoldenVector(0ULL, 0ULL, 0ULL), AddressGenerationGoldenVector(1ULL, 2ULL, 6ULL), AddressGenerationGoldenVector(0x4000'0000'0000'0000ULL, 5ULL, 5ULL), AddressGenerationGoldenVector(std::numeric_limits<unsigned long long>::max(), 3ULL, std::numeric_limits<unsigned long long>::max())};
	}

	Sh2addInstruction::AddressGenerationGoldenVector::AddressGenerationGoldenVector(unsigned long long source, unsigned long long addend, unsigned long long expected)
		: Source(source), Addend(addend), Expected(expected)
	{
	}

	unsigned long long Sh2addInstruction::AddressGenerationGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned long long Sh2addInstruction::AddressGenerationGoldenVector::getAddend() const
	{
		return Addend;
	}

	unsigned long long Sh2addInstruction::AddressGenerationGoldenVector::getExpected() const
	{
		return Expected;
	}
}
