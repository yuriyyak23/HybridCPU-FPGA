#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.AddressGeneration.AddUwInstruction.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::AddressGeneration
{

	unsigned short AddUwInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::ADD_UW);
	}

	bool AddUwInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool AddUwInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long AddUwInstruction::Execute(unsigned long long source, unsigned long long addend)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((uint)source + addend);
		return static_cast<unsigned int>(source) + addend;
	}

	unsigned long long AddUwInstruction::EvaluateXLen64(unsigned long long source, unsigned long long addend)
	{
		return Execute(source, addend);
	}

	bool AddUwInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<AddUwInstruction::AddressGenerationGoldenVector> AddUwInstruction::GetLocalGoldenVectors()
	{
		return std::vector<AddressGenerationGoldenVector> { AddressGenerationGoldenVector(0xFFFF'FFFF'0000'0001ULL, 2ULL, 3ULL), AddressGenerationGoldenVector(0xFFFF'FFFF'FFFF'FFFFULL, 1ULL, 0x1'0000'0000ULL), AddressGenerationGoldenVector(0x0000'0000'8000'0000ULL, 0ULL, 0x8000'0000ULL), AddressGenerationGoldenVector(0x0000'0000'FFFF'FFFFULL, std::numeric_limits<unsigned long long>::max(), 0xFFFF'FFFEULL)};
	}

	AddUwInstruction::AddressGenerationGoldenVector::AddressGenerationGoldenVector(unsigned long long source, unsigned long long addend, unsigned long long expected)
		: Source(source), Addend(addend), Expected(expected)
	{
	}

	unsigned long long AddUwInstruction::AddressGenerationGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned long long AddUwInstruction::AddressGenerationGoldenVector::getAddend() const
	{
		return Addend;
	}

	unsigned long long AddUwInstruction::AddressGenerationGoldenVector::getExpected() const
	{
		return Expected;
	}
}
