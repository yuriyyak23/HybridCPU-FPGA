#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.AddressGeneration.SlliUwInstruction.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::AddressGeneration
{

	unsigned short SlliUwInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::SLLI_UW);
	}

	bool SlliUwInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool SlliUwInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long SlliUwInstruction::Execute(unsigned long long source, unsigned long long immediate6)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)(uint)source << (int)(immediate6 & ImmediateMask));
		return static_cast<unsigned long long>(static_cast<unsigned int>(source)) << static_cast<int>(immediate6 & ImmediateMask);
	}

	unsigned long long SlliUwInstruction::EvaluateXLen64(unsigned long long source, unsigned long long immediate6)
	{
		return Execute(source, immediate6);
	}

	bool SlliUwInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<SlliUwInstruction::AddressGenerationImmediateGoldenVector> SlliUwInstruction::GetLocalGoldenVectors()
	{
		return std::vector<AddressGenerationImmediateGoldenVector> { AddressGenerationImmediateGoldenVector(0xFFFF'FFFF'0000'0001ULL, 0, 1ULL), AddressGenerationImmediateGoldenVector(0xFFFF'FFFF'0000'0001ULL, 1, 2ULL), AddressGenerationImmediateGoldenVector(0x0000'0000'8000'0000ULL, 1, 0x1'0000'0000ULL), AddressGenerationImmediateGoldenVector(0xFFFF'FFFF'FFFF'FFFFULL, 32, 0xFFFF'FFFF'0000'0000ULL), AddressGenerationImmediateGoldenVector(0xFFFF'FFFF'FFFF'FFFFULL, 63, 0x8000'0000'0000'0000ULL)};
	}

	SlliUwInstruction::AddressGenerationImmediateGoldenVector::AddressGenerationImmediateGoldenVector(unsigned long long source, unsigned short immediate6, unsigned long long expected)
		: Source(source), Immediate6(immediate6), Expected(expected)
	{
	}

	unsigned long long SlliUwInstruction::AddressGenerationImmediateGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned short SlliUwInstruction::AddressGenerationImmediateGoldenVector::getImmediate6() const
	{
		return Immediate6;
	}

	unsigned long long SlliUwInstruction::AddressGenerationImmediateGoldenVector::getExpected() const
	{
		return Expected;
	}
}
