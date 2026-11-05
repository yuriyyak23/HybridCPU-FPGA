#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lane06DmaStream::DescriptorOps::ShapeRange
{
	class DscScatterGatherShapeContour final : public std::enable_shared_from_this<DscScatterGatherShapeContour>
	{
	public:
		static inline const std::wstring Mnemonic = L"DSC_SHAPE_SCATTER_GATHER";
		static inline const std::wstring OperandShape = L"scatter/gather descriptor shape and index-surface sideband";
		static inline const std::wstring EvidenceBoundary = L"Lane6ShapeContourNoExecution";
		static inline const std::wstring ExecutionLaneBinding = L"Lane06DmaStream";
		static inline const std::wstring ShapeAuthorityBoundary = L"GenericLane6DescriptorRuntimeOnly";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static inline const std::wstring ProductionDecision = L"Phase10NegativeDecisionGate";
		static constexpr bool IsDescriptorOwned = true;
		static constexpr bool HasScalarOpcodeAllocation = false;
		static constexpr bool RequiresShapeEnumAllocation = true;
		static constexpr bool RequiresDescriptorParserValidation = true;
		static constexpr bool RequiresOwnerDomainGuard = true;
		static constexpr bool RequiresTokenAdmission = true;
		static constexpr bool RequiresShapeParserManifest = true;
		static constexpr bool RequiresDescriptorPayloadAbi = true;
		static constexpr bool RequiresTypedDescriptorProjection = true;
		static constexpr bool RequiresDescriptorMaterializer = true;
		static constexpr bool RequiresBackendRuntimeAdmission = true;
		static constexpr bool RequiresShapeFaultModel = true;
		static constexpr bool RequiresAliasOverlapPolicy = true;
		static constexpr bool RequiresIndexSurfaceAbi = true;
		static constexpr bool RequiresNormalizedFootprintAbi = true;
		static constexpr bool RequiresPartialCompletionPolicy = true;
		static constexpr bool RequiresStagedCommit = true;
		static constexpr bool RequiresRetireCommitAuthority = true;
		static constexpr bool RequiresFutureRetireReplayEvidence = true;
		static constexpr bool RequiresReplayDeterminism = true;
		static constexpr bool RequiresReplayRollbackConformance = true;
		static constexpr bool RequiresRollbackPolicy = true;
		static constexpr bool RequiresGoldenArtifacts = true;
		static constexpr bool NoGuestVisibleHostEvidence = true;
		static constexpr bool NoHostOwnedEvidencePublication = true;
		static constexpr bool ExistingDmaStreamComputeEvidenceIsInsufficient = true;
		static constexpr bool NoScalarOpcodePublication = true;
		static constexpr bool NoDecoderEncoderAbiPublication = true;
		static constexpr bool NoShapeEnumPublication = true;
		static constexpr bool NoDescriptorParserPublication = true;
		static constexpr bool NoInstructionIrProjectionPublication = true;
		static constexpr bool NoRegistryMaterializerPublication = true;
		static constexpr bool NoTypedMicroOpPublication = true;
		static constexpr bool NoSchedulerLaneBindingPublication = true;
		static constexpr bool NoExecutionCapturePublication = true;
		static constexpr bool NoRetireWritebackPublication = true;
		static constexpr bool NoReplayRollbackPublication = true;
		static constexpr bool NoCompilerHelperEmission = true;
		static constexpr bool NoHiddenScalarLowering = true;
		static constexpr bool NoHiddenVectorLowering = true;
		static constexpr bool NoMultiOpEmission = true;
		static constexpr bool NoGenericDmaStreamComputeFallback = true;
		static constexpr bool NoDsc2Fallback = true;
		static constexpr bool NoStreamEngineFallback = true;
		static constexpr bool NoDmaControllerFallback = true;
		static constexpr bool NoLane7Fallback = true;
		static constexpr bool NoExternalBackendFallback = true;
		static constexpr bool NoVmxSpecificPath = true;
		static constexpr bool RequiresFutureVirtualizationBoundaryPolicy = true;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;
	};
}
