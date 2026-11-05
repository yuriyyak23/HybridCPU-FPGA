#include "FakeMatMulExternalAcceleratorBackend.h"
#include "../Capabilities/MatMulCapabilityProvider.h"
#include "../Descriptors/AcceleratorCommandDescriptor.h"
#include <cstring>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Queues;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Backends
{
	namespace
	{
		template<typename T>
		T ReadScalar(std::span<const unsigned char> data, unsigned long long byteOffset)
		{
			if (byteOffset > data.size() || sizeof(T) > data.size() - static_cast<std::size_t>(byteOffset))
			{
				throw std::out_of_range("MatMul source read is outside the validated source span.");
			}
			T value{};
			std::memcpy(&value, data.data() + static_cast<std::size_t>(byteOffset), sizeof(T));
			return value;
		}

		template<typename T>
		void WriteScalar(std::span<unsigned char> data, unsigned long long byteOffset, T value)
		{
			if (byteOffset > data.size() || sizeof(T) > data.size() - static_cast<std::size_t>(byteOffset))
			{
				throw std::out_of_range("MatMul destination write is outside the validated destination span.");
			}
			std::memcpy(data.data() + static_cast<std::size_t>(byteOffset), &value, sizeof(T));
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public FakeMatMulExternalAcceleratorBackend(IAcceleratorBackendClock? clock = null, System.Nullable<AcceleratorTelemetry> telemetry = null, System.Nullable<ExternalAcceleratorFeatureSwitch> featureSwitch = null)
	FakeMatMulExternalAcceleratorBackend::FakeMatMulExternalAcceleratorBackend(std::shared_ptr<IAcceleratorBackendClock> clock, std::shared_ptr<AcceleratorTelemetry> telemetry, std::optional<std::shared_ptr<ExternalAcceleratorFeatureSwitch>> featureSwitch) : _clock((clock != nullptr) ? clock : std::make_shared<ManualAcceleratorBackendClock>()), _telemetry(telemetry), _featureSwitch(featureSwitch.value_or(ExternalAcceleratorFeatureSwitch::Enabled))
	{
	}

	bool FakeMatMulExternalAcceleratorBackend::isTestOnly() const
	{
		return true;
	}

	void FakeMatMulExternalAcceleratorBackend::RegisterDescriptor(std::shared_ptr<AcceleratorCommandDescriptor> commandDescriptor, std::shared_ptr<MatMulDescriptor> matMulDescriptor)
	{
		ArgumentNullException::ThrowIfNull(commandDescriptor);
		ArgumentNullException::ThrowIfNull(matMulDescriptor);

		std::shared_ptr<MatMulDescriptorValidationResult> validation = _validator->Validate(matMulDescriptor, commandDescriptor);
		if (validation->IsRejected)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException(validation.Message, nameof(matMulDescriptor));
			throw std::invalid_argument(std::string(validation->Message.begin(), validation->Message.end()));
		}

		_descriptorsByIdentity[commandDescriptor->Identity.DescriptorIdentityHash] = matMulDescriptor;
	}

	std::shared_ptr<AcceleratorBackendResult> FakeMatMulExternalAcceleratorBackend::TrySubmit(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<IAcceleratorCommandQueue> queue, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(request);
		ArgumentNullException::ThrowIfNull(queue);
		unsigned long long tick = _clock->Advance();

		if (!_featureSwitch->BackendExecutionEnabled)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::BackendExecutionUnavailable, L"Fake MatMul backend execution is disabled by rollback feature switch; queue admission and staged writes were not reached.", tick, request->TokenAdmission->Token));
		}

		std::shared_ptr<AcceleratorCapabilityDescriptor> acceptedCapability = request->CapabilityAcceptance ? request->CapabilityAcceptance->getDescriptor() : nullptr;
		if (!acceptedCapability || acceptedCapability->getAcceleratorId() != MatMulCapabilityProvider::AcceleratorId || !MatMulCapabilityProvider::Matches(request->Descriptor))
		{
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::BackendRejected, L"Fake MatMul backend requires ReferenceMatMul command descriptor metadata; capability metadata still does not grant execution authority.", tick, request->TokenAdmission->Token));
		}

		std::shared_ptr<AcceleratorQueueAdmissionResult> queueResult = queue->TryEnqueue(request, currentGuardEvidence);
		return RecordResult(queueResult->IsAccepted ? AcceleratorBackendResult::Submitted(queueResult, tick) : AcceleratorBackendResult::Rejected(queueResult->FaultCode, queueResult->Message, tick, (queueResult->Command == nullptr ? nullptr : queueResult->Command->Token), queueResult));
	}

	std::shared_ptr<AcceleratorBackendResult> FakeMatMulExternalAcceleratorBackend::Tick(std::shared_ptr<IAcceleratorCommandQueue> queue, std::shared_ptr<IAcceleratorMemoryPortal> memoryPortal, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(queue);
		ArgumentNullException::ThrowIfNull(memoryPortal);
		ArgumentNullException::ThrowIfNull(stagingBuffer);
		unsigned long long tick = _clock->Advance();

		if (!_featureSwitch->BackendExecutionEnabled)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::BackendExecutionUnavailable, L"Fake MatMul backend tick is disabled by rollback feature switch; queued commands remain unexecuted and no staged write occurred.", tick));
		}

		std::optional<std::shared_ptr<AcceleratorQueuedCommand>> command;
		std::shared_ptr<AcceleratorQueueAdmissionResult> dequeueResult;
		if (!queue->TryDequeueReady(currentGuardEvidence, command, dequeueResult))
		{
			return RecordResult(AcceleratorBackendResult::Rejected(dequeueResult->FaultCode, dequeueResult->Message, tick, (dequeueResult->Command == nullptr ? nullptr : dequeueResult->Command->Token), dequeueResult));
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorToken token = command!.Token;
		if (!command || !*command)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::BackendRejected, L"Fake MatMul backend queue reported success without a command.", tick));
		}
		const std::shared_ptr<AcceleratorQueuedCommand> queuedCommand = *command;
		std::shared_ptr<AcceleratorToken> token = queuedCommand->Token;
		if (!currentGuardEvidence || !*currentGuardEvidence)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::MissingGuardEvidence, L"Fake MatMul backend tick requires fresh guard evidence before Running transition.", tick, token));
		}

		const std::shared_ptr<AcceleratorGuardEvidence> guardEvidence = *currentGuardEvidence;
		std::unordered_map<unsigned long long, std::shared_ptr<MatMulDescriptor>>::const_iterator _descriptorsByIdentity_iterator = _descriptorsByIdentity.find(queuedCommand->Descriptor->Identity.DescriptorIdentityHash);
		if (_descriptorsByIdentity_iterator == _descriptorsByIdentity.end())
		{
			return RecordResult(FaultToken(token, AcceleratorTokenFaultCode::BackendRejected, L"Fake MatMul backend requires a typed MatMulDescriptor bound by descriptor identity before staged execution.", guardEvidence, tick));
		}
		std::shared_ptr<MatMulDescriptor> matMulDescriptor = _descriptorsByIdentity_iterator->second;

		std::shared_ptr<MatMulDescriptorValidationResult> validation = _validator->Validate(matMulDescriptor, queuedCommand->Descriptor);
		if (validation->IsRejected)
		{
			return RecordResult(FaultToken(token, AcceleratorTokenFaultCode::BackendRejected, validation->Message, guardEvidence, tick));
		}

		std::shared_ptr<AcceleratorTokenTransition> running = token->MarkRunning(guardEvidence);
		if (running->Rejected)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(running->FaultCode, running->Message, tick, token));
		}

		std::shared_ptr<AcceleratorMemoryPortalReadResult> reads = memoryPortal->ReadSourceRanges(token, queuedCommand->Descriptor, currentGuardEvidence);
		if (reads->IsRejected)
		{
			token->MarkFaulted(reads->FaultCode, guardEvidence);
			return RecordResult(AcceleratorBackendResult::Rejected(reads->FaultCode, reads->Message, tick, token));
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (!TryBuildStagedMatMulResultForTest( matMulDescriptor, validation.Footprint!, reads.Reads, out byte[] stagedBytes, out string buildMessage))
		std::vector<unsigned char> stagedBytes;
		std::wstring buildMessage;
		if (!validation->Footprint || !TryBuildStagedMatMulResultForTest(matMulDescriptor, validation->Footprint, reads->Reads, stagedBytes, buildMessage))
		{
			token->MarkFaulted(AcceleratorTokenFaultCode::StagingRejected, guardEvidence);
			return RecordResult(AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::StagingRejected, buildMessage, tick, token));
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorMemoryRange destination = validation.Footprint!.CRange;
		std::shared_ptr<AcceleratorMemoryRange> destination = validation->Footprint->CRange;
		std::shared_ptr<AcceleratorStagingResult> staged = stagingBuffer->StageWrite(token, destination, stagedBytes, currentGuardEvidence);
		if (staged->IsRejected)
		{
			token->MarkFaulted(staged->FaultCode, guardEvidence);
			return RecordResult(AcceleratorBackendResult::Rejected(staged->FaultCode, staged->Message, tick, token));
		}

		std::shared_ptr<AcceleratorTokenTransition> complete = token->MarkDeviceComplete(guardEvidence);
		if (complete->Rejected)
		{
			return RecordResult(AcceleratorBackendResult::Rejected(complete->FaultCode, complete->Message, tick, token));
		}

		return RecordResult(AcceleratorBackendResult::DeviceCompleted(token, reads->BytesRead, static_cast<unsigned long long>(stagedBytes.size()), 1, tick, L"Fake MatMul backend staged descriptor-bound result bytes only; DeviceComplete is not Committed."));
	}

	std::shared_ptr<AcceleratorBackendResult> FakeMatMulExternalAcceleratorBackend::TryCancel(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(tokenStore);
		std::shared_ptr<AcceleratorTokenLookupResult> cancel = tokenStore->Cancel(handle, currentGuardEvidence);
		unsigned long long tick = _clock->Advance();
		return RecordResult(cancel->IsAllowed ? AcceleratorBackendResult::Canceled(cancel, tick) : AcceleratorBackendResult::Rejected(cancel->FaultCode, cancel->Message, tick, cancel->Token, nullptr, cancel));
	}

	std::shared_ptr<AcceleratorBackendResult> FakeMatMulExternalAcceleratorBackend::RecordResult(std::shared_ptr<AcceleratorBackendResult> result)
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
// ORIGINAL LINE: public static bool TryBuildStagedMatMulResultForTest(MatMulDescriptor descriptor, MatMulFootprint footprint, IReadOnlyList<AcceleratorMemoryRead> reads, out byte[] stagedBytes, out string message)
	bool FakeMatMulExternalAcceleratorBackend::TryBuildStagedMatMulResultForTest(std::shared_ptr<MatMulDescriptor> descriptor, std::shared_ptr<MatMulFootprint> footprint, const std::vector<std::shared_ptr<AcceleratorMemoryRead>> &reads, std::vector<unsigned char> &stagedBytes, std::wstring &message)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		ArgumentNullException::ThrowIfNull(footprint);
		stagedBytes.clear();
		if (footprint->CRange->Length > std::numeric_limits<int>::max())
		{
			message = L"Fake MatMul backend cannot stage destination ranges larger than Int32.MaxValue bytes.";
			return false;
		}

		std::span<const unsigned char> aData;
		std::span<const unsigned char> bData;
		if (!TryFindRead(reads, footprint->ARange, aData) || !TryFindRead(reads, footprint->BRange, bData))
		{
			message = L"Fake MatMul backend requires exact A and B source reads from the guarded memory portal.";
			return false;
		}

		if (static_cast<unsigned long long>(aData.size()) != footprint->ARange->Length || static_cast<unsigned long long>(bData.size()) != footprint->BRange->Length)
		{
			message = L"Fake MatMul backend requires exact source bytes for A and B from the guarded memory portal.";
			return false;
		}

		stagedBytes = std::vector<unsigned char>(static_cast<std::size_t>(footprint->CRange->Length));
		try
		{
			switch (descriptor->Datatypes.OutputDatatype)
			{
				case AcceleratorDatatype::Float32:
					ComputeFloat32(descriptor, aData, bData, std::span<unsigned char>(stagedBytes));
					break;
				case AcceleratorDatatype::Float64:
					ComputeFloat64(descriptor, aData, bData, std::span<unsigned char>(stagedBytes));
					break;
				case AcceleratorDatatype::Int32:
					ComputeInt32(descriptor, aData, bData, std::span<unsigned char>(stagedBytes));
					break;
				default:
					message = L"Fake MatMul backend received an unsupported datatype after validation.";
					return false;
			}
		}
		catch (const std::overflow_error &)
		{
			stagedBytes.clear();
			message = L"Fake MatMul backend rejected malformed or overflowing descriptor-backed source data.";
			return false;
		}
		catch (const std::invalid_argument &)
		{
			stagedBytes.clear();
			message = L"Fake MatMul backend rejected malformed or overflowing descriptor-backed source data.";
			return false;
		}
		catch (const std::out_of_range &)
		{
			stagedBytes.clear();
			message = L"Fake MatMul backend rejected malformed or overflowing descriptor-backed source data.";
			return false;
		}

		message = L"Fake MatMul backend generated staged result bytes without legacy custom-accelerator execute.";
		return true;
	}

	std::shared_ptr<AcceleratorBackendResult> FakeMatMulExternalAcceleratorBackend::FaultToken(std::shared_ptr<AcceleratorToken> token, AcceleratorTokenFaultCode faultCode, const std::wstring &message, std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence, unsigned long long tick)
	{
		std::shared_ptr<AcceleratorTokenTransition> fault = token->MarkFaulted(faultCode, currentGuardEvidence);
		return fault->Succeeded ? AcceleratorBackendResult::Faulted(token, faultCode, message, tick) : AcceleratorBackendResult::Rejected(fault->FaultCode, fault->Message, tick, token);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool TryFindRead(IReadOnlyList<AcceleratorMemoryRead> reads, AcceleratorMemoryRange range, out ReadOnlyMemory<byte> data)
	bool FakeMatMulExternalAcceleratorBackend::TryFindRead(const std::vector<std::shared_ptr<AcceleratorMemoryRead>> &reads, std::shared_ptr<AcceleratorMemoryRange> range, std::span<const unsigned char> &data)
	{
		for (int index = 0; index < reads.size(); index++)
		{
				if (!reads[index])
				{
					data = {};
					return false;
				}
				std::shared_ptr<AcceleratorMemoryRead> read = reads[index];
			if (read->Range->Address == range->Address && read->Range->Length == range->Length)
			{
				data = std::span<const unsigned char>(read->Data);
				return true;
			}
		}

		data = {};
		return false;
	}

	void FakeMatMulExternalAcceleratorBackend::ComputeFloat32(std::shared_ptr<MatMulDescriptor> descriptor, std::span<const unsigned char> a, std::span<const unsigned char> b, std::span<unsigned char> c)
	{
		constexpr int size = sizeof(float);
		for (unsigned int row = 0; row < descriptor->M; row++)
		{
			for (unsigned int col = 0; col < descriptor->N; col++)
			{
				float sum = 0;
				for (unsigned int k = 0; k < descriptor->K; k++)
				{
					unsigned long long aOffset = ((static_cast<unsigned long long>(row) * descriptor->Lda) + k) * size;
					unsigned long long bOffset = ((static_cast<unsigned long long>(k) * descriptor->Ldb) + col) * size;
					sum += ReadSingle(a, aOffset) * ReadSingle(b, bOffset);
				}

				unsigned long long cOffset = ((static_cast<unsigned long long>(row) * descriptor->Ldc) + col) * size;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: BitConverter.TryWriteBytes(c.Slice(checked((int)cOffset), size), sum);
				WriteScalar(c, cOffset, sum);
			}
		}
	}

	void FakeMatMulExternalAcceleratorBackend::ComputeFloat64(std::shared_ptr<MatMulDescriptor> descriptor, std::span<const unsigned char> a, std::span<const unsigned char> b, std::span<unsigned char> c)
	{
		constexpr int size = sizeof(double);
		for (unsigned int row = 0; row < descriptor->M; row++)
		{
			for (unsigned int col = 0; col < descriptor->N; col++)
			{
				double sum = 0;
				for (unsigned int k = 0; k < descriptor->K; k++)
				{
					unsigned long long aOffset = ((static_cast<unsigned long long>(row) * descriptor->Lda) + k) * size;
					unsigned long long bOffset = ((static_cast<unsigned long long>(k) * descriptor->Ldb) + col) * size;
					sum += ReadDouble(a, aOffset) * ReadDouble(b, bOffset);
				}

				unsigned long long cOffset = ((static_cast<unsigned long long>(row) * descriptor->Ldc) + col) * size;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: BitConverter.TryWriteBytes(c.Slice(checked((int)cOffset), size), sum);
				WriteScalar(c, cOffset, sum);
			}
		}
	}

	void FakeMatMulExternalAcceleratorBackend::ComputeInt32(std::shared_ptr<MatMulDescriptor> descriptor, std::span<const unsigned char> a, std::span<const unsigned char> b, std::span<unsigned char> c)
	{
		constexpr int size = sizeof(int);
		for (unsigned int row = 0; row < descriptor->M; row++)
		{
			for (unsigned int col = 0; col < descriptor->N; col++)
			{
				int sum = 0;
				for (unsigned int k = 0; k < descriptor->K; k++)
				{
					unsigned long long aOffset = ((static_cast<unsigned long long>(row) * descriptor->Lda) + k) * size;
					unsigned long long bOffset = ((static_cast<unsigned long long>(k) * descriptor->Ldb) + col) * size;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: sum = checked(sum + (ReadInt32(a, aOffset) * ReadInt32(b, bOffset)));
					const long long product = static_cast<long long>(ReadInt32(a, aOffset)) * ReadInt32(b, bOffset);
					const long long accumulated = static_cast<long long>(sum) + product;
					if (accumulated < std::numeric_limits<int>::min() || accumulated > std::numeric_limits<int>::max())
					{
						throw std::overflow_error("MatMul int32 accumulation overflow.");
					}
					sum = static_cast<int>(accumulated);
				}

				unsigned long long cOffset = ((static_cast<unsigned long long>(row) * descriptor->Ldc) + col) * size;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: BitConverter.TryWriteBytes(c.Slice(checked((int)cOffset), size), sum);
				WriteScalar(c, cOffset, sum);
			}
		}
	}

	float FakeMatMulExternalAcceleratorBackend::ReadSingle(std::span<const unsigned char> data, unsigned long long byteOffset)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return BitConverter.ToSingle(data.Slice(checked((int)byteOffset), sizeof(float)));
		return ReadScalar<float>(data, byteOffset);
	}

	double FakeMatMulExternalAcceleratorBackend::ReadDouble(std::span<const unsigned char> data, unsigned long long byteOffset)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return BitConverter.ToDouble(data.Slice(checked((int)byteOffset), sizeof(double)));
		return ReadScalar<double>(data, byteOffset);
	}

	int FakeMatMulExternalAcceleratorBackend::ReadInt32(std::span<const unsigned char> data, unsigned long long byteOffset)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return BitConverter.ToInt32(data.Slice(checked((int)byteOffset), sizeof(int)));
		return ReadScalar<int>(data, byteOffset);
	}
}


