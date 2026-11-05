#pragma once
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile { enum class MatrixTileProjectedOperationKind : unsigned char; struct MatrixTileMemoryShapeContract; struct MatrixTileMemoryShapeValidationResult; struct MatrixTileMaccSemanticContract; struct MatrixTileTransposeSemanticContract; }

#include "../Types/MicroOp.h"
#include "../../../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileIrProjectionAndMaterializer.h"
#include "../../../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileResourceContour.h"
#include "../../../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileReplayRollbackAbi.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
#include "../../Safety/ResourceBitset.h"
#include <string>
#include <limits>
#include <optional>
#include <format>
#include <memory>
#include <utility>
#include <vector>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
	struct MatrixTileMicroOpDependencyMetadata;
	enum class MatrixTileCaptureLifecycle : unsigned char
	{
		None = 0,
		Captured = 1,
		Retired = 2,
		FaultRetired = 3,
		Cancelled = 4,
		RolledBack = 5,
		Replayed = 6,
		FaultReplayed = 7,
		ReplayDiscarded = 8
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileMicroOpDependencyMetadata(MatrixTileProjectedOperationKind OperationKind, MatrixTileCanonicalDescriptorAbi TileDescriptor, MatrixTileCanonicalDescriptorAbi SecondaryTileDescriptor, MatrixTileCanonicalDescriptorAbi ResultTileDescriptor, bool HasTileMemoryDependencyMetadata, bool HasTileRegisterDependencyMetadata, bool HasAccumulatorDependencyMetadata, bool HasTransposePolicyDependencyMetadata, bool ReadsTileState, bool WritesTileState, bool ReadsAccumulator, bool WritesAccumulator, ushort SourceTileId, ushort SecondaryTileId, ushort DestinationTileId);
	struct MatrixTileMicroOpDependencyMetadata final
	{
		const MatrixTileProjectedOperationKind OperationKind;
		const std::shared_ptr<MatrixTileCanonicalDescriptorAbi> TileDescriptor;
		const std::shared_ptr<MatrixTileCanonicalDescriptorAbi> SecondaryTileDescriptor;
		const std::shared_ptr<MatrixTileCanonicalDescriptorAbi> ResultTileDescriptor;
		const bool HasTileMemoryDependencyMetadata;
		const bool HasTileRegisterDependencyMetadata;
		const bool HasAccumulatorDependencyMetadata;
		const bool HasTransposePolicyDependencyMetadata;
		const bool ReadsTileState;
		const bool WritesTileState;
		const bool ReadsAccumulator;
		const bool WritesAccumulator;
		const unsigned short SourceTileId;
		const unsigned short SecondaryTileId;
		const unsigned short DestinationTileId;

		MatrixTileMicroOpDependencyMetadata()
			: OperationKind(static_cast<MatrixTileProjectedOperationKind>(0)),
			  TileDescriptor(nullptr), SecondaryTileDescriptor(nullptr), ResultTileDescriptor(nullptr),
			  HasTileMemoryDependencyMetadata(false), HasTileRegisterDependencyMetadata(false),
			  HasAccumulatorDependencyMetadata(false), HasTransposePolicyDependencyMetadata(false),
			  ReadsTileState(false), WritesTileState(false), ReadsAccumulator(false), WritesAccumulator(false),
			  SourceTileId(0), SecondaryTileId(0), DestinationTileId(0)
		{
		}

		MatrixTileMicroOpDependencyMetadata(
			MatrixTileProjectedOperationKind operationKind,
			std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor,
			std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor,
			std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor,
			bool hasTileMemoryDependencyMetadata, bool hasTileRegisterDependencyMetadata,
			bool hasAccumulatorDependencyMetadata, bool hasTransposePolicyDependencyMetadata,
			bool readsTileState, bool writesTileState, bool readsAccumulator, bool writesAccumulator,
			unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId)
			: OperationKind(operationKind), TileDescriptor(std::move(tileDescriptor)),
			  SecondaryTileDescriptor(std::move(secondaryTileDescriptor)),
			  ResultTileDescriptor(std::move(resultTileDescriptor)),
			  HasTileMemoryDependencyMetadata(hasTileMemoryDependencyMetadata),
			  HasTileRegisterDependencyMetadata(hasTileRegisterDependencyMetadata),
			  HasAccumulatorDependencyMetadata(hasAccumulatorDependencyMetadata),
			  HasTransposePolicyDependencyMetadata(hasTransposePolicyDependencyMetadata),
			  ReadsTileState(readsTileState), WritesTileState(writesTileState),
			  ReadsAccumulator(readsAccumulator), WritesAccumulator(writesAccumulator),
			  SourceTileId(sourceTileId), SecondaryTileId(secondaryTileId),
			  DestinationTileId(destinationTileId)
		{
		}
	};

	class MatrixTileMicroOp : public MicroOp
	{
	private:
		const std::shared_ptr<MatrixTileMaterializedInstruction> MaterializedInstruction;
		const std::shared_ptr<MatrixTileInstructionIrProjection> Projection;
		const MatrixTileProjectedOperationKind OperationKind;
		const MatrixTileRuntimeResourceClass RuntimeResourceClass;
		const std::shared_ptr<MatrixTileMicroOpDependencyMetadata> DependencyMetadata;

		std::optional<std::shared_ptr<MatrixTileExecutionCaptureRecord>> _lastExecutionCapture;
		std::shared_ptr<MatrixTileCaptureIdentity> _activeCaptureIdentity;
		std::optional<std::shared_ptr<MatrixTileRetireOutcome>> _lastRetireOutcome;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MatrixTileReplayRollbackJournal? _lastReplayRollbackJournal;
		std::shared_ptr<MatrixTileReplayRollbackJournal> _lastReplayRollbackJournal;
		MatrixTileCaptureLifecycle _captureLifecycle = static_cast<MatrixTileCaptureLifecycle>(0);

	protected:
		MatrixTileMicroOp(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction, MatrixTileProjectedOperationKind expectedOperationKind);

	public:
		std::shared_ptr<MatrixTileMaterializedInstruction> getMaterializedInstruction() const;

		std::shared_ptr<MatrixTileInstructionIrProjection> getProjection() const;

		MatrixTileProjectedOperationKind getOperationKind() const;

		MatrixTileRuntimeResourceClass getRuntimeResourceClass() const;

		std::shared_ptr<MatrixTileMicroOpDependencyMetadata> getDependencyMetadata() const;

		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> getTileDescriptor() const;

		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> getSecondaryTileDescriptor() const;

		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> getResultTileDescriptor() const;

		std::optional<std::shared_ptr<MatrixTileMemoryShapeContract>> getMemoryContract() const;

		std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> getMemoryValidation() const;

		std::optional<std::shared_ptr<MatrixTileMaccSemanticContract>> getMaccContract() const;

		std::optional<std::shared_ptr<MatrixTileTransposeSemanticContract>> getTransposeContract() const;

		bool getPublishesTypedTileMicroOp() const;

		bool getPublishesSchedulerLaneBinding() const;

		bool getPublishesIssueConstraints() const;

		bool getPublishesCaptureBarriers() const;

		bool getOpensExecution() const;

		bool getPublishesExecutionCaptureSemantics() const;

		std::optional<std::shared_ptr<MatrixTileExecutionCaptureRecord>> getLastExecutionCapture() const;

		std::optional<std::shared_ptr<MatrixTileRetireOutcome>> getLastRetireOutcome() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MatrixTileReplayRollbackJournal? LastReplayRollbackJournal
		std::shared_ptr<MatrixTileReplayRollbackJournal> getLastReplayRollbackJournal() const;

		MatrixTileCaptureLifecycle getCaptureLifecycle() const;

		bool getUsesFallbackPath() const;

		unsigned char getSchedulerLaneMask() const;

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		virtual void InitializeMetadata();

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		std::shared_ptr<MatrixTileRetireOutcome> RetireCapturedResult(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture);

		std::shared_ptr<MatrixTileRollbackOutcome> RollbackRetiredResult(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileReplayIdentity> expectedIdentity);

		std::shared_ptr<MatrixTileRetireOutcome> ReplayRolledBackResult(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileReplayIdentity> expectedIdentity);

		void DiscardReplayRollbackAuthority(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileReplayIdentity> expectedIdentity);

		void CancelCapturedResult();

		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

		std::wstring GetDescription() override;

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private IReadOnlyList<(ulong Address, ulong Length)> BuildMemoryRanges(bool readsMemory)
//		{
//			bool matchesOperation = readsMemory ? OperationKind == MatrixTileProjectedOperationKind.Load : OperationKind == MatrixTileProjectedOperationKind.Store;
//			if (!matchesOperation || !MemoryValidation.HasValue || !MemoryValidation.Value.IsMemoryShapeAbiAccepted)
//			{
//				return Array.Empty<(ulong Address, ulong Length)>();
//			}
//
//			MatrixTileMemoryShapeValidationResult validation = MemoryValidation.Value;
//			return new[] { (validation.FirstByteAddress, validation.TotalByteFootprint) };
//		}

	private:
		std::shared_ptr<MatrixTileExecutionCaptureRecord> CaptureLoad(std::shared_ptr<Processor::CPU_Core> &core);

		std::shared_ptr<MatrixTileExecutionCaptureRecord> CaptureStore(std::shared_ptr<Processor::CPU_Core> &core);

		std::shared_ptr<MatrixTileExecutionCaptureRecord> CaptureMacc(std::shared_ptr<Processor::CPU_Core> &core);

		std::shared_ptr<MatrixTileExecutionCaptureRecord> CaptureTranspose(std::shared_ptr<Processor::CPU_Core> &core);

		std::shared_ptr<MatrixTileTileImage> CaptureTileSnapshotOrDefault(std::shared_ptr<Processor::CPU_Core> &core, unsigned short tileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> expectedDescriptor);

		void EnsureNoUnretiredCapture();

		std::shared_ptr<MatrixTileReplayRollbackJournal> RequireReplayJournal();

		ResourceBitset BuildResourceMask();

		std::vector<MemoryRange> BuildMemoryRanges(bool readsMemory) const;

		unsigned char GetPublishedLatency();

		static std::shared_ptr<MatrixTileMicroOpDependencyMetadata> CreateDependencyMetadata(std::shared_ptr<MatrixTileInstructionIrProjection> projection);

		static unsigned short GetTileId(unsigned long long pointerOrTileToken);

		static unsigned short GetHighTileIdOrDefault(unsigned long long pointerOrTileToken, unsigned short fallbackTileId);

	protected:
		std::shared_ptr<MatrixTileMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<MatrixTileMicroOp>(MicroOp::shared_from_this());
		}
	};

	class MtileLoadMicroOp final : public MatrixTileMicroOp
	{
	public:
		MtileLoadMicroOp(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction);

	protected:
		std::shared_ptr<MtileLoadMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<MtileLoadMicroOp>(MatrixTileMicroOp::shared_from_this());
		}
	};

	class MtileStoreMicroOp final : public MatrixTileMicroOp
	{
	public:
		MtileStoreMicroOp(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction);

	protected:
		std::shared_ptr<MtileStoreMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<MtileStoreMicroOp>(MatrixTileMicroOp::shared_from_this());
		}
	};

	class MtileMaccMicroOp final : public MatrixTileMicroOp
	{
	public:
		MtileMaccMicroOp(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction);

	protected:
		std::shared_ptr<MtileMaccMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<MtileMaccMicroOp>(MatrixTileMicroOp::shared_from_this());
		}
	};

	class MtransposeMicroOp final : public MatrixTileMicroOp
	{
	public:
		MtransposeMicroOp(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction);

	protected:
		std::shared_ptr<MtransposeMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<MtransposeMicroOp>(MatrixTileMicroOp::shared_from_this());
		}
	};
}
