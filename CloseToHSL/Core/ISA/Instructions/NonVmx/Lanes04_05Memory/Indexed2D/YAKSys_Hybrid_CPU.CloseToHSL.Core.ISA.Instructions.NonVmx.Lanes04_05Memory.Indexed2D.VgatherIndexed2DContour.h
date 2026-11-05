#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes04_05Memory::Indexed2D
{
	class VgatherIndexed2DContour final : public std::enable_shared_from_this<VgatherIndexed2DContour>
	{
	public:
		static inline const std::wstring Mnemonic = L"VGATHER";
		static inline const std::wstring Contour = L"Indexed2D";
		static inline const std::wstring OperandShape = L"index surface, base pointer, 2D shape sideband";
		static inline const std::wstring EvidenceBoundary = L"VectorMemoryContourFailClosed";
		static inline const std::wstring ShapeContract = L"indexed+2D shape sideband must define index element meaning, rows, columns, stride, bounds, mask/tail, and fault order before execution.";
		static inline const std::wstring AddressingContourPolicy = L"base VGATHER 1D indexed evidence does not authorize the indexed+2D contour.";
		static inline const std::wstring PublicationPolicy = L"indexed+2D gather requires staged destination publication after descriptor, bounds, and replay evidence close.";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes04_05Memory";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static constexpr bool RequiresMemoryShapeAbi = true;
		static constexpr bool RequiresIndexed2DShapeSideband = true;
		static constexpr bool RequiresIndexSurfaceAbi = true;
		static constexpr bool RequiresRowColumnStrideAbi = true;
		static constexpr bool RequiresAddressBoundsPolicy = true;
		static constexpr bool RequiresMaskTailPolicyAbi = true;
		static constexpr bool RequiresFaultReplayPolicy = true;
		static constexpr bool NoBaseOpcodeDuplication = true;
		static constexpr bool RequiresVectorLegalityMatrixClosure = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool RequiresTypedVectorMemoryMicroOp = true;
		static constexpr bool RequiresRetireStagedPublication = true;
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
