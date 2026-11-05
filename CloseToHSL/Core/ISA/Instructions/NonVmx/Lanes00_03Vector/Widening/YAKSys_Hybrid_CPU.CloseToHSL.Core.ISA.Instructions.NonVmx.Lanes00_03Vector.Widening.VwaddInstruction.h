#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::Widening
{
	class VwaddInstruction final : public std::enable_shared_from_this<VwaddInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"VWADD";
		static inline const std::wstring OperandShape = L"DestSrc1Pointer, Src2Pointer, source/destination width sideband";
		static inline const std::wstring EvidenceBoundary = L"VectorWidenNarrowConvertFailClosed";
		static inline const std::wstring WidthTransformPolicy = L"Unresolved; source/destination element-width and LMUL/VL contour ABI required";
		static inline const std::wstring SignednessPolicy = L"Unresolved; signed widening add policy required";
		static inline const std::wstring OverflowPublicationPolicy = L"Unresolved; widening overflow/result footprint policy required";
		static inline const std::wstring MaskTailPolicy = L"Unresolved; mask/tail behavior requires VLM contour closure";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Vector";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static constexpr bool RequiresSourceDestinationWidthSideband = true;
		static constexpr bool RequiresSignednessAbi = true;
		static constexpr bool RequiresElementWidthLmulVlAbi = true;
		static constexpr bool RequiresWideningOverflowPolicyAbi = true;
		static constexpr bool RequiresMaskTailPolicyAbi = true;
		static constexpr bool RequiresVectorLegalityMatrixClosure = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool RequiresTypedVectorMicroOp = true;
		static constexpr bool RequiresRetireStagedPublication = true;
		static constexpr bool RequiresReplayRollbackConformance = true;
		static constexpr bool RequiresGoldenArtifacts = true;
		static constexpr bool SeparateFromBaseVectorArithmetic = true;
		static constexpr bool NoDescriptorFallback = true;
		static constexpr bool NoHiddenScalarLowering = true;
		static constexpr bool NoMultiOpEmission = true;
		static constexpr bool NoVmxSpecificPath = true;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;
	};
}
