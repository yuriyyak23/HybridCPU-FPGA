#pragma once

#include "MatrixTileIrProjectionAndMaterializer.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "MatrixTileExecuteCaptureAbi.h"
#include "MatrixTileMemoryShapeAndFaultAbi.h"
#include "MatrixTileAccumulatorAndTransposePolicyAbi.h"
#include <string>
#include <format>
#include <memory>
#include "../../../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	enum class MatrixTileRetirePublicationKind : unsigned char
	{
		None = 0,
		TileState = 1,
		MemoryStore = 2,
		Accumulator = 3,
		Fault = 4
	};

	enum class MatrixTileRetireFaultKind : unsigned char
	{
		None = 0,
		CapturedExecutionFault = 1,
		MemoryCommitFault = 2
	};

	struct MatrixTileRetireOutcome final
	{
		const MatrixTileCaptureIdentity CaptureIdentity;
		const MatrixTileRetirePublicationKind PublicationKind;
		const MatrixTileRetireFaultKind RetireFaultKind;
		const MatrixTileExecutionFaultKind ExecutionFaultKind;
		const MatrixTileMemoryFaultKind MemoryFaultKind;
		const MatrixTileSemanticFaultKind SemanticFaultKind;
		const MatrixTileMemoryFaultPoint FaultPoint;
		const bool HasFaultPoint;
		const std::wstring Message;
		const bool PublishedArchitecturalState;
		const bool CommittedMemory;
		const bool FaultRetired;
		const bool IsSuccess;

		MatrixTileRetireOutcome(MatrixTileCaptureIdentity captureIdentity,
			MatrixTileRetirePublicationKind publicationKind, MatrixTileRetireFaultKind retireFaultKind,
			MatrixTileExecutionFaultKind executionFaultKind, MatrixTileMemoryFaultKind memoryFaultKind,
			MatrixTileSemanticFaultKind semanticFaultKind, MatrixTileMemoryFaultPoint faultPoint,
			bool hasFaultPoint, std::wstring message, bool publishedArchitecturalState,
			bool committedMemory, bool faultRetired)
			: CaptureIdentity(captureIdentity), PublicationKind(publicationKind),
			  RetireFaultKind(retireFaultKind), ExecutionFaultKind(executionFaultKind),
			  MemoryFaultKind(memoryFaultKind), SemanticFaultKind(semanticFaultKind),
			  FaultPoint(faultPoint), HasFaultPoint(hasFaultPoint), Message(std::move(message)),
			  PublishedArchitecturalState(publishedArchitecturalState), CommittedMemory(committedMemory),
			  FaultRetired(faultRetired),
			  IsSuccess(!faultRetired && retireFaultKind == MatrixTileRetireFaultKind::None)
		{
		}

		static std::shared_ptr<MatrixTileRetireOutcome> Published(
			MatrixTileCaptureIdentity captureIdentity,
			MatrixTileRetirePublicationKind publicationKind)
		{
			return std::make_shared<MatrixTileRetireOutcome>(captureIdentity, publicationKind,
				MatrixTileRetireFaultKind::None, MatrixTileExecutionFaultKind::None,
				MatrixTileMemoryFaultKind::None, MatrixTileSemanticFaultKind::None,
				MatrixTileMemoryFaultPoint{}, false, L"",
				publicationKind == MatrixTileRetirePublicationKind::TileState ||
					publicationKind == MatrixTileRetirePublicationKind::Accumulator,
				publicationKind == MatrixTileRetirePublicationKind::MemoryStore, false);
		}

		static std::shared_ptr<MatrixTileRetireOutcome> Fault(
			std::shared_ptr<MatrixTileExecutionCaptureRecord> capture,
			MatrixTileRetireFaultKind retireFaultKind, const std::wstring &message)
		{
			return std::make_shared<MatrixTileRetireOutcome>(capture->CaptureIdentity,
				MatrixTileRetirePublicationKind::Fault, retireFaultKind, capture->FaultKind,
				capture->MemoryFaultKind, capture->SemanticFaultKind, capture->FaultPoint,
				capture->HasFaultPoint, message, false, false, true);
		}
	};

	// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileRetireOutcome(MatrixTileCaptureIdentity CaptureIdentity, MatrixTileRetirePublicationKind PublicationKind, MatrixTileRetireFaultKind RetireFaultKind, MatrixTileExecutionFaultKind ExecutionFaultKind, MatrixTileMemoryFaultKind MemoryFaultKind, MatrixTileSemanticFaultKind SemanticFaultKind, MatrixTileMemoryFaultPoint FaultPoint, bool HasFaultPoint, string Message, bool PublishedArchitecturalState, bool CommittedMemory, bool FaultRetired)
//	{
//		public bool IsSuccess
//		{
//			get
//			{
//				return !FaultRetired && RetireFaultKind == MatrixTileRetireFaultKind.None;
//			}
//		}
//
//		public static MatrixTileRetireOutcome Published(MatrixTileCaptureIdentity captureIdentity, MatrixTileRetirePublicationKind publicationKind)
//		{
//			return new(captureIdentity, publicationKind, MatrixTileRetireFaultKind.None, MatrixTileExecutionFaultKind.None, MatrixTileMemoryFaultKind.None, MatrixTileSemanticFaultKind.None, FaultPoint: default, HasFaultPoint: false, Message: string.Empty, PublishedArchitecturalState: publicationKind is MatrixTileRetirePublicationKind.TileState or MatrixTileRetirePublicationKind.Accumulator, CommittedMemory: publicationKind == MatrixTileRetirePublicationKind.MemoryStore, FaultRetired: false);
//		}
//
//		public static MatrixTileRetireOutcome Fault(in MatrixTileExecutionCaptureRecord capture, MatrixTileRetireFaultKind retireFaultKind, string message)
//		{
//			return new(capture.CaptureIdentity, MatrixTileRetirePublicationKind.Fault, retireFaultKind, capture.FaultKind, capture.MemoryFaultKind, capture.SemanticFaultKind, capture.FaultPoint, capture.HasFaultPoint, message, PublishedArchitecturalState: false, CommittedMemory: false, FaultRetired: true);
//		}
//	}

	class MatrixTileRetireValidationException final : public InvalidOperationException
	{
	public:
		MatrixTileRetireValidationException(const std::wstring &message);
	};

	class MatrixTileRetireFaultException final : public InvalidOperationException
	{
	private:
		const std::shared_ptr<MatrixTileRetireOutcome> Outcome;

	public:
		MatrixTileRetireFaultException(std::shared_ptr<MatrixTileRetireOutcome> outcome);

		std::shared_ptr<MatrixTileRetireOutcome> getOutcome() const;
	};

	class MatrixTileRetirePublicationAbi final
	{
	public:
		static inline const std::wstring RetirePublicationDecision = L"ClosedMatrixTileRetirePublicationAndCommit";
		static inline const std::wstring LoadPublicationDecision = L"RetireOwnedTileLoadPublication";
		static inline const std::wstring StoreCommitDecision = L"RetireOwnedAllOrNoneTileStoreCommit";
		static inline const std::wstring MaccPublicationDecision = L"RetireOwnedAccumulatorPublication";
		static inline const std::wstring TransposePublicationDecision = L"RetireOwnedTransposeDestinationPublication";
		static inline const std::wstring FaultRetirementDecision = L"DeterministicFaultRetirementWithoutPartialPublication";
		static inline const std::wstring WritebackOwnershipDecision = L"MatrixTileMicroOpWriteBackOwnsCaptureConsumption";
		static inline const std::wstring SideEffectOrderingDecision = L"ValidateThenPublishSingleArchitecturalEffectAtRetire";
		static inline const std::wstring ArchitecturalVisibilityDecision = L"CaptureInvisibleUntilRetirePublication";
		static inline const std::wstring DuplicateRetireDecision = L"DuplicateCancelledAndMismatchedRetireFailClosed";
		static inline const std::wstring CorrelationDecision = L"CoreOwnerOpcodeOperationAndCaptureOrdinalCorrelated";
		static inline const std::wstring FallbackDecision = L"NoScalarVectorBaseMemoryDotDscLane7VmxBackendOrCompilerFallback";

		static constexpr bool HasRetirePublicationAndCommit = true;
		static constexpr bool HasTileLoadRetirePublication = true;
		static constexpr bool HasTileStoreRetireCommit = true;
		static constexpr bool HasAccumulatorRetirePublication = true;
		static constexpr bool HasTransposeRetirePublication = true;
		static constexpr bool HasFaultRetirementPolicy = true;
		static constexpr bool HasWritebackOwnership = true;
		static constexpr bool HasSideEffectPublicationOrdering = true;
		static constexpr bool HasArchitecturalStateVisibilityRules = true;
		static constexpr bool HasCaptureToRetireCorrelation = true;
		static constexpr bool RejectsDuplicateRetire = true;
		static constexpr bool RejectsCancelledRetire = true;
		static constexpr bool BlocksExecuteCaptureToRetireBypass = true;
		static constexpr bool KeepsHostOwnedEvidenceNonArchitectural = true;
		static constexpr bool KeepsReplayRollbackNonAuthority = true;
		static constexpr bool KeepsCompilerScopeClosed = true;
		static constexpr bool UsesFallbackPath = false;

		static std::shared_ptr<MatrixTileCaptureIdentity> CreateCaptureIdentity(unsigned int coreId, int ownerThreadId, unsigned int opcode, MatrixTileProjectedOperationKind operationKind, unsigned long long captureOrdinal, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture);

		static std::shared_ptr<MatrixTileRetireOutcome> Retire(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileCaptureIdentity> expectedIdentity);

		static void ValidateForRetire(std::shared_ptr<Processor::CPU_Core> core, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileCaptureIdentity> expectedIdentity);

	private:
		static std::shared_ptr<MatrixTileRetireOutcome> RetireTilePublication(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, MatrixTileExecutionCaptureKind expectedCaptureKind, MatrixTileRetirePublicationKind publicationKind);

		static std::shared_ptr<MatrixTileRetireOutcome> RetireStore(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture);

		static void ValidateTilePublicationCapture(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, MatrixTileExecutionCaptureKind expectedCaptureKind);

		static void ValidateStoreCapture(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture);

		static void ValidateCorrelation(std::shared_ptr<Processor::CPU_Core> core, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileCaptureIdentity> expectedIdentity);

		static void ValidateEnvelope(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture);

		static void ValidateFaultCapture(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture);

		static void ValidateStoreWrites(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture);

		static std::wstring CreateFaultRetirementMessage(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture);

		static unsigned long long ComputeCaptureFingerprint(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture);

		static unsigned int GetOpcode(MatrixTileProjectedOperationKind operationKind);

		static std::wstring GetMnemonic(MatrixTileProjectedOperationKind operationKind);

		static std::shared_ptr<MatrixTileRetireValidationException> Validation(const std::wstring &message);
	};

}
