#pragma once

#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "MatrixTileIrProjectionAndMaterializer.h"
#include "MatrixTileRetirePublicationAbi.h"
#include "MatrixTileExecuteCaptureAbi.h"
#include "MatrixTileMemoryShapeAndFaultAbi.h"
#include "MatrixTileArchitecturalTileStateAndDescriptorAbi.h"
#include <span>
#include <string>
#include <vector>

using namespace HybridCPU_ISE::Arch;
using YAKSys_Hybrid_CPU::InstructionsEnum;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	struct MatrixTileGoldenCarrier final
	{
		unsigned long long Word0{};
		unsigned long long Word1{};
		unsigned long long Word2{};
		unsigned long long Word3{};

		VLIW_Instruction CreateInstruction() const
		{
			VLIW_Instruction instruction{};
			instruction.setWord0(Word0);
			instruction.setWord1(Word1);
			instruction.setWord2(Word2);
			instruction.setWord3(Word3);
			return instruction;
		}
	};

	struct MatrixTileExecutionGoldenVector final
	{
		std::wstring Id;
		InstructionsEnum Opcode{};
		MatrixTileProjectedOperationKind OperationKind{};
		MatrixTileGoldenCarrier Carrier{};
		MatrixTileCanonicalDescriptorAbi TileDescriptor{};
		MatrixTileCanonicalDescriptorAbi SecondaryTileDescriptor{};
		MatrixTileCanonicalDescriptorAbi ResultTileDescriptor{};
		unsigned short SourceTileId{};
		unsigned short SecondaryTileId{};
		unsigned short DestinationTileId{};
		std::wstring SourceTileHex;
		std::wstring SecondaryTileHex;
		std::wstring InitialDestinationHex;
		std::wstring InitialMemoryHex;
		std::wstring ExpectedRetiredHex;
		std::wstring ExpectedRollbackHex;
		MatrixTileRetirePublicationKind PublicationKind{};
	};

	struct MatrixTileMemoryFaultGoldenVector final
	{
		std::wstring Id;
		InstructionsEnum Opcode{};
		MatrixTileProjectedOperationKind OperationKind{};
		MatrixTileGoldenCarrier Carrier{};
		unsigned short SourceTileId{};
		unsigned short DestinationTileId{};
		std::wstring SourceTileHex;
		MatrixTileExecutionFaultKind ExpectedExecutionFault{};
		MatrixTileRetireFaultKind ExpectedRetireFault{};
		MatrixTileMemoryFaultKind ExpectedMemoryFault{};
		bool ExpectedFaultPoint{};
		unsigned short ExpectedFaultRow{};
		unsigned long long ExpectedFaultAddress{};
	};

	struct MatrixTileDescriptorFaultGoldenVector final
	{
		std::wstring Id;
		InstructionsEnum Opcode{};
		MatrixTileGoldenCarrier Carrier{};
		MatrixTileIrProjectionFaultKind ExpectedProjectionFault{};
	};

	struct MatrixTileReservedCarrierGoldenVector final
	{
		std::wstring Id;
		InstructionsEnum Opcode{};
		MatrixTileGoldenCarrier Carrier{};
		std::wstring ExpectedDecodeDecision;
	};

	class MatrixTilePositiveGoldenArtifactManifest final
	{
	private:
		static inline const MatrixTileCanonicalDescriptorAbi Tile2X2I8 = MatrixTileCanonicalDescriptorAbi::Create(2, 2, 1, 2);
		static inline const MatrixTileCanonicalDescriptorAbi Accumulator2X2I32 = MatrixTileCanonicalDescriptorAbi::Create(2, 2, 4, 8);

		static MatrixTileDescriptorFaultGoldenVector CreateDescriptorFault(const std::wstring &id, InstructionsEnum opcode, unsigned long long word0)
		{
			return {id, opcode, {word0, 0x0000000000000100ULL, 0x0000000000000002ULL, 0x0010000000030001ULL}, MatrixTileIrProjectionFaultKind::InvalidShapeEncoding};
		}

		static MatrixTileReservedCarrierGoldenVector CreateReservedCarrier(const std::wstring &id, InstructionsEnum opcode, unsigned long long word0)
		{
			return {id, opcode, {word0, 0x0000000000000100ULL, 0x0000000000000002ULL, 0x0010000000040001ULL}, L"ReservedWord0BitsRejected"};
		}

		static inline const std::vector<MatrixTileExecutionGoldenVector> ExecutionVectorTable = {
			{L"MTILE_LOAD_2X2_I8_RETIRE_REPLAY", InstructionsEnum::MTILE_LOAD, MatrixTileProjectedOperationKind::Load,
				{0x00D8000000400002ULL, 0x0000000000000100ULL, 0x0000000000000002ULL, 0x0010000000040001ULL},
				Tile2X2I8, {}, Tile2X2I8, 0, 2, 2, L"", L"", L"09080706", L"01020304", L"01020304", L"09080706", MatrixTileRetirePublicationKind::TileState},
			{L"MTILE_STORE_2X2_I8_RETIRE_REPLAY", InstructionsEnum::MTILE_STORE, MatrixTileProjectedOperationKind::Store,
				{0x00D9000000400002ULL, 0x0000000000000180ULL, 0x0000000000000002ULL, 0x0010000000040001ULL},
				Tile2X2I8, {}, Tile2X2I8, 2, 2, 2, L"05060708", L"", L"", L"15161718", L"05060708", L"15161718", MatrixTileRetirePublicationKind::MemoryStore},
			{L"MTILE_MACC_2X2_I8_I32_RETIRE_REPLAY", InstructionsEnum::MTILE_MACC, MatrixTileProjectedOperationKind::Macc,
				{0x00DA000000400002ULL, 0x0000000000000001ULL, 0x0000000000030002ULL, 0x0010000000040001ULL},
				Tile2X2I8, Tile2X2I8, Accumulator2X2I32, 1, 2, 3, L"01020304", L"01000001", L"00000000000000000000000000000000", L"", L"01000000020000000300000004000000", L"00000000000000000000000000000000", MatrixTileRetirePublicationKind::Accumulator},
			{L"MTRANSPOSE_2X2_I8_RETIRE_REPLAY", InstructionsEnum::MTRANSPOSE, MatrixTileProjectedOperationKind::Transpose,
				{0x00DB000000400002ULL, 0x0000000000000001ULL, 0x0000000000000002ULL, 0x0010000000040001ULL},
				Tile2X2I8, {}, Tile2X2I8, 1, 2, 2, L"01020304", L"", L"09090909", L"", L"01030204", L"09090909", MatrixTileRetirePublicationKind::TileState}};

		static inline const std::vector<MatrixTileMemoryFaultGoldenVector> MemoryFaultVectorTable = {
			{L"MTILE_LOAD_PARTIAL_ROW_FAULT", InstructionsEnum::MTILE_LOAD, MatrixTileProjectedOperationKind::Load,
				{0x00D8000000400002ULL, 0x00000000000003FFULL, 0x0000000000000004ULL, 0x0010000000040001ULL},
				0, 4, L"", MatrixTileExecutionFaultKind::InvalidMemoryShape, MatrixTileRetireFaultKind::CapturedExecutionFault,
				MatrixTileMemoryFaultKind::PartialMemoryFault, true, 0, 0x3FF},
			{L"MTILE_STORE_PARTIAL_COMMIT_FAULT", InstructionsEnum::MTILE_STORE, MatrixTileProjectedOperationKind::Store,
				{0x00D9000000400002ULL, 0x0000000000000180ULL, 0x0000000000000004ULL, 0x0010000000040001ULL},
				4, 4, L"0A0B0C0D", MatrixTileExecutionFaultKind::None, MatrixTileRetireFaultKind::MemoryCommitFault,
				MatrixTileMemoryFaultKind::None, false, 0, 0}};

		static inline const std::vector<MatrixTileDescriptorFaultGoldenVector> DescriptorFaultVectorTable = {
			CreateDescriptorFault(L"MTILE_LOAD_MALFORMED_2D_SHAPE", InstructionsEnum::MTILE_LOAD, 0x00D8000000400002ULL),
			CreateDescriptorFault(L"MTILE_STORE_MALFORMED_2D_SHAPE", InstructionsEnum::MTILE_STORE, 0x00D9000000400002ULL),
			CreateDescriptorFault(L"MTILE_MACC_MALFORMED_2D_SHAPE", InstructionsEnum::MTILE_MACC, 0x00DA000000400002ULL),
			CreateDescriptorFault(L"MTRANSPOSE_MALFORMED_2D_SHAPE", InstructionsEnum::MTRANSPOSE, 0x00DB000000400002ULL)};

		static inline const std::vector<MatrixTileReservedCarrierGoldenVector> ReservedCarrierVectorTable = {
			CreateReservedCarrier(L"MTILE_LOAD_RESERVED_WORD0", InstructionsEnum::MTILE_LOAD, 0x00D8010000400002ULL),
			CreateReservedCarrier(L"MTILE_STORE_RESERVED_WORD0", InstructionsEnum::MTILE_STORE, 0x00D9010000400002ULL),
			CreateReservedCarrier(L"MTILE_MACC_RESERVED_WORD0", InstructionsEnum::MTILE_MACC, 0x00DA010000400002ULL),
			CreateReservedCarrier(L"MTRANSPOSE_RESERVED_WORD0", InstructionsEnum::MTRANSPOSE, 0x00DB010000400002ULL)};

	public:
		static inline const std::wstring ManifestDecision = L"ClosedMatrixTilePositiveExecutableGoldenArtifacts";
		static inline const std::wstring VerificationDecision = L"CanonicalCarrierResourceContourTypedStreamExecuteRetireReplayGoldenVerification";
		static inline const std::wstring NoFallbackDecision = L"ClosedRuntimeNoFallbackNoHiddenLoweringRegressionEvidence";
		static inline const std::wstring ProvenanceDecision = L"RuntimeOwnedHandAuthoredGoldenInputsNoCompilerProvenance";
		static inline const std::wstring ReservedCarrierDecision = L"ReservedMatrixTileCarrierRowsFailClosed";
		static inline const std::wstring StatusBoundaryDecision = L"GoldenEvidenceIsNonAuthorityPhase14OwnsFinalStatusPromotion";
		static inline const std::wstring CompilerBoundaryDecision = L"CompilerCodeUnmodifiedRuntimeHandoffOnly";

		static constexpr bool HasPositiveExecutableGoldenArtifacts = true;
		static constexpr bool HasRuntimeNoFallbackNoHiddenLoweringRegressionEvidence = true;
		static constexpr bool HasLegalDecodeEncodeRoundTripVectors = true;
		static constexpr bool HasLegalIrMaterializerProjectionVectors = true;
		static constexpr bool HasLegalExecuteRetireVectors = true;
		static constexpr bool HasMemoryFaultVectors = true;
		static constexpr bool HasDescriptorFaultVectors = true;
		static constexpr bool HasAccumulatorVectors = true;
		static constexpr bool HasTransposeVectors = true;
		static constexpr bool HasReplayRollbackVectors = true;
		static constexpr bool HasNegativeReservedCarrierVectors = true;
		static constexpr bool UsesCompilerGeneratedInputs = false;
		static constexpr bool UsesHostOwnedArchitecturalEvidence = false;
		static constexpr bool UsesFallbackPath = false;
		static constexpr bool KeepsStatusCatalogOptionalDisabled = false;
		static constexpr bool KeepsPositiveCompilerEmissionBlocked = false;

		static std::span<const MatrixTileExecutionGoldenVector> getExecutionVectors();
		static std::span<const MatrixTileMemoryFaultGoldenVector> getMemoryFaultVectors();
		static std::span<const MatrixTileDescriptorFaultGoldenVector> getDescriptorFaultVectors();
		static std::span<const MatrixTileReservedCarrierGoldenVector> getReservedCarrierVectors();
	};

	class MatrixTileNoFallbackEvidenceContract final
	{
	private:
		static inline const std::vector<std::wstring> AuditedRuntimeTypeTable = {L"MatrixTileIrProjectionAndMaterializer", L"MatrixTileExecuteCaptureAbi", L"MatrixTileStreamTransferAbi", L"MatrixTileRetirePublicationAbi", L"MatrixTileReplayRollbackAbi", L"MatrixTileMicroOp", L"MtileLoadMicroOp", L"MtileStoreMicroOp", L"MtileMaccMicroOp", L"MtransposeMicroOp"};
		static inline const std::vector<std::wstring> ForbiddenCallTargetTable = {L"ScalarMicroOp", L"VectorMicroOp", L"DotProduct", L"DmaStreamCompute", L"Lane07", L"ExternalAccelerator", L"StreamExecutionRequest", L"ExecuteStream", L"BurstWrite", L".ISA.Instructions.Vmx.", L"VmxMicroOp", L"VmxInstruction", L"HybridCPU_Compiler"};

	public:
		static inline const std::wstring EvidenceDecision = L"ClosedMatrixTileRuntimeNoFallbackAndNoHiddenLowering";
		static inline const std::wstring CallGraphDecision = L"MtileRuntimeIlCallTargetsExcludeForbiddenHelperFamilies";
		static inline const std::wstring MaterializerDecision = L"CanonicalProjectorProducesOnlyTypedMatrixTileMicroOps";
		static inline const std::wstring MemoryDecision = L"TileMemoryAccessUsesRuntimeOwnedMatrixTileMemoryMethods";
		static inline const std::wstring CompilerDecision = L"NoCompilerAssemblyOrHelperCallTarget";

		static constexpr bool HasIlCallTargetAudit = true;
		static constexpr bool HasTypedCarrierAudit = true;
		static constexpr bool HasRuntimeOwnedMemoryAudit = true;
		static constexpr bool HasCompilerBoundaryAudit = true;
		static constexpr bool UsesFallbackPath = false;

		static std::span<const std::wstring> getAuditedRuntimeTypes();
		static std::span<const std::wstring> getForbiddenCallTargetFragments();
	};
}
