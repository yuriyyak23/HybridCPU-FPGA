#include "AcceleratorTelemetry.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Conflicts;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry
{

	std::shared_ptr<AcceleratorTelemetrySnapshot> AcceleratorTelemetry::Snapshot()
	{
		std::shared_ptr<AcceleratorTelemetrySnapshot> temp_var = std::make_shared<AcceleratorTelemetrySnapshot>();
		temp_var->CapabilityQueryAttempts = _capabilityQueryAttempts;
		temp_var->CapabilityQuerySuccess = _capabilityQuerySuccess;
		temp_var->CapabilityQueryReject = _capabilityQueryReject;
		temp_var->DescriptorParseAttempts = _descriptorParseAttempts;
		temp_var->DescriptorAccepted = _descriptorAccepted;
		temp_var->DescriptorRejected = _descriptorRejected;
		temp_var->SubmitAttempts = _submitAttempts;
		temp_var->SubmitAccepted = _submitAccepted;
		temp_var->SubmitRejected = _submitRejected;
		temp_var->Rejects = {.CapabilityQueryRejects = _capabilityQueryReject, .DescriptorParseRejects = _descriptorRejected, .SubmitRejects = _submitRejected, .BackendRejects = _backendRejects, .CommitRejects = _commitRejects, .DeviceBusyRejects = _deviceBusyRejects, .QueueFullRejects = _queueFullRejects, .DomainRejects = _domainRejects, .OwnerDriftRejects = _ownerDriftRejects, .MappingEpochDriftRejects = _mappingEpochDriftRejects, .IommuDomainEpochDriftRejects = _iommuDomainEpochDriftRejects, .FootprintConflictRejects = _footprintConflictRejects, .DirectWriteViolationRejects = _directWriteViolationRejects, .Lane7SubmitPollThrottleRejects = _lane7SubmitPollThrottleRejects};
		temp_var->Lifecycle = {.Created = _created, .Validated = _validated, .Queued = _queued, .Running = _running, .DeviceCompleted = _deviceCompleted, .CommitPending = _commitPending, .Committed = _committed, .Faulted = _faulted, .Canceled = _canceled, .TimedOut = _timedOut, .Abandoned = _abandoned};
		temp_var->Bytes = {.BytesRead = _bytesRead, .BytesStaged = _bytesStaged, .BytesCommitted = _bytesCommitted};
		temp_var->Conflicts = {.FootprintConflictRejects = _footprintConflictRejects, .DmaStreamComputeConflictRejects = _dmaStreamComputeConflictRejects, .SrfAssistConflictRejects = _srfAssistConflictRejects, .DirectWriteViolationRejects = _directWriteViolationRejects, .CommitRollbackCount = _commitRollbackCount, .SrfInvalidations = _srfInvalidations, .CacheInvalidations = _cacheInvalidations};
		temp_var->OperationCount = _operationCount;
		temp_var->LatencyCycles = _latencyCycles;
		// C# exposes a read-only copy; preserve snapshot isolation with a vector copy.
		temp_var->EvidenceRecords = _evidenceRecords;
		return temp_var;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordCapabilityQuery(bool accepted, string? message = null)
	void AcceleratorTelemetry::RecordCapabilityQuery(bool accepted, const std::wstring &message)
	{
		_capabilityQueryAttempts++;
		if (accepted)
		{
			_capabilityQuerySuccess++;
		}
		else
		{
			_capabilityQueryReject++;
		}

		AddEvidence(AcceleratorEvidenceKind::CapabilityQuery, accepted, AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode::None, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, 0, 0, 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordDescriptorParse(bool accepted, AcceleratorDescriptorFault fault = AcceleratorDescriptorFault.None, string? message = null)
	void AcceleratorTelemetry::RecordDescriptorParse(bool accepted, AcceleratorDescriptorFault fault, const std::wstring &message)
	{
		_descriptorParseAttempts++;
		if (accepted)
		{
			_descriptorAccepted++;
		}
		else
		{
			_descriptorRejected++;
		}

		AddEvidence(AcceleratorEvidenceKind::DescriptorParse, accepted, fault, AcceleratorTokenFaultCode::None, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, 0, 0, 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordSubmit(bool accepted, AcceleratorTokenFaultCode faultCode = AcceleratorTokenFaultCode.None, System.Nullable<AcceleratorGuardDecision> guardDecision = null, string? message = null)
	void AcceleratorTelemetry::RecordSubmit(bool accepted, AcceleratorTokenFaultCode faultCode, std::optional<std::shared_ptr<AcceleratorGuardDecision>> guardDecision, const std::wstring &message)
	{
		_submitAttempts++;
		if (accepted)
		{
			_submitAccepted++;
		}
		else
		{
			_submitRejected++;
			RecordRejectFault(faultCode, guardDecision);
		}

		AddEvidence(AcceleratorEvidenceKind::SubmitAdmission, accepted, AcceleratorDescriptorFault::None, faultCode, (guardDecision && *guardDecision ? (*guardDecision)->Fault : AcceleratorGuardFault::None), AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, 0, 0, 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordTokenTransition(AcceleratorToken token, AcceleratorTokenState fromState, AcceleratorTokenState toState, AcceleratorTokenFaultCode faultCode = AcceleratorTokenFaultCode.None, string? message = null)
	void AcceleratorTelemetry::RecordTokenTransition(std::shared_ptr<AcceleratorToken> token, AcceleratorTokenState fromState, AcceleratorTokenState toState, AcceleratorTokenFaultCode faultCode, const std::wstring &message)
	{
		ArgumentNullException::ThrowIfNull(token);
		IncrementLifecycle(toState);
		AddEvidence(AcceleratorEvidenceKind::TokenTransition, true, AcceleratorDescriptorFault::None, faultCode, AcceleratorGuardFault::None, AcceleratorConflictClass::None, fromState, toState, token, 0, 0, 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordDeviceBusyReject(AcceleratorTokenFaultCode faultCode, string? message = null)
	void AcceleratorTelemetry::RecordDeviceBusyReject(AcceleratorTokenFaultCode faultCode, const std::wstring &message)
	{
		_deviceBusyRejects++;
		AddEvidence(AcceleratorEvidenceKind::DeviceBusyReject, false, AcceleratorDescriptorFault::None, faultCode, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, 0, 0, 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordQueueFullReject(AcceleratorTokenFaultCode faultCode, string? message = null)
	void AcceleratorTelemetry::RecordQueueFullReject(AcceleratorTokenFaultCode faultCode, const std::wstring &message)
	{
		_queueFullRejects++;
		AddEvidence(AcceleratorEvidenceKind::QueueFullReject, false, AcceleratorDescriptorFault::None, faultCode, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, 0, 0, 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordDomainReject(AcceleratorGuardDecision guardDecision, string? message = null)
	void AcceleratorTelemetry::RecordDomainReject(std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring &message)
	{
		_domainRejects++;
		AddEvidence(AcceleratorEvidenceKind::DomainReject, false, AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode::None, guardDecision->Fault, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, 0, 0, 0, ((message != L"") ? message: guardDecision->Message));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordOwnerDriftReject(AcceleratorGuardDecision guardDecision, string? message = null)
	void AcceleratorTelemetry::RecordOwnerDriftReject(std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring &message)
	{
		_ownerDriftRejects++;
		AddEvidence(AcceleratorEvidenceKind::OwnerDriftReject, false, AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode::None, guardDecision->Fault, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, 0, 0, 0, ((message != L"") ? message: guardDecision->Message));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordMappingEpochDriftReject(AcceleratorGuardDecision guardDecision, string? message = null)
	void AcceleratorTelemetry::RecordMappingEpochDriftReject(std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring &message)
	{
		_mappingEpochDriftRejects++;
		AddEvidence(AcceleratorEvidenceKind::MappingEpochDriftReject, false, AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode::None, guardDecision->Fault, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, 0, 0, 0, ((message != L"") ? message: guardDecision->Message));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordIommuDomainEpochDriftReject(AcceleratorGuardDecision guardDecision, string? message = null)
	void AcceleratorTelemetry::RecordIommuDomainEpochDriftReject(std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring &message)
	{
		_iommuDomainEpochDriftRejects++;
		AddEvidence(AcceleratorEvidenceKind::IommuDomainEpochDriftReject, false, AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode::None, guardDecision->Fault, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, 0, 0, 0, ((message != L"") ? message: guardDecision->Message));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordGuardReject(System.Nullable<AcceleratorGuardDecision> guardDecision, string? message = null)
	void AcceleratorTelemetry::RecordGuardReject(const std::optional<std::shared_ptr<AcceleratorGuardDecision>> &guardDecision, const std::wstring &message)
	{
		if (!guardDecision || !*guardDecision || guardDecision.value()->Fault == AcceleratorGuardFault::None)
		{
			return;
		}

		switch (guardDecision.value()->Fault)
		{
			case AcceleratorGuardFault::OwnerMismatch:
			case AcceleratorGuardFault::DescriptorOwnerBindingMismatch:
			case AcceleratorGuardFault::InvalidOwnerCompletion:
				RecordOwnerDriftReject(guardDecision.value(), message);
				break;
			case AcceleratorGuardFault::DomainMismatch:
				RecordDomainReject(guardDecision.value(), message);
				break;
			case AcceleratorGuardFault::MappingEpochDrift:
				RecordMappingEpochDriftReject(guardDecision.value(), message);
				break;
			case AcceleratorGuardFault::IommuDomainEpochDrift:
				RecordIommuDomainEpochDriftReject(guardDecision.value(), message);
				break;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordFootprintConflictReject(AcceleratorConflictClass conflictClass, AcceleratorTokenFaultCode faultCode, string? message = null)
	void AcceleratorTelemetry::RecordFootprintConflictReject(AcceleratorConflictClass conflictClass, AcceleratorTokenFaultCode faultCode, const std::wstring &message)
	{
		_footprintConflictRejects++;
		if (conflictClass == AcceleratorConflictClass::DmaStreamComputeOverlapsAcceleratorWrite)
		{
			_dmaStreamComputeConflictRejects++;
			AddEvidence(AcceleratorEvidenceKind::DmaStreamComputeConflictReject, false, AcceleratorDescriptorFault::None, faultCode, AcceleratorGuardFault::None, conflictClass, std::nullopt, std::nullopt, nullptr, 0, 0, 0, message);
			return;
		}

		if ((conflictClass == AcceleratorConflictClass::AcceleratorWriteOverlapsSrfWarmedWindow || conflictClass == AcceleratorConflictClass::AssistIngressOverlapsAcceleratorWrite))
		{
			_srfAssistConflictRejects++;
		}

		AddEvidence(AcceleratorEvidenceKind::FootprintConflictReject, false, AcceleratorDescriptorFault::None, faultCode, AcceleratorGuardFault::None, conflictClass, std::nullopt, std::nullopt, nullptr, 0, 0, 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordDirectWriteViolationReject(AcceleratorToken? token = null, System.Nullable<string> message = null)
	void AcceleratorTelemetry::RecordDirectWriteViolationReject(std::shared_ptr<AcceleratorToken> token, const std::wstring &message)
	{
		_directWriteViolationRejects++;
		AddEvidence(AcceleratorEvidenceKind::DirectWriteViolationReject, false, AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode::DirectWriteViolation, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, token, 0, 0, 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordCommitRollback(bool attempted, bool succeeded, string? message = null)
	void AcceleratorTelemetry::RecordCommitRollback(bool attempted, bool succeeded, const std::wstring &message)
	{
		if (!attempted)
		{
			return;
		}

		_commitRollbackCount++;
		AddEvidence(AcceleratorEvidenceKind::CommitRollback, succeeded, AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode::None, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, 0, 0, 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordBytesRead(ulong byteCount, string? message = null)
	void AcceleratorTelemetry::RecordBytesRead(unsigned long long byteCount, const std::wstring &message)
	{
		if (byteCount == 0)
		{
			return;
		}

		_bytesRead += byteCount;
		AddEvidence(AcceleratorEvidenceKind::BytesRead, true, AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode::None, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, byteCount, 0, 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordBytesStaged(ulong byteCount, string? message = null)
	void AcceleratorTelemetry::RecordBytesStaged(unsigned long long byteCount, const std::wstring &message)
	{
		if (byteCount == 0)
		{
			return;
		}

		_bytesStaged += byteCount;
		AddEvidence(AcceleratorEvidenceKind::BytesStaged, true, AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode::None, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, byteCount, 0, 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordBytesCommitted(ulong byteCount, string? message = null)
	void AcceleratorTelemetry::RecordBytesCommitted(unsigned long long byteCount, const std::wstring &message)
	{
		if (byteCount == 0)
		{
			return;
		}

		_bytesCommitted += byteCount;
		AddEvidence(AcceleratorEvidenceKind::BytesCommitted, true, AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode::None, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, byteCount, 0, 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordOperation(ulong operationCount, ulong latencyCycles = 0, string? message = null)
	void AcceleratorTelemetry::RecordOperation(unsigned long long operationCount, unsigned long long latencyCycles, const std::wstring &message)
	{
		if (operationCount == 0 && latencyCycles == 0)
		{
			return;
		}

		_operationCount += operationCount;
		_latencyCycles += latencyCycles;
		AddEvidence(AcceleratorEvidenceKind::Operation, true, AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode::None, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, 0, operationCount, latencyCycles, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordSrfInvalidations(int count, string? message = null)
	void AcceleratorTelemetry::RecordSrfInvalidations(int count, const std::wstring &message)
	{
		if (count <= 0)
		{
			return;
		}

		_srfInvalidations += count;
		AddEvidence(AcceleratorEvidenceKind::SrfInvalidation, true, AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode::None, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, 0, static_cast<unsigned long long>(count), 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordCacheInvalidations(int count, string? message = null)
	void AcceleratorTelemetry::RecordCacheInvalidations(int count, const std::wstring &message)
	{
		if (count <= 0)
		{
			return;
		}

		_cacheInvalidations += count;
		AddEvidence(AcceleratorEvidenceKind::CacheInvalidation, true, AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode::None, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, 0, static_cast<unsigned long long>(count), 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordLane7SubmitPollThrottleReject(string? message = null)
	void AcceleratorTelemetry::RecordLane7SubmitPollThrottleReject(const std::wstring &message)
	{
		_lane7SubmitPollThrottleRejects++;
		AddEvidence(AcceleratorEvidenceKind::Lane7SubmitPollThrottleReject, false, AcceleratorDescriptorFault::None, AcceleratorTokenFaultCode::Lane7PressureRejected, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, nullptr, 0, 0, 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordBackendReject(AcceleratorTokenFaultCode faultCode, AcceleratorToken? token = null, System.Nullable<string> message = null)
	void AcceleratorTelemetry::RecordBackendReject(AcceleratorTokenFaultCode faultCode, std::shared_ptr<AcceleratorToken> token, const std::wstring &message)
	{
		_backendRejects++;
		RecordRejectFault(faultCode, std::nullopt);
		AddEvidence(AcceleratorEvidenceKind::BackendReject, false, AcceleratorDescriptorFault::None, faultCode, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, token, 0, 0, 0, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void RecordCommitReject(AcceleratorTokenFaultCode faultCode, AcceleratorToken? token = null, System.Nullable<string> message = null)
	void AcceleratorTelemetry::RecordCommitReject(AcceleratorTokenFaultCode faultCode, std::shared_ptr<AcceleratorToken> token, const std::wstring &message)
	{
		_commitRejects++;
		RecordRejectFault(faultCode, std::nullopt);
		AddEvidence(AcceleratorEvidenceKind::CommitReject, false, AcceleratorDescriptorFault::None, faultCode, AcceleratorGuardFault::None, AcceleratorConflictClass::None, std::nullopt, std::nullopt, token, 0, 0, 0, message);
	}

	void AcceleratorTelemetry::RecordRejectFault(AcceleratorTokenFaultCode faultCode, const std::optional<std::shared_ptr<AcceleratorGuardDecision>> &guardDecision)
	{
		RecordGuardReject(guardDecision);
		if (faultCode == AcceleratorTokenFaultCode::DirectWriteViolation)
		{
			_directWriteViolationRejects++;
		}
	}

	void AcceleratorTelemetry::IncrementLifecycle(AcceleratorTokenState state)
	{
		switch (state)
		{
			case AcceleratorTokenState::Created:
				_created++;
				break;
			case AcceleratorTokenState::Validated:
				_validated++;
				break;
			case AcceleratorTokenState::Queued:
				_queued++;
				break;
			case AcceleratorTokenState::Running:
				_running++;
				break;
			case AcceleratorTokenState::DeviceComplete:
				_deviceCompleted++;
				break;
			case AcceleratorTokenState::CommitPending:
				_commitPending++;
				break;
			case AcceleratorTokenState::Committed:
				_committed++;
				break;
			case AcceleratorTokenState::Faulted:
				_faulted++;
				break;
			case AcceleratorTokenState::Canceled:
				_canceled++;
				break;
			case AcceleratorTokenState::TimedOut:
				_timedOut++;
				break;
			case AcceleratorTokenState::Abandoned:
				_abandoned++;
				break;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private void AddEvidence(AcceleratorEvidenceKind kind, bool accepted, AcceleratorDescriptorFault descriptorFault = AcceleratorDescriptorFault.None, AcceleratorTokenFaultCode tokenFaultCode = AcceleratorTokenFaultCode.None, AcceleratorGuardFault guardFault = AcceleratorGuardFault.None, AcceleratorConflictClass conflictClass = AcceleratorConflictClass.None, System.Nullable<AcceleratorTokenState> fromState = null, System.Nullable<AcceleratorTokenState> toState = null, AcceleratorToken? token = null, ulong byteCount = 0, ulong operationCount = 0, ulong latencyCycles = 0, System.Nullable<string> message = null)
	void AcceleratorTelemetry::AddEvidence(AcceleratorEvidenceKind kind, bool accepted, AcceleratorDescriptorFault descriptorFault, AcceleratorTokenFaultCode tokenFaultCode, AcceleratorGuardFault guardFault, AcceleratorConflictClass conflictClass, std::optional<AcceleratorTokenState> fromState, std::optional<AcceleratorTokenState> toState, std::shared_ptr<AcceleratorToken> token, unsigned long long byteCount, unsigned long long operationCount, unsigned long long latencyCycles, const std::wstring &message)
	{
		std::shared_ptr<AcceleratorEvidenceRecord> temp_var = std::make_shared<AcceleratorEvidenceRecord>();
		temp_var->Sequence = AllocateSequence();
		temp_var->Kind = kind;
		temp_var->Accepted = accepted;
		temp_var->DescriptorFault = descriptorFault;
		temp_var->TokenFaultCode = tokenFaultCode;
		temp_var->GuardFault = guardFault;
		temp_var->ConflictClass = conflictClass;
		temp_var->FromState = fromState;
		temp_var->ToState = toState;
		temp_var->TokenId = token ? token->getTokenId() : 0;
		temp_var->TokenHandle = token && token->getHandle() ? token->getHandle()->Value : 0;
		temp_var->ByteCount = byteCount;
		temp_var->OperationCount = operationCount;
		temp_var->LatencyCycles = latencyCycles;
		temp_var->Message = message;
		_evidenceRecords.push_back(temp_var);
	}

	unsigned long long AcceleratorTelemetry::AllocateSequence()
	{
		unsigned long long sequence = _nextSequence++;
		if (_nextSequence == 0)
		{
			_nextSequence = 1;
		}

		return sequence;
	}
}
