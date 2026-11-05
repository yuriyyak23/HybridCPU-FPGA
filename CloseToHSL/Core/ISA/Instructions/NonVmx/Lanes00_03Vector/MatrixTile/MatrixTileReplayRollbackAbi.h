#pragma once

#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "MatrixTileIrProjectionAndMaterializer.h"
#include "MatrixTileResourceContour.h"
#include "../../../../../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include <string>
#include <vector>
#include <limits>
#include <optional>
#include <format>
#include <memory>
#include "tangible_exception_helper.h"

using namespace YAKSys_Hybrid_CPU::Core;


namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	struct MatrixTileNumericPolicy; struct MatrixTileLayoutPolicy;
enum class MatrixTileRetirePublicationKind : unsigned char;
struct MatrixTileReplayIdentity
{
	unsigned int CoreId;
	int OwnerThreadId;
	enum YAKSys_Hybrid_CPU::Core::SlotClass SlotClass;
	unsigned int Opcode;
	MatrixTileProjectedOperationKind OperationKind;
	unsigned long long CaptureOrdinal;
	unsigned long long CaptureFingerprint;
	unsigned long long DecodedInstructionFingerprint;
	unsigned long long MaterializedInstructionFingerprint;
	unsigned long long DescriptorFingerprint;
	MatrixTileRuntimeResourceClass ResourceClass;
	unsigned long long ResourceContourFingerprint;
	unsigned long long StreamTransferFingerprint;
	unsigned long long NumericPolicyFingerprint;
	unsigned long long LayoutPolicyFingerprint;
	unsigned long long PolicyIdentityFingerprint;
	unsigned long long ReplayEpoch;
	unsigned long long DependencyFingerprint;
	MatrixTileRetirePublicationKind PublicationSurface;
	unsigned long long CheckpointOrdinal;

	MatrixTileReplayIdentity(unsigned int coreId, int ownerThreadId, unsigned int opcode, MatrixTileProjectedOperationKind operationKind, unsigned long long captureOrdinal, unsigned long long captureFingerprint, unsigned long long decodedInstructionFingerprint, unsigned long long materializedInstructionFingerprint, unsigned long long descriptorFingerprint, MatrixTileRuntimeResourceClass resourceClass, enum YAKSys_Hybrid_CPU::Core::SlotClass slotClass, unsigned long long resourceContourFingerprint, unsigned long long streamTransferFingerprint, unsigned long long numericPolicyFingerprint, unsigned long long layoutPolicyFingerprint, unsigned long long policyIdentityFingerprint, unsigned long long replayEpoch, unsigned long long dependencyFingerprint, MatrixTileRetirePublicationKind publicationSurface, unsigned long long checkpointOrdinal)
		: CoreId(coreId), OwnerThreadId(ownerThreadId), Opcode(opcode), OperationKind(operationKind), CaptureOrdinal(captureOrdinal), CaptureFingerprint(captureFingerprint), DecodedInstructionFingerprint(decodedInstructionFingerprint), MaterializedInstructionFingerprint(materializedInstructionFingerprint), DescriptorFingerprint(descriptorFingerprint), ResourceClass(resourceClass), SlotClass(slotClass), ResourceContourFingerprint(resourceContourFingerprint), StreamTransferFingerprint(streamTransferFingerprint), NumericPolicyFingerprint(numericPolicyFingerprint), LayoutPolicyFingerprint(layoutPolicyFingerprint), PolicyIdentityFingerprint(policyIdentityFingerprint), ReplayEpoch(replayEpoch), DependencyFingerprint(dependencyFingerprint), PublicationSurface(publicationSurface), CheckpointOrdinal(checkpointOrdinal) {}

	bool getIsValid() const
	{
		return OwnerThreadId >= 0 && OwnerThreadId < Processor::CPU_Core::SmtWays && Opcode != 0 && OperationKind != MatrixTileProjectedOperationKind::Unspecified && CaptureOrdinal != 0 && CaptureFingerprint != 0 && DecodedInstructionFingerprint != 0 && MaterializedInstructionFingerprint != 0 && DescriptorFingerprint != 0 && ResourceClass != MatrixTileRuntimeResourceClass::None && ResourceContourFingerprint != 0 && (ResourceClass != MatrixTileRuntimeResourceClass::MatrixTileMemory || StreamTransferFingerprint != 0) && PolicyIdentityFingerprint != 0 && ReplayEpoch != 0 && DependencyFingerprint != 0 && static_cast<unsigned char>(PublicationSurface) != 0 && (OperationKind != MatrixTileProjectedOperationKind::Macc || (NumericPolicyFingerprint != 0 && LayoutPolicyFingerprint != 0)) && (OperationKind != MatrixTileProjectedOperationKind::Transpose || LayoutPolicyFingerprint != 0) && CheckpointOrdinal != 0;
	}
	operator bool() const { return getIsValid(); }
};
enum class MatrixTileReplayRollbackLifecycle : unsigned char;
struct MatrixTileRollbackOutcome final
{
	const MatrixTileReplayIdentity ReplayIdentity;
	const bool RestoredTileState;
	const bool RestoredMemory;
	const bool FaultOnlyRollback;
	const MatrixTileReplayRollbackLifecycle Lifecycle;

	MatrixTileRollbackOutcome(MatrixTileReplayIdentity replayIdentity, bool restoredTileState,
		bool restoredMemory, bool faultOnlyRollback, MatrixTileReplayRollbackLifecycle lifecycle)
		: ReplayIdentity(replayIdentity), RestoredTileState(restoredTileState),
		  RestoredMemory(restoredMemory), FaultOnlyRollback(faultOnlyRollback), Lifecycle(lifecycle)
	{
	}
};
struct MatrixTileRetireOutcome;
struct MatrixTileExecutionCaptureRecord;
struct MatrixTileTileImage;
struct MatrixTileCapturedMemoryWrite;
struct MatrixTileMaterializedInstruction;
struct MatrixTileCaptureIdentity;
struct MatrixTileInstructionIrProjection;
enum class MatrixTileReplayRollbackLifecycle : unsigned char
	{
		None = 0,
		Retired = 1,
		FaultRetired = 2,
		RolledBack = 3,
		Replayed = 4,
		FaultReplayed = 5,
		Discarded = 6
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileReplayIdentity(uint CoreId, int OwnerThreadId, uint Opcode, MatrixTileProjectedOperationKind OperationKind, ulong CaptureOrdinal, ulong CaptureFingerprint, ulong DecodedInstructionFingerprint, ulong MaterializedInstructionFingerprint, ulong DescriptorFingerprint, MatrixTileRuntimeResourceClass ResourceClass, SlotClass SlotClass, ulong ResourceContourFingerprint, ulong StreamTransferFingerprint, ulong NumericPolicyFingerprint, ulong LayoutPolicyFingerprint, ulong PolicyIdentityFingerprint, ulong ReplayEpoch, ulong DependencyFingerprint, MatrixTileRetirePublicationKind PublicationSurface, ulong CheckpointOrdinal)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return OwnerThreadId is >= 0 @and < Processor.CPU_Core.SmtWays && Opcode != 0 && OperationKind != MatrixTileProjectedOperationKind.Unspecified && CaptureOrdinal != 0 && CaptureFingerprint != 0 && DecodedInstructionFingerprint != 0 && MaterializedInstructionFingerprint != 0 && DescriptorFingerprint != 0 && ResourceClass != MatrixTileRuntimeResourceClass.None && ResourceContourFingerprint != 0 && (ResourceClass != MatrixTileRuntimeResourceClass.MatrixTileMemory || StreamTransferFingerprint != 0) && PolicyIdentityFingerprint != 0 && ReplayEpoch != 0 && DependencyFingerprint != 0 && PublicationSurface != MatrixTileRetirePublicationKind.None && (OperationKind != MatrixTileProjectedOperationKind.Macc || (NumericPolicyFingerprint != 0 && LayoutPolicyFingerprint != 0)) && (OperationKind != MatrixTileProjectedOperationKind.Transpose || LayoutPolicyFingerprint != 0) && CheckpointOrdinal != 0;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileRollbackOutcome(MatrixTileReplayIdentity ReplayIdentity, bool RestoredTileState, bool RestoredMemory, bool FaultOnlyRollback, MatrixTileReplayRollbackLifecycle Lifecycle);

	class MatrixTileReplayRollbackValidationException final : public InvalidOperationException
	{
	public:
		MatrixTileReplayRollbackValidationException(const std::wstring &message);
	};

	class MatrixTileReplayRollbackJournal final : public std::enable_shared_from_this<MatrixTileReplayRollbackJournal>
	{
	private:
		const std::shared_ptr<MatrixTileReplayIdentity> ReplayIdentity;
		const std::shared_ptr<MatrixTileRetireOutcome> OriginalRetireOutcome;
		std::optional<std::shared_ptr<MatrixTileRetireOutcome>> ReplayOutcome;
		MatrixTileReplayRollbackLifecycle Lifecycle = static_cast<MatrixTileReplayRollbackLifecycle>(0);
		const std::shared_ptr<MatrixTileExecutionCaptureRecord> Capture;
		const bool HadTileCheckpoint;
		const std::shared_ptr<MatrixTileTileImage> TileCheckpoint;
		const std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> MemoryCheckpoint;

	public:
		MatrixTileReplayRollbackJournal(std::shared_ptr<MatrixTileReplayIdentity> replayIdentity, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileRetireOutcome> retireOutcome, bool hadTileCheckpoint, std::shared_ptr<MatrixTileTileImage> tileCheckpoint, std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> &memoryCheckpoint);

		std::shared_ptr<MatrixTileReplayIdentity> getReplayIdentity() const;

		std::shared_ptr<MatrixTileRetireOutcome> getOriginalRetireOutcome() const;

		std::optional<std::shared_ptr<MatrixTileRetireOutcome>> getReplayOutcome() const;
		void setReplayOutcome(const std::optional<std::shared_ptr<MatrixTileRetireOutcome>> &value);

		MatrixTileReplayRollbackLifecycle getLifecycle() const;
		void setLifecycle(MatrixTileReplayRollbackLifecycle value);

		bool getHasTileCheckpoint() const;

		bool getHadArchitecturalTileBeforeRetire() const;

		int getMemoryCheckpointRowCount() const;

		bool isFaultOnly() const;

		std::shared_ptr<MatrixTileExecutionCaptureRecord> getCapture() const;

		bool getHadTileCheckpoint() const;

		std::shared_ptr<MatrixTileTileImage> getTileCheckpoint() const;

		std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> getMemoryCheckpoint() const;

	private:
		static std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> CloneWrites(std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> &writes);
	};

	class MatrixTileReplayRollbackAbi final
	{
	public:
		static inline const std::wstring ReplayRollbackDecision = L"ClosedMatrixTileReplayRollbackConformance";
		static inline const std::wstring DecodedInstructionIdentityDecision = L"ReplayStableDecodedInstructionFingerprint";
		static inline const std::wstring MaterializedInstructionIdentityDecision = L"ReplayStableMaterializedInstructionFingerprint";
		static inline const std::wstring TileDescriptorIdentityDecision = L"ReplayStableCanonicalTileDescriptorFingerprint";
		static inline const std::wstring ResourceContourIdentityDecision = L"ReplayStableMatrixTileResourceContourFingerprint";
		static inline const std::wstring StreamTransferIdentityDecision = L"ReplayStableTypedStreamTransferFingerprint";
		static inline const std::wstring PolicyBoundIdentityDecision = L"ReplayStableNumericLayoutPolicyDependencyAndEpochFingerprint";
		static inline const std::wstring TileRollbackDecision = L"CoreOwnedTileCheckpointRestore";
		static inline const std::wstring MemoryRollbackDecision = L"CoreOwnedAllOrNoneStoreCheckpointRestore";
		static inline const std::wstring AccumulatorRollbackDecision = L"CoreOwnedAccumulatorCheckpointRestore";
		static inline const std::wstring MemoryFaultReplayDecision = L"DeterministicCapturedAndRetireMemoryFaultReplay";
		static inline const std::wstring DescriptorFaultReplayDecision = L"DeterministicDescriptorAndSemanticFaultReplay";
		static inline const std::wstring DuplicateReplayDecision = L"RollbackBeforeReplayAndConsumeReplayAuthorityOnce";
		static inline const std::wstring StaleCheckpointDecision = L"StalePublishedOrPreRetireStateFailsClosed";
		static inline const std::wstring OwnershipDecision = L"CoreRegistersRetireOwnedMatrixTileReplayJournal";
		static inline const std::wstring BypassDecision = L"ReplayRequiresRegisteredRetiredCaptureJournal";
		static inline const std::wstring FallbackDecision = L"NoScalarVectorBaseMemoryDotDscLane7VmxBackendOrCompilerFallback";

		static constexpr bool HasReplayRollbackConformance = true;
		static constexpr bool HasDecodedInstructionReplayIdentity = true;
		static constexpr bool HasMaterializedInstructionReplayIdentity = true;
		static constexpr bool HasTileDescriptorReplayIdentity = true;
		static constexpr bool HasResourceContourReplayIdentity = true;
		static constexpr bool HasStreamTransferReplayIdentity = true;
		static constexpr bool HasPolicyBoundReplayIdentity = true;
		static constexpr bool HasPendingTileWriteRollback = true;
		static constexpr bool HasPendingMemoryStoreRollback = true;
		static constexpr bool HasAccumulatorRollback = true;
		static constexpr bool HasDeterministicReplayAfterMemoryFault = true;
		static constexpr bool HasDeterministicReplayAfterDescriptorFault = true;
		static constexpr bool HasLegalIllegalConformanceVectors = true;
		static constexpr bool BlocksReplayWithoutRetirePublication = true;
		static constexpr bool BlocksCaptureRecordIdentityBypass = true;
		static constexpr bool RejectsDuplicateReplay = true;
		static constexpr bool RejectsStaleRollback = true;
		static constexpr bool KeepsHostOwnedEvidenceNonArchitectural = true;
		static constexpr bool KeepsCompilerScopeClosed = true;
		static constexpr bool UsesFallbackPath = false;

		static std::shared_ptr<MatrixTileReplayRollbackJournal> RetireWithCheckpoint(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileCaptureIdentity> expectedCaptureIdentity, unsigned long long checkpointOrdinal);

		static std::shared_ptr<MatrixTileRollbackOutcome> Rollback(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileReplayRollbackJournal> journal, std::shared_ptr<MatrixTileReplayIdentity> expectedIdentity);

		static std::shared_ptr<MatrixTileRetireOutcome> Replay(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction, std::shared_ptr<MatrixTileReplayRollbackJournal> journal, std::shared_ptr<MatrixTileReplayIdentity> expectedIdentity);

		static void Discard(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileReplayRollbackJournal> journal, std::shared_ptr<MatrixTileReplayIdentity> expectedIdentity);

	private:
		static std::shared_ptr<MatrixTileReplayIdentity> CreateReplayIdentity(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, unsigned long long checkpointOrdinal);

		static void CapturePreRetireState(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, bool &hadTileCheckpoint, std::shared_ptr<MatrixTileTileImage> &tileCheckpoint, std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> &memoryCheckpoint);

		static void ValidatePreRetireState(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileReplayRollbackJournal> journal);

		static void RequireCurrentTileImage(std::shared_ptr<Processor::CPU_Core> &core, int ownerThreadId, std::shared_ptr<MatrixTileTileImage> expected, const std::wstring &operation);

		static void RequireCurrentMemoryRows(std::shared_ptr<Processor::CPU_Core> &core, std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> &expected, const std::wstring &operation);

		static void ValidateOwnedJournal(std::shared_ptr<Processor::CPU_Core> core, std::shared_ptr<MatrixTileReplayRollbackJournal> journal, std::shared_ptr<MatrixTileReplayIdentity> expectedIdentity);

		static void ValidateReplayIdentity(std::shared_ptr<Processor::CPU_Core> core, std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileReplayIdentity> replayIdentity);

		static unsigned long long ComputeDecodedInstructionFingerprint(std::shared_ptr<MatrixTileInstructionIrProjection> projection);

		static unsigned long long ComputeMaterializedInstructionFingerprint(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction);

		static unsigned long long ComputeDescriptorFingerprint(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction);

		static unsigned long long ComputeResourceContourFingerprint(unsigned int opcode);

		static unsigned long long ComputeStreamTransferIdentityFingerprint(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture);

		static bool TileImagesEqual(std::shared_ptr<MatrixTileTileImage> left, std::shared_ptr<MatrixTileTileImage> right);

		static unsigned long long BeginHash();

		static unsigned long long FinishHash(unsigned long long hash);

		static void AddByte(unsigned long long &hash, unsigned char value);

		static void AddUInt16(unsigned long long &hash, unsigned short value);

		static void AddUInt32(unsigned long long &hash, unsigned int value);

		static void AddUInt64(unsigned long long &hash, unsigned long long value);

		static void AddString(unsigned long long &hash, const std::wstring &value);

		static void AddDescriptor(unsigned long long &hash, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor);

		static void AddNumericPolicyIdentity(unsigned long long &hash, std::optional<std::shared_ptr<MatrixTileNumericPolicy>> &policy);

		static void AddLayoutPolicyIdentity(unsigned long long &hash, std::optional<std::shared_ptr<MatrixTileLayoutPolicy>> &policy);

		static std::shared_ptr<MatrixTileReplayRollbackValidationException> Validation(const std::wstring &message);
	};

}
