#include "MemoryCycleController.h"
#include <algorithm>
#include "../Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include "../../Core/Runtime/Memory/Translation/CpuInstructionTranslationContour.h"
#include <limits>
#include "../../Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../Subsystem/CanonicalVectorPhysicalBankEnvelope.h"

using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::CloseToHSL::Memory::Timing
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryCompletion(MemoryRequestId requestId, bool succeeded, byte[] data, string? failureReason, ulong publishedCycle)
	MemoryCompletion::MemoryCompletion(MemoryRequestId requestId, bool succeeded, std::vector<unsigned char>& data, const std::wstring& failureReason, unsigned long long publishedCycle)
		: RequestId(requestId), Succeeded(succeeded), FailureReason(failureReason), PublishedCycle(publishedCycle), _data(data) {}
	MemoryRequestId MemoryCompletion::getRequestId() const
	{
		return RequestId;
	}

	bool MemoryCompletion::getSucceeded() const
	{
		return Succeeded;
	}

	std::wstring MemoryCompletion::getFailureReason() const
	{
		return FailureReason;
	}

	unsigned long long MemoryCompletion::getPublishedCycle() const
	{
		return PublishedCycle;
	}

	std::span<const unsigned char> MemoryCompletion::getData() const
	{
		return _data;
	}

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal MemoryCycleController(MemorySubsystem memorySubsystem)
	MemoryCycleController::MemoryCycleController(std::shared_ptr<MemorySubsystem> memorySubsystem) : _memorySubsystem((memorySubsystem != nullptr) ? memorySubsystem : throw std::invalid_argument("memorySubsystem"))
	{
	}

	unsigned long long MemoryCycleController::getMemoryCycle() const
	{
		{
		std::lock_guard<std::mutex> lock(_gate);
			return _memoryCycle;
		}
	}

	int MemoryCycleController::getOutstandingExplicitPacketScalarLoads() const
	{
		{
		std::lock_guard<std::mutex> lock(_gate);
			return _outstandingExplicitPacketScalarLoads;
		}
	}

	int MemoryCycleController::getOutstandingSingleLaneScalarLoads() const
	{
		{
		std::lock_guard<std::mutex> lock(_gate);
			return _outstandingSingleLaneScalarLoads;
		}
	}

	int MemoryCycleController::getOutstandingVectorSegmentLoads() const
	{
		{
		std::lock_guard<std::mutex> lock(_gate);
			return _outstandingVectorSegmentLoads;
		}
	}

	int MemoryCycleController::getOutstandingCanonicalVectorTransfers() const
	{
		{
		std::lock_guard<std::mutex> lock(_gate);
			return _outstandingCanonicalVectorTransfers;
		}
	}

	int MemoryCycleController::getOutstandingExplicitPacketScalarStores() const
	{
		{
		std::lock_guard<std::mutex> lock(_gate);
			return _outstandingExplicitPacketScalarStores;
		}
	}

	int MemoryCycleController::getOutstandingSingleLaneScalarStores() const
	{
		{
		std::lock_guard<std::mutex> lock(_gate);
			return _outstandingSingleLaneScalarStores;
		}
	}

	std::shared_ptr<MemoryCycleTelemetrySnapshot> MemoryCycleController::GetTelemetrySnapshot()
	{
		{
			std::scoped_lock<std::mutex> lock(_gate);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return new MemoryCycleTelemetrySnapshot(_telemetryControllerCycles, _telemetryReadServiceCycles, _telemetryStoreReadinessServiceCycles, _telemetryCompletionPublicationCycles, _telemetryAcceptedRequests, _telemetryCompletedRequests, _telemetryDataReadAcceptedRequests, _telemetryDataReadCompletedRequests, _telemetryDataWriteAcceptedRequests, _telemetryDataWriteCompletedRequests, _telemetryDataReadBytes, _telemetryCommittedDataWriteBytes, _telemetryInstructionFetchReadBytes, _telemetryQueueFullRejects, _telemetryBaselineOutstandingRequests, _telemetryCanceledRequests, _telemetryConsumedCompletions, checked((ulong)_outstanding.Count));
			return std::make_shared<MemoryCycleTelemetrySnapshot>(_telemetryControllerCycles, _telemetryReadServiceCycles, _telemetryStoreReadinessServiceCycles, _telemetryCompletionPublicationCycles, _telemetryAcceptedRequests, _telemetryCompletedRequests, _telemetryDataReadAcceptedRequests, _telemetryDataReadCompletedRequests, _telemetryDataWriteAcceptedRequests, _telemetryDataWriteCompletedRequests, _telemetryDataReadBytes, _telemetryCommittedDataWriteBytes, _telemetryInstructionFetchReadBytes, _telemetryQueueFullRejects, _telemetryBaselineOutstandingRequests, _telemetryCanceledRequests, _telemetryConsumedCompletions, static_cast<unsigned long long>(_outstanding.size()));
		}
	}

	void MemoryCycleController::ResetTelemetry()
	{
		{
			std::scoped_lock<std::mutex> lock(_gate);
			_telemetryControllerCycles = 0;
			_telemetryReadServiceCycles = 0;
			_telemetryStoreReadinessServiceCycles = 0;
			_telemetryCompletionPublicationCycles = 0;
			_telemetryAcceptedRequests = 0;
			_telemetryCompletedRequests = 0;
			_telemetryDataReadAcceptedRequests = 0;
			_telemetryDataReadCompletedRequests = 0;
			_telemetryDataWriteAcceptedRequests = 0;
			_telemetryDataWriteCompletedRequests = 0;
			_telemetryDataReadBytes = 0;
			_telemetryCommittedDataWriteBytes = 0;
			_telemetryInstructionFetchReadBytes = 0;
			_telemetryQueueFullRejects = 0;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryBaselineOutstandingRequests = checked((ulong)_outstanding.Count);
			_telemetryBaselineOutstandingRequests = static_cast<unsigned long long>(_outstanding.size());
			_telemetryCanceledRequests = 0;
			_telemetryConsumedCompletions = 0;
		}
	}

	void MemoryCycleController::RecordInstructionFetchReadBytes(int byteCount)
	{
		if (byteCount < 0)
		{
			throw ArgumentOutOfRangeException(L"byteCount");
		}

		{
			std::scoped_lock<std::mutex> lock(_gate);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryInstructionFetchReadBytes = checked(_telemetryInstructionFetchReadBytes + (ulong)byteCount);
			_telemetryInstructionFetchReadBytes = _telemetryInstructionFetchReadBytes + static_cast<unsigned long long>(byteCount);
		}
	}

	void MemoryCycleController::RecordCommittedDataWriteBytes(int byteCount)
	{
		if (byteCount < 0)
		{
			throw ArgumentOutOfRangeException(L"byteCount");
		}

		{
			std::scoped_lock<std::mutex> lock(_gate);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryCommittedDataWriteBytes = checked(_telemetryCommittedDataWriteBytes + (ulong)byteCount);
			_telemetryCommittedDataWriteBytes = _telemetryCommittedDataWriteBytes + static_cast<unsigned long long>(byteCount);
		}
	}

	std::shared_ptr<MemoryBankGeometryUpdateResult> MemoryCycleController::TryReplacePhysicalMemoryBankGeometry(int bankCount, int bankWidthBytes)
	{
		{
			std::scoped_lock<std::mutex> lock(_gate);
			bool controllerIsQuiescent = _readQueue.empty() && _scalarStoreQueue.empty() && _outstanding.empty() && _nextCompletions.empty() && _publishedCompletions.empty();

			return _memorySubsystem->TryReplacePhysicalMemoryBankGeometryUnderControllerGate(bankCount, bankWidthBytes, controllerIsQuiescent);
		}
	}

	std::shared_ptr<MemoryAdmissionResult> MemoryCycleController::TryAcceptExplicitPacketScalarLoad(unsigned long long deviceId, unsigned long long address, int size)
	{
		return TryAcceptRead(ReadRequestClass::ExplicitPacketScalar, deviceId, address, size);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryAdmissionResult TryAcceptCpuExplicitPacketScalarLoad(ulong deviceId, ulong address, int size, YAKSys_Hybrid_CPU.Core.Memory.CpuInstructionTranslationOwner.PhysicalReadBinding? binding)
	std::shared_ptr<MemoryAdmissionResult> MemoryCycleController::TryAcceptCpuExplicitPacketScalarLoad(unsigned long long deviceId, unsigned long long address, int size, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Memory::PhysicalReadBinding> binding)
	{
		return TryAcceptRead(ReadRequestClass::ExplicitPacketScalar, deviceId, address, size, binding);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryAdmissionResult TryAcceptCpuSingleLaneScalarLoad(ulong deviceId, ulong address, int size, YAKSys_Hybrid_CPU.Core.Memory.CpuInstructionTranslationOwner.PhysicalReadBinding? binding)
	std::shared_ptr<MemoryAdmissionResult> MemoryCycleController::TryAcceptCpuSingleLaneScalarLoad(unsigned long long deviceId, unsigned long long address, int size, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Memory::PhysicalReadBinding> binding)
	{
		return TryAcceptRead(ReadRequestClass::SingleLaneScalarMicroOp, deviceId, address, size, binding);
	}

	std::shared_ptr<MemoryAdmissionResult> MemoryCycleController::TryAcceptSingleLaneScalarLoad(unsigned long long deviceId, unsigned long long address, int size)
	{
		return TryAcceptRead(ReadRequestClass::SingleLaneScalarMicroOp, deviceId, address, size);
	}

	std::shared_ptr<MemoryAdmissionResult> MemoryCycleController::TryAcceptVectorSegmentLoad(unsigned long long deviceId, unsigned long long address, int size)
	{
		return TryAcceptRead(ReadRequestClass::VectorSegmentMicroOp, deviceId, address, size);
	}

	std::shared_ptr<MemoryAdmissionResult> MemoryCycleController::TryAcceptCanonicalVectorTransfer(unsigned int opcode, unsigned long long deviceId, unsigned long long sourceAddress, unsigned long long destinationAddress, unsigned long long elementCount, int elementSize, unsigned short stride)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if ((opcode != ::YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::VLOAD && opcode != ::YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::VSTORE) || elementCount == 0 || elementSize <= 0 || stride == 0)
		{
			return std::make_shared<MemoryAdmissionResult>(MemoryAdmissionResult::Rejected(L"Canonical vector transfer requires VLOAD/VSTORE and positive element count, element size and stride."));
		}

		const auto maxAddress = std::numeric_limits<unsigned long long>::max();
		const auto unsignedElementSize = static_cast<unsigned long long>(elementSize);
		const auto lastElementIndex = elementCount - 1;
		if (elementCount > maxAddress / unsignedElementSize ||
			lastElementIndex > maxAddress / stride)
		{
			return std::make_shared<MemoryAdmissionResult>(MemoryAdmissionResult::Rejected(L"Canonical vector transfer shape overflows the supported address or byte-count envelope."));
		}
		const auto totalBytes = elementCount * unsignedElementSize;
		const auto lastElementOffset = lastElementIndex * stride;
		if (totalBytes > static_cast<unsigned long long>(std::numeric_limits<int>::max()) ||
			lastElementOffset > maxAddress - unsignedElementSize ||
			sourceAddress > maxAddress - lastElementOffset - unsignedElementSize ||
			destinationAddress > maxAddress - lastElementOffset - unsignedElementSize)
		{
			return std::make_shared<MemoryAdmissionResult>(MemoryAdmissionResult::Rejected(L"Canonical vector transfer shape overflows the supported address or byte-count envelope."));
		}
		const int packedSize = static_cast<int>(totalBytes);

		{
			std::scoped_lock<std::mutex> lock(_gate);
			if (_outstandingCanonicalVectorTransfers >= CanonicalVectorTransferCapacity)
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryQueueFullRejects = checked(_telemetryQueueFullRejects + 1);
				_telemetryQueueFullRejects = _telemetryQueueFullRejects + 1;
				return std::make_shared<MemoryAdmissionResult>(MemoryAdmissionResult::Backpressured());
			}

			CanonicalVectorPhysicalBankEnvelope physicalBankEnvelope = _memorySubsystem->CapturePublishedCanonicalVectorPhysicalBankEnvelopeUnderControllerGate(sourceAddress, elementCount, stride);
			MemoryRequestId requestId = AllocateRequestId();
			_outstanding.emplace(requestId.Value, ControllerRequest::CreateCanonicalVectorTransfer(opcode, deviceId, sourceAddress, destinationAddress, elementCount, elementSize, stride, packedSize, physicalBankEnvelope));
			_outstandingCanonicalVectorTransfers++;
			_readQueue.push_back(requestId.Value);
			RecordAcceptedRequest(_outstanding[requestId.Value]);
			return std::make_shared<MemoryAdmissionResult>(MemoryAdmissionResult::Accepted(requestId));
		}
	}

	std::shared_ptr<MemoryAdmissionResult> MemoryCycleController::TryAcceptExplicitPacketScalarStore(unsigned long long deviceId, unsigned long long address, int size, std::span<const unsigned char> data)
	{
		return TryAcceptScalarStore(ScalarStoreRequestClass::ExplicitPacket, deviceId, address, size, data);
	}

	std::shared_ptr<MemoryAdmissionResult> MemoryCycleController::TryAcceptSingleLaneScalarStore(unsigned long long deviceId, unsigned long long address, int size, std::span<const unsigned char> data)
	{
		return TryAcceptScalarStore(ScalarStoreRequestClass::SingleLaneMicroOp, deviceId, address, size, data);
	}

	std::shared_ptr<MemoryAdmissionResult> MemoryCycleController::TryAcceptScalarStore(ScalarStoreRequestClass requestClass, unsigned long long deviceId, unsigned long long address, int size, std::span<const unsigned char> data)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if ((size != 1 && size != 2 && size != 4 && size != 8))
		{
			return std::make_shared<MemoryAdmissionResult>(MemoryAdmissionResult::Rejected(std::format(L"{0} scalar store size {1} is outside the RF-10 1/2/4/8-byte envelope.", RenderRequestClass(requestClass), size)));
		}
		if (data.size() < size)
		{
			return std::make_shared<MemoryAdmissionResult>(MemoryAdmissionResult::Rejected(std::format(L"{0} scalar store provides {1} byte(s) for a {2}-byte request.", RenderRequestClass(requestClass), data.size(), size)));
		}

		{
			std::scoped_lock<std::mutex> lock(_gate);
			int outstandingForClass = requestClass == ScalarStoreRequestClass::ExplicitPacket ? _outstandingExplicitPacketScalarStores : _outstandingSingleLaneScalarStores;
			int capacity = requestClass == ScalarStoreRequestClass::ExplicitPacket ? ExplicitPacketScalarStoreCapacity : SingleLaneScalarStoreCapacity;
			if (outstandingForClass >= capacity)
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryQueueFullRejects = checked(_telemetryQueueFullRejects + 1);
				_telemetryQueueFullRejects = _telemetryQueueFullRejects + 1;
				return std::make_shared<MemoryAdmissionResult>(MemoryAdmissionResult::Backpressured());
			}

			MemoryRequestId requestId = AllocateRequestId();
			std::shared_ptr<PhysicalMemoryBankBinding> physicalBankBinding = _memorySubsystem->CapturePublishedPhysicalMemoryBankBindingUnderControllerGate(address);
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# range operator:
			_outstanding.emplace(requestId.Value, ControllerRequest::CreateScalarStore(requestClass, deviceId, address, size, std::vector<unsigned char>(data.begin(), data.begin() + size), physicalBankBinding));
			IncrementOutstandingClass(requestClass);
			_scalarStoreQueue.push_back(requestId.Value);
			RecordAcceptedRequest(_outstanding[requestId.Value]);
			return std::make_shared<MemoryAdmissionResult>(MemoryAdmissionResult::Accepted(requestId));
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MemoryAdmissionResult TryAcceptRead(ReadRequestClass requestClass, ulong deviceId, ulong address, int size, YAKSys_Hybrid_CPU.Core.Memory.CpuInstructionTranslationOwner.PhysicalReadBinding? binding = null)
	std::shared_ptr<MemoryAdmissionResult> MemoryCycleController::TryAcceptRead(ReadRequestClass requestClass, unsigned long long deviceId, unsigned long long address, int size, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Memory::PhysicalReadBinding> binding)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (requestClass != ReadRequestClass::VectorSegmentMicroOp && (size != 1 && size != 2 && size != 4 && size != 8))
		{
			return std::make_shared<MemoryAdmissionResult>(MemoryAdmissionResult::Rejected(std::format(L"{0} read size {1} is outside the RF-10 scalar 1/2/4/8-byte envelope.", RenderRequestClass(requestClass), size)));
		}
		if (requestClass == ReadRequestClass::VectorSegmentMicroOp && size <= 0)
		{
			return std::make_shared<MemoryAdmissionResult>(MemoryAdmissionResult::Rejected(std::format(L"{0} read size {1} must be positive.", RenderRequestClass(requestClass), size)));
		}

		{
			std::scoped_lock<std::mutex> lock(_gate);
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//				int outstandingForClass = requestClass switch
	//				{
	//					ReadRequestClass.ExplicitPacketScalar => _outstandingExplicitPacketScalarLoads,
	//					ReadRequestClass.SingleLaneScalarMicroOp => _outstandingSingleLaneScalarLoads,
	//					ReadRequestClass.VectorSegmentMicroOp => _outstandingVectorSegmentLoads,
	//					_ => throw new InvalidOperationException($"Unsupported read request class {requestClass}.")
	//				};
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//				int capacity = requestClass switch
	//				{
	//					ReadRequestClass.ExplicitPacketScalar => ExplicitPacketScalarLoadCapacity,
	//					ReadRequestClass.SingleLaneScalarMicroOp => SingleLaneScalarLoadCapacity,
	//					ReadRequestClass.VectorSegmentMicroOp => VectorSegmentLoadCapacity,
	//					_ => throw new InvalidOperationException($"Unsupported read request class {requestClass}.")
	//				};
			int outstandingForClass = 0;
			int capacity = 0;
			switch (requestClass)
			{
			case ReadRequestClass::ExplicitPacketScalar:
				outstandingForClass = _outstandingExplicitPacketScalarLoads;
				capacity = ExplicitPacketScalarLoadCapacity;
				break;
			case ReadRequestClass::SingleLaneScalarMicroOp:
				outstandingForClass = _outstandingSingleLaneScalarLoads;
				capacity = SingleLaneScalarLoadCapacity;
				break;
			case ReadRequestClass::VectorSegmentMicroOp:
				outstandingForClass = _outstandingVectorSegmentLoads;
				capacity = VectorSegmentLoadCapacity;
				break;
			default:
				throw InvalidOperationException(std::format(L"Unsupported read request class {0}.", static_cast<int>(requestClass)));
			}
			if (outstandingForClass >= capacity)
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryQueueFullRejects = checked(_telemetryQueueFullRejects + 1);
				_telemetryQueueFullRejects = _telemetryQueueFullRejects + 1;
				return std::make_shared<MemoryAdmissionResult>(MemoryAdmissionResult::Backpressured());
			}

			MemoryRequestId requestId = AllocateRequestId();
			std::shared_ptr<PhysicalMemoryBankBinding> physicalBankBinding = _memorySubsystem->CapturePublishedPhysicalMemoryBankBindingUnderControllerGate(address);
			if (binding != nullptr && !binding->TryClaim(shared_from_this(), requestId.Value, address, size))
			{
				return std::make_shared<MemoryAdmissionResult>(MemoryAdmissionResult::Rejected(L"CPU physical read binding is mismatched, duplicate or terminal."));
			}
// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
			_outstanding.emplace(requestId.Value, ControllerRequest::CreateRead(requestClass, deviceId, address, size, physicalBankBinding, binding));
			IncrementOutstandingClass(requestClass);
			_readQueue.push_back(requestId.Value);
			RecordAcceptedRequest(_outstanding[requestId.Value]);
			return std::make_shared<MemoryAdmissionResult>(MemoryAdmissionResult::Accepted(requestId));
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool TryTakeCompletion(MemoryRequestId requestId, out MemoryCompletion? completion)
	bool MemoryCycleController::TryTakeCompletion(MemoryRequestId requestId, std::shared_ptr<MemoryCompletion> &completion)
	{
		std::scoped_lock<std::mutex> lock(_gate);
		const auto completionIt = _publishedCompletions.find(requestId.Value);
		if (completionIt == _publishedCompletions.end())
		{
			completion = nullptr;
			return false;
		}
		completion = completionIt->second;
		_publishedCompletions.erase(completionIt);

		const auto requestIt = _outstanding.find(requestId.Value);
		if (requestIt != _outstanding.end())
		{
			const std::shared_ptr<ControllerRequest> request = requestIt->second;
			_outstanding.erase(requestIt);
			if (request->CpuPhysicalRead != nullptr)
			{
				request->CpuPhysicalRead->Close(shared_from_this(), requestId.Value);
			}
			DecrementOutstandingClass(request);
		}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryConsumedCompletions = checked(_telemetryConsumedCompletions + 1);
		_telemetryConsumedCompletions = _telemetryConsumedCompletions + 1;
		return true;
	}

	bool MemoryCycleController::TryCancel(MemoryRequestId requestId)
	{
		std::scoped_lock<std::mutex> lock(_gate);
		const auto requestIt = _outstanding.find(requestId.Value);
		if (requestIt == _outstanding.end())
		{
			return false;
		}
		const std::shared_ptr<ControllerRequest> request = requestIt->second;
		_outstanding.erase(requestIt);

		if (request->CpuPhysicalRead != nullptr)
		{
			request->CpuPhysicalRead->Close(shared_from_this(), requestId.Value);
		}
		DecrementOutstandingClass(request);
		_nextCompletions.erase(requestId.Value);
		_publishedCompletions.erase(requestId.Value);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryCanceledRequests = checked(_telemetryCanceledRequests + 1);
		_telemetryCanceledRequests = _telemetryCanceledRequests + 1;
		return true;
	}

	bool MemoryCycleController::OwnsOutstandingSingleLaneScalarLoad(MemoryRequestId requestId, unsigned long long deviceId, unsigned long long address, int size)
	{
		std::scoped_lock<std::mutex> lock(_gate);
		const auto it = _outstanding.find(requestId.Value);
		if (it == _outstanding.end()) return false;
		const auto& request = it->second;
		return request->ReadClass == ReadRequestClass::SingleLaneScalarMicroOp && request->DeviceId == deviceId && request->Address == address && request->Size == size;
	}

	bool MemoryCycleController::OwnsOutstandingVectorSegmentLoad(MemoryRequestId requestId, unsigned long long deviceId, unsigned long long address, int size)
	{
		std::scoped_lock<std::mutex> lock(_gate);
		const auto it = _outstanding.find(requestId.Value);
		if (it == _outstanding.end()) return false;
		const auto& request = it->second;
		return request->ReadClass == ReadRequestClass::VectorSegmentMicroOp && request->DeviceId == deviceId && request->Address == address && request->Size == size;
	}

	bool MemoryCycleController::OwnsOutstandingCanonicalVectorTransfer(MemoryRequestId requestId, unsigned int opcode, unsigned long long deviceId, unsigned long long sourceAddress, unsigned long long destinationAddress, unsigned long long elementCount, int elementSize, unsigned short stride)
	{
		std::scoped_lock<std::mutex> lock(_gate);
		const auto it = _outstanding.find(requestId.Value);
		if (it == _outstanding.end()) return false;
		const auto& request = it->second;
		return request->ReadClass == ReadRequestClass::CanonicalVectorTransfer && request->Opcode == opcode && request->DeviceId == deviceId && request->Address == sourceAddress && request->DestinationAddress == destinationAddress && request->ElementCount == elementCount && request->ElementSize == elementSize && request->Stride == stride;
	}

	bool MemoryCycleController::OwnsOutstandingExplicitPacketScalarStore(MemoryRequestId requestId, unsigned long long deviceId, unsigned long long address, int size, std::span<const unsigned char> data)
	{
		std::scoped_lock<std::mutex> lock(_gate);
		const auto outstanding = _outstanding.find(requestId.Value);
		if (outstanding == _outstanding.end()) return false;
		const std::shared_ptr<ControllerRequest>& request = outstanding->second;
		return request->StoreClass == ScalarStoreRequestClass::ExplicitPacket &&
			request->DeviceId == deviceId && request->Address == address && request->Size == size &&
			size >= 0 && data.size() >= static_cast<std::size_t>(size) && request->Data != nullptr &&
			request->Data->size() == static_cast<std::size_t>(size) &&
			std::equal(request->Data->begin(), request->Data->end(), data.begin(), data.begin() + size);
	}

	bool MemoryCycleController::OwnsOutstandingSingleLaneScalarStore(MemoryRequestId requestId, unsigned long long deviceId, unsigned long long address, int size, std::span<const unsigned char> data)
	{
		std::scoped_lock<std::mutex> lock(_gate);
		const auto outstanding = _outstanding.find(requestId.Value);
		if (outstanding == _outstanding.end()) return false;
		const std::shared_ptr<ControllerRequest>& request = outstanding->second;
		return request->StoreClass == ScalarStoreRequestClass::SingleLaneMicroOp &&
			request->DeviceId == deviceId && request->Address == address && request->Size == size &&
			size >= 0 && data.size() >= static_cast<std::size_t>(size) && request->Data != nullptr &&
			request->Data->size() == static_cast<std::size_t>(size) &&
			std::equal(request->Data->begin(), request->Data->end(), data.begin(), data.begin() + size);
	}

	bool MemoryCycleController::AdvancePlatformEdge(unsigned long long platformCycle)
	{
		{
			std::scoped_lock<std::mutex> lock(_gate);
			if (platformCycle <= _lastPlatformCycle)
			{
				return false;
			}

			_lastPlatformCycle = platformCycle;
			TickOneCycle();
			return true;
		}
	}

	void MemoryCycleController::AdvanceCompatibilityCycles(long long cycles)
	{
		if (cycles < 0)
		{
			throw ArgumentOutOfRangeException(L"cycles");
		}

		{
			std::scoped_lock<std::mutex> lock(_gate);
			for (long long cycle = 0; cycle < cycles; cycle++)
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _lastPlatformCycle = checked(_lastPlatformCycle + 1);
				_lastPlatformCycle = _lastPlatformCycle + 1;
				TickOneCycle();
			}
		}
	}

	void MemoryCycleController::TickOneCycle()
	{
		// RF-12.6ac lock order: both callers hold _gate. Geometry-dependent
		// owner calls below acquire the inner MemorySubsystem
		// geometryLifecycleGate and never call back into this controller.
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _memoryCycle = checked(_memoryCycle + 1);
		_memoryCycle = _memoryCycle + 1;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryControllerCycles = checked(_telemetryControllerCycles + 1);
		_telemetryControllerCycles = _telemetryControllerCycles + 1;

		bool publishedCompletionThisCycle = false;
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
		for (const auto& [requestId, completion] : _nextCompletions)
		{
			auto outstanding = _outstanding.find(requestId);
			if (outstanding == _outstanding.end()) continue;
			std::shared_ptr<ControllerRequest> request = outstanding->second;
			const std::span<const unsigned char> completionData = completion->getData();
			std::vector<unsigned char> dataCopy(completionData.begin(), completionData.end());
			_publishedCompletions.emplace(requestId, std::make_shared<MemoryCompletion>(MemoryRequestId(requestId), completion->getSucceeded(), dataCopy, completion->getFailureReason(), _memoryCycle));
			RecordPublishedCompletion(request, completion);
			publishedCompletionThisCycle = true;
		}
		_nextCompletions.clear();
		if (publishedCompletionThisCycle)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryCompletionPublicationCycles = checked(_telemetryCompletionPublicationCycles + 1);
			_telemetryCompletionPublicationCycles = _telemetryCompletionPublicationCycles + 1;
		}

		// All non-migrated queues keep their exact legacy single-cycle service
		// owner. The controller merely owns when that service edge occurs.
		_memorySubsystem->AdvanceLegacyAgentOneCycle();

		// RF-10.13: the explicitly bound persistent DMA agent advances at
		// most once on this controller edge. No caller may recursively drive
		// DMA completion.
		_memorySubsystem->AdvanceBoundDmaAgentOneCycle();

		while (_readQueue.size() > 0)
		{
			const unsigned long long requestId = _readQueue.front();
			_readQueue.pop_front();
			const auto outstanding = _outstanding.find(requestId);
			if (outstanding == _outstanding.end()) continue;
			const std::shared_ptr<ControllerRequest>& request = outstanding->second;
			if (!request->ReadClass.has_value()) continue;

			std::vector<unsigned char> data(request->Size);
			bool succeeded = request->ReadClass == ReadRequestClass::CanonicalVectorTransfer ? _memorySubsystem->ExecuteControllerVectorTransferReadStep(request->DeviceId, request->Address, request->ElementCount, request->ElementSize, request->Stride, request->PhysicalBankEnvelope, data) : _memorySubsystem->ExecuteControllerReadStep(request->DeviceId, request->Address, request->PhysicalBankBinding, data, request->CpuPhysicalRead);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: string? failureReason = succeeded ? null : string.Format("MemoryCycleController {0} read failed at 0x{1:X} for {2} byte(s).", RenderRequestClass(request.ReadRequestClass.Value), request.Address, request.Size);
			std::wstring failureReason = succeeded ? L"" : std::format(L"MemoryCycleController {0} read failed at 0x{1:X} for {2} byte(s).", RenderRequestClass(*request->ReadClass), request->Address, request->Size);
			_nextCompletions.emplace(requestId, std::make_shared<MemoryCompletion>(MemoryRequestId(requestId), succeeded, data, failureReason, 0));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryReadServiceCycles = checked(_telemetryReadServiceCycles + 1);
			_telemetryReadServiceCycles = _telemetryReadServiceCycles + 1;
			break;
		}

		while (_scalarStoreQueue.size() > 0)
		{
			const unsigned long long requestId = _scalarStoreQueue.front();
			_scalarStoreQueue.pop_front();
			const auto outstanding = _outstanding.find(requestId);
			if (outstanding == _outstanding.end()) continue;
			const std::shared_ptr<ControllerRequest>& request = outstanding->second;
			if (!request->StoreClass.has_value()) continue;

			// RF-10.4 completion establishes readiness only. The immutable
			// request snapshot is retained for identity/cancellation proof,
			// but physical publication remains selected-retire-owned.
			std::vector<unsigned char> emptyData;
			_nextCompletions.emplace(requestId, std::make_shared<MemoryCompletion>(MemoryRequestId(requestId), true, emptyData, L"", 0));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryStoreReadinessServiceCycles = checked(_telemetryStoreReadinessServiceCycles + 1);
			_telemetryStoreReadinessServiceCycles = _telemetryStoreReadinessServiceCycles + 1;
			break;
		}
	}

	void MemoryCycleController::RecordAcceptedRequest(std::shared_ptr<ControllerRequest> request)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryAcceptedRequests = checked(_telemetryAcceptedRequests + 1);
		_telemetryAcceptedRequests = _telemetryAcceptedRequests + 1;
		if (request->ReadClass.has_value())
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryDataReadAcceptedRequests = checked(_telemetryDataReadAcceptedRequests + 1);
			_telemetryDataReadAcceptedRequests = _telemetryDataReadAcceptedRequests + 1;
		}
		if (request->StoreClass.has_value() || request->ReadClass == ReadRequestClass::CanonicalVectorTransfer)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryDataWriteAcceptedRequests = checked(_telemetryDataWriteAcceptedRequests + 1);
			_telemetryDataWriteAcceptedRequests = _telemetryDataWriteAcceptedRequests + 1;
		}
	}

	void MemoryCycleController::RecordPublishedCompletion(std::shared_ptr<ControllerRequest> request, std::shared_ptr<MemoryCompletion> completion)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryCompletedRequests = checked(_telemetryCompletedRequests + 1);
		_telemetryCompletedRequests = _telemetryCompletedRequests + 1;
		if (request->ReadClass.has_value())
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryDataReadCompletedRequests = checked(_telemetryDataReadCompletedRequests + 1);
			_telemetryDataReadCompletedRequests = _telemetryDataReadCompletedRequests + 1;
			if (completion->getSucceeded())
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryDataReadBytes = checked(_telemetryDataReadBytes + (ulong)completion.Data.Length);
				_telemetryDataReadBytes = _telemetryDataReadBytes + static_cast<unsigned long long>(completion->getData().size());
			}
		}
		if (request->StoreClass.has_value() || request->ReadClass == ReadRequestClass::CanonicalVectorTransfer)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _telemetryDataWriteCompletedRequests = checked(_telemetryDataWriteCompletedRequests + 1);
			_telemetryDataWriteCompletedRequests = _telemetryDataWriteCompletedRequests + 1;
		}
	}

	MemoryRequestId MemoryCycleController::AllocateRequestId()
	{
		while (true)
		{
			unsigned long long value = _nextRequestId++;
			if (value == 0)
			{
				continue;
			}

			MemoryRequestId requestId(value);
			if (!_outstanding.contains(requestId.Value))
			{
				return requestId;
			}
		}
	}

	void MemoryCycleController::IncrementOutstandingClass(ReadRequestClass requestClass)
	{
		if (requestClass == ReadRequestClass::ExplicitPacketScalar)
		{
			_outstandingExplicitPacketScalarLoads++;
		}
		else if (requestClass == ReadRequestClass::SingleLaneScalarMicroOp)
		{
			_outstandingSingleLaneScalarLoads++;
		}
		else
		{
			_outstandingVectorSegmentLoads++;
		}
	}

	void MemoryCycleController::IncrementOutstandingClass(ScalarStoreRequestClass requestClass)
	{
		if (requestClass == ScalarStoreRequestClass::ExplicitPacket)
		{
			_outstandingExplicitPacketScalarStores++;
		}
		else
		{
			_outstandingSingleLaneScalarStores++;
		}
	}

	void MemoryCycleController::DecrementOutstandingClass(std::shared_ptr<ControllerRequest> request)
	{
		if (request->StoreClass == ScalarStoreRequestClass::ExplicitPacket)
		{
			_outstandingExplicitPacketScalarStores--;
		}
		else if (request->StoreClass == ScalarStoreRequestClass::SingleLaneMicroOp)
		{
			_outstandingSingleLaneScalarStores--;
		}
		else if (request->ReadClass == ReadRequestClass::ExplicitPacketScalar)
		{
			_outstandingExplicitPacketScalarLoads--;
		}
		else if (request->ReadClass == ReadRequestClass::SingleLaneScalarMicroOp)
		{
			_outstandingSingleLaneScalarLoads--;
		}
		else if (request->ReadClass == ReadRequestClass::VectorSegmentMicroOp)
		{
			_outstandingVectorSegmentLoads--;
		}
		else if (request->ReadClass == ReadRequestClass::CanonicalVectorTransfer)
		{
			_outstandingCanonicalVectorTransfers--;
		}
		else
		{
			throw InvalidOperationException(L"Controller request has no recognized request class.");
		}
	}

	std::wstring MemoryCycleController::RenderRequestClass(ReadRequestClass requestClass)
	{
		std::wstring temp_var184;
		switch (requestClass)
		{
			case ReadRequestClass::ExplicitPacketScalar:
				temp_var184 = L"explicit-packet scalar";
				break;
			case ReadRequestClass::SingleLaneScalarMicroOp:
				temp_var184 = L"single-lane scalar";
				break;
			case ReadRequestClass::VectorSegmentMicroOp:
				temp_var184 = L"vector-segment";
				break;
			case ReadRequestClass::CanonicalVectorTransfer:
				temp_var184 = L"canonical vector-transfer";
				break;
			default:
				throw InvalidOperationException(std::format(L"Unsupported read request class {0}.", static_cast<int>(requestClass)));
				break;
		}
		return temp_var184;
	}

	std::wstring MemoryCycleController::RenderRequestClass(ScalarStoreRequestClass requestClass)
	{
		return requestClass == ScalarStoreRequestClass::ExplicitPacket ? L"explicit-packet" : L"single-lane";
	}

    bool MemoryCyclePlatformOrchestrator::AdvanceCoreObservedPlatformEdge(std::shared_ptr<MemoryCycleController> controller, unsigned int coreId, unsigned long long platformCycle) {
        if (!controller) throw std::invalid_argument("controller");
        std::shared_ptr<DomainState> domain;
        {
            std::lock_guard lock(DomainsGate);
            for (auto i = Domains.begin(); i != Domains.end();) { if (i->first.expired()) i = Domains.erase(i); else ++i; }
            std::weak_ptr<MemoryCycleController> key(controller);
            auto it = Domains.find(key);
            if (it == Domains.end()) it = Domains.emplace(key, std::make_shared<DomainState>()).first;
            domain = it->second;
        }
        std::lock_guard lock(domain->Gate);
        auto coreClock = domain->CoreClocks[coreId];
        if (platformCycle == coreClock.LastLocalCycle) return false;
        auto epochOffset = coreClock.EpochOffset;
        const auto maximum = std::numeric_limits<unsigned long long>::max();
        if (platformCycle < coreClock.LastLocalCycle) {
            if (domain->LastNormalizedEdge == maximum) throw std::overflow_error("platform cycle overflow");
            auto next = domain->LastNormalizedEdge + 1;
            if (next < platformCycle) throw std::overflow_error("platform cycle underflow");
            epochOffset = next - platformCycle;
        }
        if (epochOffset > maximum - platformCycle) throw std::overflow_error("platform cycle overflow");
        auto normalizedEdge = epochOffset + platformCycle;
        domain->CoreClocks[coreId] = CoreClockState{epochOffset, platformCycle};
        if (normalizedEdge <= domain->LastNormalizedEdge) return false;
        domain->LastNormalizedEdge = normalizedEdge;
        return controller->AdvancePlatformEdge(normalizedEdge);
    }
}
