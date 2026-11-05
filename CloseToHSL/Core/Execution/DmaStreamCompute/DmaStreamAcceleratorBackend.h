#pragma once

#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "DmaStreamComputeTelemetry.h"
#include "DmaStreamComputeDescriptor.h"
#include <string>
#include <limits>
#include <format>
#include <memory>
#include <span>

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	struct DmaStreamComputeBackendTelemetry
	{
		bool UsedLane6Backend = 0;
		int ReadBurstCount = 0;
		unsigned long long BytesRead = 0;
		int StagedWriteCount = 0;
		unsigned long long BytesStaged = 0;
		unsigned long long ModeledLatencyCycles = 0;
		unsigned long long ElementOperations = 0;
		unsigned long long CopyOperations = 0;
		unsigned long long AddOperations = 0;
		unsigned long long MulOperations = 0;
		unsigned long long FmaOperations = 0;
		unsigned long long ReduceOperations = 0;
		int AluLaneOccupancyDelta = 0;
		int DirectDestinationWriteCount = 0;
	};

	class DmaStreamAcceleratorBackend final : public std::enable_shared_from_this<DmaStreamAcceleratorBackend>
	{
	private:
		const std::shared_ptr<Processor::MainMemoryArea> _mainMemory;
		int _readBurstCount = 0;
		unsigned long long _bytesRead = 0;
		int _stagedWriteCount = 0;
		unsigned long long _bytesStaged = 0;
		unsigned long long _modeledLatencyCycles = 0;
		unsigned long long _elementOperations = 0;
		unsigned long long _copyOperations = 0;
		unsigned long long _addOperations = 0;
		unsigned long long _mulOperations = 0;
		unsigned long long _fmaOperations = 0;
		unsigned long long _reduceOperations = 0;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private DmaStreamComputeTelemetryCounters? _telemetry;
		std::shared_ptr<DmaStreamComputeTelemetryCounters> _telemetry;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DmaStreamAcceleratorBackend(Processor.MainMemoryArea mainMemory, DmaStreamComputeTelemetryCounters? telemetry = null)
		DmaStreamAcceleratorBackend(std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry = nullptr);

		std::shared_ptr<Processor::MainMemoryArea> getMainMemory() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void AttachTelemetry(DmaStreamComputeTelemetryCounters? telemetry)
		void AttachTelemetry(std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry);

		bool TryReadRange(std::shared_ptr<DmaStreamComputeMemoryRange> range, std::span<unsigned char> buffer, std::wstring &message);

		void RecordComputeElements(unsigned long long elementCount, DmaStreamComputeOperationKind operation);

		void RecordStagedWrite(unsigned long long byteCount);

		std::shared_ptr<DmaStreamComputeBackendTelemetry> SnapshotTelemetry();

	private:
		bool HasExactMainMemoryRange(unsigned long long address, int size);
	};
}
