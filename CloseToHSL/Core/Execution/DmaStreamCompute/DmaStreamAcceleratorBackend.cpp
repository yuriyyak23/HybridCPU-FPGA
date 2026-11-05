#include "DmaStreamAcceleratorBackend.h"

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DmaStreamAcceleratorBackend(Processor.MainMemoryArea mainMemory, DmaStreamComputeTelemetryCounters? telemetry = null)
	DmaStreamAcceleratorBackend::DmaStreamAcceleratorBackend(std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry) : _mainMemory(mainMemory)
	{
		ArgumentNullException::ThrowIfNull(mainMemory);
		_telemetry = telemetry;
	}

	std::shared_ptr<Processor::MainMemoryArea> DmaStreamAcceleratorBackend::getMainMemory() const
	{
		return _mainMemory;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void AttachTelemetry(DmaStreamComputeTelemetryCounters? telemetry)
	void DmaStreamAcceleratorBackend::AttachTelemetry(std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
	{
		_telemetry = telemetry;
	}

	bool DmaStreamAcceleratorBackend::TryReadRange(std::shared_ptr<DmaStreamComputeMemoryRange> range, std::span<unsigned char> buffer, std::wstring &message)
	{
		message = L"";
		if (static_cast<unsigned long long>(buffer.size()) != range->Length)
		{
			message = std::format(L"DmaStreamCompute lane6 backend read buffer length {0} does not match range length {1}.", buffer.size(), range->Length);
			return false;
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: if (range.Length > int.MaxValue || !HasExactMainMemoryRange(range.Address, checked((int)range.Length)))
		if (range->Length > std::numeric_limits<int>::max() || !HasExactMainMemoryRange(range->Address, static_cast<int>(range->Length)))
		{
			message = std::format(L"DmaStreamCompute lane6 backend rejected out-of-range source read at 0x{0:X} for {1} byte(s).", range->Address, range->Length);
			return false;
		}

		if (!_mainMemory->TryReadPhysicalRange(range->Address, buffer))
		{
			message = std::format(L"DmaStreamCompute lane6 backend could not read source range at 0x{0:X} for {1} byte(s).", range->Address, range->Length);
			return false;
		}

		_readBurstCount++;
		_bytesRead += range->Length;
		_modeledLatencyCycles += 4 + ((range->Length + 63) / 64);
		if (_telemetry != nullptr)
		{
			_telemetry->RecordRuntimeRead(range->Length);
		}
		return true;
	}

	void DmaStreamAcceleratorBackend::RecordComputeElements(unsigned long long elementCount, DmaStreamComputeOperationKind operation)
	{
		unsigned long long effectiveElementCount = elementCount == 0 ? 1ULL : elementCount;
		_elementOperations += effectiveElementCount;
		_modeledLatencyCycles += effectiveElementCount;
		switch (operation)
		{
			case DmaStreamComputeOperationKind::Copy:
				_copyOperations += effectiveElementCount;
				break;
			case DmaStreamComputeOperationKind::Add:
				_addOperations += effectiveElementCount;
				break;
			case DmaStreamComputeOperationKind::Mul:
				_mulOperations += effectiveElementCount;
				break;
			case DmaStreamComputeOperationKind::Fma:
				_fmaOperations += effectiveElementCount;
				break;
			case DmaStreamComputeOperationKind::Reduce:
				_reduceOperations += effectiveElementCount;
				break;
		}

		if (_telemetry != nullptr)
		{
			_telemetry->RecordElementOperations(operation, effectiveElementCount);
		}
	}

	void DmaStreamAcceleratorBackend::RecordStagedWrite(unsigned long long byteCount)
	{
		_stagedWriteCount++;
		_bytesStaged += byteCount;
		_modeledLatencyCycles += 1 + ((byteCount + 63) / 64);
	}

	std::shared_ptr<DmaStreamComputeBackendTelemetry> DmaStreamAcceleratorBackend::SnapshotTelemetry()
	{
		return std::make_shared<DmaStreamComputeBackendTelemetry>(DmaStreamComputeBackendTelemetry{true, _readBurstCount, _bytesRead, _stagedWriteCount, _bytesStaged, _modeledLatencyCycles, _elementOperations, _copyOperations, _addOperations, _mulOperations, _fmaOperations, _reduceOperations, 0, 0});
	}

	bool DmaStreamAcceleratorBackend::HasExactMainMemoryRange(unsigned long long address, int size)
	{
		if (size <= 0)
		{
			return false;
		}

		unsigned long long memoryLength = static_cast<unsigned long long>(_mainMemory->getLength());
		unsigned long long requestSize = static_cast<unsigned long long>(size);
		return requestSize <= memoryLength && address <= memoryLength - requestSize;
	}
}
