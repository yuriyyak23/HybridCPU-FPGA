#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::DotMixedPrecision
{
	class VdotAccumInstruction final : public std::enable_shared_from_this<VdotAccumInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"VDOT.ACCUM";
		static inline const std::wstring OperandShape = L"dot operands, accumulator/result sideband";
		static inline const std::wstring EvidenceBoundary = L"VectorDotMatrixDeferredNoExecution";
		static inline const std::wstring AccumulatorPolicy = L"accumulating dot requires explicit accumulator precision, destination/result footprint, staged publication, replay, and golden artifacts.";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Vector";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static inline const std::wstring ProductionDecision = L"Phase09NegativeDecisionGate";
		static constexpr bool RequiresDotAbiDecision = true;
		static constexpr bool RequiresAccumulatorPrecisionAbi = true;
		static constexpr bool RequiresAccumulatorResultFootprintAbi = true;
		static constexpr bool RequiresSeparateResultSurfaceAbi = true;
		static constexpr bool NoHostOwnedEvidencePublication = true;
		static constexpr bool RequiresVectorLegalityMatrixClosure = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool NoDecoderEncoderAbiPublication = true;
		static constexpr bool NoInstructionIrProjectionPublication = true;
		static constexpr bool NoRegistryMaterializerPublication = true;
		static constexpr bool NoTypedMicroOpPublication = true;
		static constexpr bool NoSchedulerLaneBindingPublication = true;
		static constexpr bool NoExecutionCapturePublication = true;
		static constexpr bool NoRetireWritebackPublication = true;
		static constexpr bool NoReplayRollbackPublication = true;
		static constexpr bool NoCompilerHelperEmission = true;
		static constexpr bool RequiresTypedVectorMicroOp = true;
		static constexpr bool RequiresRetireStagedPublication = true;
		static constexpr bool RequiresFutureRetireReplayEvidence = true;
		static constexpr bool RequiresReplayRollbackConformance = true;
		static constexpr bool RequiresGoldenArtifacts = true;
		static constexpr bool NoDescriptorFallback = true;
		static constexpr bool NoDescriptorFallbackWithoutGenericRuntimeOwnership = true;
		static constexpr bool NoHiddenScalarLowering = true;
		static constexpr bool NoHiddenVectorLowering = true;
		static constexpr bool NoMultiOpEmission = true;
		static constexpr bool NoVmxSpecificPath = true;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;
	};
}
