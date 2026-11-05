#include "MatrixTileRuntimeOwnedVlmRows.h"
#include "MatrixTileMemoryShapeAndFaultAbi.h"
#include "MatrixTileAccumulatorAndTransposePolicyAbi.h"

using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	std::vector<InstructionsEnum> MatrixTileRuntimeOwnedVlmRows::getOpcodes()
	{
		return Opcodes;
	}

	bool MatrixTileRuntimeOwnedVlmRows::getHasRuntimeOwnedVlmRows()
	{
		return AllRowsSatisfy([&] (std::shared_ptr<VectorLegalityMatrixRow> row) {return IsRuntimeOwnedMatrixTileRow(row);});
	}

	bool MatrixTileRuntimeOwnedVlmRows::getHasMatrixTileVlmFamily()
	{
		return AllRowsSatisfy([] (std::shared_ptr<VectorLegalityMatrixRow> row)
		{
			return row->getFamilyName() == MatrixTileVlmFamily;
		});
	}

	bool MatrixTileRuntimeOwnedVlmRows::getHasDescriptorBackedOnlyContour()
	{
		return AllRowsSatisfy([] (std::shared_ptr<VectorLegalityMatrixRow> row)
		{
			return row->GetContourStatus(VectorContourKind::DescriptorBacked) == VectorContourLegalityStatus::DescriptorOnly;
		});
	}

	bool MatrixTileRuntimeOwnedVlmRows::getKeepsNonDescriptorContoursFailClosed()
	{
		return AllRowsSatisfy([] (std::shared_ptr<VectorLegalityMatrixRow> row)
		{
			return row->getOneDimensional() == VectorContourLegalityStatus::FailClosed && row->getIndexedAddressing() == VectorContourLegalityStatus::FailClosed && row->getTwoDimensionalAddressing() == VectorContourLegalityStatus::FailClosed && row->getMasked() == VectorContourLegalityStatus::FailClosed && row->getTailMaskPolicy() == VectorContourLegalityStatus::FailClosed && row->getReduction() == VectorContourLegalityStatus::FailClosed;
		});
	}

	bool MatrixTileRuntimeOwnedVlmRows::getKeepsExecutableContoursClosed()
	{
		return AllRowsSatisfy([] (std::shared_ptr<VectorLegalityMatrixRow> row)
		{
			return row->getOneDimensional() != VectorContourLegalityStatus::Executable && row->getIndexedAddressing() != VectorContourLegalityStatus::Executable && row->getTwoDimensionalAddressing() != VectorContourLegalityStatus::Executable && row->getMasked() != VectorContourLegalityStatus::Executable && row->getTailMaskPolicy() != VectorContourLegalityStatus::Executable && row->getReduction() != VectorContourLegalityStatus::Executable && row->getDescriptorBacked() != VectorContourLegalityStatus::Executable;
		});
	}

	bool MatrixTileRuntimeOwnedVlmRows::IsMatrixTileOpcode(InstructionsEnum opcode)
	{
		return (opcode == InstructionsEnum::MTILE_LOAD || opcode == InstructionsEnum::MTILE_STORE || opcode == InstructionsEnum::MTILE_MACC || opcode == InstructionsEnum::MTRANSPOSE);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TryGetRuntimeOwnedRow(InstructionsEnum opcode, out VectorLegalityMatrixRow? row)
	bool MatrixTileRuntimeOwnedVlmRows::TryGetRuntimeOwnedRow(InstructionsEnum opcode, std::shared_ptr<VectorLegalityMatrixRow> &row)
	{
		if (!IsMatrixTileOpcode(opcode))
		{
			row = nullptr;
			return false;
		}

		return VectorLegalityMatrix::TryGetRow(opcode, row) && row->getFamilyName() == MatrixTileVlmFamily;
	}

	std::shared_ptr<MatrixTileVlmLegalityValidationResult> MatrixTileRuntimeOwnedVlmRows::ValidateLoad(std::shared_ptr<MatrixTileMemoryShapeContract> contract)
	{
		std::shared_ptr<VectorLegalityMatrixRow> row;
		if (!TryGetRuntimeOwnedRow(InstructionsEnum::MTILE_LOAD, row))
		{
			return MatrixTileVlmLegalityValidationResult::Fault(InstructionsEnum::MTILE_LOAD, MatrixTileVlmLegalityFaultKind::MissingRuntimeOwnedVlmRow);
		}

		return MatrixTileMemoryShapeAndFaultAbi::Validate(contract)->isMemoryShapeAbiAccepted() ? MatrixTileVlmLegalityValidationResult::Legal(InstructionsEnum::MTILE_LOAD) : MatrixTileVlmLegalityValidationResult::Fault(InstructionsEnum::MTILE_LOAD, MatrixTileVlmLegalityFaultKind::MemoryShapeFault);
	}

	std::shared_ptr<MatrixTileVlmLegalityValidationResult> MatrixTileRuntimeOwnedVlmRows::ValidateStore(std::shared_ptr<MatrixTileMemoryShapeContract> contract)
	{
		std::shared_ptr<VectorLegalityMatrixRow> row;
		if (!TryGetRuntimeOwnedRow(InstructionsEnum::MTILE_STORE, row))
		{
			return MatrixTileVlmLegalityValidationResult::Fault(InstructionsEnum::MTILE_STORE, MatrixTileVlmLegalityFaultKind::MissingRuntimeOwnedVlmRow);
		}

		return MatrixTileMemoryShapeAndFaultAbi::Validate(contract)->isMemoryShapeAbiAccepted() ? MatrixTileVlmLegalityValidationResult::Legal(InstructionsEnum::MTILE_STORE) : MatrixTileVlmLegalityValidationResult::Fault(InstructionsEnum::MTILE_STORE, MatrixTileVlmLegalityFaultKind::MemoryShapeFault);
	}

	std::shared_ptr<MatrixTileVlmLegalityValidationResult> MatrixTileRuntimeOwnedVlmRows::ValidateMacc(std::shared_ptr<MatrixTileMaccSemanticContract> contract)
	{
		std::shared_ptr<VectorLegalityMatrixRow> row;
		if (!TryGetRuntimeOwnedRow(InstructionsEnum::MTILE_MACC, row))
		{
			return MatrixTileVlmLegalityValidationResult::Fault(InstructionsEnum::MTILE_MACC, MatrixTileVlmLegalityFaultKind::MissingRuntimeOwnedVlmRow);
		}

		return MatrixTileAccumulatorAndTransposePolicyAbi::ValidateMacc(contract)->isSemanticAbiAccepted() ? MatrixTileVlmLegalityValidationResult::Legal(InstructionsEnum::MTILE_MACC) : MatrixTileVlmLegalityValidationResult::Fault(InstructionsEnum::MTILE_MACC, MatrixTileVlmLegalityFaultKind::MaccSemanticFault);
	}

	std::shared_ptr<MatrixTileVlmLegalityValidationResult> MatrixTileRuntimeOwnedVlmRows::ValidateTranspose(std::shared_ptr<MatrixTileTransposeSemanticContract> contract)
	{
		std::shared_ptr<VectorLegalityMatrixRow> row;
		if (!TryGetRuntimeOwnedRow(InstructionsEnum::MTRANSPOSE, row))
		{
			return MatrixTileVlmLegalityValidationResult::Fault(InstructionsEnum::MTRANSPOSE, MatrixTileVlmLegalityFaultKind::MissingRuntimeOwnedVlmRow);
		}

		return MatrixTileAccumulatorAndTransposePolicyAbi::ValidateTranspose(contract)->isSemanticAbiAccepted() ? MatrixTileVlmLegalityValidationResult::Legal(InstructionsEnum::MTRANSPOSE) : MatrixTileVlmLegalityValidationResult::Fault(InstructionsEnum::MTRANSPOSE, MatrixTileVlmLegalityFaultKind::TransposeSemanticFault);
	}

	bool MatrixTileRuntimeOwnedVlmRows::IsRuntimeOwnedMatrixTileRow(std::shared_ptr<VectorLegalityMatrixRow> row)
	{
		return row->getFamilyName() == MatrixTileVlmFamily && row->GetContourStatus(VectorContourKind::DescriptorBacked) == VectorContourLegalityStatus::DescriptorOnly;
	}

	bool MatrixTileRuntimeOwnedVlmRows::AllRowsSatisfy(std::function<bool(std::shared_ptr<VectorLegalityMatrixRow>)> predicate)
	{
		for (auto opcode : getOpcodes())
		{
			std::shared_ptr<VectorLegalityMatrixRow> row;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!TryGetRuntimeOwnedRow(opcode, out VectorLegalityMatrixRow? row) || !predicate(row))
			if (!TryGetRuntimeOwnedRow(opcode, row) || !predicate(row))
			{
				return false;
			}
		}

		return true;
	}
}
