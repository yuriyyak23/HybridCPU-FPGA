#pragma once

#include "../../../../../Architecture/Common/InstructionOpcode.h"

#include "../../../../../../../Arch/DataTypeEnum.h"
#include "MatrixTileAccumulatorAndTransposePolicyAbi.h"
#include "MatrixTileRuntimeOwnedVlmRows.h"
#include <string>
#include <limits>
#include <optional>
#include <format>
#include <utility>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core { class MatrixTileMicroOp; namespace Decoder { class CanonicalDecodedInstruction; class DecodedInstruction; } }
namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps { struct VectorInstructionPayload; class InstructionIR; }

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	struct MatrixTileVlmLegalityValidationResult;
	struct MatrixTileInstructionIrProjection;
	struct MatrixTileMaterializedInstruction;
	struct MatrixTileMemoryShapeContract;
	struct MatrixTileMemoryShapeValidationResult;
	struct MatrixTileNumericPolicyValidationResult;
	struct MatrixTileLayoutPolicyValidationResult;

	enum class MatrixTileProjectedOperationKind : unsigned char
	{
		Unspecified = 0,
		Load = 1,
		Store = 2,
		Macc = 3,
		Transpose = 4
	};

	enum class MatrixTileIrProjectionFaultKind : unsigned char
	{
		None = 0,
		NonMatrixTileOpcode = 1,
		MissingVectorPayload = 2,
		UnsupportedDataType = 3,
		InvalidShapeEncoding = 4,
		MemoryShapeFault = 5,
		MaccSemanticFault = 6,
		TransposeSemanticFault = 7,
		RuntimeOwnedVlmFault = 8,
		NumericPolicyFault = 9,
		LayoutPolicyFault = 10
	};

	struct MatrixTileInstructionIrProjection final
	{
		const InstructionsEnum Opcode;
		const std::wstring Mnemonic;
		const MatrixTileProjectedOperationKind OperationKind;
		const std::shared_ptr<VectorInstructionPayload> SourcePayload;
		const std::shared_ptr<MatrixTileCanonicalDescriptorAbi> TileDescriptor;
		const std::shared_ptr<MatrixTileCanonicalDescriptorAbi> SecondaryTileDescriptor;
		const std::shared_ptr<MatrixTileCanonicalDescriptorAbi> ResultTileDescriptor;
		const std::optional<std::shared_ptr<MatrixTileMemoryShapeContract>> MemoryContract;
		const std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> MemoryValidation;
		const std::optional<std::shared_ptr<MatrixTileMaccSemanticContract>> MaccContract;
		const std::optional<std::shared_ptr<MatrixTileTransposeSemanticContract>> TransposeContract;
		const std::optional<std::shared_ptr<MatrixTileSemanticValidationResult>> SemanticValidation;
		const std::shared_ptr<MatrixTileVlmLegalityValidationResult> VlmValidation;
		const MatrixTileIrProjectionFaultKind FaultKind;
		const std::wstring DescriptorValidationDecision;
		const bool HasTileDescriptorProjection;
		const bool HasMemoryOperandProjection;
		const bool HasAccumulatorOperandProjection;
		const bool HasTransposePolicyProjection;
		const bool PreservesDescriptorValidationResults;
		const bool UsesFallbackPath;
		const bool OpensExecution;
		std::optional<std::shared_ptr<MatrixTileNumericPolicyValidationResult>> NumericPolicyValidation;
		std::optional<std::shared_ptr<MatrixTileLayoutPolicyValidationResult>> LayoutPolicyValidation;
		const bool HasExplicitNumericPolicyProjection;
		const bool HasExplicitLayoutPolicyProjection;
		const bool IsRuntimeLegal;

		MatrixTileInstructionIrProjection(
			InstructionsEnum opcode,
			std::wstring mnemonic,
			MatrixTileProjectedOperationKind operationKind,
			std::shared_ptr<VectorInstructionPayload> sourcePayload,
			std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor,
			std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor,
			std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor,
			std::optional<std::shared_ptr<MatrixTileMemoryShapeContract>> memoryContract,
			std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> memoryValidation,
			std::optional<std::shared_ptr<MatrixTileMaccSemanticContract>> maccContract,
			std::optional<std::shared_ptr<MatrixTileTransposeSemanticContract>> transposeContract,
			std::optional<std::shared_ptr<MatrixTileSemanticValidationResult>> semanticValidation,
			std::shared_ptr<MatrixTileVlmLegalityValidationResult> vlmValidation,
			MatrixTileIrProjectionFaultKind faultKind,
			std::wstring descriptorValidationDecision,
			bool hasTileDescriptorProjection,
			bool hasMemoryOperandProjection,
			bool hasAccumulatorOperandProjection,
			bool hasTransposePolicyProjection,
			bool preservesDescriptorValidationResults,
			bool usesFallbackPath,
			bool opensExecution)
			: Opcode(opcode), Mnemonic(std::move(mnemonic)), OperationKind(operationKind),
			  SourcePayload(std::move(sourcePayload)), TileDescriptor(std::move(tileDescriptor)),
			  SecondaryTileDescriptor(std::move(secondaryTileDescriptor)), ResultTileDescriptor(std::move(resultTileDescriptor)),
			  MemoryContract(std::move(memoryContract)), MemoryValidation(std::move(memoryValidation)),
			  MaccContract(std::move(maccContract)), TransposeContract(std::move(transposeContract)),
			  SemanticValidation(std::move(semanticValidation)), VlmValidation(std::move(vlmValidation)),
			  FaultKind(faultKind), DescriptorValidationDecision(std::move(descriptorValidationDecision)),
			  HasTileDescriptorProjection(hasTileDescriptorProjection), HasMemoryOperandProjection(hasMemoryOperandProjection),
			  HasAccumulatorOperandProjection(hasAccumulatorOperandProjection), HasTransposePolicyProjection(hasTransposePolicyProjection),
			  PreservesDescriptorValidationResults(preservesDescriptorValidationResults), UsesFallbackPath(usesFallbackPath),
			  OpensExecution(opensExecution),
			  HasExplicitNumericPolicyProjection(operationKind != MatrixTileProjectedOperationKind::Macc ||
				  (MaccContract && *MaccContract && (*MaccContract)->HasExplicitNumericPolicy)),
			  HasExplicitLayoutPolicyProjection(operationKind == MatrixTileProjectedOperationKind::Macc
				  ? (MaccContract && *MaccContract && (*MaccContract)->HasExplicitLayoutPolicy)
				  : operationKind == MatrixTileProjectedOperationKind::Transpose
				      ? (TransposeContract && *TransposeContract && (*TransposeContract)->HasExplicitLayoutPolicy)
				      : true),
			  IsRuntimeLegal(faultKind == MatrixTileIrProjectionFaultKind::None && VlmValidation &&
				  VlmValidation->IsLegal && !usesFallbackPath && !opensExecution)
		{
		}
	};

	struct MatrixTileMaterializedInstruction final
	{
		const InstructionsEnum Opcode;
		const std::wstring Mnemonic;
		const MatrixTileProjectedOperationKind OperationKind;
		const std::shared_ptr<MatrixTileInstructionIrProjection> Projection;
		const std::shared_ptr<MatrixTileCanonicalDescriptorAbi> TileDescriptor;
		const std::shared_ptr<MatrixTileCanonicalDescriptorAbi> SecondaryTileDescriptor;
		const std::shared_ptr<MatrixTileCanonicalDescriptorAbi> ResultTileDescriptor;
		const bool IsTypedCloseToHSLRuntimeObject;
		const bool PublishesTypedTileMicroOp;
		const bool OpensExecution;
		const bool UsesFallbackPath;
		const bool IsRuntimeLegal;

		MatrixTileMaterializedInstruction(
			InstructionsEnum opcode,
			std::wstring mnemonic,
			MatrixTileProjectedOperationKind operationKind,
			std::shared_ptr<MatrixTileInstructionIrProjection> projection,
			std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor,
			std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor,
			std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor,
			bool isTypedCloseToHSLRuntimeObject,
			bool publishesTypedTileMicroOp,
			bool opensExecution,
			bool usesFallbackPath)
			: Opcode(opcode), Mnemonic(std::move(mnemonic)), OperationKind(operationKind),
			  Projection(std::move(projection)), TileDescriptor(std::move(tileDescriptor)),
			  SecondaryTileDescriptor(std::move(secondaryTileDescriptor)),
			  ResultTileDescriptor(std::move(resultTileDescriptor)),
			  IsTypedCloseToHSLRuntimeObject(isTypedCloseToHSLRuntimeObject),
			  PublishesTypedTileMicroOp(publishesTypedTileMicroOp), OpensExecution(opensExecution),
			  UsesFallbackPath(usesFallbackPath),
			  IsRuntimeLegal(Projection && Projection->IsRuntimeLegal && IsTypedCloseToHSLRuntimeObject &&
				  !PublishesTypedTileMicroOp && !OpensExecution && !UsesFallbackPath)
		{
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileInstructionIrProjection(InstructionsEnum Opcode, string Mnemonic, MatrixTileProjectedOperationKind OperationKind, VectorInstructionPayload SourcePayload, MatrixTileCanonicalDescriptorAbi TileDescriptor, MatrixTileCanonicalDescriptorAbi SecondaryTileDescriptor, MatrixTileCanonicalDescriptorAbi ResultTileDescriptor, System.Nullable<MatrixTileMemoryShapeContract> MemoryContract, System.Nullable<MatrixTileMemoryShapeValidationResult> MemoryValidation, System.Nullable<MatrixTileMaccSemanticContract> MaccContract, System.Nullable<MatrixTileTransposeSemanticContract> TransposeContract, System.Nullable<MatrixTileSemanticValidationResult> SemanticValidation, MatrixTileVlmLegalityValidationResult VlmValidation, MatrixTileIrProjectionFaultKind FaultKind, string DescriptorValidationDecision, bool HasTileDescriptorProjection, bool HasMemoryOperandProjection, bool HasAccumulatorOperandProjection, bool HasTransposePolicyProjection, bool PreservesDescriptorValidationResults, bool UsesFallbackPath, bool OpensExecution)
//	{
//		public System.Nullable<MatrixTileNumericPolicyValidationResult> NumericPolicyValidation
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<MatrixTileLayoutPolicyValidationResult> LayoutPolicyValidation
//		{
//			get;
//			set;
//		}
//
//		public bool HasExplicitNumericPolicyProjection
//		{
//			get
//			{
//				return OperationKind != MatrixTileProjectedOperationKind.Macc || MaccContract is { HasExplicitNumericPolicy: true };
//			}
//		}
//
//		public bool HasExplicitLayoutPolicyProjection
//		{
//			bool temp_var68;
//			switch (OperationKind)
//			{
//				case MatrixTileProjectedOperationKind.Macc:
//					temp_var68 = MaccContract is { HasExplicitLayoutPolicy: true };
//					break;
//				case MatrixTileProjectedOperationKind.Transpose:
//					temp_var68 = TransposeContract is { HasExplicitLayoutPolicy: true };
//					break;
//				default:
//					temp_var68 = true;
//					break;
//			}
//			return temp_var68;
//		}
//
//		public bool IsRuntimeLegal
//		{
//			get
//			{
//				return FaultKind == MatrixTileIrProjectionFaultKind.None && VlmValidation.IsLegal && !UsesFallbackPath && !OpensExecution;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileMaterializedInstruction(InstructionsEnum Opcode, string Mnemonic, MatrixTileProjectedOperationKind OperationKind, MatrixTileInstructionIrProjection Projection, MatrixTileCanonicalDescriptorAbi TileDescriptor, MatrixTileCanonicalDescriptorAbi SecondaryTileDescriptor, MatrixTileCanonicalDescriptorAbi ResultTileDescriptor, bool IsTypedCloseToHSLRuntimeObject, bool PublishesTypedTileMicroOp, bool OpensExecution, bool UsesFallbackPath)
//	{
//		public bool IsRuntimeLegal
//		{
//			get
//			{
//				return Projection.IsRuntimeLegal && IsTypedCloseToHSLRuntimeObject && !PublishesTypedTileMicroOp && !OpensExecution && !UsesFallbackPath;
//			}
//		}
//	}

	class MatrixTileIrProjectionAndMaterializer final
	{
	public:
		static inline const std::wstring IrProjectionMaterializerDecision = L"ClosedMatrixTileIrProjectionAndMaterializer";
		static inline const std::wstring TileDescriptorProjectionDecision = L"ClosedInstructionIrTileDescriptorProjection";
		static inline const std::wstring MemoryOperandProjectionDecision = L"ClosedMatrixTileMemoryOperandProjection";
		static inline const std::wstring AccumulatorProjectionDecision = L"ClosedMatrixTileAccumulatorOperandProjection";
		static inline const std::wstring TransposeProjectionDecision = L"ClosedMatrixTileTransposePolicyProjection";
		static inline const std::wstring RegistryEntryDecision = L"ClosedMatrixTileRegistryEntries";
		static inline const std::wstring MaterializerFactoryDecision = L"ClosedMatrixTileMaterializerFactories";
		static inline const std::wstring TypedObjectDecision = L"ClosedMaterializedTypedCloseToHSLInstructionObjects";
		static inline const std::wstring InvalidIrDecision = L"ClosedInvalidTileIrProjectionFaultAbi";
		static inline const std::wstring CompilerIrBoundaryDecision = L"CompilerIrIsNotRuntimeProjectionEvidence";
		static inline const std::wstring FallbackDecision = L"NoScalarVectorDotDscLane7VmxOrBackendFallbackAuthority";

		static constexpr bool HasInstructionIrTileProjection = true;
		static constexpr bool HasTileDescriptorIrCarrier = true;
		static constexpr bool HasMemoryOperandProjection = true;
		static constexpr bool HasAccumulatorOperandProjection = true;
		static constexpr bool HasTransposePolicyProjection = true;
		static constexpr bool HasRegistryEntries = true;
		static constexpr bool HasMaterializerFactories = true;
		static constexpr bool HasMaterializedTypedRuntimeObjects = true;
		static constexpr bool HasDescriptorValidationResultPreservation = true;
		static constexpr bool HasInvalidIrProjectionFaults = true;

		/// <summary>
		/// Materializes the existing typed MatrixTile carrier from the immutable
		/// canonical decode handoff. The generated binding is consumed from the
		/// exact decoded slot; no opcode/provider/materializer lookup is performed.
		/// </summary>
		static std::shared_ptr<MatrixTileMicroOp> MaterializeCanonicalCarrier(std::shared_ptr<CanonicalDecodedInstruction> canonical, std::shared_ptr<DecodedInstruction> compatibilitySlot);

		/// <summary>
		/// Single typed MatrixTile carrier factory shared by the registry adapter
		/// and canonical handoff. Selection is based on the already-decoded typed
		/// operation kind, never on a second opcode registry.
		/// </summary>
		static std::shared_ptr<MatrixTileMicroOp> CreateTypedMicroOp(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction);
		static constexpr bool KeepsCompilerIrNonAuthority = true;
		static constexpr bool KeepsCompilerScopeClosed = true;
		static constexpr bool KeepsCompilerHandoffBlocked = true;
		static constexpr bool PublishesTypedTileMicroOp = false;
		static constexpr bool OpensExecution = false;
		static constexpr bool UsesFallbackPath = false;

		static bool TryProject(std::shared_ptr<InstructionIR> instruction, std::shared_ptr<MatrixTileInstructionIrProjection> &projection, MatrixTileIrProjectionFaultKind &faultKind);

		static std::shared_ptr<MatrixTileInstructionIrProjection> ProjectDecodedVectorPayload(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, long long immediate);

		static std::shared_ptr<MatrixTileInstructionIrProjection> ProjectDecodedVectorPayload(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, long long immediate, bool requireExplicitNumericPolicy);

		static bool TryMaterialize(std::shared_ptr<InstructionIR> instruction, std::shared_ptr<MatrixTileMaterializedInstruction> &materializedInstruction, MatrixTileIrProjectionFaultKind &faultKind);

		static bool TryMaterialize(std::shared_ptr<MatrixTileInstructionIrProjection> projection, std::shared_ptr<MatrixTileMaterializedInstruction> &materializedInstruction, MatrixTileIrProjectionFaultKind &faultKind);

	private:
		static std::shared_ptr<MatrixTileInstructionIrProjection> ProjectLoad(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor);

		static std::shared_ptr<MatrixTileInstructionIrProjection> ProjectStore(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor);

		static std::shared_ptr<MatrixTileInstructionIrProjection> ProjectMacc(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> leftDescriptor, DataTypeEnum dataType, MatrixTileNumericElementKind elementKind, bool requireExplicitNumericPolicy);

		static std::shared_ptr<MatrixTileInstructionIrProjection> ProjectTranspose(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> sourceDescriptor, bool requireExplicitLayoutPolicy);

		static std::shared_ptr<MatrixTileInstructionIrProjection> CreateProjection(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, MatrixTileProjectedOperationKind operationKind, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultDescriptor, std::optional<std::shared_ptr<MatrixTileMemoryShapeContract>> &memoryContract, std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> &memoryValidation, std::optional<std::shared_ptr<MatrixTileMaccSemanticContract>> &maccContract, std::optional<std::shared_ptr<MatrixTileTransposeSemanticContract>> &transposeContract, std::optional<std::shared_ptr<MatrixTileSemanticValidationResult>> &semanticValidation, std::shared_ptr<MatrixTileVlmLegalityValidationResult> vlmValidation, MatrixTileIrProjectionFaultKind faultKind, bool hasMemoryProjection, bool hasAccumulatorProjection, bool hasTransposeProjection);

		static std::shared_ptr<MatrixTileInstructionIrProjection> CreateFaultProjection(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, MatrixTileProjectedOperationKind operationKind, MatrixTileIrProjectionFaultKind faultKind, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor = nullptr);

		static bool TryCreatePrimaryDescriptor(std::shared_ptr<VectorInstructionPayload> payload, long long immediate, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> &descriptor);

		static bool TryGetShape(std::shared_ptr<VectorInstructionPayload> payload, long long immediate, unsigned short elementSizeBytes, unsigned short &rows, unsigned short &columns, unsigned int &strideBytes);

		static bool TryGetDataType(unsigned char rawDataType, DataTypeEnum &dataType, MatrixTileNumericElementKind &elementKind);

		static bool IsKnownProjectionDataType(DataTypeEnum dataType);

		static MatrixTileProjectedOperationKind GetOperationKind(InstructionsEnum opcode);

		static MatrixTileIrProjectionFaultKind ToProjectionFault(std::shared_ptr<MatrixTileVlmLegalityValidationResult> validation);

		static unsigned int GetCanonicalStrideBytes(unsigned short columns, unsigned short elementSizeBytes);

		static unsigned short GetTileId(unsigned long long pointerOrTileToken);

		static std::wstring GetMnemonic(InstructionsEnum opcode);
	};

}
