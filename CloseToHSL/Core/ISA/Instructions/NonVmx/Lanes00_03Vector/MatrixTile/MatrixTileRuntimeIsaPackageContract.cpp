#include "MatrixTileRuntimeIsaPackageContract.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	bool MatrixTileRuntimeIsaPackageContract::getHasVectorLegalityMatrixRows()
	{
		return MatrixTileRuntimeOwnedVlmRows::getHasRuntimeOwnedVlmRows();
	}

	bool MatrixTileRuntimeIsaPackageContract::getHasMatrixTileVlmContour()
	{
		return MatrixTileRuntimeOwnedVlmRows::getHasDescriptorBackedOnlyContour();
	}

	bool MatrixTileRuntimeIsaPackageContract::getHasPhase05RuntimeOwnedVlmRows()
	{
		return MatrixTileRuntimeOwnedVlmRows::getHasRuntimeOwnedVlmRows();
	}

	bool MatrixTileRuntimeIsaPackageContract::getHasPhase05MatrixTileVlmFamily()
	{
		return MatrixTileRuntimeOwnedVlmRows::getHasMatrixTileVlmFamily();
	}

	bool MatrixTileRuntimeIsaPackageContract::getHasPhase05DescriptorBackedOnlyContour()
	{
		return MatrixTileRuntimeOwnedVlmRows::getHasDescriptorBackedOnlyContour();
	}

	bool MatrixTileRuntimeIsaPackageContract::getHasPhase05FeatureGate()
	{
		return MatrixTileRuntimeOwnedVlmRows::HasFeatureGate;
	}

	bool MatrixTileRuntimeIsaPackageContract::getHasPhase05ElementWidthLegality()
	{
		return MatrixTileRuntimeOwnedVlmRows::HasElementWidthLegality;
	}

	bool MatrixTileRuntimeIsaPackageContract::getHasPhase05TileShapeLegality()
	{
		return MatrixTileRuntimeOwnedVlmRows::HasTileShapeLegality;
	}

	bool MatrixTileRuntimeIsaPackageContract::getHasPhase05MemoryLayoutLegality()
	{
		return MatrixTileRuntimeOwnedVlmRows::HasMemoryLayoutLegality;
	}

	bool MatrixTileRuntimeIsaPackageContract::getHasPhase05ReservedDisabledRows()
	{
		return MatrixTileRuntimeOwnedVlmRows::HasReservedDisabledRows;
	}

	bool MatrixTileRuntimeIsaPackageContract::getPhase05KeepsNonDescriptorContoursFailClosed()
	{
		return MatrixTileRuntimeOwnedVlmRows::getKeepsNonDescriptorContoursFailClosed();
	}

	bool MatrixTileRuntimeIsaPackageContract::getPhase05KeepsExecutableContoursClosed()
	{
		return MatrixTileRuntimeOwnedVlmRows::getKeepsExecutableContoursClosed();
	}

	std::vector<std::shared_ptr<MatrixTileRuntimeIsaPackageRow>> MatrixTileRuntimeIsaPackageContract::getRows()
	{
		return RowTable;
	}

	std::vector<std::shared_ptr<MatrixTileOpcodeAuthorityRow>> MatrixTileRuntimeIsaPackageContract::getOpcodeAuthorityRows()
	{
		return OpcodeAuthorityRowTable;
	}

	std::vector<std::shared_ptr<MatrixTilePhase01AuthorityDecisionRow>> MatrixTileRuntimeIsaPackageContract::getPhase01AuthorityDecisionRows()
	{
		return Phase01AuthorityDecisionRows;
	}

	std::vector<std::shared_ptr<MatrixTilePhase02StateDescriptorDecisionRow>> MatrixTileRuntimeIsaPackageContract::getPhase02StateDescriptorDecisionRows()
	{
		return Phase02StateDescriptorDecisionRows;
	}

	std::vector<std::shared_ptr<MatrixTilePhase02InstructionDescriptorRequirementRow>> MatrixTileRuntimeIsaPackageContract::getPhase02InstructionDescriptorRequirementRows()
	{
		return Phase02InstructionDescriptorRequirementRows;
	}

	std::vector<std::shared_ptr<MatrixTilePhase03MemoryFaultDecisionRow>> MatrixTileRuntimeIsaPackageContract::getPhase03MemoryFaultDecisionRows()
	{
		return Phase03MemoryFaultDecisionRows;
	}

	std::vector<std::shared_ptr<MatrixTilePhase04SemanticAbiDecisionRow>> MatrixTileRuntimeIsaPackageContract::getPhase04SemanticAbiDecisionRows()
	{
		return Phase04SemanticAbiDecisionRows;
	}

	std::vector<std::shared_ptr<MatrixTilePhase05VlmDecisionRow>> MatrixTileRuntimeIsaPackageContract::getPhase05VlmDecisionRows()
	{
		return Phase05VlmDecisionRows;
	}

	std::vector<std::shared_ptr<MatrixTilePhase06DecoderEncoderDecisionRow>> MatrixTileRuntimeIsaPackageContract::getPhase06DecoderEncoderDecisionRows()
	{
		return Phase06DecoderEncoderDecisionRows;
	}

	std::vector<std::shared_ptr<MatrixTilePhase07IrMaterializerDecisionRow>> MatrixTileRuntimeIsaPackageContract::getPhase07IrMaterializerDecisionRows()
	{
		return Phase07IrMaterializerDecisionRows;
	}

	std::vector<std::shared_ptr<MatrixTilePhase08MicroOpSchedulerDecisionRow>> MatrixTileRuntimeIsaPackageContract::getPhase08MicroOpSchedulerDecisionRows()
	{
		return Phase08MicroOpSchedulerDecisionRows;
	}

	std::vector<std::shared_ptr<MatrixTilePhase09ExecuteCaptureDecisionRow>> MatrixTileRuntimeIsaPackageContract::getPhase09ExecuteCaptureDecisionRows()
	{
		return Phase09ExecuteCaptureDecisionRowTable;
	}

	std::vector<std::shared_ptr<MatrixTilePhase10RetirePublicationDecisionRow>> MatrixTileRuntimeIsaPackageContract::getPhase10RetirePublicationDecisionRows()
	{
		return Phase10RetirePublicationDecisionRowTable;
	}

	std::vector<std::shared_ptr<MatrixTilePhase11ReplayRollbackDecisionRow>> MatrixTileRuntimeIsaPackageContract::getPhase11ReplayRollbackDecisionRows()
	{
		return Phase11ReplayRollbackDecisionRowTable;
	}

	std::vector<std::shared_ptr<MatrixTilePhase12GoldenArtifactDecisionRow>> MatrixTileRuntimeIsaPackageContract::getPhase12GoldenArtifactDecisionRows()
	{
		return Phase12GoldenArtifactDecisionRowTable;
	}

	std::vector<std::wstring> MatrixTileRuntimeIsaPackageContract::getRequiredExecutableEvidenceChain()
	{
		return RequiredExecutableEvidenceChain;
	}

	std::vector<std::wstring> MatrixTileRuntimeIsaPackageContract::getRequestedClosureGates()
	{
		return RequestedClosureGates;
	}

	std::vector<std::wstring> MatrixTileRuntimeIsaPackageContract::getBlockedProductionGates()
	{
		return BlockedProductionGates;
	}

	std::vector<std::wstring> MatrixTileRuntimeIsaPackageContract::getTileExecutionModelBlockers()
	{
		return TileExecutionModelBlockers;
	}

	std::vector<std::wstring> MatrixTileRuntimeIsaPackageContract::getTileDescriptorAbiBlockers()
	{
		return TileDescriptorAbiBlockers;
	}

	std::vector<std::wstring> MatrixTileRuntimeIsaPackageContract::getMemoryFaultModelBlockers()
	{
		return MemoryFaultModelBlockers;
	}

	std::vector<std::wstring> MatrixTileRuntimeIsaPackageContract::getAccumulatorTileAbiBlockers()
	{
		return AccumulatorTileAbiBlockers;
	}

	std::vector<std::wstring> MatrixTileRuntimeIsaPackageContract::getTransposePolicyAbiBlockers()
	{
		return TransposePolicyAbiBlockers;
	}

	std::vector<std::wstring> MatrixTileRuntimeIsaPackageContract::getVectorLegalityMatrixBlockers()
	{
		return VectorLegalityMatrixBlockers;
	}

	std::vector<std::wstring> MatrixTileRuntimeIsaPackageContract::getMicroOpSchedulerBlockers()
	{
		return MicroOpSchedulerBlockers;
	}

	std::vector<std::shared_ptr<MatrixTileRuntimeIsaClosureGateRow>> MatrixTileRuntimeIsaPackageContract::getClosureGateRows()
	{
		return ClosureGateRowTable;
	}

	std::vector<std::wstring> MatrixTileRuntimeIsaPackageContract::getPositiveGoldenPublicationPrerequisites()
	{
		return PositiveGoldenPublicationPrerequisites;
	}

	std::vector<std::shared_ptr<MatrixTileRemainingRuntimeIsaTaskRow>> MatrixTileRuntimeIsaPackageContract::getRemainingRuntimeIsaOpenTasks()
	{
		return RemainingRuntimeIsaOpenTasks;
	}

	std::vector<std::wstring> MatrixTileRuntimeIsaPackageContract::getClosedAsBlockedRuntimeIsaTaskPools()
	{
		return ClosedAsBlockedRuntimeIsaTaskPools;
	}

	std::vector<std::wstring> MatrixTileRuntimeIsaPackageContract::getExternalEvidenceNonAuthoritySources()
	{
		return ExternalEvidenceNonAuthoritySources;
	}

	std::vector<std::wstring> MatrixTileRuntimeIsaPackageContract::getRuntimePipelineBlockers()
	{
		return RuntimePipelineBlockers;
	}

	std::shared_ptr<MatrixTileRuntimeIsaPackageRow> MatrixTileRuntimeIsaPackageContract::GetRow(const std::wstring &mnemonic)
	{
		for (auto row : RowTable)
		{
			if (row->Mnemonic == mnemonic)
			{
				return row;
			}
		}

		throw ArgumentOutOfRangeException(L"mnemonic", mnemonic, L"Unknown matrix/tile runtime ISA package row.");
	}

	bool MatrixTileRuntimeIsaPackageContract::ContainsRow(const std::wstring &mnemonic)
	{
		for (auto row : RowTable)
		{
			if (row->Mnemonic == mnemonic)
			{
				return true;
			}
		}

		return false;
	}

	void MatrixTileRuntimeIsaPackageContract::RequireExecutableAuthority(const std::wstring &mnemonic)
	{
		GetRow(mnemonic);
		MatrixTileCompilerEmissionHandoffPackage::RequireRuntimeExecutableAuthority(mnemonic);
	}

	void MatrixTileRuntimeIsaPackageContract::RequireCompilerUpdateReadiness()
	{
		RequirePositiveCompilerEmissionReadiness();
	}

	void MatrixTileRuntimeIsaPackageContract::RequirePositiveCompilerEmissionReadiness()
	{
		if (!IsReadyForPositiveCompilerEmissionHandoff || !HasPositiveCompilerEmissionHandoffPackage || BlocksPositiveCompilerEmission || !PositiveNumericHandoffReady)
		{
			throw InvalidOperationException(std::format(L"{0:s} is not ready for positive compiler emission scope: {1:s}.", PackageName, Phase15NumericReadinessDecision));
		}
	}

	void MatrixTileRuntimeIsaPackageContract::RequireCompilerVisibleNoEmissionBoundaryReadiness()
	{
		if (!IsReadyForCompilerVisibleNoEmissionBoundary)
		{
			throw InvalidOperationException(std::format(L"{0:s} is not ready for compiler-visible no-emission boundary: {1:s}.", PackageName, CompilerNoEmissionBoundaryDecision));
		}
	}
}
