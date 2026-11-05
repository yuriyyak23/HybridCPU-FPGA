#pragma once
namespace YAKSys_Hybrid_CPU::Core::Memory { class PhysicalReadBinding; }

#include "MemoryAdmissionTypes.h"
#include "MemoryRequestId.h"
#include "MemoryCycleTelemetrySnapshot.h"
#include "../Subsystem/CanonicalVectorPhysicalBankEnvelope.h"



#include <string>
#include <unordered_map>
#include <vector>
#include <span>
#include <deque>
#include <stdexcept>
#include <any>
#include <mutex>
#include <map>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include <optional>

namespace YAKSys_Hybrid_CPU::Memory { class PhysicalMemoryBankBinding; }

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace HybridCPU_ISE::CloseToHSL::Memory::Timing { class DomainState; }
namespace YAKSys_Hybrid_CPU::Memory { class MemorySubsystem; struct MemoryBankGeometryUpdateResult; }

using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::CloseToHSL::Memory::Timing
{

	/// <summary>
	/// Identity of one request accepted by <see cref="MemoryCycleController"/>.
	/// This identity is local to the memory controller and is not an ISA, replay,
	/// FSP, DMA-channel, or retirement identity.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MemoryRequestId(ulong Value)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return Value != 0;
//			}
//		}
//	}

	

	/// <summary>
	/// Bounded admission result. Only <see cref="MemoryAdmissionStatus.Accepted"/>
	/// carries a request identity.
	/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public readonly record struct MemoryAdmissionResult(MemoryAdmissionStatus Status, MemoryRequestId RequestId, string? Reason)
//	public readonly record struct MemoryAdmissionResult(MemoryAdmissionStatus Status, MemoryRequestId RequestId, string Reason)
//	{
//		public static MemoryAdmissionResult Accepted(MemoryRequestId requestId)
//		{
//			return new(MemoryAdmissionStatus.Accepted, requestId, nullptr);
//		}
//
//		public static MemoryAdmissionResult Backpressured()
//		{
//			return new(MemoryAdmissionStatus.Backpressured, default, nullptr);
//		}
//
//		public static MemoryAdmissionResult Rejected(string reason)
//		{
//			return new(MemoryAdmissionStatus.Rejected, default, reason);
//		}
//	}

	/// <summary>
	/// Immutable consumer-facing completion for one accepted request.
	/// </summary>
	class MemoryCompletion final : public std::enable_shared_from_this<MemoryCompletion>
	{
	private:
		const MemoryRequestId RequestId;
		const bool Succeeded;
		const std::wstring FailureReason;
		const unsigned long long PublishedCycle;

		const std::vector<unsigned char> _data;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryCompletion(MemoryRequestId requestId, bool succeeded, byte[] data, string? failureReason, ulong publishedCycle)
		MemoryCompletion(MemoryRequestId requestId, bool succeeded, std::vector<unsigned char> &data, const std::wstring &failureReason, unsigned long long publishedCycle);

		MemoryRequestId getRequestId() const;
		bool getSucceeded() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public string? FailureReason {get;}
		std::wstring getFailureReason() const;
		unsigned long long getPublishedCycle() const;
		std::span<const unsigned char> getData() const;
	};

	/// <summary>
	/// RF-10 memory-cycle authority. The implemented ingress contains the two
	/// RF-10.2/10.3 scalar-load classes, the RF-10.4/10.5 scalar-store readiness
	/// classes, the RF-10.6 vector-segment read class and the RF-10.10 canonical
	/// vector-transfer source-read class; every other request family remains on
	/// its legacy owner.
	/// </summary>
	class MemoryCycleController final : public std::enable_shared_from_this<MemoryCycleController>
	{
	public:
		static constexpr int ExplicitPacketScalarLoadCapacity = 8;
		static constexpr int SingleLaneScalarLoadCapacity = 8;
		static constexpr int VectorSegmentLoadCapacity = 8;
		static constexpr int CanonicalVectorTransferCapacity = 8;
		static constexpr int ExplicitPacketScalarStoreCapacity = 8;
		static constexpr int SingleLaneScalarStoreCapacity = 8;

	private:
		enum class ReadRequestClass : unsigned char;
		enum class ScalarStoreRequestClass : unsigned char;
		struct ControllerRequest;
		mutable std::mutex _gate;
		const std::shared_ptr<MemorySubsystem> _memorySubsystem;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Queue<MemoryRequestId> _readQueue = new();
		std::deque<unsigned long long> _readQueue;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Queue<MemoryRequestId> _scalarStoreQueue = new();
		std::deque<unsigned long long> _scalarStoreQueue;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<MemoryRequestId, ControllerRequest> _outstanding = new();
		std::unordered_map<unsigned long long, std::shared_ptr<ControllerRequest>> _outstanding;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<MemoryRequestId, MemoryCompletion> _nextCompletions = new();
		std::unordered_map<unsigned long long, std::shared_ptr<MemoryCompletion>> _nextCompletions;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<MemoryRequestId, MemoryCompletion> _publishedCompletions = new();
		std::unordered_map<unsigned long long, std::shared_ptr<MemoryCompletion>> _publishedCompletions;
		unsigned long long _nextRequestId = 1;
		unsigned long long _memoryCycle = 0;
		unsigned long long _lastPlatformCycle = 0;
		int _outstandingExplicitPacketScalarLoads = 0;
		int _outstandingSingleLaneScalarLoads = 0;
		int _outstandingVectorSegmentLoads = 0;
		int _outstandingCanonicalVectorTransfers = 0;
		int _outstandingExplicitPacketScalarStores = 0;
		int _outstandingSingleLaneScalarStores = 0;
		unsigned long long _telemetryControllerCycles = 0;
		unsigned long long _telemetryReadServiceCycles = 0;
		unsigned long long _telemetryStoreReadinessServiceCycles = 0;
		unsigned long long _telemetryCompletionPublicationCycles = 0;
		unsigned long long _telemetryAcceptedRequests = 0;
		unsigned long long _telemetryCompletedRequests = 0;
		unsigned long long _telemetryDataReadAcceptedRequests = 0;
		unsigned long long _telemetryDataReadCompletedRequests = 0;
		unsigned long long _telemetryDataWriteAcceptedRequests = 0;
		unsigned long long _telemetryDataWriteCompletedRequests = 0;
		unsigned long long _telemetryDataReadBytes = 0;
		unsigned long long _telemetryCommittedDataWriteBytes = 0;
		unsigned long long _telemetryInstructionFetchReadBytes = 0;
		unsigned long long _telemetryQueueFullRejects = 0;
		unsigned long long _telemetryBaselineOutstandingRequests = 0;
		unsigned long long _telemetryCanceledRequests = 0;
		unsigned long long _telemetryConsumedCompletions = 0;

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal MemoryCycleController(MemorySubsystem memorySubsystem)
		MemoryCycleController(std::shared_ptr<MemorySubsystem> memorySubsystem);

		unsigned long long getMemoryCycle() const;

		int getOutstandingExplicitPacketScalarLoads() const;

		int getOutstandingSingleLaneScalarLoads() const;

		int getOutstandingVectorSegmentLoads() const;

		int getOutstandingCanonicalVectorTransfers() const;

		int getOutstandingExplicitPacketScalarStores() const;

		int getOutstandingSingleLaneScalarStores() const;

		std::shared_ptr<MemoryCycleTelemetrySnapshot> GetTelemetrySnapshot();

		void ResetTelemetry();

		void RecordInstructionFetchReadBytes(int byteCount);

		void RecordCommittedDataWriteBytes(int byteCount);

		std::shared_ptr<MemoryBankGeometryUpdateResult> TryReplacePhysicalMemoryBankGeometry(int bankCount, int bankWidthBytes);

		std::shared_ptr<MemoryAdmissionResult> TryAcceptExplicitPacketScalarLoad(unsigned long long deviceId, unsigned long long address, int size);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryAdmissionResult TryAcceptCpuExplicitPacketScalarLoad(ulong deviceId, ulong address, int size, YAKSys_Hybrid_CPU.Core.Memory.CpuInstructionTranslationOwner.PhysicalReadBinding? binding)
		std::shared_ptr<MemoryAdmissionResult> TryAcceptCpuExplicitPacketScalarLoad(unsigned long long deviceId, unsigned long long address, int size, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Memory::PhysicalReadBinding> binding);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryAdmissionResult TryAcceptCpuSingleLaneScalarLoad(ulong deviceId, ulong address, int size, YAKSys_Hybrid_CPU.Core.Memory.CpuInstructionTranslationOwner.PhysicalReadBinding? binding)
		std::shared_ptr<MemoryAdmissionResult> TryAcceptCpuSingleLaneScalarLoad(unsigned long long deviceId, unsigned long long address, int size, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Memory::PhysicalReadBinding> binding);

		std::shared_ptr<MemoryAdmissionResult> TryAcceptSingleLaneScalarLoad(unsigned long long deviceId, unsigned long long address, int size);

		std::shared_ptr<MemoryAdmissionResult> TryAcceptVectorSegmentLoad(unsigned long long deviceId, unsigned long long address, int size);

		std::shared_ptr<MemoryAdmissionResult> TryAcceptCanonicalVectorTransfer(unsigned int opcode, unsigned long long deviceId, unsigned long long sourceAddress, unsigned long long destinationAddress, unsigned long long elementCount, int elementSize, unsigned short stride);

		std::shared_ptr<MemoryAdmissionResult> TryAcceptExplicitPacketScalarStore(unsigned long long deviceId, unsigned long long address, int size, std::span<const unsigned char> data);

		std::shared_ptr<MemoryAdmissionResult> TryAcceptSingleLaneScalarStore(unsigned long long deviceId, unsigned long long address, int size, std::span<const unsigned char> data);

	private:
		std::shared_ptr<MemoryAdmissionResult> TryAcceptScalarStore(ScalarStoreRequestClass requestClass, unsigned long long deviceId, unsigned long long address, int size, std::span<const unsigned char> data);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MemoryAdmissionResult TryAcceptRead(ReadRequestClass requestClass, ulong deviceId, ulong address, int size, YAKSys_Hybrid_CPU.Core.Memory.CpuInstructionTranslationOwner.PhysicalReadBinding? binding = null)
		std::shared_ptr<MemoryAdmissionResult> TryAcceptRead(ReadRequestClass requestClass, unsigned long long deviceId, unsigned long long address, int size, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Memory::PhysicalReadBinding> binding = nullptr);

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool TryTakeCompletion(MemoryRequestId requestId, out MemoryCompletion? completion)
		bool TryTakeCompletion(MemoryRequestId requestId, std::shared_ptr<MemoryCompletion> &completion);
		bool TryTakeCompletion(const std::shared_ptr<MemoryRequestId>& requestId, std::shared_ptr<MemoryCompletion> &completion)
		{
			if (!requestId)
			{
				completion = nullptr;
				return false;
			}
			return TryTakeCompletion(*requestId, completion);
		}

		/// <summary>
		/// Terminal cancellation for a squashed consumer. A canceled identity no
		/// longer reserves capacity and can never publish a completion.
		/// </summary>
		bool TryCancel(MemoryRequestId requestId);
		bool TryCancel(const std::shared_ptr<MemoryRequestId>& requestId)
		{
			return requestId && TryCancel(*requestId);
		}

		bool OwnsOutstandingSingleLaneScalarLoad(MemoryRequestId requestId, unsigned long long deviceId, unsigned long long address, int size);
		bool OwnsOutstandingSingleLaneScalarLoad(const std::shared_ptr<MemoryRequestId>& requestId, unsigned long long deviceId, unsigned long long address, int size)
		{
			return requestId && OwnsOutstandingSingleLaneScalarLoad(*requestId, deviceId, address, size);
		}

		bool OwnsOutstandingVectorSegmentLoad(MemoryRequestId requestId, unsigned long long deviceId, unsigned long long address, int size);
		bool OwnsOutstandingVectorSegmentLoad(const std::shared_ptr<MemoryRequestId>& requestId, unsigned long long deviceId, unsigned long long address, int size)
		{
			return requestId && OwnsOutstandingVectorSegmentLoad(*requestId, deviceId, address, size);
		}

		bool OwnsOutstandingCanonicalVectorTransfer(MemoryRequestId requestId, unsigned int opcode, unsigned long long deviceId, unsigned long long sourceAddress, unsigned long long destinationAddress, unsigned long long elementCount, int elementSize, unsigned short stride);
		bool OwnsOutstandingCanonicalVectorTransfer(const std::shared_ptr<MemoryRequestId>& requestId, unsigned int opcode, unsigned long long deviceId, unsigned long long sourceAddress, unsigned long long destinationAddress, unsigned long long elementCount, int elementSize, unsigned short stride)
		{
			return requestId && OwnsOutstandingCanonicalVectorTransfer(*requestId, opcode, deviceId, sourceAddress, destinationAddress, elementCount, elementSize, stride);
		}

		bool OwnsOutstandingExplicitPacketScalarStore(MemoryRequestId requestId, unsigned long long deviceId, unsigned long long address, int size, std::span<const unsigned char> data);
		bool OwnsOutstandingExplicitPacketScalarStore(const std::shared_ptr<MemoryRequestId>& requestId, unsigned long long deviceId, unsigned long long address, int size, std::span<const unsigned char> data)
		{
			return requestId && OwnsOutstandingExplicitPacketScalarStore(*requestId, deviceId, address, size, data);
		}

		bool OwnsOutstandingSingleLaneScalarStore(MemoryRequestId requestId, unsigned long long deviceId, unsigned long long address, int size, std::span<const unsigned char> data);
		bool OwnsOutstandingSingleLaneScalarStore(const std::shared_ptr<MemoryRequestId>& requestId, unsigned long long deviceId, unsigned long long address, int size, std::span<const unsigned char> data)
		{
			return requestId && OwnsOutstandingSingleLaneScalarStore(*requestId, deviceId, address, size, data);
		}

		bool AdvancePlatformEdge(unsigned long long platformCycle);

		void AdvanceCompatibilityCycles(long long cycles);

	private:
		void TickOneCycle();

		void RecordAcceptedRequest(std::shared_ptr<ControllerRequest> request);

		void RecordPublishedCompletion(std::shared_ptr<ControllerRequest> request, std::shared_ptr<MemoryCompletion> completion);

		MemoryRequestId AllocateRequestId();

		void IncrementOutstandingClass(ReadRequestClass requestClass);

		void IncrementOutstandingClass(ScalarStoreRequestClass requestClass);

		void DecrementOutstandingClass(std::shared_ptr<ControllerRequest> request);

		static std::wstring RenderRequestClass(ReadRequestClass requestClass);

		static std::wstring RenderRequestClass(ScalarStoreRequestClass requestClass);

	private:
		enum class ReadRequestClass : unsigned char
		{
			ExplicitPacketScalar,
			SingleLaneScalarMicroOp,
			VectorSegmentMicroOp,
			CanonicalVectorTransfer
		};

	private:
		enum class ScalarStoreRequestClass : unsigned char
		{
			ExplicitPacket,
			SingleLaneMicroOp
		};

		struct ControllerRequest final
		{
			std::optional<ReadRequestClass> ReadClass;
			std::optional<ScalarStoreRequestClass> StoreClass;
			unsigned long long DeviceId = 0;
			unsigned long long Address = 0;
			int Size = 0;
			std::shared_ptr<std::vector<unsigned char>> Data;
			std::shared_ptr<PhysicalMemoryBankBinding> PhysicalBankBinding;
			unsigned int Opcode = 0;
			unsigned long long DestinationAddress = 0;
			unsigned long long ElementCount = 0;
			int ElementSize = 0;
			unsigned short Stride = 0;
			CanonicalVectorPhysicalBankEnvelope PhysicalBankEnvelope;
			std::shared_ptr<YAKSys_Hybrid_CPU::Core::Memory::PhysicalReadBinding> CpuPhysicalRead;

			static std::shared_ptr<ControllerRequest> CreateRead(ReadRequestClass requestClass, unsigned long long deviceId, unsigned long long address, int size, std::shared_ptr<PhysicalMemoryBankBinding> physicalBankBinding, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Memory::PhysicalReadBinding> cpuPhysicalRead)
			{
				auto request = std::make_shared<ControllerRequest>();
				request->ReadClass = requestClass;
				request->DeviceId = deviceId;
				request->Address = address;
				request->Size = size;
				request->Data = std::make_shared<std::vector<unsigned char>>();
				request->PhysicalBankBinding = std::move(physicalBankBinding);
				request->CpuPhysicalRead = std::move(cpuPhysicalRead);
				return request;
			}

			static std::shared_ptr<ControllerRequest> CreateCanonicalVectorTransfer(unsigned int opcode, unsigned long long deviceId, unsigned long long sourceAddress, unsigned long long destinationAddress, unsigned long long elementCount, int elementSize, unsigned short stride, int packedSize, CanonicalVectorPhysicalBankEnvelope physicalBankEnvelope)
			{
				auto request = std::make_shared<ControllerRequest>();
				request->ReadClass = ReadRequestClass::CanonicalVectorTransfer;
				request->DeviceId = deviceId;
				request->Address = sourceAddress;
				request->Size = packedSize;
				request->Data = std::make_shared<std::vector<unsigned char>>();
				request->Opcode = opcode;
				request->DestinationAddress = destinationAddress;
				request->ElementCount = elementCount;
				request->ElementSize = elementSize;
				request->Stride = stride;
				request->PhysicalBankEnvelope = std::move(physicalBankEnvelope);
				return request;
			}

			static std::shared_ptr<ControllerRequest> CreateScalarStore(ScalarStoreRequestClass requestClass, unsigned long long deviceId, unsigned long long address, int size, std::vector<unsigned char> data, std::shared_ptr<PhysicalMemoryBankBinding> physicalBankBinding)
			{
				auto request = std::make_shared<ControllerRequest>();
				request->StoreClass = requestClass;
				request->DeviceId = deviceId;
				request->Address = address;
				request->Size = size;
				request->Data = std::make_shared<std::vector<unsigned char>>(std::move(data));
				request->PhysicalBankBinding = std::move(physicalBankBinding);
				return request;
			}
		};
	};

	/// <summary>
	/// Compatibility platform-edge adapter for existing per-core drivers. Cores
	/// sharing a controller report the same platform-cycle number; only the first
	/// observation supplies the controller edge.
	/// </summary>
    class MemoryCyclePlatformOrchestrator final {
        struct CoreClockState { unsigned long long EpochOffset = 0, LastLocalCycle = 0; };
        struct DomainState { std::mutex Gate; std::unordered_map<unsigned int, CoreClockState> CoreClocks; unsigned long long LastNormalizedEdge = 0; };
        static inline std::mutex DomainsGate;
        static inline std::map<std::weak_ptr<MemoryCycleController>, std::shared_ptr<DomainState>, std::owner_less<std::weak_ptr<MemoryCycleController>>> Domains;
    public:
        static bool AdvanceCoreObservedPlatformEdge(std::shared_ptr<MemoryCycleController> controller, unsigned int coreId, unsigned long long platformCycle);
    };
}
