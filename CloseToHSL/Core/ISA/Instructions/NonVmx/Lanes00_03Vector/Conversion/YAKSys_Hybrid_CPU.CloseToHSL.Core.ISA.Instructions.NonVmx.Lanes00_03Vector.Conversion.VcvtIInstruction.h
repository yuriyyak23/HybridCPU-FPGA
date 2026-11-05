#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::Conversion
{
	class VcvtIInstruction final : public std::enable_shared_from_this<VcvtIInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"VCVT.I";
		static inline const std::wstring OperandShape = L"DestSrc1Pointer, signed integer conversion/rounding sideband";
		static inline const std::wstring EvidenceBoundary = L"VectorWidenNarrowConvertFailClosed";
		static inline const std::wstring ConversionPolicy = L"Unresolved; source/destination type conversion ABI required";
		static inline const std::wstring ResultFootprintPolicy = L"Unresolved; signed integer result footprint ABI required";
		static inline const std::wstring NanPolicy = L"Unresolved; NaN and invalid conversion policy required";
		static inline const std::wstring RoundingSaturationTrapPolicy = L"Unresolved; rounding, saturation, and trap policy required";
		static inline const std::wstring MaskTailPolicy = L"Unresolved; mask/tail behavior requires VLM contour closure";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Vector";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static constexpr bool RequiresConversionPolicyAbi = true;
		static constexpr bool RequiresRoundingSaturationTrapPolicy = true;
		static constexpr bool RequiresResultFootprintAbi = true;
		static constexpr bool RequiresNanPolicyAbi = true;
		static constexpr bool RequiresMaskTailPolicyAbi = true;
		static constexpr bool RequiresVectorLegalityMatrixClosure = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool RequiresTypedVectorMicroOp = true;
		static constexpr bool RequiresRetireStagedPublication = true;
		static constexpr bool RequiresReplayRollbackConformance = true;
		static constexpr bool RequiresGoldenArtifacts = true;
		static constexpr bool NoDescriptorFallback = true;
		static constexpr bool NoHiddenScalarLowering = true;
		static constexpr bool NoMultiOpEmission = true;
		static constexpr bool NoVmxSpecificPath = true;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;
	};
}
