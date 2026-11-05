#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.BitExtract.BextiInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitExtract
{

	unsigned short BextiInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::BEXTI);
	}

	bool BextiInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool BextiInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long BextiInstruction::Execute(unsigned long long source, unsigned long long immediate6)
	{
		return (source >> static_cast<int>(immediate6 & ImmediateMask)) & 1ULL;
	}

	unsigned long long BextiInstruction::EvaluateXLen64(unsigned long long source, unsigned long long immediate6)
	{
		return Execute(source, immediate6);
	}

	bool BextiInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<BextiInstruction::BitfieldImmediateGoldenVector> BextiInstruction::GetLocalGoldenVectors()
	{
		return std::vector<BitfieldImmediateGoldenVector> { BitfieldImmediateGoldenVector(0x0000'0000'0000'0001ULL, 0, 1ULL), BitfieldImmediateGoldenVector(0x0000'0000'0000'0001ULL, 1, 0ULL), BitfieldImmediateGoldenVector(0x8000'0000'0000'0000ULL, 63, 1ULL), BitfieldImmediateGoldenVector(0x0000'0000'0000'0010ULL, 4, 1ULL), BitfieldImmediateGoldenVector(0x0000'0000'0000'0000ULL, 4, 0ULL)};
	}

	BextiInstruction::BitfieldImmediateGoldenVector::BitfieldImmediateGoldenVector(unsigned long long source, unsigned short immediate6, unsigned long long expected)
		: Source(source), Immediate6(immediate6), Expected(expected)
	{
	}

	unsigned long long BextiInstruction::BitfieldImmediateGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned short BextiInstruction::BitfieldImmediateGoldenVector::getImmediate6() const
	{
		return Immediate6;
	}

	unsigned long long BextiInstruction::BitfieldImmediateGoldenVector::getExpected() const
	{
		return Expected;
	}
}
