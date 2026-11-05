#include "MatrixTileReplayRollbackAbi.h"
#include "MatrixTileIrProjectionAndMaterializer.h"
#include "MatrixTileRetirePublicationAbi.h"
#include "MatrixTileResourceContour.h"
#include "MatrixTilePolicyBoundIdentityAbi.h"
#include "MatrixTileNumericPolicyAbi.h"
#include "MatrixTileLayoutPolicyAbi.h"
#include "../../../../../Pipeline/MicroOps/IR/InstructionIR.h"
#include "../../../../../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	namespace
	{
		bool ReplayIdentitiesEqual(const MatrixTileReplayIdentity &left, const MatrixTileReplayIdentity &right)
		{
			return left.CoreId == right.CoreId && left.OwnerThreadId == right.OwnerThreadId &&
				left.Opcode == right.Opcode && left.OperationKind == right.OperationKind &&
				left.CaptureOrdinal == right.CaptureOrdinal && left.CaptureFingerprint == right.CaptureFingerprint &&
				left.DecodedInstructionFingerprint == right.DecodedInstructionFingerprint &&
				left.MaterializedInstructionFingerprint == right.MaterializedInstructionFingerprint &&
				left.DescriptorFingerprint == right.DescriptorFingerprint && left.ResourceClass == right.ResourceClass &&
				left.SlotClass == right.SlotClass && left.ResourceContourFingerprint == right.ResourceContourFingerprint &&
				left.StreamTransferFingerprint == right.StreamTransferFingerprint &&
				left.NumericPolicyFingerprint == right.NumericPolicyFingerprint &&
				left.LayoutPolicyFingerprint == right.LayoutPolicyFingerprint &&
				left.PolicyIdentityFingerprint == right.PolicyIdentityFingerprint && left.ReplayEpoch == right.ReplayEpoch &&
				left.DependencyFingerprint == right.DependencyFingerprint &&
				left.PublicationSurface == right.PublicationSurface && left.CheckpointOrdinal == right.CheckpointOrdinal;
		}

		bool RetireOutcomesEqual(const std::shared_ptr<MatrixTileRetireOutcome> &left,
			const std::shared_ptr<MatrixTileRetireOutcome> &right)
		{
			if (!left || !right)
				return left == right;

			const auto &leftFaultPoint = left->FaultPoint;
			const auto &rightFaultPoint = right->FaultPoint;
			return left->CaptureIdentity == right->CaptureIdentity && left->PublicationKind == right->PublicationKind &&
				left->RetireFaultKind == right->RetireFaultKind && left->ExecutionFaultKind == right->ExecutionFaultKind &&
				left->MemoryFaultKind == right->MemoryFaultKind && left->SemanticFaultKind == right->SemanticFaultKind &&
				leftFaultPoint.Row == rightFaultPoint.Row && leftFaultPoint.Column == rightFaultPoint.Column &&
				leftFaultPoint.ByteOffsetInRow == rightFaultPoint.ByteOffsetInRow &&
				leftFaultPoint.Address == rightFaultPoint.Address && leftFaultPoint.IsStore == rightFaultPoint.IsStore &&
				left->HasFaultPoint == right->HasFaultPoint && left->Message == right->Message &&
				left->PublishedArchitecturalState == right->PublishedArchitecturalState &&
				left->CommittedMemory == right->CommittedMemory && left->FaultRetired == right->FaultRetired;
		}
	}

	MatrixTileReplayRollbackValidationException::MatrixTileReplayRollbackValidationException(const std::wstring &message) : InvalidOperationException(message)
	{
	}

	MatrixTileReplayRollbackJournal::MatrixTileReplayRollbackJournal(std::shared_ptr<MatrixTileReplayIdentity> replayIdentity, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileRetireOutcome> retireOutcome, bool hadTileCheckpoint, std::shared_ptr<MatrixTileTileImage> tileCheckpoint, std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> &memoryCheckpoint)
		: ReplayIdentity(replayIdentity), OriginalRetireOutcome(retireOutcome),
		  Capture(std::make_shared<MatrixTileExecutionCaptureRecord>(capture->DeepClone())),
		  HadTileCheckpoint(hadTileCheckpoint),
		  TileCheckpoint(tileCheckpoint ? std::make_shared<MatrixTileTileImage>(tileCheckpoint->DeepClone()) : nullptr),
		  MemoryCheckpoint(CloneWrites(memoryCheckpoint))
	{
		setLifecycle(retireOutcome->FaultRetired ? MatrixTileReplayRollbackLifecycle::FaultRetired : MatrixTileReplayRollbackLifecycle::Retired);
	}

	std::shared_ptr<MatrixTileReplayIdentity> MatrixTileReplayRollbackJournal::getReplayIdentity() const
	{
		return ReplayIdentity;
	}

	std::shared_ptr<MatrixTileRetireOutcome> MatrixTileReplayRollbackJournal::getOriginalRetireOutcome() const
	{
		return OriginalRetireOutcome;
	}

	std::optional<std::shared_ptr<MatrixTileRetireOutcome>> MatrixTileReplayRollbackJournal::getReplayOutcome() const
	{
		return ReplayOutcome;
	}

	void MatrixTileReplayRollbackJournal::setReplayOutcome(const std::optional<std::shared_ptr<MatrixTileRetireOutcome>> &value)
	{
		ReplayOutcome = value;
	}

	MatrixTileReplayRollbackLifecycle MatrixTileReplayRollbackJournal::getLifecycle() const
	{
		return Lifecycle;
	}

	void MatrixTileReplayRollbackJournal::setLifecycle(MatrixTileReplayRollbackLifecycle value)
	{
		Lifecycle = value;
	}

	bool MatrixTileReplayRollbackJournal::getHasTileCheckpoint() const
	{
		const auto operationKind = getCapture()->OperationKind;
		return (operationKind == MatrixTileProjectedOperationKind::Load ||
			operationKind == MatrixTileProjectedOperationKind::Macc ||
			operationKind == MatrixTileProjectedOperationKind::Transpose) && !getCapture()->HasFault;
	}

	bool MatrixTileReplayRollbackJournal::getHadArchitecturalTileBeforeRetire() const
	{
		return getHadTileCheckpoint();
	}

	int MatrixTileReplayRollbackJournal::getMemoryCheckpointRowCount() const
	{
		return static_cast<int>(getMemoryCheckpoint().size());
	}

	bool MatrixTileReplayRollbackJournal::isFaultOnly() const
	{
		return getOriginalRetireOutcome()->FaultRetired;
	}

	std::shared_ptr<MatrixTileExecutionCaptureRecord> MatrixTileReplayRollbackJournal::getCapture() const
	{
		return Capture;
	}

	bool MatrixTileReplayRollbackJournal::getHadTileCheckpoint() const
	{
		return HadTileCheckpoint;
	}

	std::shared_ptr<MatrixTileTileImage> MatrixTileReplayRollbackJournal::getTileCheckpoint() const
	{
		return TileCheckpoint;
	}

	std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> MatrixTileReplayRollbackJournal::getMemoryCheckpoint() const
	{
		return MemoryCheckpoint;
	}

	std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> MatrixTileReplayRollbackJournal::CloneWrites(std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> &writes)
	{
		std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> clone(writes.size());
		for (std::size_t index = 0; index < writes.size(); index++)
		{
			if (!writes[index])
				throw ArgumentNullException("writes[index]");
			clone[index] = std::make_shared<MatrixTileCapturedMemoryWrite>(*writes[index]);
		}

		return clone;
	}

	std::shared_ptr<MatrixTileReplayRollbackJournal> MatrixTileReplayRollbackAbi::RetireWithCheckpoint(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileCaptureIdentity> expectedCaptureIdentity, unsigned long long checkpointOrdinal)
	{
		MatrixTileRetirePublicationAbi::ValidateForRetire(core, capture, expectedCaptureIdentity);

		std::shared_ptr<MatrixTileReplayIdentity> replayIdentity = CreateReplayIdentity(materializedInstruction, capture, checkpointOrdinal);
		ValidateReplayIdentity(core, materializedInstruction, capture, replayIdentity);

		bool hadTileCheckpoint;
		std::shared_ptr<MatrixTileTileImage> tileCheckpoint;
		std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> memoryCheckpoint;
		CapturePreRetireState(core, capture, hadTileCheckpoint, tileCheckpoint, memoryCheckpoint);

		std::shared_ptr<MatrixTileRetireOutcome> retireOutcome = MatrixTileRetirePublicationAbi::Retire(core, capture, expectedCaptureIdentity);

		std::shared_ptr<MatrixTileReplayRollbackJournal> journal = std::make_shared<MatrixTileReplayRollbackJournal>(replayIdentity, capture, retireOutcome, hadTileCheckpoint, tileCheckpoint, memoryCheckpoint);
		core->RegisterMatrixTileReplayJournal(journal);
		return journal;
	}

	std::shared_ptr<MatrixTileRollbackOutcome> MatrixTileReplayRollbackAbi::Rollback(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileReplayRollbackJournal> journal, std::shared_ptr<MatrixTileReplayIdentity> expectedIdentity)
	{
		ValidateOwnedJournal(core, journal, expectedIdentity);
		if (journal->getLifecycle() != MatrixTileReplayRollbackLifecycle::Retired && journal->getLifecycle() != MatrixTileReplayRollbackLifecycle::FaultRetired)
		{
			throw *Validation(std::format(L"MTILE rollback requires a retired journal, not lifecycle {}.", static_cast<unsigned int>(journal->getLifecycle())));
		}

		bool restoredTile = false;
		bool restoredMemory = false;
		if (!journal->getOriginalRetireOutcome()->FaultRetired)
		{
			std::shared_ptr<MatrixTileExecutionCaptureRecord> capture = journal->getCapture();
			switch (capture->OperationKind)
			{
				case MatrixTileProjectedOperationKind::Load:
				case MatrixTileProjectedOperationKind::Macc:
				case MatrixTileProjectedOperationKind::Transpose:
					RequireCurrentTileImage(core, capture->CaptureIdentity.OwnerThreadId, capture->ResultImage, L"rollback");
					core->RestoreRetiredMatrixTileCheckpoint(capture->CaptureIdentity.OwnerThreadId, capture->DestinationTileId, journal->getHadTileCheckpoint(), journal->getTileCheckpoint());
					restoredTile = true;
					break;

				case MatrixTileProjectedOperationKind::Store:
					RequireCurrentMemoryRows(core, capture->PendingStoreWrites, L"rollback");
					{
						auto memoryCheckpoint = journal->getMemoryCheckpoint();
						core->RestoreRetiredMatrixTileStoreAllOrNone(memoryCheckpoint);
					}
					restoredMemory = true;
					break;

				default:
					throw *Validation(std::format(L"MTILE rollback rejected unsupported operation {}.", static_cast<unsigned int>(capture->OperationKind)));
			}
		}

		journal->setLifecycle(MatrixTileReplayRollbackLifecycle::RolledBack);
		return std::make_shared<MatrixTileRollbackOutcome>(*journal->getReplayIdentity(), restoredTile, restoredMemory, journal->getOriginalRetireOutcome()->FaultRetired, journal->getLifecycle());
	}

	std::shared_ptr<MatrixTileRetireOutcome> MatrixTileReplayRollbackAbi::Replay(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction, std::shared_ptr<MatrixTileReplayRollbackJournal> journal, std::shared_ptr<MatrixTileReplayIdentity> expectedIdentity)
	{
		ValidateOwnedJournal(core, journal, expectedIdentity);
		ValidateReplayIdentity(core, materializedInstruction, journal->getCapture(), journal->getReplayIdentity());
		if (journal->getLifecycle() != MatrixTileReplayRollbackLifecycle::RolledBack)
		{
			throw *Validation(std::format(L"MTILE replay requires one completed rollback, not lifecycle {}.", static_cast<unsigned int>(journal->getLifecycle())));
		}

		ValidatePreRetireState(core, journal);

		std::shared_ptr<MatrixTileRetireOutcome> replayOutcome;
		if (journal->getOriginalRetireOutcome()->FaultRetired)
		{
			replayOutcome = journal->getOriginalRetireOutcome();
		}
		else
		{
			replayOutcome = MatrixTileRetirePublicationAbi::Retire(core, journal->getCapture(), std::make_shared<MatrixTileCaptureIdentity>(journal->getCapture()->CaptureIdentity));
			if (!RetireOutcomesEqual(replayOutcome, journal->getOriginalRetireOutcome()))
			{
				throw *Validation(std::format(L"{} replay outcome diverged from its retired outcome.", journal->getCapture()->Mnemonic));
			}
		}

		journal->setReplayOutcome(replayOutcome);
		journal->setLifecycle(replayOutcome->FaultRetired ? MatrixTileReplayRollbackLifecycle::FaultReplayed : MatrixTileReplayRollbackLifecycle::Replayed);
		core->ReleaseMatrixTileReplayJournal(journal);
		return replayOutcome;
	}

	void MatrixTileReplayRollbackAbi::Discard(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileReplayRollbackJournal> journal, std::shared_ptr<MatrixTileReplayIdentity> expectedIdentity)
	{
		ValidateOwnedJournal(core, journal, expectedIdentity);
		if (journal->getLifecycle() != MatrixTileReplayRollbackLifecycle::Retired && journal->getLifecycle() != MatrixTileReplayRollbackLifecycle::FaultRetired)
		{
			throw *Validation(std::format(L"MTILE replay journal discard rejected lifecycle {}.", static_cast<unsigned int>(journal->getLifecycle())));
		}

		journal->setLifecycle(MatrixTileReplayRollbackLifecycle::Discarded);
		core->ReleaseMatrixTileReplayJournal(journal);
	}

	std::shared_ptr<MatrixTileReplayIdentity> MatrixTileReplayRollbackAbi::CreateReplayIdentity(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, unsigned long long checkpointOrdinal)
	{
		std::shared_ptr<MatrixTileCaptureIdentity> captureIdentity = std::make_shared<MatrixTileCaptureIdentity>(capture->CaptureIdentity);
		std::shared_ptr<MatrixTilePolicyBoundCaptureIdentity> policyIdentity = capture->PolicyIdentity;
		return std::make_shared<MatrixTileReplayIdentity>(captureIdentity->CoreId, captureIdentity->OwnerThreadId, captureIdentity->Opcode, captureIdentity->OperationKind, captureIdentity->CaptureOrdinal, captureIdentity->CaptureFingerprint, ComputeDecodedInstructionFingerprint(materializedInstruction->Projection), ComputeMaterializedInstructionFingerprint(materializedInstruction), ComputeDescriptorFingerprint(materializedInstruction), MatrixTileResourceContour::Classify(captureIdentity->Opcode), MatrixTileResourceContour::ResolveSlotClass(MatrixTileResourceContour::Classify(captureIdentity->Opcode)), ComputeResourceContourFingerprint(captureIdentity->Opcode), ComputeStreamTransferIdentityFingerprint(capture), policyIdentity->NumericPolicyFingerprint, policyIdentity->LayoutPolicyFingerprint, policyIdentity->IdentityFingerprint, policyIdentity->ReplayEpoch, policyIdentity->DependencyFingerprint, policyIdentity->PublicationSurface, checkpointOrdinal);
	}

	void MatrixTileReplayRollbackAbi::CapturePreRetireState(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, bool &hadTileCheckpoint, std::shared_ptr<MatrixTileTileImage> &tileCheckpoint, std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> &memoryCheckpoint)
	{
		hadTileCheckpoint = false;
		tileCheckpoint = nullptr;
		memoryCheckpoint.clear();
		if (capture->HasFault)
		{
			return;
		}

		if (capture->OperationKind == MatrixTileProjectedOperationKind::Store)
		{
			std::wstring failureMessage;
			if (!core->TryCaptureMatrixTileStoreRollbackImage(capture->PendingStoreWrites, memoryCheckpoint, failureMessage))
			{
				throw *Validation(failureMessage);
			}

			return;
		}

		hadTileCheckpoint = core->TryCaptureAnyMatrixTileSnapshot(capture->CaptureIdentity.OwnerThreadId, capture->DestinationTileId, tileCheckpoint);
	}

	void MatrixTileReplayRollbackAbi::ValidatePreRetireState(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileReplayRollbackJournal> journal)
	{
		std::shared_ptr<MatrixTileExecutionCaptureRecord> capture = journal->getCapture();
		if (journal->getOriginalRetireOutcome()->FaultRetired)
		{
			return;
		}

		if (capture->OperationKind == MatrixTileProjectedOperationKind::Store)
		{
			auto memoryCheckpoint = journal->getMemoryCheckpoint();
			RequireCurrentMemoryRows(core, memoryCheckpoint, L"replay");
			return;
		}

		std::shared_ptr<MatrixTileTileImage> current;
		bool hasCurrent = core->TryCaptureAnyMatrixTileSnapshot(capture->CaptureIdentity.OwnerThreadId, capture->DestinationTileId, current);
		if (journal->getHadTileCheckpoint())
		{
			if (!hasCurrent || !TileImagesEqual(current, journal->getTileCheckpoint()))
			{
				throw *Validation(std::format(L"{} replay rejected stale pre-retire tile state.", capture->Mnemonic));
			}
		}
		else if (hasCurrent)
		{
			throw *Validation(std::format(L"{} replay expected an absent pre-retire destination tile.", capture->Mnemonic));
		}
	}

	void MatrixTileReplayRollbackAbi::RequireCurrentTileImage(std::shared_ptr<Processor::CPU_Core> &core, int ownerThreadId, std::shared_ptr<MatrixTileTileImage> expected, const std::wstring &operation)
	{
		std::shared_ptr<MatrixTileTileImage> current;
		if (!expected || !core->TryCaptureAnyMatrixTileSnapshot(ownerThreadId, expected->TileId, current) || !TileImagesEqual(current, expected))
		{
			throw *Validation(std::format(L"MTILE {} rejected a stale or replaced architectural tile publication.", operation));
		}
	}

	void MatrixTileReplayRollbackAbi::RequireCurrentMemoryRows(std::shared_ptr<Processor::CPU_Core> &core, std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> &expected, const std::wstring &operation)
	{
		if (!core->MatrixTileMemoryRowsMatch(expected))
		{
			throw *Validation(std::format(L"MTILE_STORE {} rejected stale or replaced committed memory.", operation));
		}
	}

	void MatrixTileReplayRollbackAbi::ValidateOwnedJournal(std::shared_ptr<Processor::CPU_Core> core, std::shared_ptr<MatrixTileReplayRollbackJournal> journal, std::shared_ptr<MatrixTileReplayIdentity> expectedIdentity)
	{
		ArgumentNullException::ThrowIfNull(journal);
		if (!expectedIdentity || !expectedIdentity->getIsValid() || !journal->getReplayIdentity() ||
			!ReplayIdentitiesEqual(*journal->getReplayIdentity(), *expectedIdentity) ||
			expectedIdentity->CoreId != core->getCoreID() || !core->OwnsMatrixTileReplayJournal(journal))
		{
			throw *Validation(L"MTILE replay/rollback rejected a missing, mismatched, released, or wrong-core journal.");
		}
	}

	void MatrixTileReplayRollbackAbi::ValidateReplayIdentity(std::shared_ptr<Processor::CPU_Core> core, std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileReplayIdentity> replayIdentity)
	{
		std::shared_ptr<MatrixTileCaptureIdentity> captureIdentity = std::make_shared<MatrixTileCaptureIdentity>(capture->CaptureIdentity);
		std::shared_ptr<MatrixTilePolicyBoundCaptureIdentity> policyIdentity = capture->PolicyIdentity;
		if (!replayIdentity->getIsValid() || replayIdentity->CoreId != core->getCoreID() || replayIdentity->CoreId != captureIdentity->CoreId || replayIdentity->OwnerThreadId != captureIdentity->OwnerThreadId || replayIdentity->Opcode != captureIdentity->Opcode || replayIdentity->OperationKind != captureIdentity->OperationKind || replayIdentity->CaptureOrdinal != captureIdentity->CaptureOrdinal || replayIdentity->CaptureFingerprint != captureIdentity->CaptureFingerprint || replayIdentity->DecodedInstructionFingerprint != ComputeDecodedInstructionFingerprint(materializedInstruction->Projection) || replayIdentity->MaterializedInstructionFingerprint != ComputeMaterializedInstructionFingerprint(materializedInstruction) || replayIdentity->DescriptorFingerprint != ComputeDescriptorFingerprint(materializedInstruction) || replayIdentity->ResourceClass != MatrixTileResourceContour::Classify(captureIdentity->Opcode) || replayIdentity->SlotClass != MatrixTileResourceContour::ResolveSlotClass(replayIdentity->ResourceClass) || replayIdentity->ResourceContourFingerprint != ComputeResourceContourFingerprint(captureIdentity->Opcode) || replayIdentity->StreamTransferFingerprint != ComputeStreamTransferIdentityFingerprint(capture) || replayIdentity->NumericPolicyFingerprint != policyIdentity->NumericPolicyFingerprint || replayIdentity->LayoutPolicyFingerprint != policyIdentity->LayoutPolicyFingerprint || replayIdentity->PolicyIdentityFingerprint != policyIdentity->IdentityFingerprint || replayIdentity->ReplayEpoch != policyIdentity->ReplayEpoch || replayIdentity->ReplayEpoch != core->GetMatrixTileReplayInvalidationEpoch() || replayIdentity->DependencyFingerprint != policyIdentity->DependencyFingerprint || replayIdentity->DependencyFingerprint != MatrixTilePolicyBoundIdentityAbi::ComputeDependencyFingerprint(materializedInstruction) || replayIdentity->PublicationSurface != policyIdentity->PublicationSurface || !MatrixTilePolicyBoundIdentityAbi::ValidateCapture(capture, core->GetMatrixTileReplayInvalidationEpoch()) || !MatrixTilePolicyBoundIdentityAbi::MatchesMaterializedInstruction(policyIdentity, materializedInstruction))
		{
			throw *Validation(L"MTILE replay identity does not match its decoded, materialized, descriptor, policy-bound, and capture correlation.");
		}
	}

	unsigned long long MatrixTileReplayRollbackAbi::ComputeDecodedInstructionFingerprint(std::shared_ptr<MatrixTileInstructionIrProjection> projection)
	{
		unsigned long long hash = BeginHash();
		AddUInt32(hash, static_cast<unsigned int>(projection->Opcode));
		AddString(hash, projection->Mnemonic);
		AddByte(hash, static_cast<unsigned char>(projection->OperationKind));
		std::shared_ptr<VectorInstructionPayload> payload = projection->SourcePayload;
		AddUInt64(hash, payload->PrimaryPointer);
		AddUInt64(hash, payload->SecondaryPointer);
		AddUInt32(hash, payload->StreamLength);
		AddUInt16(hash, payload->Stride);
		AddUInt16(hash, payload->RowStride);
		AddByte(hash, payload->Indexed ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddByte(hash, payload->Is2D ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddByte(hash, payload->TailAgnostic ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddByte(hash, payload->MaskAgnostic ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddByte(hash, payload->Saturating ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddByte(hash, payload->PredicateMask);
		AddByte(hash, payload->DataType);
		std::optional<std::shared_ptr<MatrixTileNumericPolicy>> numericPolicy = payload->MatrixTileNumericPolicy;
		std::optional<std::shared_ptr<MatrixTileLayoutPolicy>> layoutPolicy = payload->MatrixTileLayoutPolicy;
		AddNumericPolicyIdentity(hash, numericPolicy);
		AddLayoutPolicyIdentity(hash, layoutPolicy);
		return FinishHash(hash);
	}

	unsigned long long MatrixTileReplayRollbackAbi::ComputeMaterializedInstructionFingerprint(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction)
	{
		unsigned long long hash = BeginHash();
		AddUInt32(hash, static_cast<unsigned int>(materializedInstruction->Opcode));
		AddString(hash, materializedInstruction->Mnemonic);
		AddByte(hash, static_cast<unsigned char>(materializedInstruction->OperationKind));
		AddUInt64(hash, ComputeDecodedInstructionFingerprint(materializedInstruction->Projection));
		AddByte(hash, materializedInstruction->IsTypedCloseToHSLRuntimeObject ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddByte(hash, materializedInstruction->PublishesTypedTileMicroOp ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddByte(hash, materializedInstruction->OpensExecution ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddByte(hash, materializedInstruction->UsesFallbackPath ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		return FinishHash(hash);
	}

	unsigned long long MatrixTileReplayRollbackAbi::ComputeDescriptorFingerprint(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction)
	{
		unsigned long long hash = BeginHash();
		AddDescriptor(hash, materializedInstruction->TileDescriptor);
		AddDescriptor(hash, materializedInstruction->SecondaryTileDescriptor);
		AddDescriptor(hash, materializedInstruction->ResultTileDescriptor);
		return FinishHash(hash);
	}

	unsigned long long MatrixTileReplayRollbackAbi::ComputeResourceContourFingerprint(unsigned int opcode)
	{
		MatrixTileRuntimeResourceClass resourceClass = MatrixTileResourceContour::Classify(opcode);
		SlotClass slotClass = MatrixTileResourceContour::ResolveSlotClass(resourceClass);
		unsigned long long hash = BeginHash();
		AddUInt32(hash, opcode);
		AddByte(hash, static_cast<unsigned char>(resourceClass));
		AddByte(hash, static_cast<unsigned char>(slotClass));
		AddByte(hash, SlotClassLaneMap::GetLaneMask(slotClass));
		AddByte(hash, resourceClass == MatrixTileRuntimeResourceClass::MatrixTileMemory ? static_cast<unsigned char>(MatrixTileResourceContour::StreamEngineChannel) : std::numeric_limits<unsigned char>::max());
		return FinishHash(hash);
	}

	unsigned long long MatrixTileReplayRollbackAbi::ComputeStreamTransferIdentityFingerprint(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture)
	{
		if (capture->StreamTransfer.TransferFingerprint != 0)
		{
			return capture->StreamTransfer.TransferFingerprint;
		}

		unsigned long long hash = BeginHash();
		AddUInt32(hash, capture->CaptureIdentity.Opcode);
		AddByte(hash, static_cast<unsigned char>(capture->OperationKind));
		AddByte(hash, static_cast<unsigned char>(capture->FaultKind));
		AddByte(hash, static_cast<unsigned char>(capture->MemoryFaultKind));
		AddByte(hash, capture->HasFault ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		return FinishHash(hash);
	}

	bool MatrixTileReplayRollbackAbi::TileImagesEqual(std::shared_ptr<MatrixTileTileImage> left, std::shared_ptr<MatrixTileTileImage> right)
	{
		if (!left || !right || left->TileId != right->TileId || left->Descriptor != right->Descriptor || left->Data.empty() || right->Data.empty() || left->Data.size() != right->Data.size())
		{
			return false;
		}

		return left->Data == right->Data;
	}

	unsigned long long MatrixTileReplayRollbackAbi::BeginHash()
	{
		return 14695981039346656037ULL;
	}

	unsigned long long MatrixTileReplayRollbackAbi::FinishHash(unsigned long long hash)
	{
		return hash == 0 ? 1ULL : hash;
	}

	void MatrixTileReplayRollbackAbi::AddByte(unsigned long long &hash, unsigned char value)
	{
		hash ^= value;
		hash *= 1099511628211ULL;
	}

	void MatrixTileReplayRollbackAbi::AddUInt16(unsigned long long &hash, unsigned short value)
	{
		AddByte(hash, static_cast<unsigned char>(value));
		AddByte(hash, static_cast<unsigned char>(value >> 8));
	}

	void MatrixTileReplayRollbackAbi::AddUInt32(unsigned long long &hash, unsigned int value)
	{
		for (int shift = 0; shift < 32; shift += 8)
		{
			AddByte(hash, static_cast<unsigned char>(value >> shift));
		}
	}

	void MatrixTileReplayRollbackAbi::AddUInt64(unsigned long long &hash, unsigned long long value)
	{
		for (int shift = 0; shift < 64; shift += 8)
		{
			AddByte(hash, static_cast<unsigned char>(value >> shift));
		}
	}

	void MatrixTileReplayRollbackAbi::AddString(unsigned long long &hash, const std::wstring &value)
	{
		for (int index = 0; index < value.length(); index++)
		{
			AddUInt16(hash, value[index]);
		}

		AddByte(hash, 0);
	}

	void MatrixTileReplayRollbackAbi::AddDescriptor(unsigned long long &hash, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor)
	{
		if (!descriptor)
		{
			AddUInt16(hash, 0);
			AddUInt16(hash, 0);
			AddUInt16(hash, 0);
			AddUInt32(hash, 0);
			AddByte(hash, 0);
			return;
		}

		AddUInt16(hash, descriptor->Rows);
		AddUInt16(hash, descriptor->Columns);
		AddUInt16(hash, descriptor->ElementSizeBytes);
		AddUInt32(hash, descriptor->StrideBytes);
		AddByte(hash, static_cast<unsigned char>(descriptor->Layout));
	}

	void MatrixTileReplayRollbackAbi::AddNumericPolicyIdentity(unsigned long long &hash, std::optional<std::shared_ptr<MatrixTileNumericPolicy>> &policy)
	{
		const bool hasPolicy = policy.has_value() && static_cast<bool>(policy.value());
		AddByte(hash, hasPolicy ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		if (!hasPolicy)
		{
			return;
		}

		AddUInt16(hash, policy.value()->AbiVersion);
		AddUInt64(hash, policy.value()->Fingerprint);
	}

	void MatrixTileReplayRollbackAbi::AddLayoutPolicyIdentity(unsigned long long &hash, std::optional<std::shared_ptr<MatrixTileLayoutPolicy>> &policy)
	{
		const bool hasPolicy = policy.has_value() && static_cast<bool>(policy.value());
		AddByte(hash, hasPolicy ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		if (!hasPolicy)
		{
			return;
		}

		AddUInt16(hash, policy.value()->AbiVersion);
		AddUInt64(hash, policy.value()->Fingerprint);
	}

	std::shared_ptr<MatrixTileReplayRollbackValidationException> MatrixTileReplayRollbackAbi::Validation(const std::wstring &message)
	{
		return std::make_shared<MatrixTileReplayRollbackValidationException>(message);
	}
}
