#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Scalar.BitManipulation.BitSetClearInvert.BsetiInstruction.h"
#include "../../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitSetClearInvert
{

	unsigned short BsetiInstruction::getOpcode()
	{
		return static_cast<unsigned short>(Processor::CPU_Core::InstructionsEnum::BSETI);
	}

	bool BsetiInstruction::getWritesScalarRegister()
	{
		return true;
	}

	bool BsetiInstruction::getHasSideEffects()
	{
		return false;
	}

	unsigned long long BsetiInstruction::Execute(unsigned long long source, unsigned long long immediate6)
	{
		return source | (1ULL << static_cast<int>(immediate6 & ImmediateMask));
	}

	unsigned long long BsetiInstruction::EvaluateXLen64(unsigned long long source, unsigned long long immediate6)
	{
		return Execute(source, immediate6);
	}

	bool BsetiInstruction::RetireWritesDestination(unsigned short destinationRegister)
	{
		return destinationRegister != 0;
	}

	std::vector<BsetiInstruction::BitfieldImmediateGoldenVector> BsetiInstruction::GetLocalGoldenVectors()
	{
		return std::vector<BitfieldImmediateGoldenVector> { BitfieldImmediateGoldenVector(0x0000'0000'0000'0000ULL, 0, 0x0000'0000'0000'0001ULL), BitfieldImmediateGoldenVector(0x0000'0000'0000'0000ULL, 63, 0x8000'0000'0000'0000ULL), BitfieldImmediateGoldenVector(0x0000'0000'0000'0010ULL, 4, 0x0000'0000'0000'0010ULL), BitfieldImmediateGoldenVector(0x0123'4567'89AB'CDEFULL, 4, 0x0123'4567'89AB'CDFFULL)};
	}

	BsetiInstruction::BitfieldImmediateGoldenVector::BitfieldImmediateGoldenVector(unsigned long long source, unsigned short immediate6, unsigned long long expected)
		: Source(source), Immediate6(immediate6), Expected(expected)
	{
	}

	unsigned long long BsetiInstruction::BitfieldImmediateGoldenVector::getSource() const
	{
		return Source;
	}

	unsigned short BsetiInstruction::BitfieldImmediateGoldenVector::getImmediate6() const
	{
		return Immediate6;
	}

	unsigned long long BsetiInstruction::BitfieldImmediateGoldenVector::getExpected() const
	{
		return Expected;
	}
}
