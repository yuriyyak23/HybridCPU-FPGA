#include "MatrixTileResourceContour.h"
#include <stdexcept>

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	bool MatrixTileResourceContour::IsMatrixTileMemoryOpcode(InstructionsEnum opcode)
	{
		return (opcode == InstructionsEnum::MTILE_LOAD || opcode == InstructionsEnum::MTILE_STORE);
	}

	bool MatrixTileResourceContour::IsMatrixTileMemoryOpcode(unsigned int opcode)
	{
		return IsMatrixTileMemoryOpcode(static_cast<InstructionsEnum>(opcode));
	}

	bool MatrixTileResourceContour::IsMatrixTileComputeOpcode(InstructionsEnum opcode)
	{
		return (opcode == InstructionsEnum::MTILE_MACC || opcode == InstructionsEnum::MTRANSPOSE);
	}

	bool MatrixTileResourceContour::IsMatrixTileComputeOpcode(unsigned int opcode)
	{
		return IsMatrixTileComputeOpcode(static_cast<InstructionsEnum>(opcode));
	}

	bool MatrixTileResourceContour::IsMatrixTileOpcode(InstructionsEnum opcode)
	{
		return IsMatrixTileMemoryOpcode(opcode) || IsMatrixTileComputeOpcode(opcode);
	}

	bool MatrixTileResourceContour::IsMatrixTileOpcode(unsigned int opcode)
	{
		return IsMatrixTileOpcode(static_cast<InstructionsEnum>(opcode));
	}

	MatrixTileRuntimeResourceClass MatrixTileResourceContour::Classify(InstructionsEnum opcode)
	{
		if (IsMatrixTileMemoryOpcode(opcode))
		{
			return MatrixTileRuntimeResourceClass::MatrixTileMemory;
		}

		if (IsMatrixTileComputeOpcode(opcode))
		{
			return MatrixTileRuntimeResourceClass::MatrixTileCompute;
		}

		return MatrixTileRuntimeResourceClass::None;
	}

	MatrixTileRuntimeResourceClass MatrixTileResourceContour::Classify(unsigned int opcode)
	{
		return Classify(static_cast<InstructionsEnum>(opcode));
	}

	SlotClass MatrixTileResourceContour::ResolveSlotClass(MatrixTileRuntimeResourceClass resourceClass)
	{
        switch (resourceClass)
        {
        case MatrixTileRuntimeResourceClass::MatrixTileMemory:
            return SlotClass::MatrixTileStreamClass;
        case MatrixTileRuntimeResourceClass::MatrixTileCompute:
            return SlotClass::AluClass;
        default:
            throw std::invalid_argument("A MatrixTile runtime resource class is required.");
        }
    }
}
