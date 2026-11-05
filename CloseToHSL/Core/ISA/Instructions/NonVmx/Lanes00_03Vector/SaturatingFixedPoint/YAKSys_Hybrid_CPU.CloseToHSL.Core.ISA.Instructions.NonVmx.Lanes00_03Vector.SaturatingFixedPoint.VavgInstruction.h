#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::SaturatingFixedPoint
{
	class VavgInstruction final : public std::enable_shared_from_this<VavgInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"VAVG";
		static inline const std::wstring OperandShape = L"DestSrc1Pointer, Src2Pointer, average policy sideband";
		static inline const std::wstring EvidenceBoundary = L"VectorFixedPointSaturatingFailClosed";
		static inline const std::wstring AveragePolicy = L"fixed-point average requires explicit signedness, element width, truncation/rounding, overflow, mask/tail, and staged publication policy.";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Vector";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static constexpr bool RequiresAveragePolicyAbi = true;
		static constexpr bool RequiresElementWidthAbi = true;
		static constexpr bool RequiresSignednessAbi = true;
		static constexpr bool RequiresRoundingTruncationPolicyAbi = true;
		static constexpr bool RequiresOverflowPolicyAbi = true;
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
