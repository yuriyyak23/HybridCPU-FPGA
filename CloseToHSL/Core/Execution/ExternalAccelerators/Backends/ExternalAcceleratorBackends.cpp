#include "ExternalAcceleratorBackends.h"
#include "../Auth/AcceleratorOwnerDomainGuard.h"
#include "../../../Pipeline/Safety/SafetyVerifier.Types.h"

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Queues;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Backends
{

	unsigned long long ManualAcceleratorBackendClock::getTickCount() const
	{
		return TickCount;
	}

	void ManualAcceleratorBackendClock::setTickCount(unsigned long long value)
	{
		TickCount = value;
	}

	unsigned long long ManualAcceleratorBackendClock::Advance()
	{
//		unchecked
			setTickCount(getTickCount() + 1);
			if (getTickCount() == 0)
			{
				setTickCount(1);
			}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.

		return getTickCount();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public NullExternalAcceleratorBackend(NullExternalAcceleratorBackendPolicy policy = NullExternalAcceleratorBackendPolicy.RejectSubmit, IAcceleratorBackendClock? clock = null, System.Nullable<AcceleratorTelemetry> telemetry = null, System.Nullable<ExternalAcceleratorFeatureSwitch> featureSwitch = null)
	NullExternalAcceleratorBackend::NullExternalAcceleratorBackend(NullExternalAcceleratorBackendPolicy policy, std::shared_ptr<IAcceleratorBackendClock> clock, std::shared_ptr<AcceleratorTelemetry> telemetry, std::optional<std::shared_ptr<ExternalAcceleratorFeatureSwitch>> featureSwitch) : _clock((clock != nullptr) ? clock : std::make_shared<ManualAcceleratorBackendClock>()), _policy(policy), _telemetry(telemetry), _featureSwitch(featureSwitch.value_or(ExternalAcceleratorFeatureSwitch::Enabled))
	{
	}

	std::shared_ptr<AcceleratorBackendResult> NullExternalAcceleratorBackend::TrySubmit(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<IAcceleratorCommandQueue> queue, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(request);
		ArgumentNullException::ThrowIfNull(queue);
		unsigned long long tick = _clock->Advance();

		if (!_featureSwitch->BackendExecutionEnabled)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::BackendExecutionUnavailable, L"L7-SDC backend execution is disabled by rollback feature switch; queue, staging, commit, and direct writes were not reached.", tick, request->TokenAdmission->Token));
		}

		if (_policy == NullExternalAcceleratorBackendPolicy::FaultToken && request->TokenAdmission->IsAccepted && request->TokenAdmission->Token != nullptr && currentGuardEvidence)
		{
			std::shared_ptr<AcceleratorToken> token;
			AcceleratorTokenFaultCode rejectFaultCode;
			std::wstring rejectMessage;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!TryValidateFaultPolicyRequest(request, *currentGuardEvidence, out AcceleratorToken? token, out AcceleratorTokenFaultCode rejectFaultCode, out string rejectMessage))
			if (!TryValidateFaultPolicyRequest(request, *currentGuardEvidence, token, rejectFaultCode, rejectMessage))
			{
				return RecordResult(AcceleratorBackendResult::Rejected(rejectFaultCode, rejectMessage, tick, request->TokenAdmission->Token));
			}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorTokenTransition fault = token!.MarkFaulted( AcceleratorTokenFaultCode.BackendExecutionUnavailable, currentGuardEvidence);
			std::shared_ptr<AcceleratorTokenTransition> fault = token->MarkFaulted(AcceleratorTokenFaultCode::BackendExecutionUnavailable, *currentGuardEvidence);
			return RecordResult(fault->Succeeded ? AcceleratorBackendResult::Faulted(token, AcceleratorTokenFaultCode::BackendExecutionUnavailable, L"Null L7-SDC backend faulted the token by explicit policy; no queue or memory write occurred.", tick) : AcceleratorBackendResult::Rejected(fault->FaultCode, fault->Message, tick, token));
		}

		return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::BackendExecutionUnavailable, L"Null L7-SDC backend deterministically rejects submit; no queue admission or memory write occurred.", tick, request->TokenAdmission->Token));
	}

	std::shared_ptr<AcceleratorBackendResult> NullExternalAcceleratorBackend::Tick(std::shared_ptr<IAcceleratorCommandQueue> queue, std::shared_ptr<IAcceleratorMemoryPortal> memoryPortal, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(queue);
		ArgumentNullException::ThrowIfNull(memoryPortal);
		ArgumentNullException::ThrowIfNull(stagingBuffer);
		if (!_featureSwitch->BackendExecutionEnabled)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::BackendExecutionUnavailable, L"L7-SDC backend tick is disabled by rollback feature switch; no command execution or staged write occurred.", _clock->Advance()));
		}

		return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::BackendExecutionUnavailable, L"Null L7-SDC backend has no execution engine and cannot tick commands.", _clock->Advance()));
	}

	std::shared_ptr<AcceleratorBackendResult> NullExternalAcceleratorBackend::TryCancel(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(tokenStore);
		std::shared_ptr<AcceleratorTokenLookupResult> cancel = tokenStore->Cancel(handle, currentGuardEvidence);
		unsigned long long tick = _clock->Advance();
		return RecordResult(cancel->IsAllowed ? AcceleratorBackendResult::Canceled(cancel, tick) : AcceleratorBackendResult::Rejected(cancel->FaultCode, cancel->Message, tick, cancel->Token, nullptr, cancel));
	}

	std::shared_ptr<AcceleratorBackendResult> NullExternalAcceleratorBackend::RecordResult(std::shared_ptr<AcceleratorBackendResult> result)
	{
		if (_telemetry != nullptr && result->IsRejected)
		{
			_telemetry->RecordBackendReject(result->FaultCode, result->Token, result->Message);
		}

		return result;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool TryValidateFaultPolicyRequest(AcceleratorQueueAdmissionRequest request, AcceleratorGuardEvidence currentGuardEvidence, out AcceleratorToken? token, out AcceleratorTokenFaultCode faultCode, out string message)
	bool NullExternalAcceleratorBackend::TryValidateFaultPolicyRequest(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence, std::shared_ptr<AcceleratorToken> &token, AcceleratorTokenFaultCode &faultCode, std::wstring &message)
	{
		token = request->TokenAdmission->Token;
		faultCode = AcceleratorTokenFaultCode::None;
		message = L"";

		if (token == nullptr)
		{
			faultCode = AcceleratorTokenFaultCode::SubmitAdmissionRejected;
			message = L"Null L7-SDC backend fault policy requires accepted Phase 06 token admission evidence.";
			return false;
		}

		std::wstring descriptorMessage;
		if (!AcceleratorOwnerDomainGuard::getDefault()->IsDescriptorGuardBacked(request->Descriptor, descriptorMessage))
		{
			faultCode = AcceleratorTokenFaultCode::DescriptorNotGuardBacked;
			message = descriptorMessage;
			return false;
		}

		std::shared_ptr<AcceleratorGuardDecision> capabilityGuardDecision = request->CapabilityAcceptance->getGuardDecision();
		if (!request->CapabilityAcceptance->isAccepted() || !capabilityGuardDecision || !capabilityGuardDecision->IsAllowed || (!capabilityGuardDecision->Evidence || capabilityGuardDecision->Evidence->Source != AcceleratorGuardEvidenceSource::GuardPlane) || capabilityGuardDecision->LegalityDecision.getAuthoritySource() != LegalityAuthoritySource::GuardPlane || capabilityGuardDecision->LegalityDecision.getAttemptedReplayCertificateReuse() || capabilityGuardDecision->DescriptorOwnerBinding == nullptr || !capabilityGuardDecision->DescriptorOwnerBinding->Equals(request->Descriptor->OwnerBinding))
		{
			faultCode = request->CapabilityAcceptance->isAccepted() ? AcceleratorTokenFaultCode::CapabilityNotAccepted : AcceleratorTokenFaultCode::CapabilityRejected;
			message = L"Null L7-SDC backend fault policy requires guard-backed capability acceptance bound to the descriptor.";
			return false;
		}

		if ((token->getDescriptor() != request->Descriptor && !token->getDescriptor()->Equals(request->Descriptor)) || token->getCapabilityAcceptance() != request->CapabilityAcceptance)
		{
			faultCode = AcceleratorTokenFaultCode::QueueAdmissionRejected;
			message = L"Null L7-SDC backend fault policy requires request descriptor and capability evidence to match the token admission.";
			return false;
		}

		std::optional<std::shared_ptr<AcceleratorGuardEvidence>> evidence = currentGuardEvidence;
		std::shared_ptr<AcceleratorGuardDecision> submitGuardDecision = AcceleratorOwnerDomainGuard::getDefault()->EnsureBeforeSubmit(request->Descriptor, evidence);
		if (!submitGuardDecision->IsAllowed)
		{
			faultCode = AcceleratorTokenStore::MapGuardFault(submitGuardDecision->Fault);
			message = L"Null L7-SDC backend fault policy requires fresh guarded submit authority. " + submitGuardDecision->Message;
			return false;
		}

		std::shared_ptr<AcceleratorGuardDecision> tokenGuardDecision = AcceleratorOwnerDomainGuard::getDefault()->ValidateMappingEpoch(token->getSubmitGuardDecision(), evidence);
		if (!tokenGuardDecision->IsAllowed)
		{
			faultCode = AcceleratorTokenStore::MapGuardFault(tokenGuardDecision->Fault);
			message = L"Null L7-SDC backend fault policy requires token-bound owner/domain and epoch revalidation. " + tokenGuardDecision->Message;
			return false;
		}

		message = L"Null L7-SDC backend fault policy request is guard-backed and token-bound.";
		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public FakeExternalAcceleratorBackend(IAcceleratorBackendClock? clock = null, System.Nullable<AcceleratorTelemetry> telemetry = null, System.Nullable<ExternalAcceleratorFeatureSwitch> featureSwitch = null)
	FakeExternalAcceleratorBackend::FakeExternalAcceleratorBackend(std::shared_ptr<IAcceleratorBackendClock> clock, std::shared_ptr<AcceleratorTelemetry> telemetry, std::optional<std::shared_ptr<ExternalAcceleratorFeatureSwitch>> featureSwitch) : _clock((clock != nullptr) ? clock : std::make_shared<ManualAcceleratorBackendClock>()), _telemetry(telemetry), _featureSwitch(featureSwitch.value_or(ExternalAcceleratorFeatureSwitch::Enabled))
	{
	}

	bool FakeExternalAcceleratorBackend::isTestOnly() const
	{
		return true;
	}

	std::shared_ptr<AcceleratorBackendResult> FakeExternalAcceleratorBackend::TrySubmit(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<IAcceleratorCommandQueue> queue, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(request);
		ArgumentNullException::ThrowIfNull(queue);
		unsigned long long tick = _clock->Advance();
		if (!_featureSwitch->BackendExecutionEnabled)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::BackendExecutionUnavailable, L"L7-SDC fake backend execution is disabled by rollback feature switch; queue admission and staged writes were not reached.", tick, request->TokenAdmission->Token));
		}

		std::shared_ptr<AcceleratorQueueAdmissionResult> queueResult = queue->TryEnqueue(request, currentGuardEvidence);
		std::shared_ptr<AcceleratorBackendResult> result = queueResult->IsAccepted ? AcceleratorBackendResult::Submitted(queueResult, tick) : AcceleratorBackendResult::Rejected(queueResult->FaultCode, queueResult->Message, tick, (queueResult->Command == nullptr ? nullptr : queueResult->Command->Token), queueResult);
		return RecordResult(result);
	}

	std::shared_ptr<AcceleratorBackendResult> FakeExternalAcceleratorBackend::Tick(std::shared_ptr<IAcceleratorCommandQueue> queue, std::shared_ptr<IAcceleratorMemoryPortal> memoryPortal, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(queue);
		ArgumentNullException::ThrowIfNull(memoryPortal);
		ArgumentNullException::ThrowIfNull(stagingBuffer);
		unsigned long long tick = _clock->Advance();

		if (!_featureSwitch->BackendExecutionEnabled)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::BackendExecutionUnavailable, L"L7-SDC fake backend tick is disabled by rollback feature switch; queued commands remain unexecuted and no staged write occurred.", tick));
		}

		std::optional<std::shared_ptr<AcceleratorQueuedCommand>> command;
		std::shared_ptr<AcceleratorQueueAdmissionResult> dequeueResult;
		if (!queue->TryDequeueReady(currentGuardEvidence, command, dequeueResult))
		{
			return RecordResult(AcceleratorBackendResult::Rejected(dequeueResult->FaultCode, dequeueResult->Message, tick, (dequeueResult->Command == nullptr ? nullptr : dequeueResult->Command->Token), dequeueResult));
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorToken token = command!.Token;
		if (!command || !*command) return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::BackendRejected, L"Queue returned no command.", tick));
		std::shared_ptr<AcceleratorToken> token = (*command)->Token;
		if (!currentGuardEvidence || !*currentGuardEvidence)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::MissingGuardEvidence, L"L7-SDC fake backend tick requires fresh guard evidence before Running transition.", tick, token));
		}

		std::shared_ptr<AcceleratorTokenTransition> running = token->MarkRunning(*currentGuardEvidence);
		if (running->Rejected)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(running->FaultCode, running->Message, tick, token));
		}

		std::shared_ptr<AcceleratorMemoryPortalReadResult> reads = memoryPortal->ReadSourceRanges(token, (*command)->Descriptor, currentGuardEvidence);
		if (reads->IsRejected)
		{
			token->MarkFaulted(reads->FaultCode, *currentGuardEvidence);
			return RecordResult(AcceleratorBackendResult::Rejected(reads->FaultCode, reads->Message, tick, token));
		}

		unsigned long long bytesStaged = 0;
		int stagedWriteCount = 0;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<AcceleratorMemoryRange> destinations = (*command)->Descriptor.DestinationRanges;
		const std::vector<std::shared_ptr<AcceleratorMemoryRange>> destinations = (*command)->Descriptor->DestinationRanges;
		for (int index = 0; index < destinations.size(); index++)
		{
			std::shared_ptr<AcceleratorMemoryRange> destination = destinations[index];
			if (destination->Length > std::numeric_limits<int>::max())
			{
				token->MarkFaulted(AcceleratorTokenFaultCode::StagingRejected, *currentGuardEvidence);
				return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::StagingRejected, L"L7-SDC fake backend cannot materialize destination staging range larger than Int32.MaxValue bytes.", tick, token));
			}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: byte[] stagedBytes = BuildDeterministicStagedBytes(token, reads.Reads, checked((int)destination.Length));
			std::vector<unsigned char> stagedBytes = BuildDeterministicStagedBytes(token, reads->Reads, static_cast<int>(destination->Length));
			std::shared_ptr<AcceleratorStagingResult> staged = stagingBuffer->StageWrite(token, destination, stagedBytes, currentGuardEvidence);
			if (staged->IsRejected)
			{
				token->MarkFaulted(staged->FaultCode, *currentGuardEvidence);
				return RecordResult(AcceleratorBackendResult::Rejected(staged->FaultCode, staged->Message, tick, token));
			}

			bytesStaged += destination->Length;
			stagedWriteCount++;
		}

		std::shared_ptr<AcceleratorTokenTransition> complete = token->MarkDeviceComplete(*currentGuardEvidence);
		if (complete->Rejected)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(complete->FaultCode, complete->Message, tick, token));
		}

		return RecordResult(AcceleratorBackendResult::DeviceCompleted(token, reads->BytesRead, bytesStaged, stagedWriteCount, tick, L"Fake L7-SDC backend completed device-private model execution and staged bytes only; token is DeviceComplete, not Committed."));
	}

	std::shared_ptr<AcceleratorBackendResult> FakeExternalAcceleratorBackend::TryCancel(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(tokenStore);
		std::shared_ptr<AcceleratorTokenLookupResult> cancel = tokenStore->Cancel(handle, currentGuardEvidence);
		unsigned long long tick = _clock->Advance();
		std::shared_ptr<AcceleratorBackendResult> result = cancel->IsAllowed ? AcceleratorBackendResult::Canceled(cancel, tick) : AcceleratorBackendResult::Rejected(cancel->FaultCode, cancel->Message, tick, cancel->Token, nullptr, cancel);
		return RecordResult(result);
	}

	std::shared_ptr<AcceleratorBackendResult> FakeExternalAcceleratorBackend::RecordResult(std::shared_ptr<AcceleratorBackendResult> result)
	{
		if (_telemetry == nullptr)
		{
			return result;
		}

		if (result->Kind == AcceleratorBackendResultKind::DeviceCompleted)
		{
			_telemetry->RecordBytesRead(result->BytesRead, result->Message);
			_telemetry->RecordBytesStaged(result->BytesStaged, result->Message);
			_telemetry->RecordOperation(1, 1, result->Message);
		}
		else if (result->IsRejected)
		{
			_telemetry->RecordBackendReject(result->FaultCode, result->Token, result->Message);
		}

		return result;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static byte[] BuildDeterministicStagedBytes(AcceleratorToken token, IReadOnlyList<AcceleratorMemoryRead> reads, int length)
	std::vector<unsigned char> FakeExternalAcceleratorBackend::BuildDeterministicStagedBytes(std::shared_ptr<AcceleratorToken> token, const std::vector<std::shared_ptr<AcceleratorMemoryRead>> &reads, int length)
	{
		std::vector<unsigned char> output(length);
		if (length == 0)
		{
			return output;
		}

		unsigned char seed = static_cast<unsigned char>(token->getTokenId() ^ token->getDescriptor()->Identity.NormalizedFootprintHash);
		int offset = 0;
		for (int readIndex = 0; readIndex < reads.size() && offset < output.size(); readIndex++)
		{
			std::span<const unsigned char> source(reads[readIndex]->Data);
			for (int sourceIndex = 0; sourceIndex < source.size() && offset < output.size(); sourceIndex++)
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: output[offset++] = unchecked((byte)(source[sourceIndex] ^ seed));
				output[offset++] = static_cast<unsigned char>(source[sourceIndex] ^ seed);
			}
		}

		while (offset < output.size())
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: output[offset] = unchecked((byte)(seed + offset));
			output[offset] = static_cast<unsigned char>(seed + offset);
			offset++;
		}

		return output;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DirectWriteViolationBackend(IAcceleratorBackendClock? clock = null, System.Nullable<AcceleratorTelemetry> telemetry = null)
	DirectWriteViolationBackend::DirectWriteViolationBackend(std::shared_ptr<IAcceleratorBackendClock> clock, std::shared_ptr<AcceleratorTelemetry> telemetry) : _clock((clock != nullptr) ? clock : std::make_shared<ManualAcceleratorBackendClock>()), _telemetry(telemetry)
	{
	}

	bool DirectWriteViolationBackend::isTestOnly() const
	{
		return true;
	}

	std::shared_ptr<AcceleratorBackendResult> DirectWriteViolationBackend::TrySubmit(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<IAcceleratorCommandQueue> queue, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(request);
		ArgumentNullException::ThrowIfNull(queue);
		return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::DirectWriteViolation, L"Direct-write violation backend is a test-only negative control and cannot accept production submit.", _clock->Advance(), request->TokenAdmission->Token));
	}

	std::shared_ptr<AcceleratorBackendResult> DirectWriteViolationBackend::Tick(std::shared_ptr<IAcceleratorCommandQueue> queue, std::shared_ptr<IAcceleratorMemoryPortal> memoryPortal, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(queue);
		ArgumentNullException::ThrowIfNull(memoryPortal);
		ArgumentNullException::ThrowIfNull(stagingBuffer);
		return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::DirectWriteViolation, L"Direct-write violation backend has no legal tick path; direct architectural publication must fault closed.", _clock->Advance()));
	}

	std::shared_ptr<AcceleratorBackendResult> DirectWriteViolationBackend::TryCancel(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(tokenStore);
		std::shared_ptr<AcceleratorTokenLookupResult> cancel = tokenStore->Cancel(handle, currentGuardEvidence);
		unsigned long long tick = _clock->Advance();
		return RecordResult(cancel->IsAllowed ? AcceleratorBackendResult::Canceled(cancel, tick) : AcceleratorBackendResult::Rejected(cancel->FaultCode, cancel->Message, tick, cancel->Token, nullptr, cancel));
	}

	std::shared_ptr<AcceleratorBackendResult> DirectWriteViolationBackend::AttemptDirectWriteForTest(std::shared_ptr<AcceleratorQueuedCommand> command, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, unsigned long long address, std::span<const unsigned char> attemptedData)
	{
		ArgumentNullException::ThrowIfNull(command);
		unsigned long long tick = _clock->Advance();
		if (!currentGuardEvidence || !*currentGuardEvidence)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::MissingGuardEvidence, L"Direct-write violation test requires current guard evidence before fault publication.", tick, command->Token));
		}

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = AcceleratorOwnerDomainGuard::getDefault()->ValidateMappingEpoch(command->Token->getSubmitGuardDecision(), currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), L"Direct-write violation test detected stale owner/domain or epoch evidence before any side effect. " + guardDecision->Message, tick, command->Token));
		}

		std::shared_ptr<AcceleratorTokenTransition> fault = command->Token->MarkFaulted(AcceleratorTokenFaultCode::DirectWriteViolation, *currentGuardEvidence);
		std::shared_ptr<AcceleratorBackendResult> result = fault->Succeeded ? AcceleratorBackendResult::Faulted(command->Token, AcceleratorTokenFaultCode::DirectWriteViolation, std::format(L"Direct architectural write attempt to 0x{:X} for {} byte(s) was detected and rejected; no memory portal exposes publication authority in Phase 07.", address, attemptedData.size()), tick, true) : AcceleratorBackendResult::Rejected(fault->FaultCode, fault->Message, tick, command->Token);
		return RecordResult(result);
	}

	std::shared_ptr<AcceleratorBackendResult> DirectWriteViolationBackend::RecordResult(std::shared_ptr<AcceleratorBackendResult> result)
	{
		if (_telemetry == nullptr)
		{
			return result;
		}

		if (result->DirectWriteViolationDetected)
		{
			_telemetry->RecordDirectWriteViolationReject(result->Token, result->Message);
		}
		else if (result->IsRejected)
		{
			_telemetry->RecordBackendReject(result->FaultCode, result->Token, result->Message);
		}

		return result;
	}
}

