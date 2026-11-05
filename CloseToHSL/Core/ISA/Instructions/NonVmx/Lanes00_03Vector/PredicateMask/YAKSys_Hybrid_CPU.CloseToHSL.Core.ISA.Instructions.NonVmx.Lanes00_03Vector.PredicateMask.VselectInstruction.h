#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::PredicateMask
{
	class VselectInstruction final : public std::enable_shared_from_this<VselectInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"VSELECT";
		static inline const std::wstring OperandShape = L"DestSrc1Pointer, Src2Pointer, predicate-mask sideband";
		static inline const std::wstring EvidenceBoundary = L"VectorContourFailClosed";
		static inline const std::wstring AliasPolicy = L"Distinct mnemonic until a Phase05 ABI decision proves aliasing";
		static inline const std::wstring ResultSourcePolarityPolicy = L"Unresolved; no canonical result/source polarity";
		static inline const std::wstring MaskedOffTailPolicy = L"Unresolved; requires explicit mask/tail ABI";
		static inline const std::wstring PredicateMaskSidebandPolicy = L"Explicit carrier sideband required; not inferred from VLM";
		static inline const std::wstring ElementWidthLmulVlPolicy = L"Unresolved; requires element-width, LMUL, and VL contour closure";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Vector";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static constexpr bool RequiresPredicateMaskSideband = true;
		static constexpr bool RequiresAliasPolarityDecision = true;
		static constexpr bool RequiresMaskedOffTailPolicyAbi = true;
		static constexpr bool RequiresElementWidthLmulVlAbi = true;
		static constexpr bool RequiresVectorLegalityMatrixClosure = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool RequiresTypedVectorMicroOp = true;
		static constexpr bool RequiresRetireStagedPublication = true;
		static constexpr bool RequiresReplayRollbackConformance = true;
		static constexpr bool RequiresGoldenArtifacts = true;
		static constexpr bool MaskSidebandInferredFromVlm = false;
		static constexpr bool NoDescriptorFallback = true;
		static constexpr bool NoHiddenStreamEngineFallback = true;
		static constexpr bool NoHiddenDmaFallback = true;
		static constexpr bool NoHiddenScalarLowering = true;
		static constexpr bool NoMultiOpEmission = true;
		static constexpr bool NoVmxSpecificPath = true;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;
	};
}
