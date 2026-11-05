#include "ReferenceExternalAcceleratorBackend.h"
#include "../Tokens/AcceleratorTokenStatusWord.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Queues;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Backends
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ReferenceExternalAcceleratorBackend(IAcceleratorBackendClock? clock = null, System.Nullable<AcceleratorTelemetry> telemetry = null, System.Nullable<ExternalAcceleratorFeatureSwitch> featureSwitch = null)
	ReferenceExternalAcceleratorBackend::ReferenceExternalAcceleratorBackend(std::shared_ptr<IAcceleratorBackendClock> clock, std::shared_ptr<AcceleratorTelemetry> telemetry, std::optional<std::shared_ptr<ExternalAcceleratorFeatureSwitch>> featureSwitch) : _clock((clock != nullptr) ? clock : std::make_shared<ManualAcceleratorBackendClock>()), _telemetry(telemetry), _featureSwitch(featureSwitch.value_or(ExternalAcceleratorFeatureSwitch::Enabled))
	{
	}

	std::shared_ptr<AcceleratorBackendResult> ReferenceExternalAcceleratorBackend::TrySubmit(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<IAcceleratorCommandQueue> queue, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(request);
		ArgumentNullException::ThrowIfNull(queue);
		unsigned long long tick = _clock->Advance();

		if (!_featureSwitch->BackendExecutionEnabled)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::BackendExecutionUnavailable, L"L7-SDC reference backend execution is disabled by rollback feature switch; queue admission and staged writes were not reached.", tick, request->TokenAdmission->Token));
		}

		std::shared_ptr<AcceleratorQueueAdmissionResult> queueResult = queue->TryEnqueue(request, currentGuardEvidence);
		return RecordResult(queueResult->IsAccepted ? AcceleratorBackendResult::Submitted(queueResult, tick) : AcceleratorBackendResult::Rejected(queueResult->FaultCode, queueResult->Message, tick, (queueResult->Command == nullptr ? nullptr : queueResult->Command->Token), queueResult));
	}

	std::shared_ptr<AcceleratorBackendResult> ReferenceExternalAcceleratorBackend::Tick(std::shared_ptr<IAcceleratorCommandQueue> queue, std::shared_ptr<IAcceleratorMemoryPortal> memoryPortal, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(queue);
		ArgumentNullException::ThrowIfNull(memoryPortal);
		ArgumentNullException::ThrowIfNull(stagingBuffer);
		unsigned long long tick = _clock->Advance();

		if (!_featureSwitch->BackendExecutionEnabled)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::BackendExecutionUnavailable, L"L7-SDC reference backend tick is disabled by rollback feature switch; queued commands remain unexecuted and no staged write occurred.", tick));
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
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::MissingGuardEvidence, L"L7-SDC reference backend tick requires fresh guard evidence before Running transition.", tick, token));
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
				return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::StagingRejected, L"L7-SDC reference backend cannot materialize destination staging range larger than Int32.MaxValue bytes.", tick, token));
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

		return RecordResult(AcceleratorBackendResult::DeviceCompleted(token, reads->BytesRead, bytesStaged, stagedWriteCount, tick, L"L7-SDC reference backend completed guarded execution and staged bytes only; token is DeviceComplete, not Committed."));
	}

	std::shared_ptr<AcceleratorBackendResult> ReferenceExternalAcceleratorBackend::TryCancel(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(tokenStore);
		std::shared_ptr<AcceleratorTokenLookupResult> cancel = tokenStore->Cancel(handle, currentGuardEvidence);
		unsigned long long tick = _clock->Advance();
		return RecordResult(cancel->IsAllowed ? AcceleratorBackendResult::Canceled(cancel, tick) : AcceleratorBackendResult::Rejected(cancel->FaultCode, cancel->Message, tick, cancel->Token, nullptr, cancel));
	}

	std::shared_ptr<AcceleratorBackendResult> ReferenceExternalAcceleratorBackend::RecordResult(std::shared_ptr<AcceleratorBackendResult> result)
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
	std::vector<unsigned char> ReferenceExternalAcceleratorBackend::BuildDeterministicStagedBytes(std::shared_ptr<AcceleratorToken> token, const std::vector<std::shared_ptr<AcceleratorMemoryRead>> &reads, int length)
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
}


