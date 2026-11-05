#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::PredicateMask
{
	class VanyInstruction final : public std::enable_shared_from_this<VanyInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"VANY";
		static inline const std::wstring OperandShape = L"rd, predicate-mask sideband";
		static inline const std::wstring EvidenceBoundary = L"VectorScalarResultContourFailClosed";
		static inline const std::wstring ScalarResultDestinationPolicy = L"Unresolved; scalar rd publication ABI required";
		static inline const std::wstring EmptyMaskResultPolicy = L"Unresolved; empty-mask boolean result policy required";
		static inline const std::wstring BooleanResultEncodingPolicy = L"Unresolved; canonical false/true scalar encoding required";
		static inline const std::wstring PredicateMaskSidebandPolicy = L"Explicit carrier sideband required; not inferred from VLM";
		static inline const std::wstring ActiveVlTailPolicy = L"Unresolved; active VL and tail policy ABI required";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Vector";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static constexpr bool RequiresPredicateMaskSideband = true;
		static constexpr bool RequiresScalarResultAbi = true;
		static constexpr bool RequiresScalarResultDestinationAbi = true;
		static constexpr bool RequiresEmptyMaskResultPolicy = true;
		static constexpr bool RequiresBooleanResultEncodingAbi = true;
		static constexpr bool RequiresActiveVlTailPolicyAbi = true;
		static constexpr bool RequiresVectorLegalityMatrixClosure = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool RequiresTypedVectorMicroOp = true;
		static constexpr bool RequiresRetireStagedPublication = true;
		static constexpr bool RequiresRetireReplayPublicationAbi = true;
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
