#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	class MtransposeInstruction final : public std::enable_shared_from_this<MtransposeInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"MTRANSPOSE";
		static inline const std::wstring OperandShape = L"tile descriptor, transpose policy sideband";
		static inline const std::wstring EvidenceBoundary = L"MatrixTileRuntimeExecutableAuthority";
		static inline const std::wstring TransposePolicy = L"matrix transpose requires explicit tile descriptor ABI, transpose tile policy, execution model, staged publication, replay, and golden artifacts.";
		static inline const std::wstring ExecutionLaneBinding = L"MatrixTileComputeLanes00_03";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static inline const std::wstring ProductionDecision = L"Phase14TileStreamResourceContourClosed";
		static constexpr bool OptionalDisabledInIsaV4 = false;
		static constexpr bool OptionalEnabledInIsaV4 = true;
		static constexpr bool RequiresTileExecutionModel = true;
		static constexpr bool RequiresTileDescriptorAbi = true;
		static constexpr bool RequiresTransposeTilePolicyAbi = true;
		static constexpr bool NoHostOwnedEvidencePublication = true;
		static constexpr bool RequiresVectorLegalityMatrixClosure = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool NoDecoderEncoderAbiPublication = true;
		static constexpr bool NoInstructionIrProjectionPublication = false;
		static constexpr bool NoRegistryMaterializerPublication = false;
		static constexpr bool NoTypedMicroOpPublication = false;
		static constexpr bool NoSchedulerLaneBindingPublication = false;
		static constexpr bool NoExecutionCapturePublication = false;
		static constexpr bool NoRetireWritebackPublication = false;
		static constexpr bool NoReplayRollbackPublication = false;
		static constexpr bool NoCompilerHelperEmission = true;
		static constexpr bool RequiresTypedTileMicroOp = true;
		static constexpr bool RequiresRetireStagedPublication = true;
		static constexpr bool RequiresFutureRetireReplayEvidence = true;
		static constexpr bool RequiresReplayRollbackConformance = true;
		static constexpr bool RequiresGoldenArtifacts = true;
		static constexpr bool NoDescriptorFallback = true;
		static constexpr bool NoDescriptorFallbackWithoutGenericRuntimeOwnership = true;
		static constexpr bool NoHiddenScalarLowering = true;
		static constexpr bool NoHiddenVectorLowering = true;
		static constexpr bool NoMultiOpEmission = true;
		static constexpr bool NoLane6Placement = true;
		static constexpr bool NoLane6DscFallback = true;
		static constexpr bool NoLane7Fallback = true;
		static constexpr bool NoExternalBackendFallback = true;
		static constexpr bool NoVmxSpecificPath = true;
		static constexpr bool IsExecutable = true;
		static constexpr bool CompilerHelperAllowed = false;
	};
}
