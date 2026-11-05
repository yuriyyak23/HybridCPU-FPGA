#pragma once

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute { class DmaStreamComputeTelemetryCounters; class DmaStreamComputeDescriptor; class DmaStreamComputeValidationResult; class DmaStreamComputeFaultRecord; struct DmaStreamComputeTokenLifecycleEvidence; class DmaStreamComputeTokenAdmissionResult; class DmaStreamComputeCommitResult; class DmaStreamComputeOwnerGuardDecision; class DmaStreamComputeMemoryRange; }
namespace YAKSys_Hybrid_CPU::Core::Memory { class MemoryCoherencyObserver; }
#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "DmaStreamComputeValidationResult.h"
#include "DmaStreamComputeProgressDiagnostics.h"
#include "DmaStreamComputeStagedWrite.h"
#include <string>
#include <vector>
#include <limits>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Memory;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	enum class DmaStreamComputeTokenState : unsigned char
	{
		Admitted = 0,
		Issued = 1,
		ReadsComplete = 2,
		ComputeComplete = 3,
		CommitPending = 4,
		Committed = 5,
		Faulted = 6,
		Canceled = 7
	};

	enum class DmaStreamComputeTokenCancelReason : unsigned char
	{
		None = 0,
		Flush = 1,
		ReplayDiscard = 2,
		Trap = 3
	};

	enum class DmaStreamComputeTokenFaultKind : unsigned char
	{
		None = 0,
		DescriptorDecodeFault = 1,
		UnsupportedAbiOrOperation = 2,
		TranslationFault = 3,
		PermissionFault = 4,
		DomainViolation = 5,
		OwnerContextViolation = 6,
		AlignmentFault = 7,
		AliasOverlapFault = 8,
		DmaDeviceFault = 9,
		PartialCompletionFault = 10,
		ReplayInvalidationBeforeCommit = 11,
		MemoryFault = 12,
		ExecutionDisabled = 13
	};

	enum class DmaStreamComputeFaultSourcePhase : unsigned char
	{
		Unknown = 0,
		CarrierDecode = 1,
		DescriptorParse = 2,
		Admission = 3,
		Read = 4,
		Compute = 5,
		Stage = 6,
		Completion = 7,
		Commit = 8,
		Backend = 9,
		Iommu = 10,
		Ordering = 11,
		Cancellation = 12
	};

	enum class DmaStreamComputeFaultPublicationContract : unsigned char
	{
		ModelRetireStyleOnly = 0,
		FuturePreciseRetireRequiresPublicationMetadata = 1
	};

	enum class DmaStreamComputeFaultPriorityClass : unsigned char
	{
		PreIssueCancellation = 1,
		CarrierDecodeAndTypedSlot = 2,
		DescriptorAdmission = 3,
		QuotaBackpressureTokenCap = 4,
		RuntimeReadBackendIommu = 5,
		ComputeStageCoverage = 6,
		CommitGuard = 7,
		CommitPhysicalWriteRollback = 8,
		PostIssueCancellation = 9
	};

	enum class DmaStreamComputeTokenAdmissionStatus : unsigned char
	{
		Accepted = 0,
		TelemetryReject = 1,
		ArchitecturalFault = 2
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeStagedWrite
//	{
//		public DmaStreamComputeStagedWrite(ulong address, ReadOnlySpan<byte> data)
//		{
//			Address = address;
//			byte[] copy = new byte[data.Length];
//			data.CopyTo(copy);
//			Data = copy;
//		}
//
//		public ulong Address
//		{
//			get;
//		}
//
//		public ReadOnlyMemory<byte> Data
//		{
//			get;
//		}
//
//		public ulong Length
//		{
//			get
//			{
//				return (ulong)Data.Length;
//			}
//		}
//	}

		class DmaStreamComputeFaultRecord final
	{
	public:
		DmaStreamComputeTokenFaultKind FaultKind{}; std::wstring Message; unsigned long long FaultAddress=0; bool IsWrite=false; int VirtualThreadId=0; unsigned long long OwnerDomainTag=0,ActiveDomainCertificate=0; DmaStreamComputeFaultSourcePhase SourcePhase{}; bool BackendExceptionNormalized=false; std::optional<std::wstring> NormalizedHostExceptionType; DmaStreamComputeFaultPublicationContract PublicationContract{};
		DmaStreamComputeFaultRecord(DmaStreamComputeTokenFaultKind kind,const std::wstring& message,unsigned long long address,bool write,int vt,unsigned long long ownerTag,unsigned long long certificate,DmaStreamComputeFaultSourcePhase phase=DmaStreamComputeFaultSourcePhase::Unknown,bool normalized=false,std::optional<std::wstring> hostType={},DmaStreamComputeFaultPublicationContract contract=DmaStreamComputeFaultPublicationContract::ModelRetireStyleOnly);
		bool getRequiresRetireExceptionPublication() const { return true; }
		std::runtime_error CreateRetireException() const;
		static std::shared_ptr<DmaStreamComputeFaultRecord> FromBackendException(std::shared_ptr<DmaStreamComputeDescriptor> descriptor,std::runtime_error exception,unsigned long long faultAddress,bool isWrite,DmaStreamComputeFaultSourcePhase phase=DmaStreamComputeFaultSourcePhase::Backend);
	};
class DmaStreamComputeCommitResult final
	{
	public:
		const bool Succeeded, IsCanceled; const DmaStreamComputeTokenState TokenState; const std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> Fault;
		DmaStreamComputeCommitResult(bool succeeded,bool canceled,DmaStreamComputeTokenState state,std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> fault={}):Succeeded(succeeded),IsCanceled(canceled),TokenState(state),Fault(std::move(fault)){}
		bool getRequiresRetireExceptionPublication() const;
		std::runtime_error CreateRetireException() const;

		static std::shared_ptr<DmaStreamComputeCommitResult> Pending(DmaStreamComputeTokenState state);
		static std::shared_ptr<DmaStreamComputeCommitResult> Success();
		static std::shared_ptr<DmaStreamComputeCommitResult> Canceled();
		static std::shared_ptr<DmaStreamComputeCommitResult> Faulted(std::shared_ptr<DmaStreamComputeFaultRecord> fault);
	};

	class DmaStreamComputeToken;
	class DmaStreamComputeTokenAdmissionResult final
	{
	public:
		const DmaStreamComputeTokenAdmissionStatus Status;
		const DmaStreamComputeValidationFault ValidationFault;
		const std::shared_ptr<DmaStreamComputeToken> Token;
		const std::shared_ptr<DmaStreamComputeFaultRecord> Fault;
		const std::wstring Message;
		const bool IsAccepted;
		const bool IsTelemetryOnlyReject;
	private:
		DmaStreamComputeTokenAdmissionResult(DmaStreamComputeTokenAdmissionStatus status, DmaStreamComputeValidationFault validationFault, std::shared_ptr<DmaStreamComputeToken> token, std::shared_ptr<DmaStreamComputeFaultRecord> fault, std::wstring message)
			: Status(status), ValidationFault(validationFault), Token(std::move(token)), Fault(std::move(fault)), Message(std::move(message)), IsAccepted(status == DmaStreamComputeTokenAdmissionStatus::Accepted && Token != nullptr), IsTelemetryOnlyReject(status == DmaStreamComputeTokenAdmissionStatus::TelemetryReject) {}
	public:
		static std::shared_ptr<DmaStreamComputeTokenAdmissionResult> Accepted(std::shared_ptr<DmaStreamComputeToken> token) {
			return std::shared_ptr<DmaStreamComputeTokenAdmissionResult>(new DmaStreamComputeTokenAdmissionResult(DmaStreamComputeTokenAdmissionStatus::Accepted, DmaStreamComputeValidationFault::None, std::move(token), nullptr, L"DmaStreamCompute token admitted after descriptor, owner/domain, and footprint guards."));
		}
		static std::shared_ptr<DmaStreamComputeTokenAdmissionResult> TelemetryReject(DmaStreamComputeValidationFault fault, const std::wstring& message) {
			return std::shared_ptr<DmaStreamComputeTokenAdmissionResult>(new DmaStreamComputeTokenAdmissionResult(DmaStreamComputeTokenAdmissionStatus::TelemetryReject, fault, nullptr, nullptr, message));
		}
		static std::shared_ptr<DmaStreamComputeTokenAdmissionResult> ArchitecturalFault(DmaStreamComputeValidationFault fault, std::shared_ptr<DmaStreamComputeFaultRecord> record, const std::wstring& message) {
			return std::shared_ptr<DmaStreamComputeTokenAdmissionResult>(new DmaStreamComputeTokenAdmissionResult(DmaStreamComputeTokenAdmissionStatus::ArchitecturalFault, fault, nullptr, std::move(record), message));
		}
		bool RequiresRetireExceptionPublication() const;
	};

	class DmaStreamComputeToken final : public std::enable_shared_from_this<DmaStreamComputeToken>
	{
	private:
		const unsigned long long TokenId;
		const std::shared_ptr<DmaStreamComputeDescriptor> Descriptor;
		DmaStreamComputeTokenState State = static_cast<DmaStreamComputeTokenState>(0);
		std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> LastFault;
		DmaStreamComputeTokenCancelReason CancelReason = static_cast<DmaStreamComputeTokenCancelReason>(0);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<DmaStreamComputeStagedWrite> _stagedWrites = new();
		std::vector<std::shared_ptr<DmaStreamComputeStagedWrite>> _stagedWrites = std::vector<std::shared_ptr<DmaStreamComputeStagedWrite>>();
		std::shared_ptr<DmaStreamComputeProgressDiagnostics> _progressDiagnostics = DmaStreamComputeProgressDiagnostics::Empty;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DmaStreamComputeToken(DmaStreamComputeDescriptor descriptor, ulong tokenId, DmaStreamComputeTelemetryCounters? telemetry = null)
		DmaStreamComputeToken(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, unsigned long long tokenId, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry = nullptr);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly DmaStreamComputeTelemetryCounters? _telemetry;
		const std::shared_ptr<DmaStreamComputeTelemetryCounters> _telemetry;

	public:
		unsigned long long getTokenId() const;

		std::shared_ptr<DmaStreamComputeDescriptor> getDescriptor() const;

		DmaStreamComputeTokenState getState() const;
	private:
		void setState(DmaStreamComputeTokenState value);

	public:
		std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> getLastFault() const;
	private:
		void setLastFault(const std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> &value);

	public:
		DmaStreamComputeTokenCancelReason getCancelReason() const;
	private:
		void setCancelReason(DmaStreamComputeTokenCancelReason value);

	public:
		bool getHasCommitted() const;

		int getStagedWriteCount() const;

		std::shared_ptr<DmaStreamComputeProgressDiagnostics> getProgressDiagnostics() const;

		const std::vector<std::shared_ptr<DmaStreamComputeStagedWrite>> GetStagedWriteSnapshot();

		std::shared_ptr<DmaStreamComputeTokenLifecycleEvidence> ExportLifecycleEvidence();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeTokenAdmissionResult TryAdmit(DmaStreamComputeValidationResult validationResult, ulong tokenId, DmaStreamComputeTelemetryCounters? telemetry = null)
		static std::shared_ptr<DmaStreamComputeTokenAdmissionResult> TryAdmit(std::shared_ptr<DmaStreamComputeValidationResult> validationResult, unsigned long long tokenId, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry = nullptr);

		void MarkIssued();

		std::shared_ptr<DmaStreamComputeProgressDiagnostics> RecordProgressDiagnostics(unsigned long long bytesRead = 0, unsigned long long bytesStaged = 0, unsigned long long elementOperations = 0, unsigned long long modeledLatencyCycles = 0, unsigned long long backendStepCount = 0);

		void MarkReadsComplete();

		void StageDestinationWrite(unsigned long long address, std::span<const unsigned char> data);

		std::shared_ptr<DmaStreamComputeCommitResult> MarkComputeComplete();

		void Cancel(DmaStreamComputeTokenCancelReason reason);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DmaStreamComputeCommitResult Commit(Processor.MainMemoryArea mainMemory, DmaStreamComputeOwnerGuardDecision commitGuardDecision, MemoryCoherencyObserver? coherencyObserver = null)
		std::shared_ptr<DmaStreamComputeCommitResult> Commit(std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> commitGuardDecision, std::shared_ptr<MemoryCoherencyObserver> coherencyObserver = nullptr);

		std::shared_ptr<DmaStreamComputeCommitResult> PublishFault(DmaStreamComputeTokenFaultKind faultKind, const std::wstring &message, unsigned long long faultAddress, bool isWrite);

		std::shared_ptr<DmaStreamComputeCommitResult> PublishFault(std::shared_ptr<DmaStreamComputeFaultRecord> fault);

	private:
		static bool IsTelemetryOnlyAdmissionReject(DmaStreamComputeValidationFault fault);

		bool TryCommitAllOrNone(std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> &fault);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private void NotifyCommittedWrites(MemoryCoherencyObserver? coherencyObserver)
		void NotifyCommittedWrites(std::shared_ptr<MemoryCoherencyObserver> coherencyObserver);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private bool HasExactStagedWriteCoverage(out ulong faultAddress, out string? failureMessage)
		bool HasExactStagedWriteCoverage(unsigned long long &faultAddress, std::wstring &failureMessage);

		void EnsureState(DmaStreamComputeTokenState expectedState, const std::wstring &operation);

		std::shared_ptr<DmaStreamComputeFaultRecord> CreateFault(DmaStreamComputeTokenFaultKind faultKind, const std::wstring &message, unsigned long long faultAddress, bool isWrite);

		static std::shared_ptr<DmaStreamComputeFaultRecord> CreateFaultFromValidationFailure(DmaStreamComputeValidationFault validationFault, const std::wstring &message);

		static bool TryValidateDescriptorGuards(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> guardDecision, const std::wstring &surface, std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> &fault);

		static bool IsDomainCoveredByCertificate(unsigned long long ownerDomainTag, unsigned long long activeDomainCertificate);

		static std::shared_ptr<DmaStreamComputeFaultRecord> CreateDescriptorFault(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, DmaStreamComputeTokenFaultKind faultKind, const std::wstring &message);

		static std::shared_ptr<DmaStreamComputeFaultRecord> CreateGuardFault(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> guardDecision, DmaStreamComputeTokenFaultKind faultKind, const std::wstring &message);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool IsRangeCoveredByFootprint(ulong address, ulong length, IReadOnlyList<DmaStreamComputeMemoryRange> footprint)
		static bool IsRangeCoveredByFootprint(unsigned long long address, unsigned long long length, const std::vector<DmaStreamComputeMemoryRange> &footprint);

		static bool HasExactMainMemoryRange(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, int size);
	};
}








