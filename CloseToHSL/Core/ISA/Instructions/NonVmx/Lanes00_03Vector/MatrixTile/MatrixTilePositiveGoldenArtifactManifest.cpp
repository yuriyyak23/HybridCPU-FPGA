#include "MatrixTilePositiveGoldenArtifactManifest.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	std::span<const MatrixTileExecutionGoldenVector> MatrixTilePositiveGoldenArtifactManifest::getExecutionVectors()
	{
		return ExecutionVectorTable;
	}

	std::span<const MatrixTileMemoryFaultGoldenVector> MatrixTilePositiveGoldenArtifactManifest::getMemoryFaultVectors()
	{
		return MemoryFaultVectorTable;
	}

	std::span<const MatrixTileDescriptorFaultGoldenVector> MatrixTilePositiveGoldenArtifactManifest::getDescriptorFaultVectors()
	{
		return DescriptorFaultVectorTable;
	}

	std::span<const MatrixTileReservedCarrierGoldenVector> MatrixTilePositiveGoldenArtifactManifest::getReservedCarrierVectors()
	{
		return ReservedCarrierVectorTable;
	}

	std::span<const std::wstring> MatrixTileNoFallbackEvidenceContract::getAuditedRuntimeTypes()
	{
		return AuditedRuntimeTypeTable;
	}

	std::span<const std::wstring> MatrixTileNoFallbackEvidenceContract::getForbiddenCallTargetFragments()
	{
		return ForbiddenCallTargetTable;
	}
}
