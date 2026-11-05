#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes04_05Memory::Shapes2D
{
	class Vstore2DContour final : public std::enable_shared_from_this<Vstore2DContour>
	{
	public:
		static inline const std::wstring Mnemonic = L"VSTORE";
		static inline const std::wstring Contour = L"2D";
		static inline const std::wstring OperandShape = L"base pointer, row/column/stride shape sideband";
		static inline const std::wstring EvidenceBoundary = L"VectorMemoryContourFailClosed";
		static inline const std::wstring ShapeContract = L"2D memory shape sideband must define rows, columns, element size, row stride, mask/tail, and fault granularity before execution.";
		static inline const std::wstring AddressingContourPolicy = L"base VSTORE 1D transfer evidence does not authorize the 2D contour.";
		static inline const std::wstring PublicationPolicy = L"2D stores require staged byte commit after replay-safe row/column fault ordering closes.";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes04_05Memory";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static constexpr bool RequiresMemoryShapeAbi = true;
		static constexpr bool Requires2DShapeSideband = true;
		static constexpr bool RequiresRowColumnStrideAbi = true;
		static constexpr bool RequiresMaskTailPolicyAbi = true;
		static constexpr bool RequiresAddressBoundsPolicy = true;
		static constexpr bool RequiresFaultReplayPolicy = true;
		static constexpr bool NoBaseOpcodeDuplication = true;
		static constexpr bool RequiresVectorLegalityMatrixClosure = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool RequiresTypedVectorMemoryMicroOp = true;
		static constexpr bool RequiresRetireStagedCommit = true;
		static constexpr bool RequiresReplayRollbackConformance = true;
		static constexpr bool RequiresGoldenArtifacts = true;
		static constexpr bool NoDescriptorFallback = true;
		static constexpr bool NoHiddenStreamEngineFallback = true;
		static constexpr bool NoHiddenDmaFallback = true;
		static constexpr bool NoMultiOpEmission = true;
		static constexpr bool NoVmxSpecificPath = true;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;
	};
}
