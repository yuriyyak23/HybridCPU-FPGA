#pragma once

#include "../Descriptors/AcceleratorCommandDescriptor.h"
#include "../Tokens/AcceleratorTokenStatusWord.h"
#include "../Tokens/AcceleratorToken.h"
#include "../Conflicts/ExternalAcceleratorConflictManager.h"
#include "../Auth/AcceleratorOwnerDomainGuard.h"
#include <string>
#include <vector>
#include <optional>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Conflicts;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry
{
	enum class AcceleratorEvidenceKind : unsigned char
	{
		CapabilityQuery = 0,
		DescriptorParse = 1,
		SubmitAdmission = 2,
		TokenTransition = 3,
		DeviceBusyReject = 4,
		QueueFullReject = 5,
		DomainReject = 6,
		OwnerDriftReject = 7,
		MappingEpochDriftReject = 8,
		IommuDomainEpochDriftReject = 9,
		FootprintConflictReject = 10,
		DirectWriteViolationReject = 11,
		CommitRollback = 12,
		BytesRead = 13,
		BytesStaged = 14,
		BytesCommitted = 15,
		Operation = 16,
		SrfInvalidation = 17,
		CacheInvalidation = 18,
		DmaStreamComputeConflictReject = 19,
		Lane7SubmitPollThrottleReject = 20,
		BackendReject = 21,
		CommitReject = 22
	};

	// Public snapshot DTOs corresponding to the C# telemetry records. Keep field
	// names and types aligned: profile JSON consumers use these names directly.
	struct AcceleratorEvidenceRecord final
	{
		unsigned long long Sequence{};
		AcceleratorEvidenceKind Kind{};
		bool Accepted{};
		AcceleratorDescriptorFault DescriptorFault{};
		AcceleratorTokenFaultCode TokenFaultCode{};
		AcceleratorGuardFault GuardFault{};
		AcceleratorConflictClass ConflictClass{};
		std::optional<AcceleratorTokenState> FromState;
		std::optional<AcceleratorTokenState> ToState;
		unsigned long long TokenId{};
		unsigned long long TokenHandle{};
		unsigned long long ByteCount{};
		unsigned long long OperationCount{};
		unsigned long long LatencyCycles{};
		std::optional<std::wstring> Message;
	};

	struct AcceleratorRejectCounters final
	{
		long long CapabilityQueryRejects{};
		long long DescriptorParseRejects{};
		long long SubmitRejects{};
		long long BackendRejects{};
		long long CommitRejects{};
		long long DeviceBusyRejects{};
		long long QueueFullRejects{};
		long long DomainRejects{};
		long long OwnerDriftRejects{};
		long long MappingEpochDriftRejects{};
		long long IommuDomainEpochDriftRejects{};
		long long FootprintConflictRejects{};
		long long DirectWriteViolationRejects{};
		long long Lane7SubmitPollThrottleRejects{};
	};

	struct AcceleratorLifecycleCounters final
	{
		long long Created{};
		long long Validated{};
		long long Queued{};
		long long Running{};
		long long DeviceCompleted{};
		long long CommitPending{};
		long long Committed{};
		long long Faulted{};
		long long Canceled{};
		long long TimedOut{};
		long long Abandoned{};
	};

	struct AcceleratorByteCounters final
	{
		unsigned long long BytesRead{};
		unsigned long long BytesStaged{};
		unsigned long long BytesCommitted{};
	};

	struct AcceleratorConflictCounters final
	{
		long long FootprintConflictRejects{};
		long long DmaStreamComputeConflictRejects{};
		long long SrfAssistConflictRejects{};
		long long DirectWriteViolationRejects{};
		long long CommitRollbackCount{};
		long long SrfInvalidations{};
		long long CacheInvalidations{};
	};

	struct AcceleratorTelemetrySnapshot final
	{
		long long CapabilityQueryAttempts{};
		long long CapabilityQuerySuccess{};
		long long CapabilityQueryReject{};
		long long DescriptorParseAttempts{};
		long long DescriptorAccepted{};
		long long DescriptorRejected{};
		long long SubmitAttempts{};
		long long SubmitAccepted{};
		long long SubmitRejected{};
		AcceleratorRejectCounters Rejects{};
		AcceleratorLifecycleCounters Lifecycle{};
		AcceleratorByteCounters Bytes{};
		AcceleratorConflictCounters Conflicts{};
		unsigned long long OperationCount{};
		unsigned long long LatencyCycles{};
		std::vector<std::shared_ptr<AcceleratorEvidenceRecord>> EvidenceRecords;
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorEvidenceRecord
//	{
//		public required ulong Sequence
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorEvidenceKind Kind
//		{
//			get;
//			set;
//		}
//
//		public bool Accepted
//		{
//			get;
//			set;
//		}
//
//		public AcceleratorDescriptorFault DescriptorFault
//		{
//			get;
//			set;
//		}
//
//		public AcceleratorTokenFaultCode TokenFaultCode
//		{
//			get;
//			set;
//		}
//
//		public AcceleratorGuardFault GuardFault
//		{
//			get;
//			set;
//		}
//
//		public AcceleratorConflictClass ConflictClass
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<AcceleratorTokenState> FromState
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<AcceleratorTokenState> ToState
//		{
//			get;
//			set;
//		}
//
//		public ulong TokenId
//		{
//			get;
//			set;
//		}
//
//		public ulong TokenHandle
//		{
//			get;
//			set;
//		}
//
//		public ulong ByteCount
//		{
//			get;
//			set;
//		}
//
//		public ulong OperationCount
//		{
//			get;
//			set;
//		}
//
//		public ulong LatencyCycles
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<string> Message
//		{
//			get;
//			set;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorRejectCounters
//	{
//		public long CapabilityQueryRejects
//		{
//			get;
//			set;
//		}
//
//		public long DescriptorParseRejects
//		{
//			get;
//			set;
//		}
//
//		public long SubmitRejects
//		{
//			get;
//			set;
//		}
//
//		public long BackendRejects
//		{
//			get;
//			set;
//		}
//
//		public long CommitRejects
//		{
//			get;
//			set;
//		}
//
//		public long DeviceBusyRejects
//		{
//			get;
//			set;
//		}
//
//		public long QueueFullRejects
//		{
//			get;
//			set;
//		}
//
//		public long DomainRejects
//		{
//			get;
//			set;
//		}
//
//		public long OwnerDriftRejects
//		{
//			get;
//			set;
//		}
//
//		public long MappingEpochDriftRejects
//		{
//			get;
//			set;
//		}
//
//		public long IommuDomainEpochDriftRejects
//		{
//			get;
//			set;
//		}
//
//		public long FootprintConflictRejects
//		{
//			get;
//			set;
//		}
//
//		public long DirectWriteViolationRejects
//		{
//			get;
//			set;
//		}
//
//		public long Lane7SubmitPollThrottleRejects
//		{
//			get;
//			set;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorLifecycleCounters
//	{
//		public long Created
//		{
//			get;
//			set;
//		}
//
//		public long Validated
//		{
//			get;
//			set;
//		}
//
//		public long Queued
//		{
//			get;
//			set;
//		}
//
//		public long Running
//		{
//			get;
//			set;
//		}
//
//		public long DeviceCompleted
//		{
//			get;
//			set;
//		}
//
//		public long CommitPending
//		{
//			get;
//			set;
//		}
//
//		public long Committed
//		{
//			get;
//			set;
//		}
//
//		public long Faulted
//		{
//			get;
//			set;
//		}
//
//		public long Canceled
//		{
//			get;
//			set;
//		}
//
//		public long TimedOut
//		{
//			get;
//			set;
//		}
//
//		public long Abandoned
//		{
//			get;
//			set;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorByteCounters
//	{
//		public ulong BytesRead
//		{
//			get;
//			set;
//		}
//
//		public ulong BytesStaged
//		{
//			get;
//			set;
//		}
//
//		public ulong BytesCommitted
//		{
//			get;
//			set;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorConflictCounters
//	{
//		public long FootprintConflictRejects
//		{
//			get;
//			set;
//		}
//
//		public long DmaStreamComputeConflictRejects
//		{
//			get;
//			set;
//		}
//
//		public long SrfAssistConflictRejects
//		{
//			get;
//			set;
//		}
//
//		public long DirectWriteViolationRejects
//		{
//			get;
//			set;
//		}
//
//		public long CommitRollbackCount
//		{
//			get;
//			set;
//		}
//
//		public long SrfInvalidations
//		{
//			get;
//			set;
//		}
//
//		public long CacheInvalidations
//		{
//			get;
//			set;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorTelemetrySnapshot
//	{
//		public long CapabilityQueryAttempts
//		{
//			get;
//			set;
//		}
//
//		public long CapabilityQuerySuccess
//		{
//			get;
//			set;
//		}
//
//		public long CapabilityQueryReject
//		{
//			get;
//			set;
//		}
//
//		public long DescriptorParseAttempts
//		{
//			get;
//			set;
//		}
//
//		public long DescriptorAccepted
//		{
//			get;
//			set;
//		}
//
//		public long DescriptorRejected
//		{
//			get;
//			set;
//		}
//
//		public long SubmitAttempts
//		{
//			get;
//			set;
//		}
//
//		public long SubmitAccepted
//		{
//			get;
//			set;
//		}
//
//		public long SubmitRejected
//		{
//			get;
//			set;
//		}
//
//		public AcceleratorRejectCounters Rejects
//		{
//			get;
//			set;
//		} = new();
//
//		public AcceleratorLifecycleCounters Lifecycle
//		{
//			get;
//			set;
//		} = new();
//
//		public AcceleratorByteCounters Bytes
//		{
//			get;
//			set;
//		} = new();
//
//		public AcceleratorConflictCounters Conflicts
//		{
//			get;
//			set;
//		} = new();
//
//		public ulong OperationCount
//		{
//			get;
//			set;
//		}
//
//		public ulong LatencyCycles
//		{
//			get;
//			set;
//		}
//
//		public IReadOnlyList<AcceleratorEvidenceRecord> EvidenceRecords
//		{
//			get;
//			set;
//		} = Array.Empty<AcceleratorEvidenceRecord>();
//	}

	class AcceleratorTelemetry final : public std::enable_shared_from_this<AcceleratorTelemetry>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<AcceleratorEvidenceRecord> _evidenceRecords = new();
		std::vector<std::shared_ptr<AcceleratorEvidenceRecord>> _evidenceRecords = std::vector<std::shared_ptr<AcceleratorEvidenceRecord>>();
		unsigned long long _nextSequence = 1;

		long long _capabilityQueryAttempts = 0;
		long long _capabilityQuerySuccess = 0;
		long long _capabilityQueryReject = 0;
		long long _descriptorParseAttempts = 0;
		long long _descriptorAccepted = 0;
		long long _descriptorRejected = 0;
		long long _submitAttempts = 0;
		long long _submitAccepted = 0;
		long long _submitRejected = 0;
		long long _backendRejects = 0;
		long long _commitRejects = 0;
		long long _deviceBusyRejects = 0;
		long long _queueFullRejects = 0;
		long long _domainRejects = 0;
		long long _ownerDriftRejects = 0;
		long long _mappingEpochDriftRejects = 0;
		long long _iommuDomainEpochDriftRejects = 0;
		long long _footprintConflictRejects = 0;
		long long _directWriteViolationRejects = 0;
		long long _lane7SubmitPollThrottleRejects = 0;
		long long _created = 0;
		long long _validated = 0;
		long long _queued = 0;
		long long _running = 0;
		long long _deviceCompleted = 0;
		long long _commitPending = 0;
		long long _committed = 0;
		long long _faulted = 0;
		long long _canceled = 0;
		long long _timedOut = 0;
		long long _abandoned = 0;
		unsigned long long _bytesRead = 0;
		unsigned long long _bytesStaged = 0;
		unsigned long long _bytesCommitted = 0;
		long long _dmaStreamComputeConflictRejects = 0;
		long long _srfAssistConflictRejects = 0;
		long long _commitRollbackCount = 0;
		long long _srfInvalidations = 0;
		long long _cacheInvalidations = 0;
		unsigned long long _operationCount = 0;
		unsigned long long _latencyCycles = 0;

	public:
		std::shared_ptr<AcceleratorTelemetrySnapshot> Snapshot();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordCapabilityQuery(bool accepted, string? message = null)
		void RecordCapabilityQuery(bool accepted, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordDescriptorParse(bool accepted, AcceleratorDescriptorFault fault = AcceleratorDescriptorFault.None, string? message = null)
		void RecordDescriptorParse(bool accepted, AcceleratorDescriptorFault fault = AcceleratorDescriptorFault::None, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordSubmit(bool accepted, AcceleratorTokenFaultCode faultCode = AcceleratorTokenFaultCode.None, System.Nullable<AcceleratorGuardDecision> guardDecision = null, string? message = null)
		void RecordSubmit(bool accepted, AcceleratorTokenFaultCode faultCode = AcceleratorTokenFaultCode::None, std::optional<std::shared_ptr<AcceleratorGuardDecision>> guardDecision = std::nullopt, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordTokenTransition(AcceleratorToken token, AcceleratorTokenState fromState, AcceleratorTokenState toState, AcceleratorTokenFaultCode faultCode = AcceleratorTokenFaultCode.None, string? message = null)
		void RecordTokenTransition(std::shared_ptr<AcceleratorToken> token, AcceleratorTokenState fromState, AcceleratorTokenState toState, AcceleratorTokenFaultCode faultCode = AcceleratorTokenFaultCode::None, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordDeviceBusyReject(AcceleratorTokenFaultCode faultCode, string? message = null)
		void RecordDeviceBusyReject(AcceleratorTokenFaultCode faultCode, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordQueueFullReject(AcceleratorTokenFaultCode faultCode, string? message = null)
		void RecordQueueFullReject(AcceleratorTokenFaultCode faultCode, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordDomainReject(AcceleratorGuardDecision guardDecision, string? message = null)
		void RecordDomainReject(std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordOwnerDriftReject(AcceleratorGuardDecision guardDecision, string? message = null)
		void RecordOwnerDriftReject(std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordMappingEpochDriftReject(AcceleratorGuardDecision guardDecision, string? message = null)
		void RecordMappingEpochDriftReject(std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordIommuDomainEpochDriftReject(AcceleratorGuardDecision guardDecision, string? message = null)
		void RecordIommuDomainEpochDriftReject(std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordGuardReject(System.Nullable<AcceleratorGuardDecision> guardDecision, string? message = null)
		void RecordGuardReject(const std::optional<std::shared_ptr<AcceleratorGuardDecision>> &guardDecision, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordFootprintConflictReject(AcceleratorConflictClass conflictClass, AcceleratorTokenFaultCode faultCode, string? message = null)
		void RecordFootprintConflictReject(AcceleratorConflictClass conflictClass, AcceleratorTokenFaultCode faultCode, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordDirectWriteViolationReject(AcceleratorToken? token = null, System.Nullable<string> message = null)
		void RecordDirectWriteViolationReject(std::shared_ptr<AcceleratorToken> token = nullptr, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordCommitRollback(bool attempted, bool succeeded, string? message = null)
		void RecordCommitRollback(bool attempted, bool succeeded, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordBytesRead(ulong byteCount, string? message = null)
		void RecordBytesRead(unsigned long long byteCount, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordBytesStaged(ulong byteCount, string? message = null)
		void RecordBytesStaged(unsigned long long byteCount, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordBytesCommitted(ulong byteCount, string? message = null)
		void RecordBytesCommitted(unsigned long long byteCount, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordOperation(ulong operationCount, ulong latencyCycles = 0, string? message = null)
		void RecordOperation(unsigned long long operationCount, unsigned long long latencyCycles = 0, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordSrfInvalidations(int count, string? message = null)
		void RecordSrfInvalidations(int count, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordCacheInvalidations(int count, string? message = null)
		void RecordCacheInvalidations(int count, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordLane7SubmitPollThrottleReject(string? message = null)
		void RecordLane7SubmitPollThrottleReject(const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordBackendReject(AcceleratorTokenFaultCode faultCode, AcceleratorToken? token = null, System.Nullable<string> message = null)
		void RecordBackendReject(AcceleratorTokenFaultCode faultCode, std::shared_ptr<AcceleratorToken> token = nullptr, const std::wstring &message = L"");

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordCommitReject(AcceleratorTokenFaultCode faultCode, AcceleratorToken? token = null, System.Nullable<string> message = null)
		void RecordCommitReject(AcceleratorTokenFaultCode faultCode, std::shared_ptr<AcceleratorToken> token = nullptr, const std::wstring &message = L"");

	private:
		void RecordRejectFault(AcceleratorTokenFaultCode faultCode, const std::optional<std::shared_ptr<AcceleratorGuardDecision>> &guardDecision);

		void IncrementLifecycle(AcceleratorTokenState state);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private void AddEvidence(AcceleratorEvidenceKind kind, bool accepted, AcceleratorDescriptorFault descriptorFault = AcceleratorDescriptorFault.None, AcceleratorTokenFaultCode tokenFaultCode = AcceleratorTokenFaultCode.None, AcceleratorGuardFault guardFault = AcceleratorGuardFault.None, AcceleratorConflictClass conflictClass = AcceleratorConflictClass.None, System.Nullable<AcceleratorTokenState> fromState = null, System.Nullable<AcceleratorTokenState> toState = null, AcceleratorToken? token = null, ulong byteCount = 0, ulong operationCount = 0, ulong latencyCycles = 0, System.Nullable<string> message = null)
		void AddEvidence(AcceleratorEvidenceKind kind, bool accepted, AcceleratorDescriptorFault descriptorFault = AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode tokenFaultCode = AcceleratorTokenFaultCode::None, AcceleratorGuardFault guardFault = AcceleratorGuardFault::None, AcceleratorConflictClass conflictClass = AcceleratorConflictClass::None, std::optional<AcceleratorTokenState> fromState = std::nullopt, std::optional<AcceleratorTokenState> toState = std::nullopt, std::shared_ptr<AcceleratorToken> token = nullptr, unsigned long long byteCount = 0, unsigned long long operationCount = 0, unsigned long long latencyCycles = 0, const std::wstring &message = L"");

		unsigned long long AllocateSequence();
	};

}
