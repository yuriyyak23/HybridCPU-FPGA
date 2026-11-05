#include "MatrixTileIrProjectionAndMaterializer.h"
#include "../../../../../Decoder/DecodedInstruction.h"
#include "../../../../../Pipeline/MicroOps/MatrixTile/MatrixTileMicroOps.h"
#include "../../../../../Pipeline/MicroOps/IR/InstructionIR.h"
#include "../../../../../Diagnostics/DecodeProjectionFaultException.h"
#include "MatrixTileRuntimeOwnedVlmRows.h"
#include "MatrixTileNumericPolicyAbi.h"
#include "MatrixTileLayoutPolicyAbi.h"
#include "MatrixTileMemoryShapeAndFaultAbi.h"
#include "MatrixTileArchitecturalTileStateAndDescriptorAbi.h"
#include "../../../../../../../Arch/DataType.h"
#include "../../../../../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	namespace
	{
		template <typename T>
		std::optional<std::shared_ptr<T>> OptionalShared(const std::shared_ptr<T> &value)
		{
			if (!value)
				return std::nullopt;
			return value;
		}
	}

	std::shared_ptr<MatrixTileMicroOp> MatrixTileIrProjectionAndMaterializer::MaterializeCanonicalCarrier(std::shared_ptr<CanonicalDecodedInstruction> canonical, std::shared_ptr<DecodedInstruction> compatibilitySlot)
	{
		ArgumentNullException::ThrowIfNull(canonical);
		ArgumentNullException::ThrowIfNull(compatibilitySlot);

		if (!canonical->StaticBinding)
		{
		throw InvalidOperationException(L"Canonical MatrixTile materialization requires the generated static binding carried by decode.");
		}
		const auto &binding = canonical->StaticBinding;
		std::shared_ptr<InstructionIR> instruction = compatibilitySlot->RequireInstruction();
		if (!canonical->IsOccupied || compatibilitySlot->SlotIndex != canonical->SlotIndex || binding->Opcode != canonical->Opcode || !instruction->CanonicalOpcode || static_cast<unsigned int>(instruction->CanonicalOpcode->Value) != canonical->Opcode || instruction->Class != canonical->InstructionClass || instruction->SerializationClass != canonical->SerializationClass)
		{
			throw InvalidOperationException(L"Canonical MatrixTile materialization received mismatched decoded slot identity or binding.");
		}

		if (!instruction->MatrixTileProjection || !instruction->MatrixTileProjection.value())
		{
		throw DecodeProjectionFaultException(L"Canonical MatrixTile materialization requires the decoder-published typed projection.");
		}
		const auto &projection = instruction->MatrixTileProjection.value();
		if (static_cast<unsigned int>(projection->Opcode) != canonical->Opcode)
		{
			throw DecodeProjectionFaultException(L"Canonical MatrixTile materialization received a projection for a different opcode.");
		}

		std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction;
		MatrixTileIrProjectionFaultKind faultKind;
		if (!TryMaterialize(projection, materializedInstruction, faultKind))
		{
			throw DecodeProjectionFaultException(std::format(L"Canonical MatrixTile materialization rejected the decoded projection: {}.", static_cast<unsigned int>(faultKind)));
		}

		std::shared_ptr<MatrixTileMicroOp> carrier = CreateTypedMicroOp(materializedInstruction);
		carrier->InitializeMetadata();
		return carrier;
	}

	std::shared_ptr<MatrixTileMicroOp> MatrixTileIrProjectionAndMaterializer::CreateTypedMicroOp(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction)
	{
		if (!materializedInstruction)
			throw ArgumentNullException(L"materializedInstruction");
		switch (materializedInstruction->OperationKind)
		{
			case MatrixTileProjectedOperationKind::Load:
				return std::make_shared<MtileLoadMicroOp>(materializedInstruction);
			case MatrixTileProjectedOperationKind::Store:
				return std::make_shared<MtileStoreMicroOp>(materializedInstruction);
			case MatrixTileProjectedOperationKind::Macc:
				return std::make_shared<MtileMaccMicroOp>(materializedInstruction);
			case MatrixTileProjectedOperationKind::Transpose:
				return std::make_shared<MtransposeMicroOp>(materializedInstruction);
			default:
				throw DecodeProjectionFaultException(std::format(L"Unsupported MTILE materialized operation kind {}.", static_cast<unsigned int>(materializedInstruction->OperationKind)));
		}
	}

	bool MatrixTileIrProjectionAndMaterializer::TryProject(std::shared_ptr<InstructionIR> instruction, std::shared_ptr<MatrixTileInstructionIrProjection> &projection, MatrixTileIrProjectionFaultKind &faultKind)
	{
		InstructionsEnum opcode = instruction->CanonicalOpcode->ToInstructionsEnum();
		if (!MatrixTileRuntimeOwnedVlmRows::IsMatrixTileOpcode(opcode))
		{
			projection = nullptr;
			faultKind = MatrixTileIrProjectionFaultKind::NonMatrixTileOpcode;
			return false;
		}

		if (!instruction->VectorPayload.has_value() || !instruction->VectorPayload.value())
		{
			projection = nullptr;
			faultKind = MatrixTileIrProjectionFaultKind::MissingVectorPayload;
			return false;
		}

		projection = ProjectDecodedVectorPayload(opcode, instruction->VectorPayload.value(), instruction->Imm, true);
		faultKind = projection->FaultKind;
		return true;
	}

	std::shared_ptr<MatrixTileInstructionIrProjection> MatrixTileIrProjectionAndMaterializer::ProjectDecodedVectorPayload(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, long long immediate)
	{
		return ProjectDecodedVectorPayload(opcode, payload, immediate, false);
	}

	std::shared_ptr<MatrixTileInstructionIrProjection> MatrixTileIrProjectionAndMaterializer::ProjectDecodedVectorPayload(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, long long immediate, bool requireExplicitNumericPolicy)
	{
		if (!MatrixTileRuntimeOwnedVlmRows::IsMatrixTileOpcode(opcode))
		{
			return CreateFaultProjection(opcode, payload, MatrixTileProjectedOperationKind::Unspecified, MatrixTileIrProjectionFaultKind::NonMatrixTileOpcode);
		}

		MatrixTileProjectedOperationKind operationKind = GetOperationKind(opcode);
		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor;
		if (!TryCreatePrimaryDescriptor(payload, immediate, descriptor))
		{
			return CreateFaultProjection(opcode, payload, operationKind, MatrixTileIrProjectionFaultKind::InvalidShapeEncoding);
		}

		DataTypeEnum dataType;
		MatrixTileNumericElementKind elementKind;
		if (!TryGetDataType(payload->DataType, dataType, elementKind))
		{
			return CreateFaultProjection(opcode, payload, operationKind, MatrixTileIrProjectionFaultKind::UnsupportedDataType, descriptor);
		}

		switch (opcode)
		{
			case InstructionsEnum::MTILE_LOAD:
				return ProjectLoad(opcode, payload, descriptor);
			case InstructionsEnum::MTILE_STORE:
				return ProjectStore(opcode, payload, descriptor);
			case InstructionsEnum::MTILE_MACC:
				return ProjectMacc(opcode, payload, descriptor, dataType, elementKind, requireExplicitNumericPolicy);
			case InstructionsEnum::MTRANSPOSE:
				return ProjectTranspose(opcode, payload, descriptor, requireExplicitNumericPolicy);
			default:
				return CreateFaultProjection(opcode, payload, operationKind, MatrixTileIrProjectionFaultKind::NonMatrixTileOpcode, descriptor);
		}
	}

	bool MatrixTileIrProjectionAndMaterializer::TryMaterialize(std::shared_ptr<InstructionIR> instruction, std::shared_ptr<MatrixTileMaterializedInstruction> &materializedInstruction, MatrixTileIrProjectionFaultKind &faultKind)
	{
		materializedInstruction = nullptr;
		std::shared_ptr<MatrixTileInstructionIrProjection> projection;
		if (!TryProject(instruction, projection, faultKind))
		{
			return false;
		}

		return TryMaterialize(projection, materializedInstruction, faultKind);
	}

	bool MatrixTileIrProjectionAndMaterializer::TryMaterialize(std::shared_ptr<MatrixTileInstructionIrProjection> projection, std::shared_ptr<MatrixTileMaterializedInstruction> &materializedInstruction, MatrixTileIrProjectionFaultKind &faultKind)
	{
		materializedInstruction = nullptr;
		faultKind = projection->FaultKind;
		if (!projection->IsRuntimeLegal)
		{
			if (faultKind == MatrixTileIrProjectionFaultKind::None)
			{
				faultKind = MatrixTileIrProjectionFaultKind::RuntimeOwnedVlmFault;
			}

			return false;
		}

		materializedInstruction = std::make_shared<MatrixTileMaterializedInstruction>(projection->Opcode, projection->Mnemonic, projection->OperationKind, projection, projection->TileDescriptor, projection->SecondaryTileDescriptor, projection->ResultTileDescriptor, true, false, false, false);
		faultKind = MatrixTileIrProjectionFaultKind::None;
		return true;
	}

	std::shared_ptr<MatrixTileInstructionIrProjection> MatrixTileIrProjectionAndMaterializer::ProjectLoad(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor)
	{
		std::shared_ptr<MatrixTileMemoryShapeContract> contract = MatrixTileMemoryShapeAndFaultAbi::CreateLoadContract(descriptor, payload->PrimaryPointer);
		std::shared_ptr<MatrixTileMemoryShapeValidationResult> memoryValidation = MatrixTileMemoryShapeAndFaultAbi::Validate(contract);
		std::shared_ptr<MatrixTileVlmLegalityValidationResult> vlmValidation = MatrixTileRuntimeOwnedVlmRows::ValidateLoad(contract);
		auto memoryContract = OptionalShared(contract);
		auto memoryValidationOptional = OptionalShared(memoryValidation);
		std::optional<std::shared_ptr<MatrixTileMaccSemanticContract>> maccContract;
		std::optional<std::shared_ptr<MatrixTileTransposeSemanticContract>> transposeContract;
		std::optional<std::shared_ptr<MatrixTileSemanticValidationResult>> semanticValidation;
		return CreateProjection(opcode, payload, MatrixTileProjectedOperationKind::Load, descriptor, nullptr, descriptor,
			memoryContract, memoryValidationOptional, maccContract, transposeContract, semanticValidation, vlmValidation,
			memoryValidation->isMemoryShapeAbiAccepted() ? ToProjectionFault(vlmValidation) : MatrixTileIrProjectionFaultKind::MemoryShapeFault,
			true, false, false);
	}

	std::shared_ptr<MatrixTileInstructionIrProjection> MatrixTileIrProjectionAndMaterializer::ProjectStore(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor)
	{
		std::shared_ptr<MatrixTileMemoryShapeContract> contract = MatrixTileMemoryShapeAndFaultAbi::CreateStoreContract(descriptor, payload->PrimaryPointer);
		std::shared_ptr<MatrixTileMemoryShapeValidationResult> memoryValidation = MatrixTileMemoryShapeAndFaultAbi::Validate(contract);
		std::shared_ptr<MatrixTileVlmLegalityValidationResult> vlmValidation = MatrixTileRuntimeOwnedVlmRows::ValidateStore(contract);
		auto memoryContract = OptionalShared(contract);
		auto memoryValidationOptional = OptionalShared(memoryValidation);
		std::optional<std::shared_ptr<MatrixTileMaccSemanticContract>> maccContract;
		std::optional<std::shared_ptr<MatrixTileTransposeSemanticContract>> transposeContract;
		std::optional<std::shared_ptr<MatrixTileSemanticValidationResult>> semanticValidation;
		return CreateProjection(opcode, payload, MatrixTileProjectedOperationKind::Store, descriptor, nullptr, descriptor,
			memoryContract, memoryValidationOptional, maccContract, transposeContract, semanticValidation, vlmValidation,
			memoryValidation->isMemoryShapeAbiAccepted() ? ToProjectionFault(vlmValidation) : MatrixTileIrProjectionFaultKind::MemoryShapeFault,
			true, false, false);
	}

	std::shared_ptr<MatrixTileInstructionIrProjection> MatrixTileIrProjectionAndMaterializer::ProjectMacc(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> leftDescriptor, DataTypeEnum dataType, MatrixTileNumericElementKind elementKind, bool requireExplicitNumericPolicy)
	{
		auto layoutPolicy = OptionalShared(payload->MatrixTileLayoutPolicy);
		const bool hasExplicitLayoutPolicy = layoutPolicy.has_value();
		if (!layoutPolicy.has_value() && !requireExplicitNumericPolicy)
		{
			layoutPolicy = MatrixTileLayoutPolicyAbi::CreateMaccPolicy();
		}

		std::shared_ptr<MatrixTileLayoutPolicyValidationResult> layoutValidation = MatrixTileLayoutPolicyAbi::Validate(layoutPolicy, MatrixTileProjectedOperationKind::Macc);
		if (!layoutValidation->isRuntimeOwnedLayoutPolicyAccepted())
		{
			auto failure = CreateFaultProjection(opcode, payload, MatrixTileProjectedOperationKind::Macc, MatrixTileIrProjectionFaultKind::LayoutPolicyFault, leftDescriptor);
			failure->LayoutPolicyValidation = layoutValidation;
			return failure;
		}

		auto numericPolicy = OptionalShared(payload->MatrixTileNumericPolicy);
		const bool hasExplicitNumericPolicy = numericPolicy.has_value();
		std::shared_ptr<MatrixTileNumericPolicy> legacyPolicy;
		if (!numericPolicy.has_value() && !requireExplicitNumericPolicy && MatrixTileNumericPolicyAbi::TryCreateLegacyPhase14Policy(dataType, legacyPolicy))
		{
			numericPolicy = legacyPolicy;
		}

		std::shared_ptr<MatrixTileNumericPolicyValidationResult> numericValidation = MatrixTileNumericPolicyAbi::Validate(numericPolicy);
		if (numericValidation->isRuntimeOwnedNumericPolicyAccepted() && numericPolicy.has_value() && numericPolicy.value() && numericPolicy.value()->ElementType != dataType)
		{
			numericValidation = MatrixTileNumericPolicyValidationResult::Fault(MatrixTileNumericPolicyFaultKind::ContradictoryElementType, numericValidation->Profile);
		}

		if (!numericValidation->isRuntimeOwnedNumericPolicyAccepted())
		{
			auto failure = CreateFaultProjection(opcode, payload, MatrixTileProjectedOperationKind::Macc, MatrixTileIrProjectionFaultKind::NumericPolicyFault, leftDescriptor);
			failure->NumericPolicyValidation = numericValidation;
			failure->LayoutPolicyValidation = layoutValidation;
			return failure;
		}

		if (!numericPolicy.has_value() || !numericPolicy.value())
		{
			auto failure = CreateFaultProjection(opcode, payload, MatrixTileProjectedOperationKind::Macc, MatrixTileIrProjectionFaultKind::NumericPolicyFault, leftDescriptor);
			failure->NumericPolicyValidation = numericValidation;
			failure->LayoutPolicyValidation = layoutValidation;
			return failure;
		}
		const int accumulatorElementSizeRaw = MatrixTileNumericPolicyAbi::GetElementSizeBytes(numericPolicy.value()->AccumulatorType);
		if (accumulatorElementSizeRaw <= 0 || accumulatorElementSizeRaw > std::numeric_limits<unsigned short>::max())
			throw std::overflow_error("MatrixTile accumulator element size is out of range.");
		const auto accumulatorElementSize = static_cast<unsigned short>(accumulatorElementSizeRaw);
		auto rightDescriptor = std::make_shared<MatrixTileCanonicalDescriptorAbi>(MatrixTileCanonicalDescriptorAbi::Create(leftDescriptor->Columns, leftDescriptor->Rows, leftDescriptor->ElementSizeBytes, GetCanonicalStrideBytes(leftDescriptor->Rows, leftDescriptor->ElementSizeBytes)));
		auto accumulatorDescriptor = std::make_shared<MatrixTileCanonicalDescriptorAbi>(MatrixTileCanonicalDescriptorAbi::Create(leftDescriptor->Rows, rightDescriptor->Columns, accumulatorElementSize, GetCanonicalStrideBytes(rightDescriptor->Columns, accumulatorElementSize)));
		auto contract = hasExplicitNumericPolicy
			? MatrixTileAccumulatorAndTransposePolicyAbi::CreateMaccContract(leftDescriptor, rightDescriptor, accumulatorDescriptor, numericPolicy.value())
			: MatrixTileAccumulatorAndTransposePolicyAbi::CreateMaccContract(leftDescriptor, rightDescriptor, accumulatorDescriptor, elementKind);
		contract->LayoutPolicy = layoutPolicy.value();
		contract->HasExplicitLayoutPolicy = hasExplicitLayoutPolicy;
		auto semanticValidation = requireExplicitNumericPolicy ? MatrixTileAccumulatorAndTransposePolicyAbi::ValidateRuntimeMacc(contract) : MatrixTileAccumulatorAndTransposePolicyAbi::ValidateMacc(contract);
		std::shared_ptr<MatrixTileVlmLegalityValidationResult> vlmValidation = MatrixTileRuntimeOwnedVlmRows::ValidateMacc(contract);
		std::optional<std::shared_ptr<MatrixTileMemoryShapeContract>> memoryContract;
		std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> memoryValidation;
		auto maccContract = OptionalShared(contract);
		std::optional<std::shared_ptr<MatrixTileTransposeSemanticContract>> transposeContract;
		auto semanticValidationOptional = OptionalShared(semanticValidation);
		auto result = CreateProjection(opcode, payload, MatrixTileProjectedOperationKind::Macc, leftDescriptor,
			rightDescriptor, accumulatorDescriptor, memoryContract, memoryValidation, maccContract, transposeContract,
			semanticValidationOptional, vlmValidation,
			semanticValidation->isSemanticAbiAccepted() ? ToProjectionFault(vlmValidation) : MatrixTileIrProjectionFaultKind::MaccSemanticFault,
			false, true, false);
		result->NumericPolicyValidation = numericValidation;
		result->LayoutPolicyValidation = layoutValidation;
		return result;

	}

	std::shared_ptr<MatrixTileInstructionIrProjection> MatrixTileIrProjectionAndMaterializer::ProjectTranspose(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> sourceDescriptor, bool requireExplicitLayoutPolicy)
	{
		auto layoutPolicy = OptionalShared(payload->MatrixTileLayoutPolicy);
		const bool hasExplicitLayoutPolicy = layoutPolicy.has_value();
		if (payload->MatrixTileNumericPolicy)
		{
			return CreateFaultProjection(opcode, payload, MatrixTileProjectedOperationKind::Transpose, MatrixTileIrProjectionFaultKind::NumericPolicyFault, sourceDescriptor);
		}

		if (!layoutPolicy.has_value() && !requireExplicitLayoutPolicy)
		{
			layoutPolicy = MatrixTileLayoutPolicyAbi::CreateTransposePolicy();
		}

		std::shared_ptr<MatrixTileLayoutPolicyValidationResult> layoutValidation = MatrixTileLayoutPolicyAbi::Validate(layoutPolicy, MatrixTileProjectedOperationKind::Transpose);
		if (!layoutValidation->isRuntimeOwnedLayoutPolicyAccepted())
		{
			auto failure = CreateFaultProjection(opcode, payload, MatrixTileProjectedOperationKind::Transpose, MatrixTileIrProjectionFaultKind::LayoutPolicyFault, sourceDescriptor);
			failure->LayoutPolicyValidation = layoutValidation;
			return failure;
		}

		auto destinationDescriptor = std::make_shared<MatrixTileCanonicalDescriptorAbi>(MatrixTileCanonicalDescriptorAbi::Create(sourceDescriptor->Columns, sourceDescriptor->Rows, sourceDescriptor->ElementSizeBytes, GetCanonicalStrideBytes(sourceDescriptor->Rows, sourceDescriptor->ElementSizeBytes)));
		auto contract = MatrixTileAccumulatorAndTransposePolicyAbi::CreateTransposeContract(sourceDescriptor, destinationDescriptor, GetTileId(payload->PrimaryPointer), GetTileId(payload->SecondaryPointer));
		contract->LayoutPolicy = layoutPolicy.value();
		contract->HasExplicitLayoutPolicy = hasExplicitLayoutPolicy;
		std::shared_ptr<MatrixTileSemanticValidationResult> semanticValidation = requireExplicitLayoutPolicy ? MatrixTileAccumulatorAndTransposePolicyAbi::ValidateRuntimeTranspose(contract) : MatrixTileAccumulatorAndTransposePolicyAbi::ValidateTranspose(contract);
		std::shared_ptr<MatrixTileVlmLegalityValidationResult> vlmValidation = MatrixTileRuntimeOwnedVlmRows::ValidateTranspose(contract);

		std::optional<std::shared_ptr<MatrixTileMemoryShapeContract>> memoryContract;
		std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> memoryValidation;
		std::optional<std::shared_ptr<MatrixTileMaccSemanticContract>> maccContract;
		auto transposeContract = OptionalShared(contract);
		auto semanticValidationOptional = OptionalShared(semanticValidation);
		auto result = CreateProjection(opcode, payload, MatrixTileProjectedOperationKind::Transpose, sourceDescriptor,
			nullptr, destinationDescriptor, memoryContract, memoryValidation, maccContract, transposeContract,
			semanticValidationOptional, vlmValidation,
			semanticValidation->isSemanticAbiAccepted() ? ToProjectionFault(vlmValidation) : MatrixTileIrProjectionFaultKind::TransposeSemanticFault,
			false, false, true);
		result->LayoutPolicyValidation = layoutValidation;
		return result;
	}

	std::shared_ptr<MatrixTileInstructionIrProjection> MatrixTileIrProjectionAndMaterializer::CreateProjection(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, MatrixTileProjectedOperationKind operationKind, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultDescriptor, std::optional<std::shared_ptr<MatrixTileMemoryShapeContract>> &memoryContract, std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> &memoryValidation, std::optional<std::shared_ptr<MatrixTileMaccSemanticContract>> &maccContract, std::optional<std::shared_ptr<MatrixTileTransposeSemanticContract>> &transposeContract, std::optional<std::shared_ptr<MatrixTileSemanticValidationResult>> &semanticValidation, std::shared_ptr<MatrixTileVlmLegalityValidationResult> vlmValidation, MatrixTileIrProjectionFaultKind faultKind, bool hasMemoryProjection, bool hasAccumulatorProjection, bool hasTransposeProjection)
	{
		return std::make_shared<MatrixTileInstructionIrProjection>(opcode, GetMnemonic(opcode), operationKind, payload, descriptor, secondaryDescriptor, resultDescriptor, memoryContract, memoryValidation, maccContract, transposeContract, semanticValidation, vlmValidation, faultKind, MatrixTileArchitecturalTileStateAndDescriptorAbi::ValidateDescriptor(descriptor), descriptor && descriptor->isCanonical(), hasMemoryProjection, hasAccumulatorProjection, hasTransposeProjection, true, false, false);
	}

	std::shared_ptr<MatrixTileInstructionIrProjection> MatrixTileIrProjectionAndMaterializer::CreateFaultProjection(InstructionsEnum opcode, std::shared_ptr<VectorInstructionPayload> payload, MatrixTileProjectedOperationKind operationKind, MatrixTileIrProjectionFaultKind faultKind, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor)
	{
		std::shared_ptr<MatrixTileVlmLegalityValidationResult> vlmValidation = MatrixTileVlmLegalityValidationResult::Fault(MatrixTileRuntimeOwnedVlmRows::IsMatrixTileOpcode(opcode) ? opcode : InstructionsEnum::MTILE_LOAD, faultKind == MatrixTileIrProjectionFaultKind::NonMatrixTileOpcode ? MatrixTileVlmLegalityFaultKind::NonMatrixTileOpcode : MatrixTileVlmLegalityFaultKind::NonDescriptorBackedContour);

		return std::make_shared<MatrixTileInstructionIrProjection>(opcode, GetMnemonic(opcode), operationKind, payload, descriptor, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, vlmValidation, faultKind, descriptor ? MatrixTileArchitecturalTileStateAndDescriptorAbi::ValidateDescriptor(descriptor) : L"NoMatrixTileDescriptorProjected", descriptor && descriptor->isCanonical(), operationKind == MatrixTileProjectedOperationKind::Load || operationKind == MatrixTileProjectedOperationKind::Store, operationKind == MatrixTileProjectedOperationKind::Macc, operationKind == MatrixTileProjectedOperationKind::Transpose, true, false, false);
	}

	bool MatrixTileIrProjectionAndMaterializer::TryCreatePrimaryDescriptor(std::shared_ptr<VectorInstructionPayload> payload, long long immediate, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> &descriptor)
	{
		descriptor = nullptr;
		DataTypeEnum dataType;
		MatrixTileNumericElementKind ignoredElementKind;
		if (!TryGetDataType(payload->DataType, dataType, ignoredElementKind))
		{
			unsigned short rows = 1;
			unsigned short columns = payload->StreamLength <= std::numeric_limits<unsigned short>::max() ? static_cast<unsigned short>(payload->StreamLength) : static_cast<unsigned short>(0);
			descriptor = std::make_shared<MatrixTileCanonicalDescriptorAbi>(MatrixTileCanonicalDescriptorAbi::Create(rows, columns, 0, payload->Stride));
			return true;
		}

		const int elementSizeRaw = DataTypeUtils::SizeOf(dataType);
		if (elementSizeRaw <= 0 || elementSizeRaw > std::numeric_limits<unsigned short>::max())
			return false;
		unsigned short elementSize = static_cast<unsigned short>(elementSizeRaw);
		unsigned short rowsValue;
		unsigned short columnsValue;
		unsigned int strideBytes;
		if (!TryGetShape(payload, immediate, elementSize, rowsValue, columnsValue, strideBytes))
		{
			return false;
		}

		descriptor = std::make_shared<MatrixTileCanonicalDescriptorAbi>(MatrixTileCanonicalDescriptorAbi::Create(rowsValue, columnsValue, elementSize, strideBytes));
		return true;
	}

	bool MatrixTileIrProjectionAndMaterializer::TryGetShape(std::shared_ptr<VectorInstructionPayload> payload, long long immediate, unsigned short elementSizeBytes, unsigned short &rows, unsigned short &columns, unsigned int &strideBytes)
	{
		rows = 0;
		columns = 0;
		strideBytes = 0;

		if (payload->StreamLength == 0 || payload->StreamLength > std::numeric_limits<unsigned short>::max())
		{
			return false;
		}

		if (payload->Is2D)
		{
			if (immediate <= 0 || immediate > std::numeric_limits<unsigned short>::max() || payload->StreamLength % static_cast<unsigned int>(immediate) != 0)
			{
				return false;
			}

			unsigned int rowCount = payload->StreamLength / static_cast<unsigned int>(immediate);
			if (rowCount == 0 || rowCount > std::numeric_limits<unsigned short>::max())
			{
				return false;
			}

			rows = static_cast<unsigned short>(rowCount);
			columns = static_cast<unsigned short>(immediate);
			strideBytes = payload->RowStride != 0 ? payload->RowStride : GetCanonicalStrideBytes(columns, elementSizeBytes);
			return true;
		}

		rows = 1;
		columns = static_cast<unsigned short>(payload->StreamLength);
		strideBytes = payload->Stride != 0 ? payload->Stride : GetCanonicalStrideBytes(columns, elementSizeBytes);
		return true;
	}

	bool MatrixTileIrProjectionAndMaterializer::TryGetDataType(unsigned char rawDataType, DataTypeEnum &dataType, MatrixTileNumericElementKind &elementKind)
	{
		dataType = static_cast<DataTypeEnum>(rawDataType);
		elementKind = MatrixTileNumericElementKind::Unspecified;
		if (!IsKnownProjectionDataType(dataType))
		{
			return false;
		}

		if (DataTypeUtils::IsSignedInteger(dataType))
		{
			elementKind = MatrixTileNumericElementKind::SignedInteger;
		}
		else if (DataTypeUtils::IsUnsignedInteger(dataType))
		{
			elementKind = MatrixTileNumericElementKind::UnsignedInteger;
		}
		else if (DataTypeUtils::IsFloatingPoint(dataType))
		{
			elementKind = MatrixTileNumericElementKind::FloatingPoint;
		}

		return true;
	}

	bool MatrixTileIrProjectionAndMaterializer::IsKnownProjectionDataType(DataTypeEnum dataType)
	{
		try
		{
        int ignoredSize = DataTypeUtils::SizeOf(dataType);
			return true;
		}
		catch (const ArgumentOutOfRangeException &e1)
		{
			return false;
		}
	}

	MatrixTileProjectedOperationKind MatrixTileIrProjectionAndMaterializer::GetOperationKind(InstructionsEnum opcode)
	{
		switch (opcode)
		{
			case InstructionsEnum::MTILE_LOAD: return MatrixTileProjectedOperationKind::Load;
			case InstructionsEnum::MTILE_STORE: return MatrixTileProjectedOperationKind::Store;
			case InstructionsEnum::MTILE_MACC: return MatrixTileProjectedOperationKind::Macc;
			case InstructionsEnum::MTRANSPOSE: return MatrixTileProjectedOperationKind::Transpose;
			default: return MatrixTileProjectedOperationKind::Unspecified;
		}
	}

	MatrixTileIrProjectionFaultKind MatrixTileIrProjectionAndMaterializer::ToProjectionFault(std::shared_ptr<MatrixTileVlmLegalityValidationResult> validation)
	{
		return validation->IsLegal ? MatrixTileIrProjectionFaultKind::None : MatrixTileIrProjectionFaultKind::RuntimeOwnedVlmFault;
	}

	unsigned int MatrixTileIrProjectionAndMaterializer::GetCanonicalStrideBytes(unsigned short columns, unsigned short elementSizeBytes)
	{
		return static_cast<unsigned int>(columns) * elementSizeBytes;
	}

	unsigned short MatrixTileIrProjectionAndMaterializer::GetTileId(unsigned long long pointerOrTileToken)
	{
		return static_cast<unsigned short>(pointerOrTileToken & std::numeric_limits<unsigned short>::max());
	}

	std::wstring MatrixTileIrProjectionAndMaterializer::GetMnemonic(InstructionsEnum opcode)
	{
		std::wstring mnemonic;
		return OpcodeRegistry::TryGetMnemonic(static_cast<unsigned int>(opcode), mnemonic)
			? mnemonic
			: std::format(L"{}", static_cast<unsigned int>(opcode));
	}
}
