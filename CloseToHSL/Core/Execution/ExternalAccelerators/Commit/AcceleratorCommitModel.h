#pragma once

#include "../../../../../ExternalRuntime/Contracts/ExternalOperationContracts.h"
#include "../../../../Memory/Registers/YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.h"
#include "../Telemetry/AcceleratorTelemetry.h"
#include "../Conflicts/ExternalAcceleratorConflictManager.h"
#include "../Memory/AcceleratorMemoryModel.h"
#include "../Tokens/AcceleratorToken.h"
#include "../../../Memory/MemoryCoherencyObserver.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../Tokens/AcceleratorTokenStatusWord.h"
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <optional>
#include <format>
#include <memory>

using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Conflicts;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Commit
{

	enum class AcceleratorCommitInvalidationTarget : unsigned char
	{
		SrfWindow = 0,
		CacheWindow = 1
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorCommitFault
//	{
//		public AcceleratorCommitFault(AcceleratorTokenFaultCode faultCode, string message, ulong faultAddress = 0, bool isWrite = false, System.Nullable<AcceleratorGuardDecision> guardDecision = nullptr)
//		{
//			if (faultCode == AcceleratorTokenFaultCode.None)
//			{
//				throw new ArgumentException("L7-SDC commit faults require a non-None fault code.", nameof(faultCode));
//			}
//
//			FaultCode = faultCode;
//			Message = string.IsNullOrWhiteSpace(message) ? string.Format("L7-SDC commit fault: {0}.", faultCode) : message;
//			FaultAddress = faultAddress;
//			IsWrite = isWrite;
//			GuardDecision = guardDecision;
//		}
//
//		public AcceleratorTokenFaultCode FaultCode
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public ulong FaultAddress
//		{
//			get;
//		}
//
//		public bool IsWrite
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorGuardDecision> GuardDecision
//		{
//			get;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorRollbackRecord
//	{
//		private AcceleratorRollbackRecord(IReadOnlyList<AcceleratorStagedWrite> attemptedWrites, IReadOnlyList<AcceleratorStagedWrite> backups, int writeFailureIndex, bool rollbackAttempted, bool rollbackSucceeded, string message)
//		{
//			AttemptedWrites = attemptedWrites;
//			Backups = backups;
//			WriteFailureIndex = writeFailureIndex;
//			RollbackAttempted = rollbackAttempted;
//			RollbackSucceeded = rollbackSucceeded;
//			Message = message;
//		}
//
//		public IReadOnlyList<AcceleratorStagedWrite> AttemptedWrites
//		{
//			get;
//		}
//
//		public IReadOnlyList<AcceleratorStagedWrite> Backups
//		{
//			get;
//		}
//
//		public int WriteFailureIndex
//		{
//			get;
//		}
//
//		public bool RollbackAttempted
//		{
//			get;
//		}
//
//		public bool RollbackSucceeded
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public static AcceleratorRollbackRecord None
//		{
//			get;
//		} = new(Array.Empty<AcceleratorStagedWrite>(), Array.Empty<AcceleratorStagedWrite>(), writeFailureIndex: -1, rollbackAttempted: false, rollbackSucceeded: true, "No L7-SDC rollback was required.");
//
//		public static AcceleratorRollbackRecord SnapshotOnly(IReadOnlyList<AcceleratorStagedWrite> attemptedWrites, IReadOnlyList<AcceleratorStagedWrite> backups)
//		{
//			return new(FreezeWrites(attemptedWrites), FreezeWrites(backups), writeFailureIndex: -1, rollbackAttempted: false, rollbackSucceeded: true, "L7-SDC commit snapshot was captured; no rollback was required.");
//		}
//
//		public static AcceleratorRollbackRecord Completed(IReadOnlyList<AcceleratorStagedWrite> attemptedWrites, IReadOnlyList<AcceleratorStagedWrite> backups, int writeFailureIndex, bool rollbackSucceeded, string message)
//		{
//			return new(FreezeWrites(attemptedWrites), FreezeWrites(backups), writeFailureIndex, rollbackAttempted: true, rollbackSucceeded, message);
//		}
//
//		internal static IReadOnlyList<AcceleratorStagedWrite> FreezeWrites(IReadOnlyList<AcceleratorStagedWrite> writes)
//		{
//			if (writes == nullptr || writes.Count == 0)
//			{
//				return Array.Empty<AcceleratorStagedWrite>();
//			}
//
//			AcceleratorStagedWrite[] copy = new AcceleratorStagedWrite[writes.Count];
//			for (int index = 0; index < writes.Count; index++)
//			{
//				AcceleratorStagedWrite write = writes[index];
//				copy[index] = write with { Data = write.Data.ToArray() };
//			}
//
//			return Array.AsReadOnly(copy);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorCommitInvalidationRecord(AcceleratorCommitInvalidationTarget Target, AcceleratorMemoryRange Window, AcceleratorMemoryRange CommittedRange, bool Overlapped, bool Invalidated);

	struct AcceleratorCommitInvalidationRecord final { const AcceleratorCommitInvalidationTarget Target; const std::shared_ptr<AcceleratorMemoryRange> Window; const std::shared_ptr<AcceleratorMemoryRange> CommittedRange; const bool Overlapped; const bool Invalidated; AcceleratorCommitInvalidationRecord(AcceleratorCommitInvalidationTarget target, std::shared_ptr<AcceleratorMemoryRange> window, std::shared_ptr<AcceleratorMemoryRange> committedRange, bool overlapped, bool invalidated) : Target(target), Window(std::move(window)), CommittedRange(std::move(committedRange)), Overlapped(overlapped), Invalidated(invalidated) {} };
		class AcceleratorCommitFault final {
	public:
		const AcceleratorTokenFaultCode FaultCode; const std::wstring Message; const unsigned long long FaultAddress; const bool IsWrite; const std::shared_ptr<AcceleratorGuardDecision> GuardDecision;
		AcceleratorCommitFault(AcceleratorTokenFaultCode faultCode, const std::wstring& message, unsigned long long faultAddress=0, bool isWrite=false, std::shared_ptr<AcceleratorGuardDecision> guardDecision=nullptr) : FaultCode(faultCode), Message(message.empty()?L"L7-SDC commit fault.":message), FaultAddress(faultAddress), IsWrite(isWrite), GuardDecision(std::move(guardDecision)) { if(faultCode==AcceleratorTokenFaultCode::None) throw std::invalid_argument("L7-SDC commit faults require a non-None fault code."); }
	};
	class AcceleratorRollbackRecord final {
	public:
		const std::vector<std::shared_ptr<AcceleratorStagedWrite>> AttemptedWrites, Backups; const int WriteFailureIndex; const bool RollbackAttempted, RollbackSucceeded; const std::wstring Message;
		AcceleratorRollbackRecord(std::vector<std::shared_ptr<AcceleratorStagedWrite>> attempted, std::vector<std::shared_ptr<AcceleratorStagedWrite>> backups, int index, bool attemptedRollback, bool succeeded, const std::wstring& message) : AttemptedWrites(std::move(attempted)), Backups(std::move(backups)), WriteFailureIndex(index), RollbackAttempted(attemptedRollback), RollbackSucceeded(succeeded), Message(message) {}
		static const std::shared_ptr<AcceleratorRollbackRecord> None;
		static std::shared_ptr<AcceleratorRollbackRecord> SnapshotOnly(const std::vector<std::shared_ptr<AcceleratorStagedWrite>>& attempted, const std::vector<std::shared_ptr<AcceleratorStagedWrite>>& backups) { return std::make_shared<AcceleratorRollbackRecord>(attempted,backups,-1,false,false,L"Rollback snapshot captured; no write rollback was required."); }
		static std::shared_ptr<AcceleratorRollbackRecord> Completed(const std::vector<std::shared_ptr<AcceleratorStagedWrite>>& attempted, const std::vector<std::shared_ptr<AcceleratorStagedWrite>>& backups, int index, bool succeeded, const std::wstring& message) { return std::make_shared<AcceleratorRollbackRecord>(attempted,backups,index,true,succeeded,message); }
	};
	inline const std::shared_ptr<AcceleratorRollbackRecord> AcceleratorRollbackRecord::None = std::make_shared<AcceleratorRollbackRecord>(std::vector<std::shared_ptr<AcceleratorStagedWrite>>{},std::vector<std::shared_ptr<AcceleratorStagedWrite>>{},-1,false,false,L"No rollback was required.");
	class AcceleratorCommitInvalidationPlan final : public std::enable_shared_from_this<AcceleratorCommitInvalidationPlan>
	{
	private:
		const std::shared_ptr<YAKSys_Hybrid_CPU::Memory::StreamRegisterFile> StreamRegisterFile;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<AcceleratorMemoryRange> SrfWindows;
		const std::vector<std::shared_ptr<AcceleratorMemoryRange>> SrfWindows;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<AcceleratorMemoryRange> CacheWindows;
		const std::vector<std::shared_ptr<AcceleratorMemoryRange>> CacheWindows;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<AcceleratorCommitInvalidationRecord> Records;
		const std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> Records;
		

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private AcceleratorCommitInvalidationPlan(StreamRegisterFile? streamRegisterFile, IReadOnlyList<AcceleratorMemoryRange> srfWindows, IReadOnlyList<AcceleratorMemoryRange> cacheWindows, IReadOnlyList<AcceleratorCommitInvalidationRecord> records)
		AcceleratorCommitInvalidationPlan(std::shared_ptr<YAKSys_Hybrid_CPU::Memory::StreamRegisterFile> streamRegisterFile, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &srfWindows, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &cacheWindows, const std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> &records);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public StreamRegisterFile? StreamRegisterFile {get;}
	public:
		std::shared_ptr<YAKSys_Hybrid_CPU::Memory::StreamRegisterFile> getStreamRegisterFile() const;

		const std::vector<std::shared_ptr<AcceleratorMemoryRange>> getSrfWindows() const;

		const std::vector<std::shared_ptr<AcceleratorMemoryRange>> getCacheWindows() const;

		const std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> getRecords() const;

		int getSrfInvalidationCount() const;

		int getCacheInvalidationCount() const;

		bool getHasInvalidationEvidence() const;

		static std::shared_ptr<AcceleratorCommitInvalidationPlan> getNone();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static AcceleratorCommitInvalidationPlan Observe(IReadOnlyList<AcceleratorMemoryRange>? srfWindows = null, IReadOnlyList<AcceleratorMemoryRange>? cacheWindows = null, System.Nullable<StreamRegisterFile> streamRegisterFile = null)
		static std::shared_ptr<AcceleratorCommitInvalidationPlan> Observe(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &srfWindows = {}, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &cacheWindows = {}, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::StreamRegisterFile> streamRegisterFile = nullptr);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public AcceleratorCommitInvalidationPlan WithRecords(IReadOnlyList<AcceleratorCommitInvalidationRecord> records)
		std::shared_ptr<AcceleratorCommitInvalidationPlan> WithRecords(const std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> &records);

	private:
		int CountInvalidated(AcceleratorCommitInvalidationTarget target) const;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static IReadOnlyList<AcceleratorMemoryRange> FreezeRanges(IReadOnlyList<AcceleratorMemoryRange> ranges)
		static const std::vector<std::shared_ptr<AcceleratorMemoryRange>> FreezeRanges(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static IReadOnlyList<AcceleratorCommitInvalidationRecord> FreezeRecords(IReadOnlyList<AcceleratorCommitInvalidationRecord> records)
		static const std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> FreezeRecords(const std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> &records);
	};

	class AcceleratorStagedWriteSet final : public std::enable_shared_from_this<AcceleratorStagedWriteSet>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<AcceleratorStagedWrite> Writes;
		const std::vector<std::shared_ptr<AcceleratorStagedWrite>> Writes;

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public AcceleratorStagedWriteSet(IReadOnlyList<AcceleratorStagedWrite> writes)
		AcceleratorStagedWriteSet(const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &writes);

		const std::vector<std::shared_ptr<AcceleratorStagedWrite>> getWrites() const;

		int getCount() const;

		unsigned long long getBytesStaged() const;

		static std::shared_ptr<AcceleratorStagedWriteSet> FromStagingReadResult(std::shared_ptr<AcceleratorStagingReadResult> readResult);

		const std::vector<std::shared_ptr<AcceleratorMemoryRange>> NormalizeDestinationFootprint();

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public bool CoversExactly(IReadOnlyList<AcceleratorMemoryRange> normalizedDestinationFootprint)
		bool CoversExactly(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalizedDestinationFootprint);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public bool CoversExactly(IReadOnlyList<AcceleratorMemoryRange> normalizedDestinationFootprint, out AcceleratorMemoryRange faultRange, out string message)
		bool CoversExactly(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalizedDestinationFootprint, std::shared_ptr<AcceleratorMemoryRange> &faultRange, std::wstring &message);

		bool IsTokenBound(std::shared_ptr<AcceleratorTokenHandle> tokenHandle, std::wstring &message);

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool TryNormalizeStagedWriteRanges(IReadOnlyList<AcceleratorStagedWrite> writes, out IReadOnlyList<AcceleratorMemoryRange> normalized, out AcceleratorMemoryRange faultRange, out string message)
		static bool TryNormalizeStagedWriteRanges(const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &writes, std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalized, std::shared_ptr<AcceleratorMemoryRange> &faultRange, std::wstring &message);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool TryNormalizeExpectedRanges(IReadOnlyList<AcceleratorMemoryRange> ranges, out IReadOnlyList<AcceleratorMemoryRange> normalized, out AcceleratorMemoryRange faultRange, out string message)
		static bool TryNormalizeExpectedRanges(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges, std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalized, std::shared_ptr<AcceleratorMemoryRange> &faultRange, std::wstring &message);
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorCommitResult
//	{
//		private AcceleratorCommitResult(bool succeeded, AcceleratorTokenState tokenState, System.Nullable<AcceleratorCommitFault> fault, AcceleratorRollbackRecord rollback, AcceleratorCommitInvalidationPlan invalidationPlan, ulong bytesCommitted, string message)
//		{
//			Succeeded = succeeded;
//			TokenState = tokenState;
//			Fault = fault;
//			Rollback = rollback;
//			InvalidationPlan = invalidationPlan;
//			BytesCommitted = bytesCommitted;
//			Message = message;
//		}
//
//		public bool Succeeded
//		{
//			get;
//		}
//
//		public bool IsRejected
//		{
//			get
//			{
//				return !Succeeded;
//			}
//		}
//
//		public AcceleratorTokenState TokenState
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorCommitFault> Fault
//		{
//			get;
//		}
//
//		public AcceleratorTokenFaultCode FaultCode
//		{
//			get
//			{
//				return (Fault == nullptr ? nullptr : Fault.FaultCode) ?? AcceleratorTokenFaultCode.None;
//			}
//		}
//
//		public AcceleratorRollbackRecord Rollback
//		{
//			get;
//		}
//
//		public AcceleratorCommitInvalidationPlan InvalidationPlan
//		{
//			get;
//		}
//
//		public ulong BytesCommitted
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public bool CanPublishArchitecturalMemory
//		{
//			get
//			{
//				return Succeeded;
//			}
//		}
//
//		public bool UserVisiblePublicationAllowed
//		{
//			get
//			{
//				return Succeeded;
//			}
//		}
//
//		public bool RequiresRetireExceptionPublication
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public static AcceleratorCommitResult Success(AcceleratorToken token, ulong bytesCommitted, AcceleratorCommitInvalidationPlan invalidationPlan)
//		{
//			return new(succeeded: true, token.State, fault: nullptr, AcceleratorRollbackRecord.None, invalidationPlan, bytesCommitted, "L7-SDC Phase 08 commit published staged writes architecturally through the guarded commit coordinator.");
//		}
//
//		public static AcceleratorCommitResult Rejected(AcceleratorToken token, AcceleratorCommitFault fault, System.Nullable<AcceleratorRollbackRecord> rollback = nullptr, System.Nullable<AcceleratorCommitInvalidationPlan> invalidationPlan = nullptr)
//		{
//			return new(succeeded: false, token.State, fault, rollback ?? AcceleratorRollbackRecord.None, invalidationPlan ?? AcceleratorCommitInvalidationPlan.None, bytesCommitted: 0, fault.Message);
//		}
//	}

		class AcceleratorCommitResult final {
	public:
		const bool Succeeded; const AcceleratorTokenState TokenState; const std::shared_ptr<AcceleratorCommitFault> Fault; const std::shared_ptr<AcceleratorRollbackRecord> Rollback; const std::shared_ptr<AcceleratorCommitInvalidationPlan> InvalidationPlan; const unsigned long long BytesCommitted; const std::wstring Message;
		const bool IsRejected, CanPublishArchitecturalMemory, UserVisiblePublicationAllowed; const bool RequiresRetireExceptionPublication=false;
	private:
		AcceleratorCommitResult(bool succeeded, AcceleratorTokenState state, std::shared_ptr<AcceleratorCommitFault> fault, std::shared_ptr<AcceleratorRollbackRecord> rollback, std::shared_ptr<AcceleratorCommitInvalidationPlan> invalidation, unsigned long long bytes, const std::wstring& message) : Succeeded(succeeded), TokenState(state), Fault(std::move(fault)), Rollback(std::move(rollback)), InvalidationPlan(std::move(invalidation)), BytesCommitted(bytes), Message(message), IsRejected(!succeeded), CanPublishArchitecturalMemory(succeeded), UserVisiblePublicationAllowed(succeeded) {}
	public:
		AcceleratorTokenFaultCode getFaultCode() const { return Fault ? Fault->FaultCode : AcceleratorTokenFaultCode::None; }
		static std::shared_ptr<AcceleratorCommitResult> Success(std::shared_ptr<AcceleratorToken> token, unsigned long long bytes, std::shared_ptr<AcceleratorCommitInvalidationPlan> plan) { if(!token) throw std::invalid_argument("token"); return std::shared_ptr<AcceleratorCommitResult>(new AcceleratorCommitResult(true,token->getState(),nullptr,AcceleratorRollbackRecord::None,plan,bytes,L"L7-SDC Phase 08 commit published staged writes architecturally through the guarded commit coordinator.")); }
		static std::shared_ptr<AcceleratorCommitResult> Rejected(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommitFault> fault, std::shared_ptr<AcceleratorRollbackRecord> rollback=nullptr, std::shared_ptr<AcceleratorCommitInvalidationPlan> plan=nullptr) { if(!token||!fault) throw std::invalid_argument("token and fault are required"); if(!rollback) rollback=AcceleratorRollbackRecord::None; if(!plan) plan=AcceleratorCommitInvalidationPlan::getNone(); return std::shared_ptr<AcceleratorCommitResult>(new AcceleratorCommitResult(false,token->getState(),fault,rollback,plan,0,fault->Message)); }
	};
	class AcceleratorCommitCoordinator final : public std::enable_shared_from_this<AcceleratorCommitCoordinator>
	{
	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly AcceleratorTelemetry? _telemetry;
		const std::shared_ptr<AcceleratorTelemetry> _telemetry;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorCommitCoordinator(AcceleratorTelemetry? telemetry = null)
		AcceleratorCommitCoordinator(std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorCommitResult TryCommit(AcceleratorToken token, AcceleratorCommandDescriptor descriptor, IAcceleratorStagingBuffer stagingBuffer, Processor.MainMemoryArea mainMemory, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence, AcceleratorCommitInvalidationPlan invalidationPlan, bool commitConflictPlaceholderAccepted, bool directWriteViolationDetected = false, ExternalAcceleratorConflictManager? conflictManager = null, System.Nullable<MemoryCoherencyObserver> coherencyObserver = null, System.Nullable<ExternalOperationPublicationEvidence> externalPublicationEvidence = null, bool externalPublicationEvidenceRequired = false)
		std::shared_ptr<AcceleratorCommitResult> TryCommit(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<AcceleratorCommitInvalidationPlan> invalidationPlan, bool commitConflictPlaceholderAccepted, bool directWriteViolationDetected = false, std::shared_ptr<ExternalAcceleratorConflictManager> conflictManager = nullptr, std::shared_ptr<MemoryCoherencyObserver> coherencyObserver = nullptr, std::optional<std::shared_ptr<ExternalOperationPublicationEvidence>> externalPublicationEvidence = std::nullopt, bool externalPublicationEvidenceRequired = false);

	private:
		std::shared_ptr<AcceleratorCommitResult> RecordCommitResult(std::shared_ptr<AcceleratorCommitResult> result, std::shared_ptr<AcceleratorToken> token);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static void NotifyCommittedWrites(MemoryCoherencyObserver? coherencyObserver, AcceleratorCommandDescriptor descriptor, AcceleratorStagedWriteSet stagedWriteSet)
		static void NotifyCommittedWrites(std::shared_ptr<MemoryCoherencyObserver> coherencyObserver, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<AcceleratorStagedWriteSet> stagedWriteSet);

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool ValidateCommitPreconditions(AcceleratorToken token, AcceleratorCommandDescriptor descriptor, IAcceleratorStagingBuffer stagingBuffer, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence, bool commitConflictPlaceholderAccepted, bool directWriteViolationDetected, ExternalAcceleratorConflictManager? conflictManager, out System.Nullable<AcceleratorStagedWriteSet> stagedWriteSet, out System.Nullable<AcceleratorCommitResult> rejection)
		bool ValidateCommitPreconditions(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, bool commitConflictPlaceholderAccepted, bool directWriteViolationDetected, std::shared_ptr<ExternalAcceleratorConflictManager> conflictManager, std::shared_ptr<AcceleratorStagedWriteSet> &stagedWriteSet, std::optional<std::shared_ptr<AcceleratorCommitResult>> &rejection);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public bool ValidateExactCoverage(AcceleratorStagedWriteSet stagedWriteSet, IReadOnlyList<AcceleratorMemoryRange> normalizedDestinationFootprint, out AcceleratorMemoryRange faultRange, out string message)
		bool ValidateExactCoverage(std::shared_ptr<AcceleratorStagedWriteSet> stagedWriteSet, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalizedDestinationFootprint, std::shared_ptr<AcceleratorMemoryRange> &faultRange, std::wstring &message);

		std::shared_ptr<AcceleratorGuardDecision> ValidateOwnerDomainAndMapping(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		bool ApplyAllOrNone(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<AcceleratorStagedWriteSet> stagedWriteSet, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<AcceleratorRollbackRecord> &rollbackRecord, std::optional<std::shared_ptr<AcceleratorCommitFault>> &fault, unsigned long long &bytesCommitted);

	private:
		bool ApplyAllOrNoneAfterValidation(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<AcceleratorStagedWriteSet> stagedWriteSet, std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence, std::shared_ptr<AcceleratorRollbackRecord> &rollbackRecord, std::optional<std::shared_ptr<AcceleratorCommitFault>> &fault, unsigned long long &bytesCommitted);

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public AcceleratorRollbackRecord Rollback(AcceleratorToken token, AcceleratorCommandDescriptor descriptor, Processor.MainMemoryArea mainMemory, IReadOnlyList<AcceleratorStagedWrite> backups, IReadOnlyList<AcceleratorStagedWrite> attemptedWrites, int writeFailureIndex, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence)
		std::shared_ptr<AcceleratorRollbackRecord> Rollback(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<Processor::MainMemoryArea> mainMemory, const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &backups, const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &attemptedWrites, int writeFailureIndex, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static AcceleratorRollbackRecord RollbackAfterValidation(Processor.MainMemoryArea mainMemory, IReadOnlyList<AcceleratorStagedWrite> backups, IReadOnlyList<AcceleratorStagedWrite> attemptedWrites, int writeFailureIndex)
		static std::shared_ptr<AcceleratorRollbackRecord> RollbackAfterValidation(std::shared_ptr<Processor::MainMemoryArea> mainMemory, const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &backups, const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &attemptedWrites, int writeFailureIndex);

	public:
		bool InvalidateSrfAndCache(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<AcceleratorStagedWriteSet> stagedWriteSet, std::shared_ptr<AcceleratorCommitInvalidationPlan> invalidationPlan, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<AcceleratorCommitInvalidationPlan> &invalidated, std::optional<std::shared_ptr<AcceleratorCommitFault>> &fault);

	private:
		static std::shared_ptr<AcceleratorCommitInvalidationPlan> InvalidateSrfAndCacheAfterValidation(std::shared_ptr<AcceleratorStagedWriteSet> stagedWriteSet, std::shared_ptr<AcceleratorCommitInvalidationPlan> invalidationPlan);

		static bool ValidateDescriptorAndFootprintBinding(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<AcceleratorGuardDecision> guardDecision, std::optional<std::shared_ptr<AcceleratorCommitFault>> &fault);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private bool ValidateRollbackWriteEvidence(AcceleratorToken token, IReadOnlyList<AcceleratorStagedWrite> writes, AcceleratorGuardDecision guardDecision, string evidenceName, out string message)
		bool ValidateRollbackWriteEvidence(std::shared_ptr<AcceleratorToken> token, const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &writes, std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring &evidenceName, std::wstring &message);

		static bool IsRollbackFailureIndexValid(int writeFailureIndex, int backupCount);

		static std::shared_ptr<AcceleratorCommitResult> FaultToken(std::shared_ptr<AcceleratorToken> token, AcceleratorTokenFaultCode faultCode, const std::wstring &message, std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence, std::shared_ptr<AcceleratorGuardDecision> guardDecision, unsigned long long faultAddress, bool isWrite);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static void ReleaseTerminalReservationIfNeeded(ExternalAcceleratorConflictManager? conflictManager, AcceleratorToken token, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence)
		static void ReleaseTerminalReservationIfNeeded(std::shared_ptr<ExternalAcceleratorConflictManager> conflictManager, std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		static bool IsDescriptorIdentityBound(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor);

		static bool IsNormalizedFootprintBound(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void RecordInvalidations(AcceleratorCommitInvalidationTarget target, IReadOnlyList<AcceleratorMemoryRange> windows, AcceleratorMemoryRange committedRange, List<AcceleratorCommitInvalidationRecord> records)
		static void RecordInvalidations(AcceleratorCommitInvalidationTarget target, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &windows, std::shared_ptr<AcceleratorMemoryRange> committedRange, std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> &records);

		static bool RangesOverlap(std::shared_ptr<AcceleratorMemoryRange> left, std::shared_ptr<AcceleratorMemoryRange> right);

		static bool HasExactMainMemoryRange(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, int size);
	};

}






