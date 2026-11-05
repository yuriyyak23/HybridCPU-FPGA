#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.AddressGeneration.Sh2addUwInstruction.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::AddressGeneration
{

	unsigned short Sh2addUwInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::SH2ADD_UW);
	}

	bool Sh2addUwInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool Sh2addUwInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long Sh2addUwInstruction::Execute(unsigned long long source, unsigned long long addend)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked(((ulong)(uint)source << 2) + addend);
		return (static_cast<unsigned long long>(static_cast<unsigned int>(source)) << 2) + addend;
	}

	unsigned long long Sh2addUwInstruction::EvaluateXLen64(unsigned long long source, unsigned long long addend)
	{
		return Execute(source, addend);
	}

	bool Sh2addUwInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<Sh2addUwInstruction::AddressGenerationGoldenVector> Sh2addUwInstruction::GetLocalGoldenVectors()
	{
		return std::vector<AddressGenerationGoldenVector> { AddressGenerationGoldenVector(0xFFFF'FFFF'0000'0001ULL, 2ULL, 6ULL), AddressGenerationGoldenVector(0xFFFF'FFFF'FFFF'FFFFULL, 1ULL, 0x3'FFFF'FFFDULL), AddressGenerationGoldenVector(0x0000'0000'8000'0000ULL, 0ULL, 0x2'0000'0000ULL), AddressGenerationGoldenVector(0x0000'0000'FFFF'FFFFULL, std::numeric_limits<unsigned long long>::max(), 0x3'FFFF'FFFBULL)};
	}

	Sh2addUwInstruction::AddressGenerationGoldenVector::AddressGenerationGoldenVector(unsigned long long source, unsigned long long addend, unsigned long long expected)
		: Source(source), Addend(addend), Expected(expected)
	{
	}

	unsigned long long Sh2addUwInstruction::AddressGenerationGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned long long Sh2addUwInstruction::AddressGenerationGoldenVector::getAddend() const
	{
		return Addend;
	}

	unsigned long long Sh2addUwInstruction::AddressGenerationGoldenVector::getExpected() const
	{
		return Expected;
	}
}
