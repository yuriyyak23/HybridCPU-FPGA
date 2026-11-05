#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::PredicateMask
{
	class VmsofInstruction final : public std::enable_shared_from_this<VmsofInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"VMSOF";
		static inline const std::wstring OperandShape = L"predicate destination/source sideband";
		static inline const std::wstring EvidenceBoundary = L"VectorPredicateOnlyContourFailClosed";
		static inline const std::wstring PredicateDestinationRepresentationPolicy = L"Unresolved; predicate-only destination representation ABI required";
		static inline const std::wstring PrefixSuffixMaskSemanticsPolicy = L"Unresolved; only-first semantics require explicit predicate publication ABI";
		static inline const std::wstring TailMaskPolicy = L"Unresolved; tail/mask policy ABI required";
		static inline const std::wstring StagedPredicatePublicationPolicy = L"Unresolved; retire-staged predicate publication ABI required";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Vector";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static constexpr bool RequiresPredicateMaskSideband = true;
		static constexpr bool RequiresPredicateOnlyPublication = true;
		static constexpr bool RequiresPredicateOnlyDestinationRepresentation = true;
		static constexpr bool RequiresPrefixSuffixMaskSemantics = true;
		static constexpr bool RequiresTailMaskPolicyAbi = true;
		static constexpr bool RequiresVectorLegalityMatrixClosure = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool RequiresTypedVectorMicroOp = true;
		static constexpr bool RequiresRetireStagedPublication = true;
		static constexpr bool RequiresReplayRollbackConformance = true;
		static constexpr bool RequiresGoldenArtifacts = true;
		static constexpr bool NoVectorRfExposure = true;
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
