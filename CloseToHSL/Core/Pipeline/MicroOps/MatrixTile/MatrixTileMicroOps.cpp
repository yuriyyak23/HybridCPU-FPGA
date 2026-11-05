#include "MatrixTileMicroOps.h"
#include "../../../Diagnostics/DecodeProjectionFaultException.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../../../../NonRTL/Arch/InstructionClass.h"
#include "../../../../../NonRTL/Arch/SerializationClass.h"
#include "../../../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTilePolicyBoundIdentityAbi.h"
#include "../../../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileRetirePublicationAbi.h"
#include "../../../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileStreamTransferAbi.h"
#include "../../../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileExecuteCaptureAbi.h"
#include "../../../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../../../NonRTL/Arch/OpcodeInfo.Types.h"
#include "../IR/InstructionIR.h"
#include "../../../Architecture/Common/StreamEngineId.h"

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		std::shared_ptr<MatrixTileMaterializedInstruction> ValidateMaterializedInstruction(
			std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction,
			MatrixTileProjectedOperationKind expectedOperationKind)
		{
			if (!materializedInstruction ||
				materializedInstruction->OperationKind != expectedOperationKind ||
				!materializedInstruction->IsRuntimeLegal)
			{
				const auto opcode = materializedInstruction
					? static_cast<unsigned int>(materializedInstruction->Opcode)
					: 0u;
				throw DecodeProjectionFaultException(
					L"MTILE Phase08 MicroOp factory rejected opcode " + std::to_wstring(opcode) +
					L" because the Phase07 materialized runtime object is not a legal " +
					std::to_wstring(static_cast<unsigned int>(expectedOperationKind)) + L" carrier.");
			}
			return materializedInstruction;
		}
	}

	MatrixTileMicroOp::MatrixTileMicroOp(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction, MatrixTileProjectedOperationKind expectedOperationKind)
		: MaterializedInstruction(ValidateMaterializedInstruction(std::move(materializedInstruction), expectedOperationKind)),
		  Projection(MaterializedInstruction->Projection),
		  OperationKind(expectedOperationKind),
		  RuntimeResourceClass(MatrixTileResourceContour::Classify(MaterializedInstruction->Opcode)),
		  DependencyMetadata(CreateDependencyMetadata(Projection))
	{
		setOpCode(static_cast<unsigned int>(MaterializedInstruction->Opcode));
		setPredicateMask(getProjection()->SourcePayload->PredicateMask);
		if (getRuntimeResourceClass() == MatrixTileRuntimeResourceClass::MatrixTileMemory)
		{
			SetHardPinnedPlacement(SlotClass::MatrixTileStreamClass, MatrixTileResourceContour::TileStreamLaneId);
		}
		else
		{
			SetClassFlexiblePlacement(MatrixTileResourceContour::ResolveSlotClass(getRuntimeResourceClass()));
		}
	}

	std::shared_ptr<MatrixTileMaterializedInstruction> MatrixTileMicroOp::getMaterializedInstruction() const
	{
		return MaterializedInstruction;
	}

	std::shared_ptr<MatrixTileInstructionIrProjection> MatrixTileMicroOp::getProjection() const
	{
		return Projection;
	}

	MatrixTileProjectedOperationKind MatrixTileMicroOp::getOperationKind() const
	{
		return OperationKind;
	}

	MatrixTileRuntimeResourceClass MatrixTileMicroOp::getRuntimeResourceClass() const
	{
		return RuntimeResourceClass;
	}

	std::shared_ptr<MatrixTileMicroOpDependencyMetadata> MatrixTileMicroOp::getDependencyMetadata() const
	{
		return DependencyMetadata;
	}

	std::shared_ptr<MatrixTileCanonicalDescriptorAbi> MatrixTileMicroOp::getTileDescriptor() const
	{
		return getMaterializedInstruction()->TileDescriptor;
	}

	std::shared_ptr<MatrixTileCanonicalDescriptorAbi> MatrixTileMicroOp::getSecondaryTileDescriptor() const
	{
		return getMaterializedInstruction()->SecondaryTileDescriptor;
	}

	std::shared_ptr<MatrixTileCanonicalDescriptorAbi> MatrixTileMicroOp::getResultTileDescriptor() const
	{
		return getMaterializedInstruction()->ResultTileDescriptor;
	}

	std::optional<std::shared_ptr<MatrixTileMemoryShapeContract>> MatrixTileMicroOp::getMemoryContract() const
	{
		return getProjection()->MemoryContract;
	}

	std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> MatrixTileMicroOp::getMemoryValidation() const
	{
		return getProjection()->MemoryValidation;
	}

	std::optional<std::shared_ptr<MatrixTileMaccSemanticContract>> MatrixTileMicroOp::getMaccContract() const
	{
		return getProjection()->MaccContract;
	}

	std::optional<std::shared_ptr<MatrixTileTransposeSemanticContract>> MatrixTileMicroOp::getTransposeContract() const
	{
		return getProjection()->TransposeContract;
	}

	bool MatrixTileMicroOp::getPublishesTypedTileMicroOp() const
	{
		return true;
	}

	bool MatrixTileMicroOp::getPublishesSchedulerLaneBinding() const
	{
		return true;
	}

	bool MatrixTileMicroOp::getPublishesIssueConstraints() const
	{
		return true;
	}

	bool MatrixTileMicroOp::getPublishesCaptureBarriers() const
	{
		return true;
	}

	bool MatrixTileMicroOp::getOpensExecution() const
	{
		return true;
	}

	bool MatrixTileMicroOp::getPublishesExecutionCaptureSemantics() const
	{
		return true;
	}

	std::optional<std::shared_ptr<MatrixTileExecutionCaptureRecord>> MatrixTileMicroOp::getLastExecutionCapture() const
	{
		if (!_lastExecutionCapture || !_lastExecutionCapture.value())
		{
			return std::nullopt;
		}
		return std::make_shared<MatrixTileExecutionCaptureRecord>(
			_lastExecutionCapture.value()->DeepClone());
	}

	std::optional<std::shared_ptr<MatrixTileRetireOutcome>> MatrixTileMicroOp::getLastRetireOutcome() const
	{
		return _lastRetireOutcome;
	}

	std::shared_ptr<MatrixTileReplayRollbackJournal> MatrixTileMicroOp::getLastReplayRollbackJournal() const
	{
		return _lastReplayRollbackJournal;
	}

	MatrixTileCaptureLifecycle MatrixTileMicroOp::getCaptureLifecycle() const
	{
		return _captureLifecycle;
	}

	bool MatrixTileMicroOp::getUsesFallbackPath() const
	{
		return false;
	}

	unsigned char MatrixTileMicroOp::getSchedulerLaneMask() const
	{
		return SlotClassLaneMap::GetLaneMask(getPlacement().RequiredSlotClass);
	}

	CanonicalDecodePublicationMode MatrixTileMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	void MatrixTileMicroOp::InitializeMetadata()
	{
		setReadRegisters(std::vector<int>{});
		setWriteRegisters(std::vector<int>{});
		setReadMemoryRanges(BuildMemoryRanges(true));
		setWriteMemoryRanges(BuildMemoryRanges(false));
		setWritesRegister(false);
		setControlFlow(false);
		switch (getRuntimeResourceClass())
		{
		case MatrixTileRuntimeResourceClass::MatrixTileMemory:
			setClass(MicroOpClass::MatrixTileMemory);
			break;
		case MatrixTileRuntimeResourceClass::MatrixTileCompute:
			setClass(MicroOpClass::MatrixTileCompute);
			break;
		default:
			throw DecodeProjectionFaultException(
				getProjection()->Mnemonic + L" has no MatrixTile runtime resource classification.");
		}
		const auto operationKind = getOperationKind();
		const bool isMemoryOperation = operationKind == MatrixTileProjectedOperationKind::Load ||
			operationKind == MatrixTileProjectedOperationKind::Store;
		setInstructionClass(isMemoryOperation ? InstructionClass::Memory : InstructionClass::ScalarAlu);
		setSerializationClass(operationKind == MatrixTileProjectedOperationKind::Store
			? SerializationClass::MemoryOrdered
			: SerializationClass::Free);
		setMemoryOp(isMemoryOperation);
		setHasSideEffects(true);
		setLatency(GetPublishedLatency());
		setResourceMask(BuildResourceMask());
		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata(shared_from_this());
	}
	bool MatrixTileMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		EnsureNoUnretiredCapture();
		std::shared_ptr<MatrixTileExecutionCaptureRecord> capture;
		switch (getOperationKind())
		{
		case MatrixTileProjectedOperationKind::Load:
			capture = CaptureLoad(core);
			break;
		case MatrixTileProjectedOperationKind::Store:
			capture = CaptureStore(core);
			break;
		case MatrixTileProjectedOperationKind::Macc:
			capture = CaptureMacc(core);
			break;
		case MatrixTileProjectedOperationKind::Transpose:
			capture = CaptureTranspose(core);
			break;
		default:
		{
			std::optional<std::shared_ptr<MatrixTileMemoryShapeContract>> noMemoryContract;
			std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> noMemoryValidation;
			capture = MatrixTileExecuteCaptureAbi::CaptureLoad(
				getProjection()->Mnemonic,
				getOperationKind(),
				getDependencyMetadata()->SourceTileId,
				getDependencyMetadata()->SecondaryTileId,
				getDependencyMetadata()->DestinationTileId,
				getTileDescriptor(),
				getSecondaryTileDescriptor(),
				getResultTileDescriptor(),
				noMemoryContract,
				noMemoryValidation,
				[](unsigned long long, int) -> std::optional<std::vector<unsigned char>>
				{
					return std::nullopt;
				});
			break;
		}
		}
		if (!capture)
		{
			throw DecodeProjectionFaultException(
				getProjection()->Mnemonic + L" produced no MatrixTile execution capture.");
		}

		const unsigned long long captureOrdinal = core->AllocateMatrixTileCaptureOrdinal();
		capture = MatrixTilePolicyBoundIdentityAbi::Bind(
			capture,
			getMaterializedInstruction(),
			getDependencyMetadata(),
			getOwnerThreadId(),
			getOwnerThreadId(),
			core->GetMatrixTileReplayInvalidationEpoch());
		_activeCaptureIdentity = MatrixTileRetirePublicationAbi::CreateCaptureIdentity(
			core->getCoreID(), getOwnerThreadId(), getOpCode(), getOperationKind(), captureOrdinal, capture);
		auto publishedCapture = std::make_shared<MatrixTileExecutionCaptureRecord>(capture->DeepClone());
		publishedCapture->CaptureIdentity = *_activeCaptureIdentity;
		_lastExecutionCapture = std::move(publishedCapture);
		_lastRetireOutcome = std::nullopt;
		_captureLifecycle = MatrixTileCaptureLifecycle::Captured;
		return true;
	}
	std::shared_ptr<MatrixTileRetireOutcome> MatrixTileMicroOp::RetireCapturedResult(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture)
	{
		if (_captureLifecycle != MatrixTileCaptureLifecycle::Captured || !_lastExecutionCapture)
		{
			throw MatrixTileRetireValidationException(
				getProjection()->Mnemonic + L" retire requires one live, uncancelled Phase09 capture.");
		}

		std::shared_ptr<MatrixTileReplayRollbackJournal> journal = MatrixTileReplayRollbackAbi::RetireWithCheckpoint(core, getMaterializedInstruction(), capture, _activeCaptureIdentity, core->AllocateMatrixTileReplayCheckpointOrdinal());
		std::shared_ptr<MatrixTileRetireOutcome> outcome = journal->getOriginalRetireOutcome();
		_lastReplayRollbackJournal = journal;

		_lastRetireOutcome = outcome;
		_lastExecutionCapture = std::nullopt;
		_activeCaptureIdentity.reset();
		_captureLifecycle = outcome->FaultRetired ? MatrixTileCaptureLifecycle::FaultRetired : MatrixTileCaptureLifecycle::Retired;
		return outcome;
	}

	std::shared_ptr<MatrixTileRollbackOutcome> MatrixTileMicroOp::RollbackRetiredResult(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileReplayIdentity> expectedIdentity)
	{
		std::shared_ptr<MatrixTileReplayRollbackJournal> journal = RequireReplayJournal();
		std::shared_ptr<MatrixTileRollbackOutcome> outcome = MatrixTileReplayRollbackAbi::Rollback(core, journal, expectedIdentity);
		_captureLifecycle = MatrixTileCaptureLifecycle::RolledBack;
		return outcome;
	}

	std::shared_ptr<MatrixTileRetireOutcome> MatrixTileMicroOp::ReplayRolledBackResult(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileReplayIdentity> expectedIdentity)
	{
		std::shared_ptr<MatrixTileReplayRollbackJournal> journal = RequireReplayJournal();
		std::shared_ptr<MatrixTileRetireOutcome> outcome = MatrixTileReplayRollbackAbi::Replay(core, getMaterializedInstruction(), journal, expectedIdentity);
		_lastRetireOutcome = outcome;
		_captureLifecycle = outcome->FaultRetired ? MatrixTileCaptureLifecycle::FaultReplayed : MatrixTileCaptureLifecycle::Replayed;
		return outcome;
	}

	void MatrixTileMicroOp::DiscardReplayRollbackAuthority(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileReplayIdentity> expectedIdentity)
	{
		std::shared_ptr<MatrixTileReplayRollbackJournal> journal = RequireReplayJournal();
		MatrixTileReplayRollbackAbi::Discard(core, journal, expectedIdentity);
		_captureLifecycle = MatrixTileCaptureLifecycle::ReplayDiscarded;
	}

	void MatrixTileMicroOp::CancelCapturedResult()
	{
		if (_captureLifecycle != MatrixTileCaptureLifecycle::Captured || !_lastExecutionCapture)
		{
			throw MatrixTileRetireValidationException(
				getProjection()->Mnemonic + L" cancel requires one live Phase09 capture.");
		}

		_lastExecutionCapture = std::nullopt;
		_activeCaptureIdentity.reset();
		_lastRetireOutcome = std::nullopt;
		_lastReplayRollbackJournal = nullptr;
		_captureLifecycle = MatrixTileCaptureLifecycle::Cancelled;
	}

	void MatrixTileMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		if (!_lastExecutionCapture)
		{
			throw MatrixTileRetireValidationException(
				getProjection()->Mnemonic + L" writeback reached retire without a Phase09 capture.");
		}

		std::shared_ptr<MatrixTileExecutionCaptureRecord> capture = _lastExecutionCapture.value();
		std::shared_ptr<MatrixTileRetireOutcome> outcome = RetireCapturedResult(core, capture);
		if (outcome->FaultRetired)
		{
			throw MatrixTileRetireFaultException(outcome);
		}
	}

	std::wstring MatrixTileMicroOp::GetDescription()
	{
		std::wstring typeName;
		switch (getOperationKind())
		{
		case MatrixTileProjectedOperationKind::Load: typeName = L"MtileLoadMicroOp"; break;
		case MatrixTileProjectedOperationKind::Store: typeName = L"MtileStoreMicroOp"; break;
		case MatrixTileProjectedOperationKind::Macc: typeName = L"MtileMaccMicroOp"; break;
		case MatrixTileProjectedOperationKind::Transpose: typeName = L"MtransposeMicroOp"; break;
		default: typeName = L"MatrixTileMicroOp"; break;
		}
		const auto descriptor = getTileDescriptor();
		const auto rows = descriptor ? descriptor->Rows : 0;
		const auto columns = descriptor ? descriptor->Columns : 0;
		const auto elementSize = descriptor ? descriptor->ElementSizeBytes : 0;
		const auto strideBytes = descriptor ? descriptor->StrideBytes : 0u;
		const std::wstring layoutName = !descriptor
			? L"Unspecified"
			: descriptor->Layout == MatrixTileDescriptorLayoutKind::RowMajor
				? L"RowMajor"
				: L"Unspecified";
		return std::format(
			L"{}: {}, Descriptor=MatrixTileCanonicalDescriptorAbi {{ Rows = {}, Columns = {}, ElementSizeBytes = {}, StrideBytes = {}, Layout = {} }}, LaneMask=0x{:02X}",
			typeName, getProjection()->Mnemonic, rows, columns, elementSize, strideBytes,
			layoutName, getSchedulerLaneMask());
	}

	std::vector<MatrixTileMicroOp::MemoryRange> MatrixTileMicroOp::BuildMemoryRanges(bool readsMemory) const
	{
		const bool matchesOperation = readsMemory
			? OperationKind == MatrixTileProjectedOperationKind::Load
			: OperationKind == MatrixTileProjectedOperationKind::Store;
		const auto memoryValidation = getMemoryValidation();
		if (!matchesOperation || !memoryValidation || !memoryValidation.value() ||
			!memoryValidation.value()->isMemoryShapeAbiAccepted())
		{
			return {};
		}

		const auto &validation = *memoryValidation.value();
		return {MemoryRange{validation.FirstByteAddress, validation.TotalByteFootprint}};
	}

	std::shared_ptr<MatrixTileExecutionCaptureRecord> MatrixTileMicroOp::CaptureLoad(std::shared_ptr<Processor::CPU_Core> &core)
	{
		auto transfer = MatrixTileStreamTransferAbi::BeginLoad(
			core, getOwnerThreadId(), getOpCode(), getTileDescriptor()->Rows);
		auto memoryContract = getMemoryContract();
		auto memoryValidation = getMemoryValidation();
		auto capture = MatrixTileExecuteCaptureAbi::CaptureLoad(
			getProjection()->Mnemonic, getOperationKind(),
			getDependencyMetadata()->SourceTileId, getDependencyMetadata()->SecondaryTileId,
			getDependencyMetadata()->DestinationTileId, getTileDescriptor(),
			getSecondaryTileDescriptor(), getResultTileDescriptor(), memoryContract,
			memoryValidation,
			[transfer](unsigned long long address, int byteCount)
			{
				return transfer->ReadIngress(address, byteCount);
			});
		capture->StreamTransfer = *transfer->Complete(capture->HasFault);
		return capture;
	}

	std::shared_ptr<MatrixTileExecutionCaptureRecord> MatrixTileMicroOp::CaptureStore(std::shared_ptr<Processor::CPU_Core> &core)
	{
		auto sourceSnapshot = CaptureTileSnapshotOrDefault(
			core, getDependencyMetadata()->SourceTileId, getTileDescriptor());
		auto transfer = MatrixTileStreamTransferAbi::BeginStore(
			core, getOwnerThreadId(), getOpCode(), getTileDescriptor()->Rows);
		auto memoryContract = getMemoryContract();
		auto memoryValidation = getMemoryValidation();
		auto stagedSnapshot = transfer->StageEgress(sourceSnapshot, memoryValidation);
		auto capture = MatrixTileExecuteCaptureAbi::CaptureStore(
			getProjection()->Mnemonic, getOperationKind(),
			getDependencyMetadata()->SourceTileId, getDependencyMetadata()->SecondaryTileId,
			getDependencyMetadata()->DestinationTileId, getTileDescriptor(),
			getSecondaryTileDescriptor(), getResultTileDescriptor(), memoryContract,
			memoryValidation, stagedSnapshot);
		capture->StreamTransfer = *transfer->Complete(capture->HasFault);
		return capture;
	}
	std::shared_ptr<MatrixTileExecutionCaptureRecord> MatrixTileMicroOp::CaptureMacc(std::shared_ptr<Processor::CPU_Core> &core)
	{
		std::shared_ptr<MatrixTileTileImage> leftSnapshot = CaptureTileSnapshotOrDefault(core, getDependencyMetadata()->SourceTileId, getTileDescriptor());
		std::shared_ptr<MatrixTileTileImage> rightSnapshot = CaptureTileSnapshotOrDefault(core, getDependencyMetadata()->SecondaryTileId, getSecondaryTileDescriptor());
		std::shared_ptr<MatrixTileTileImage> accumulatorSnapshot = CaptureTileSnapshotOrDefault(core, getDependencyMetadata()->DestinationTileId, getResultTileDescriptor());
		auto semanticValidation = getProjection()->SemanticValidation;
		auto maccContract = getMaccContract();

		return MatrixTileExecuteCaptureAbi::CaptureMacc(getProjection()->Mnemonic, getOperationKind(), getDependencyMetadata()->SourceTileId, getDependencyMetadata()->SecondaryTileId, getDependencyMetadata()->DestinationTileId, getTileDescriptor(), getSecondaryTileDescriptor(), getResultTileDescriptor(), semanticValidation, maccContract, leftSnapshot, rightSnapshot, accumulatorSnapshot);
	}

	std::shared_ptr<MatrixTileExecutionCaptureRecord> MatrixTileMicroOp::CaptureTranspose(std::shared_ptr<Processor::CPU_Core> &core)
	{
		std::shared_ptr<MatrixTileTileImage> sourceSnapshot = CaptureTileSnapshotOrDefault(core, getDependencyMetadata()->SourceTileId, getTileDescriptor());
		auto semanticValidation = getProjection()->SemanticValidation;
		auto transposeContract = getTransposeContract();

		return MatrixTileExecuteCaptureAbi::CaptureTranspose(getProjection()->Mnemonic, getOperationKind(), getDependencyMetadata()->SourceTileId, getDependencyMetadata()->SecondaryTileId, getDependencyMetadata()->DestinationTileId, getTileDescriptor(), getSecondaryTileDescriptor(), getResultTileDescriptor(), semanticValidation, transposeContract, sourceSnapshot);
	}

	std::shared_ptr<MatrixTileTileImage> MatrixTileMicroOp::CaptureTileSnapshotOrDefault(std::shared_ptr<Processor::CPU_Core> &core, unsigned short tileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> expectedDescriptor)
	{
		std::shared_ptr<MatrixTileTileImage> snapshot;
		return core->TryCaptureMatrixTileSnapshot(getOwnerThreadId(), tileId, expectedDescriptor, snapshot) ? snapshot : nullptr;
	}

	void MatrixTileMicroOp::EnsureNoUnretiredCapture()
	{
		if (_lastExecutionCapture)
		{
			throw InvalidOperationException(
				getProjection()->Mnemonic + L" already has a Phase09 execution capture record that must reach Phase10 retire publication or be discarded before re-execution.");
		}

		if (_lastReplayRollbackJournal)
		{
			const auto lifecycle = _lastReplayRollbackJournal->getLifecycle();
			if (lifecycle == MatrixTileReplayRollbackLifecycle::Retired ||
				lifecycle == MatrixTileReplayRollbackLifecycle::FaultRetired ||
				lifecycle == MatrixTileReplayRollbackLifecycle::RolledBack)
			{
				throw MatrixTileReplayRollbackValidationException(
					getProjection()->Mnemonic + L" replay/rollback authority must be replayed or discarded before MicroOp re-execution.");
			}
			if (lifecycle == MatrixTileReplayRollbackLifecycle::Replayed ||
				lifecycle == MatrixTileReplayRollbackLifecycle::FaultReplayed ||
				lifecycle == MatrixTileReplayRollbackLifecycle::Discarded)
			{
				_lastReplayRollbackJournal.reset();
			}
		}

		if (_captureLifecycle == MatrixTileCaptureLifecycle::Cancelled)
		{
			_captureLifecycle = MatrixTileCaptureLifecycle::None;
		}
	}

	std::shared_ptr<MatrixTileReplayRollbackJournal> MatrixTileMicroOp::RequireReplayJournal()
	{
		if (!_lastReplayRollbackJournal)
		{
			throw MatrixTileReplayRollbackValidationException(
				getProjection()->Mnemonic + L" has no retire-owned replay/rollback journal.");
		}
		return _lastReplayRollbackJournal;
	}
	ResourceBitset MatrixTileMicroOp::BuildResourceMask()
	{
		ResourceBitset mask = ResourceBitset::Zero;
		std::shared_ptr<MatrixTileMicroOpDependencyMetadata> dependency = getDependencyMetadata();

		if (getOperationKind() == MatrixTileProjectedOperationKind::Load)
		{
			mask = mask | ResourceMaskBuilder::ForMemoryDomain(getOwnerThreadId());
			mask = mask | ResourceMaskBuilder::ForLoad();
			mask = mask | ResourceMaskBuilder::ForStreamEngine(StreamEngineId::Zero);
			mask = mask | ResourceMaskBuilder::ForMatrixTileStreamWindow();
			mask = mask | ResourceMaskBuilder::ForMatrixTileIngress();
		}

		if (getOperationKind() == MatrixTileProjectedOperationKind::Store)
		{
			mask = mask | ResourceMaskBuilder::ForMemoryDomain(getOwnerThreadId());
			mask = mask | ResourceMaskBuilder::ForStore();
			mask = mask | ResourceMaskBuilder::ForStreamEngine(StreamEngineId::Zero);
			mask = mask | ResourceMaskBuilder::ForMatrixTileStreamWindow();
			mask = mask | ResourceMaskBuilder::ForMatrixTileEgress();
		}

		if (dependency->ReadsTileState)
		{
			mask = mask | ResourceMaskBuilder::ForMatrixTileStateRead();
		}

		if (dependency->WritesTileState)
		{
			mask = mask | ResourceMaskBuilder::ForMatrixTileStateWrite();
		}

		if (dependency->ReadsAccumulator)
		{
			mask = mask | ResourceMaskBuilder::ForMatrixTileAccumulatorRead();
		}

		if (dependency->WritesAccumulator)
		{
			mask = mask | ResourceMaskBuilder::ForMatrixTileAccumulatorWrite();
		}

		if (dependency->HasTransposePolicyDependencyMetadata)
		{
			mask = mask | ResourceMaskBuilder::ForMatrixTileTransposePolicy();
		}

		return mask;
	}

	unsigned char MatrixTileMicroOp::GetPublishedLatency()
	{
		const auto info = OpcodeRegistry::GetInfo(getOpCode());
		return info ? info->ExecutionLatency : static_cast<unsigned char>(1);
	}

	std::shared_ptr<MatrixTileMicroOpDependencyMetadata> MatrixTileMicroOp::CreateDependencyMetadata(
		std::shared_ptr<MatrixTileInstructionIrProjection> projection)
	{
		if (!projection || !projection->SourcePayload)
		{
			throw DecodeProjectionFaultException(L"MatrixTile projection has no source payload for dependency metadata.");
		}

		const auto &payload = *projection->SourcePayload;
		const unsigned short sourceTileId = GetTileId(payload.PrimaryPointer);
		const unsigned short secondaryTileId = GetTileId(payload.SecondaryPointer);
		const unsigned short destinationTileId = projection->OperationKind == MatrixTileProjectedOperationKind::Macc
			? GetHighTileIdOrDefault(payload.SecondaryPointer, secondaryTileId)
			: (projection->TransposeContract && projection->TransposeContract.value()
				? projection->TransposeContract.value()->DestinationTileId
				: secondaryTileId);

		const auto create = [&](bool hasTileMemoryDependencyMetadata,
			bool hasTileRegisterDependencyMetadata, bool hasAccumulatorDependencyMetadata,
			bool hasTransposePolicyDependencyMetadata, bool readsTileState, bool writesTileState,
			bool readsAccumulator, bool writesAccumulator, unsigned short operationSourceTileId)
		{
			return std::make_shared<MatrixTileMicroOpDependencyMetadata>(
				projection->OperationKind,
				projection->TileDescriptor,
				projection->SecondaryTileDescriptor,
				projection->ResultTileDescriptor,
				hasTileMemoryDependencyMetadata,
				hasTileRegisterDependencyMetadata,
				hasAccumulatorDependencyMetadata,
				hasTransposePolicyDependencyMetadata,
				readsTileState,
				writesTileState,
				readsAccumulator,
				writesAccumulator,
				operationSourceTileId,
				secondaryTileId,
				destinationTileId);
		};

		switch (projection->OperationKind)
		{
		case MatrixTileProjectedOperationKind::Load:
			return create(true, true, false, false, false, true, false, false, 0);
		case MatrixTileProjectedOperationKind::Store:
			return create(true, true, false, false, true, false, false, false, secondaryTileId);
		case MatrixTileProjectedOperationKind::Macc:
			return create(false, true, true, false, true, true, true, true, sourceTileId);
		case MatrixTileProjectedOperationKind::Transpose:
			return create(false, true, false, true, true, true, false, false,
				projection->TransposeContract && projection->TransposeContract.value()
					? projection->TransposeContract.value()->SourceTileId
					: sourceTileId);
		default:
			throw DecodeProjectionFaultException(
				L"Unsupported MTILE Phase08 operation kind " +
				std::to_wstring(static_cast<unsigned int>(projection->OperationKind)) + L".");
		}
	}
	unsigned short MatrixTileMicroOp::GetTileId(unsigned long long pointerOrTileToken)
	{
		return static_cast<unsigned short>(pointerOrTileToken & std::numeric_limits<unsigned short>::max());
	}

	unsigned short MatrixTileMicroOp::GetHighTileIdOrDefault(unsigned long long pointerOrTileToken, unsigned short fallbackTileId)
	{
		unsigned short highTileId = static_cast<unsigned short>((pointerOrTileToken >> 16) & std::numeric_limits<unsigned short>::max());
		return highTileId == 0 ? fallbackTileId : highTileId;
	}

	MtileLoadMicroOp::MtileLoadMicroOp(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction) : MatrixTileMicroOp(materializedInstruction, MatrixTileProjectedOperationKind::Load)
	{
	}

	MtileStoreMicroOp::MtileStoreMicroOp(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction) : MatrixTileMicroOp(materializedInstruction, MatrixTileProjectedOperationKind::Store)
	{
	}

	MtileMaccMicroOp::MtileMaccMicroOp(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction) : MatrixTileMicroOp(materializedInstruction, MatrixTileProjectedOperationKind::Macc)
	{
	}

	MtransposeMicroOp::MtransposeMicroOp(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction) : MatrixTileMicroOp(materializedInstruction, MatrixTileProjectedOperationKind::Transpose)
	{
	}
}
