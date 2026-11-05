#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::PrefixScan
{
	class VscanMinInstruction final : public std::enable_shared_from_this<VscanMinInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"VSCAN.MIN";
		static inline const std::wstring OperandShape = L"DestSrc1Pointer, element-type/prefix-policy sideband";
		static inline const std::wstring EvidenceBoundary = L"VectorScanContourFailClosed";
		static inline const std::wstring ScanPolicy = L"prefix min requires explicit inclusive/exclusive policy, element ordering, signedness/element type, tail behavior, replay determinism, and staged publication.";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Vector";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static constexpr bool RequiresPrefixScanPolicyAbi = true;
		static constexpr bool RequiresElementTypeSideband = true;
		static constexpr bool RequiresSignednessAbi = true;
		static constexpr bool RequiresTailPolicyAbi = true;
		static constexpr bool RequiresMaskPolicyAbi = true;
		static constexpr bool SeparateFromClosedVscanSum = true;
		static constexpr bool RequiresReplayDeterminism = true;
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
