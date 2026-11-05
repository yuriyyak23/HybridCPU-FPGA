#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes04_05Memory::Segments
{
	class Vstseg2Instruction final : public std::enable_shared_from_this<Vstseg2Instruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"VSTSEG2";
		static inline const std::wstring OperandShape = L"source surfaces, base pointer, stride/shape sideband";
		static inline const std::wstring EvidenceBoundary = L"VectorSegmentMemoryFailClosed";
		static inline const std::wstring ShapeContract = L"segment memory shape sideband must define segment count, stride, alignment, byte order, mask/tail, and fault granularity before execution.";
		static inline const std::wstring SegmentOrderingPolicy = L"segment store interleaving and source surface order require golden byte-commit evidence.";
		static inline const std::wstring PublicationPolicy = L"segment stores require staged byte commit after replay-safe fault ordering closes.";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes04_05Memory";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static constexpr int SegmentCount = 2;
		static constexpr bool IsSegmentStore = true;
		static constexpr bool RequiresMemoryShapeAbi = true;
		static constexpr bool RequiresSegmentShapeAbi = true;
		static constexpr bool RequiresByteOrderingPolicy = true;
		static constexpr bool RequiresAlignmentFaultPolicy = true;
		static constexpr bool RequiresMaskTailPolicyAbi = true;
		static constexpr bool RequiresFaultReplayPolicy = true;
		static constexpr bool RequiresVectorLegalityMatrixClosure = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool RequiresTypedVectorMemoryMicroOp = true;
		static constexpr bool RequiresRetireStagedCommit = true;
		static constexpr bool RequiresReplayRollbackConformance = true;
		static constexpr bool RequiresGoldenArtifacts = true;
		static constexpr bool NoBaseOpcodeDuplication = true;
		static constexpr bool NoDescriptorFallback = true;
		static constexpr bool NoHiddenStreamEngineFallback = true;
		static constexpr bool NoHiddenDmaFallback = true;
		static constexpr bool NoMultiOpEmission = true;
		static constexpr bool NoVmxSpecificPath = true;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;
	};
}
