#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::StructureMovement
{
	class VdeinterleaveInstruction final : public std::enable_shared_from_this<VdeinterleaveInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"VDEINTERLEAVE";
		static inline const std::wstring OperandShape = L"DestSrc1Pointer, Src2Pointer, structure shape sideband";
		static inline const std::wstring EvidenceBoundary = L"VectorStructureMovementFailClosed";
		static inline const std::wstring ShapeContract = L"structure shape sideband must define element order, lane grouping, aliasing, mask/tail, and publication footprint before execution.";
		static inline const std::wstring PublicationPolicy = L"structure movement requires staged vector publication with replay-safe all-or-fail visibility.";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Vector";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static constexpr bool RequiresStructureShapeAbi = true;
		static constexpr bool RequiresShapeOrderingPolicy = true;
		static constexpr bool RequiresMaskTailPolicyAbi = true;
		static constexpr bool NoHiddenStreamEngineFallback = true;
		static constexpr bool RequiresVectorLegalityMatrixClosure = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool RequiresTypedVectorMicroOp = true;
		static constexpr bool RequiresRetireStagedPublication = true;
		static constexpr bool RequiresReplayRollbackConformance = true;
		static constexpr bool RequiresGoldenArtifacts = true;
		static constexpr bool NoBaseOpcodeDuplication = true;
		static constexpr bool NoDescriptorFallback = true;
		static constexpr bool NoHiddenDmaFallback = true;
		static constexpr bool NoMultiOpEmission = true;
		static constexpr bool NoVmxSpecificPath = true;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;
	};
}
