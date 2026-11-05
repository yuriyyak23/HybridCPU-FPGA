#pragma once

#include "../../Core/Execution/BurstIO/IBurstBackend.h"
#include "../../Core/Memory/IHardwareOccupancyInput.h"
#include "MemorySubsystem.Types.h"
#include "MemoryRequestTypes.h"
#include "../Registers/YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.h"
#include "../Cache/GlobalDistributedCache.h"
#include "../Timing/MemoryCycleController.h"
#include "../../Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
namespace HybridCPU_ISE::CloseToHSL::Memory::DMA { class DMAController; }
#include "CanonicalVectorPhysicalBankEnvelope.h"
#include "../../Core/Pipeline/Certificates/SafetyMask128.h"
#include "../../Core/Execution/StreamEngine/BurstIO/TransactionReorderBuffer.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <span>
#include <deque>
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <mutex>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../../tangible_event.h"
#include "../../../tangible_string_helper.h"

// C# TO C++ CONVERTER NOTE: Forward class declarations:
using namespace HybridCPU_ISE::CloseToHSL::Memory::Cache;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Execution;
namespace YAKSys_Hybrid_CPU::Memory { class PhysicalMemoryBankGeometry; }
using namespace HybridCPU_ISE::CloseToHSL::Memory::Cache;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Execution;



namespace YAKSys_Hybrid_CPU::Memory
{
class MemorySubsystem : public IBurstBackend, public IHardwareOccupancyInput, public std::enable_shared_from_this<MemorySubsystem>
{
public:
		class BankRequest
		{
		public:
			unsigned long long RequestID = 0;
			unsigned long long DeviceID = 0;
			unsigned long long Address = 0;
			int Length = 0;
			bool IsRead = false;
			bool DefersPhysicalWriteUntilRetire = false;
			int Priority = 0;
			long long EnqueueCycle = 0;
			std::vector<unsigned char> Buffer; // For deferred execution
			std::shared_ptr<PhysicalMemoryBankBinding> PhysicalBankBinding;
		};

	private:
		int DmaThresholdBytes = 1024;
		int AxiBoundary = 4096;
		int MaxBurstLength = 256;
		int ReadWriteTurnaroundPenalty = 1;
		double BankBandwidthGBps = 25.6;
		BankArbitrationPolicy ArbitrationPolicy = BankArbitrationPolicy::RoundRobin;
		int PortSwitchingPenalty = 3;
		int RowBufferHitLatency = 10;
		int RowBufferMissLatency = 30;
		int RowBufferSize = 1024;
		std::shared_ptr<StreamRegisterFile> StreamRegisters;
		std::shared_ptr<GlobalDistributedCache> L3Cache;
		long long TotalBursts = 0;
		long long TotalBytesTransferred = 0;
		long long BankConflicts = 0;
		long long StallCycles = 0;
		long long DmaTransfers = 0;
		long long TotalWaitCycles = 0;
		int MaxQueueDepth = 0;
		mutable std::mutex cycleControllerInitializationGate;
		mutable std::shared_ptr<MemoryCycleController> CycleController;
		long long UnalignedAccessCount = 0;
		long long TotalAlignmentPenalty = 0;
		double BurstEfficiency = 0.0;
		long long TotalBurstTimingCycles = 0;
		int MaxInflightBurstsPerDomain = 8;
		long long DomainBurstStalls = 0;

		static constexpr int HardwareQueueSaturationThreshold = 4;
		static constexpr unsigned char LiveWidenedMemoryIssueWidth = 2;

//		#region Configuration Properties

		// RF-12.6ac: sole owner-local serialization boundary for the split
		// physical-bank geometry and every positional state derived from it.
		// Controller-owned paths may enter this gate only while already
		// holding MemoryCycleController._gate.
		mutable std::mutex geometryLifecycleGate;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly Processor.MainMemoryArea? boundMainMemory;
		const std::shared_ptr<Processor::MainMemoryArea> boundMainMemory;

		/// <summary>Construction selection only; this does not certify mapping or reset lifetime.</summary>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static MemorySubsystem CreateForMainMemory(ref Processor proc, Processor.MainMemoryArea mainMemory, DMAController? dmaController = null)
		static std::shared_ptr<MemorySubsystem> CreateForMainMemory(Processor &proc, std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<DMAController> dmaController = nullptr);

		bool getHasExplicitMainMemoryBinding() const;
		bool IsBoundToMainMemory(std::shared_ptr<Processor::MainMemoryArea> memory);

	private:
		int _numBanks = 8;
		unsigned long long _lastIssuedPhysicalBankGeometryGeneration = 0;
		std::shared_ptr<PhysicalMemoryBankGeometry> _publishedPhysicalBankGeometry;

		/// <summary>
		/// Number of memory banks (default: 8).
		/// Resizes the live bank-state arrays so configured bank geometry
		/// cannot drift from the executing memory subsystem.
		/// </summary>
	public:
		int getNumBanks() const;
		void setNumBanks(int value);

	private:
		int _bankWidthBytes = 64;

		/// <summary>
		/// Width of each memory bank in bytes (default: 64)
		/// </summary>
	public:
		int getBankWidthBytes() const;
		void setBankWidthBytes(int value);

		/// <summary>
		/// Current authoritative physical-bank geometry snapshot. Compatibility
		/// setters do not publish or advance this snapshot.
		/// </summary>
		std::shared_ptr<PhysicalMemoryBankGeometry> getPublishedPhysicalBankGeometry() const;

		/// <summary>
		/// Attempts one atomic authoritative replacement of the physical-bank
		/// geometry. The controller coordinates its request stores first; the
		/// memory owner then selects the exact lifecycle result.
		/// </summary>
		std::shared_ptr<MemoryBankGeometryUpdateResult> TryReplacePhysicalMemoryBankGeometry(int bankCount, int bankWidthBytes);

		std::shared_ptr<PhysicalMemoryBankBinding> CapturePublishedPhysicalMemoryBankBindingUnderControllerGate(unsigned long long address);

		CanonicalVectorPhysicalBankEnvelope CapturePublishedCanonicalVectorPhysicalBankEnvelopeUnderControllerGate(unsigned long long sourceAddress, unsigned long long elementCount, unsigned short stride);

		/// <summary>
		/// Retained compatibility threshold setting. RF-10.13 removes hidden
		/// size-based DMA routing from synchronous memory APIs.
		/// </summary>
		int getDmaThresholdBytes() const;
		void setDmaThresholdBytes(int value);

		/// <summary>
		/// AXI4 boundary alignment requirement (default: 4096 = 4KB)
		/// </summary>
		int getAxiBoundary() const;
		void setAxiBoundary(int value);

		/// <summary>
		/// Maximum burst length in beats (default: 256 for AXI4)
		/// </summary>
		int getMaxBurstLength() const;
		void setMaxBurstLength(int value);

	private:
		int _numMemoryPorts = 2;

		/// <summary>
		/// Number of memory ports (default: 2).
		/// Resizes the sampled port state so directional occupancy truth
		/// follows the configured runtime surface.
		/// </summary>
	public:
		int getNumMemoryPorts() const;
		void setNumMemoryPorts(int value);

		/// <summary>
		/// Additional turnaround delay in cycles when a memory port flips
		/// between read traffic and write traffic.
		/// This feeds the sampled directional occupancy truth used by the
		/// live widened lane4..5 LSU subset.
		/// </summary>
		int getReadWriteTurnaroundPenalty() const;
		void setReadWriteTurnaroundPenalty(int value);

		/// <summary>
		/// Memory bandwidth in GB/s per bank (default: 25.6)
		/// Used for adaptive stall calculation
		/// </summary>
		double getBankBandwidthGBps() const;
		void setBankBandwidthGBps(double value);

		/// <summary>
		/// Bank arbitration policy (default: RoundRobin)
		/// </summary>
		BankArbitrationPolicy getArbitrationPolicy() const;
		void setArbitrationPolicy(BankArbitrationPolicy value);

		/// <summary>
		/// Port switching penalty in cycles (Phase 2: Requirement 2.2)
		/// Default: 3 cycles
		/// </summary>
		int getPortSwitchingPenalty() const;
		void setPortSwitchingPenalty(int value);

		/// <summary>
		/// Row buffer hit latency in cycles (Phase 2: Requirement 2.2)
		/// Default: 10 cycles
		/// </summary>
		int getRowBufferHitLatency() const;
		void setRowBufferHitLatency(int value);

		/// <summary>
		/// Row buffer miss latency in cycles (Phase 2: Requirement 2.2)
		/// Default: 30 cycles
		/// </summary>
		int getRowBufferMissLatency() const;
		void setRowBufferMissLatency(int value);

		/// <summary>
		/// Row buffer size in bytes (Phase 2: Requirement 2.2)
		/// Default: 1024 bytes (1KB)
		/// </summary>
		int getRowBufferSize() const;
		void setRowBufferSize(int value);

		/// <summary>
		/// Stream Register File for hardware prefetching
		/// </summary>
		std::shared_ptr<StreamRegisterFile> getStreamRegisters() const;
	private:
		void setStreamRegisters(std::shared_ptr<StreamRegisterFile> value);

		/// <summary>
		/// L3 Global Distributed Cache — shared SRAM buffer for HBM4 (req.md §2).
		/// Acts as intermediate cache between L2 (per-Pod) and DRAM/HBM4.
		/// </summary>
	public:
		std::shared_ptr<GlobalDistributedCache> getL3Cache() const;
	private:
		void setL3Cache(std::shared_ptr<GlobalDistributedCache> value);

//		#endregion

//		#region Performance Statistics

		/// <summary>
		/// Total number of burst operations
		/// </summary>
	public:
		long long getTotalBursts() const;
	private:
		void setTotalBursts(long long value);

		/// <summary>
		/// Total bytes transferred through this subsystem
		/// </summary>
	public:
		long long getTotalBytesTransferred() const;
	private:
		void setTotalBytesTransferred(long long value);

		/// <summary>
		/// Number of bank conflicts detected
		/// </summary>
	public:
		long long getBankConflicts() const;
	private:
		void setBankConflicts(long long value);

		/// <summary>
		/// Number of stall cycles due to conflicts
		/// </summary>
	public:
		long long getStallCycles() const;
	private:
		void setStallCycles(long long value);

		/// <summary>
		/// Number of transfers routed through DMA
		/// </summary>
	public:
		long long getDmaTransfers() const;
	private:
		void setDmaTransfers(long long value);

		/// <summary>
		/// Average burst length in bytes
		/// </summary>
	public:
		double getAverageBurstLength() const;

		/// <summary>
		/// Total wait time in cycles across all requests
		/// </summary>
		long long getTotalWaitCycles() const;
	private:
		void setTotalWaitCycles(long long value);

		/// <summary>
		/// Average wait time per request in cycles
		/// </summary>
	public:
		double getAverageWaitCycles() const;

		/// <summary>
		/// Maximum queue depth observed across all banks
		/// </summary>
		int getMaxQueueDepth() const;
	private:
		void setMaxQueueDepth(int value);

		/// <summary>
		/// Current total requests queued across all banks
		/// </summary>
	public:
		int getCurrentQueuedRequests() const;

		/// <summary>
		/// Get a snapshot of current per-bank queue depths for diagnostics.
		/// </summary>
		std::vector<int> GetBankQueueDepthsSnapshot();

		/// <summary>
		/// Current memory subsystem cycle for diagnostics.
		/// </summary>
		long long getCurrentCycle() const;

		/// <summary>
		/// Sole RF-10 owner of the shared memory-controller cycle and migrated
		/// request/completion latches.
		/// </summary>
		std::shared_ptr<MemoryCycleController> getCycleController() const;

		/// <summary>
		/// Indicates if the memory channel is overloaded (Memory Wall FSP).
		/// Used by MicroOpScheduler to suppress LSU operations and steal ALU-only slots.
		/// </summary>
		bool isChannelOverloaded() const;

		/// <summary>
		/// Returns the deterministic sampled hardware occupancy snapshot for the current cycle.
		/// The structural mask is kept as a compatibility projection, while the explicit
		/// memory budget closes the widened lane4..5 contract for already-live LSU loads/stores.
		/// The snapshot now also carries deterministic read-credit / write-credit projections
		/// derived from the live per-port read/write turnaround model.
		/// </summary>
		HardwareOccupancySnapshot128 GetHardwareOccupancySnapshot128() override;

		/// <summary>
		/// Generates a 128-bit safety mask representing the currently overloaded banks or channels.
		/// Compatibility projection from the deterministic occupancy snapshot.
		/// </summary>
		Core::SafetyMask128 GetHardwareOccupancyMask128() override;

		void AdvanceSamplingEpoch() override;

	private:
		HardwareOccupancySnapshot128 BuildHardwareOccupancySnapshot();

		inline static unsigned char SaturateMemoryIssueBudget(int remainingGlobalBudget);

		inline static int ProjectDirectionalBudget(int remainingSharedBudget, int queuedDirectionalRequests, int directionalCapacity, int directionallyReadyPorts);

		// Phase 3: Burst timing and efficiency metrics
		/// <summary>
		/// Number of unaligned memory accesses (Phase 3)
		/// </summary>
	public:
		long long getUnalignedAccessCount() const;
	private:
		void setUnalignedAccessCount(long long value);

		/// <summary>
		/// Total alignment penalty cycles (Phase 3)
		/// </summary>
	public:
		long long getTotalAlignmentPenalty() const;
	private:
		void setTotalAlignmentPenalty(long long value);

		/// <summary>
		/// Burst efficiency (data cycles / total cycles) (Phase 3)
		/// </summary>
	public:
		double getBurstEfficiency() const;
	private:
		void setBurstEfficiency(double value);

		/// <summary>
		/// Total burst timing cycles (Phase 3)
		/// </summary>
	public:
		long long getTotalBurstTimingCycles() const;
	private:
		void setTotalBurstTimingCycles(long long value);

//		#endregion

//		#region Events

		/// <summary>
		/// Event arguments for burst operations
		/// </summary>
	public:
		using BurstEventArgs = YAKSys_Hybrid_CPU::Memory::BurstEventArgs;

		/// <summary>
		/// Fired when a burst operation starts
		/// </summary>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public event std::function<void(void*, std::shared_ptr<BurstEventArgs>)>? BurstStarted;
		std::shared_ptr<TangibleEvent<std::function<void(void*, std::shared_ptr<BurstEventArgs>)>>> BurstStarted = std::make_shared<TangibleEvent<std::function<void(void*, std::shared_ptr<BurstEventArgs>)>>>();

		/// <summary>
		/// Fired when a burst operation completes
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public event std::function<void(void*, std::shared_ptr<BurstEventArgs>)>? BurstCompleted;
		std::shared_ptr<TangibleEvent<std::function<void(void*, std::shared_ptr<BurstEventArgs>)>>> BurstCompleted = std::make_shared<TangibleEvent<std::function<void(void*, std::shared_ptr<BurstEventArgs>)>>>();

//		#endregion

//		#region Asynchronous Request Tracking

		/// <summary>
		/// Token for tracking asynchronous memory requests
		/// </summary>
	public:
		using MemoryRequestToken = YAKSys_Hybrid_CPU::Memory::MemoryRequestToken;

//		#endregion

//		#region Internal State

		/// <summary>
		/// Port state tracking for port switching penalty simulation (Phase 2: Requirement 2.2)
		/// </summary>
	private:
		class PortState
		{
		public:
			int PortId = 0; // Port identifier
			int CurrentBankId = 0; // Which bank is this port currently accessing (-1 = none)
			long long LastSwitchCycle = 0; // Cycle when port last switched banks
			bool Busy = false; // Is port currently busy?
			bool HasLastDirection = false; // Has this port already observed traffic?
			bool LastWasRead = false; // Direction of the most recent completed request
			long long DirectionReadyCycle = 0; // Earliest cycle when opposite direction is admitted again
		};

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly DMAController? dma;
		const std::shared_ptr<DMAController> dma;
		const Processor processor;

		// Track bank occupancy for conflict detection
		std::vector<bool> bankOccupied;
		std::vector<long long> bankLastAccessCycle;
		long long currentCycle = 0;

		// Phase 2: Port management for switching penalties
		std::vector<PortState> _portStates;

		// Per-bank request queues for dynamic scheduling
		std::vector<std::deque<BankRequest>> bankQueues;
		int roundRobinIndex = 0; // For round-robin arbitration

		// Track asynchronous memory requests
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Collections.Generic.Dictionary<ulong, MemoryRequestToken> pendingRequests;
		std::mutex pendingRequestsGate;
		std::unordered_map<unsigned long long, std::shared_ptr<MemoryRequestToken>> pendingRequests;
		unsigned long long nextRequestID = 0;
		HardwareOccupancySnapshot128 _cachedHardwareOccupancySnapshot;
		unsigned long long _hardwareSamplingEpoch = 0;

		/// <summary>
		/// Transaction Reorder Buffer: allows out-of-order burst completion
		/// while maintaining in-order retirement (Plan 08).
		/// </summary>
	public:
		TransactionReorderBuffer TRB;

		/// <summary>
		/// Represents a memory request in the bank queue
		/// </summary>
	private:


	private:
		class PhysicalBankTopologyCandidate final : public std::enable_shared_from_this<PhysicalBankTopologyCandidate>
		{
		private:
			const std::vector<bool> BankOccupied;
			const std::vector<long long> BankLastAccessCycle;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Queue<BankRequest>[] BankQueues;
			std::vector<std::deque<BankRequest>> BankQueues;
			const std::vector<PortState> PortStates;

		public:
			PhysicalBankTopologyCandidate(const std::vector<bool> &bankOccupied, const std::vector<long long> &bankLastAccessCycle, const std::vector<std::deque<BankRequest>> &bankQueues, const std::vector<PortState> &portStates);

			std::vector<bool> getBankOccupied() const;
			std::vector<long long> getBankLastAccessCycle() const;
			std::vector<std::deque<BankRequest>> getBankQueues() const;
			std::vector<PortState> getPortStates() const;
		};

		// ── Domain Inflight Burst Tracking (Q1 Review §4) ──────────

		/// <summary>
		/// Per-domain inflight burst counter.
		/// Prevents one security domain from saturating shared memory bandwidth.
		/// Key: domain tag hash slot (0–15). Value: current inflight count.
		/// HLS: 16 × 8-bit saturating counters = 128 bits.
		/// </summary>
	private:
		const std::vector<int> _domainInflightBursts = std::vector<int>(16);

		/// <summary>
		/// Maximum inflight bursts per domain before local stall.
		/// Default: 8 (allows 8 concurrent bursts per domain).
		/// Zero disables domain burst limiting.
		/// </summary>
	public:
		int getMaxInflightBurstsPerDomain() const;
		void setMaxInflightBurstsPerDomain(int value);

		/// <summary>Statistics: burst requests rejected due to domain inflight limit.</summary>
		long long getDomainBurstStalls() const;
	private:
		void setDomainBurstStalls(long long value);

//		#endregion

		void ReconfigureBankTopology();

		void ReconfigurePortStates();

		void RefreshHardwareSamplingAfterTopologyChange();

	public:
		std::shared_ptr<MemoryBankGeometryUpdateResult> TryReplacePhysicalMemoryBankGeometryUnderControllerGate(int bankCount, int bankWidthBytes, bool controllerIsQuiescent);

	private:
		bool IsPhysicalBankGeometryOwnerQuiescent();

		bool TryPreparePhysicalBankTopologyCandidate(int bankCount, std::shared_ptr<PhysicalBankTopologyCandidate> &candidate);

//		#region Constructor

		/// <summary>
		/// Initialize memory subsystem
		/// </summary>
		/// <param name="proc">Reference to processor for memory access</param>
		/// <param name="dmaController">Optional DMA controller for offloading</param>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MemorySubsystem(ref Processor proc, DMAController? dmaController = null) : this(ref proc, dmaController, null)
		MemorySubsystem(Processor &proc, std::shared_ptr<DMAController> dmaController = nullptr);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MemorySubsystem(ref Processor proc, DMAController? dmaController, System.Nullable<Processor.MainMemoryArea> mainMemory)
		MemorySubsystem(Processor &proc, std::shared_ptr<DMAController> dmaController, std::shared_ptr<Processor::MainMemoryArea> mainMemory);

//		#endregion

//		#region IBurstBackend Implementation

		/// <summary>
		/// Read data from memory with burst optimization and conflict detection
		/// </summary>
	public:
		bool Read(unsigned long long deviceID, unsigned long long address, std::span<unsigned char> buffer) override;

		/// <summary>
		/// Write data to memory with burst optimization and conflict detection
		/// </summary>
		bool Write(unsigned long long deviceID, unsigned long long address, std::span<const unsigned char> buffer) override;

//		#endregion


//		#region Helper Methods

		/// <summary>
		/// Compute which memory bank an address belongs to
		/// </summary>
	private:
		std::shared_ptr<PhysicalMemoryBankIndex> ComputeBankId(unsigned long long address);

		bool RemoveQueuedBankRequest(unsigned long long requestID, int bankId);

		/// <summary>
		/// Calculate adaptive stall cycles based on queue depth and transfer size
		/// </summary>
		/// <param name="queueDepth">Current queue depth for the bank</param>
		/// <param name="transferSize">Size of the transfer in bytes</param>
		/// <returns>Number of stall cycles</returns>
		int CalculateAdaptiveStall(int queueDepth, int transferSize);

		/// <summary>
		/// Select next bank for processing based on arbitration policy
		/// </summary>
		/// <returns>Bank ID to process, or -1 if no banks have pending requests</returns>
		int SelectNextBank();

		/// <summary>
		/// Round-robin bank selection
		/// </summary>
		int SelectBankRoundRobin();

		/// <summary>
		/// Weighted fair queueing bank selection
		/// Prioritizes banks with longer queues but considers occupancy
		/// </summary>
		int SelectBankWeightedFair();

		/// <summary>
		/// Priority-based bank selection
		/// Selects bank with highest priority request
		/// </summary>
		int SelectBankPriority();

		/// <summary>
		/// Process queued requests for available banks
		/// </summary>
		void ProcessBankQueues();

		/// <summary>
		/// Execute a bank request immediately
		/// </summary>
		void ExecuteBankRequest(int bankId, BankRequest request);

		/// <summary>
		/// Advance the explicitly bound persistent DMA agent once. The sole
		/// caller is MemoryCycleController's normalized shared-domain edge.
		/// </summary>
	public:
		void AdvanceBoundDmaAgentOneCycle();

		/// <summary>
		/// Reset all performance statistics
		/// </summary>
		void ResetStatistics();

		/// <summary>
		/// Advance the internal cycle counter (for simulation)
		/// </summary>
		void AdvanceCycles(long long cycles);

		/// <summary>
		/// One compatibility service edge for request families not yet migrated
		/// to typed controller admission. Only MemoryCycleController may call it.
		/// </summary>
		void AdvanceLegacyAgentOneCycle();

		/// <summary>
		/// Functional backing adapter for the RF-10 controller-native exact
		/// read classes.
		/// Translation remains on its existing IOMMU owner; no PTW/cache/DMA
		/// timing authority is inferred by this narrow cutover.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool ExecuteControllerReadStep(ulong deviceId, ulong address, PhysicalMemoryBankBinding physicalBankBinding, byte[] destination, YAKSys_Hybrid_CPU.Core.Memory.CpuInstructionTranslationOwner.PhysicalReadBinding? cpuPhysicalRead = null)
		bool ExecuteControllerReadStep(unsigned long long deviceId, unsigned long long address, std::shared_ptr<PhysicalMemoryBankBinding> physicalBankBinding, std::vector<unsigned char> &destination, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Memory::PhysicalReadBinding> cpuPhysicalRead = nullptr);

		/// <summary>
		/// Canonical vector-transfer source adapter retained independently:
		/// one accepted source binding cannot represent every address reached
		/// by a strided transfer.
		/// </summary>
	private:
		bool ExecuteControllerCanonicalVectorElementReadStepUnderOwnerGate(unsigned long long deviceId, unsigned long long address, std::vector<unsigned char> &destination);

		/// <summary>
		/// Functional source-read adapter for one controller-native canonical
		/// vector transfer. One invocation is one controller service decision;
		/// element iteration is not a new byte-port or multi-beat timing model.
		/// </summary>
	public:
		bool ExecuteControllerVectorTransferReadStep(unsigned long long deviceId, unsigned long long sourceAddress, unsigned long long elementCount, int elementSize, unsigned short stride, CanonicalVectorPhysicalBankEnvelope physicalBankEnvelope, std::vector<unsigned char> &packedDestination);

//		#endregion

//		#region Event Handlers

	protected:
		virtual void OnBurstStarted(unsigned long long address, int length, bool isRead, int bankId);

		virtual void OnBurstCompleted(unsigned long long address, int length, bool isRead, int bankId);

//		#endregion

//		#region Phase 2: Microarchitecture Fidelity

		/// <summary>
		/// Calculate port switching penalty when a port needs to switch from one bank to another.
		/// (Phase 2: Requirement 2.2)
		/// </summary>
		/// <param name="portId">Port ID to use</param>
		/// <param name="newBankId">Bank ID to switch to</param>
		/// <returns>Number of penalty cycles (0 if no switch needed)</returns>
	private:
		int CalculatePortSwitchPenalty(int portId, int newBankId);

		/// <summary>
		/// Calculate memory latency based on row buffer state.
		/// (Phase 2: Requirement 2.2)
		/// </summary>
		/// <param name="address">Memory address to access</param>
		/// <param name="size">Size of access in bytes</param>
		/// <returns>Latency in cycles</returns>
		int CalculateMemoryLatency(unsigned long long address, int size);

		/// <summary>
		/// Allocate a port for accessing a bank, updating port state.
		/// (Phase 2: Requirement 2.2)
		/// </summary>
		/// <param name="bankId">Bank ID to access</param>
		/// <returns>Port ID that was allocated</returns>
		int AllocatePortForBank(int bankId);

		/// <summary>
		/// Allocate a port for accessing a bank with direction-aware turnaround.
		/// </summary>
		int AllocatePortForBank(int bankId, bool isRead);

		/// <summary>
		/// Release a port after completing an operation.
		/// (Phase 2: Requirement 2.2)
		/// </summary>
		/// <param name="portId">Port ID to release</param>
		void ReleasePortAfterOperation(int portId);

		/// <summary>
		/// Release a port after completing an operation and update the
		/// direction-ready timestamp for the next sampled occupancy snapshot.
		/// </summary>
		void ReleasePortAfterOperation(int portId, int bankId, bool isRead);

		inline bool IsPortDirectionReady(int portId, bool isRead);

		inline int CountDirectionallyReadyPorts(bool isRead);

//		#endregion

//		#region Phase 4: Accelerator DMA Support (fail-closed)

		/// <summary>
		/// Register accelerator device.
		/// </summary>
	public:
		void RegisterAcceleratorDevice(unsigned long long deviceId, YAKSys_Hybrid_CPU::Execution::AcceleratorDMACapabilities capabilities) override;

		/// <summary>
		/// Initiate accelerator DMA transfer.
		/// </summary>
		std::shared_ptr<YAKSys_Hybrid_CPU::Execution::DMATransferToken> InitiateAcceleratorDMA(unsigned long long deviceId, unsigned long long srcAddr, unsigned long long dstAddr, int size) override;

//		#endregion


//		#region Asynchronous Memory Operations

		/// <summary>
		/// Enqueue an asynchronous read request
		/// Returns a token that can be polled for completion
		/// </summary>
		/// <param name="deviceID">Device initiating the request</param>
		/// <param name="address">Memory address to read from</param>
		/// <param name="size">Number of bytes to read</param>
		/// <param name="buffer">Buffer to store the read data</param>
		/// <returns>Token for tracking request completion</returns>
		std::shared_ptr<MemoryRequestToken> EnqueueRead(unsigned long long deviceID, unsigned long long address, int size, std::vector<unsigned char> &buffer);

		/// <summary>
		/// Enqueue an asynchronous write request
		/// Returns a token that can be polled for completion
		/// </summary>
		/// <param name="deviceID">Device initiating the request</param>
		/// <param name="address">Memory address to write to</param>
		/// <param name="size">Number of bytes to write</param>
		/// <param name="buffer">Buffer containing the data to write</param>
		/// <returns>Token for tracking request completion</returns>
		std::shared_ptr<MemoryRequestToken> EnqueueWrite(unsigned long long deviceID, unsigned long long address, int size, std::vector<unsigned char> &buffer, bool deferPhysicalWriteUntilRetire = false);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool CancelPendingRequest(MemoryRequestToken? token)
		bool CancelPendingRequest(std::shared_ptr<MemoryRequestToken> token);

		bool CancelPendingRequest(unsigned long long requestID);

//		#endregion

	};
}
