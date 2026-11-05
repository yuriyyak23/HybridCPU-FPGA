#pragma once

#include "../CloseToHSL/Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../CloseToHSL/Core/State/CoreTimebaseSnapshot.h"
#include "../CloseToHSL/Core/Decoder/DecodedBundleDescriptor.h"
#include "../CloseToHSL/Core/Pipeline/Core/PipelineContourCertificate.h"
#include "../CloseToHSL/Core/Pipeline/Certificates/ReplayPhaseSubstrate.h"
#include "../CloseToHSL/Core/Pipeline/Certificates/ReplayPhaseSubstrate.Implementations.h"
#include "../CloseToHSL/Core/Architecture/State/Pipeline/PipelineState.h"
#include <string>
#include <vector>
#include <memory>
#include <utility>

using namespace HybridCPU_ISE::Machine;
using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE
{
	class StackFlagsSnapshot
	{
	public:
		int CallStackDepth = 0;
		unsigned long long CallStackTop = 0;
		int InterruptStackDepth = 0;
		unsigned long long InterruptStackTop = 0;

		bool AluZero = false;
		bool AluSign = false;
		bool AluOverflow = false;
		bool AluParity = false;

		bool BaseEnableInterrupt = false;
		bool BaseIoPrivilege = false;
		bool BaseOsKernel = false;

		bool CommonDirection = false;
		bool CommonJump = false;
		bool CommonJumpOutsideVliw = false;

		unsigned char EilpOffsetVliwOpcode = 0;
	};

	/// <summary>
	/// Thread-safe read-only observation service for GUI and bridge state reads.
	/// All methods return snapshots (immutable copies) to ensure thread safety
	/// between ISE emulation thread and GUI update thread.
	///
	/// Usage from GUI:
	/// - Never access Processor.CPU_Cores directly from GUI thread
	/// - Always use observation-service methods
	/// - All returned data is copied, safe to use without locks
	/// </summary>

//	#region Snapshot Structures

	/// <summary>
	/// Immutable snapshot of core state for GUI display.
	/// </summary>
	class CoreStateSnapshot
	{
	public:
		CoreStateSnapshot() = default;
		CoreStateSnapshot(CoreTimebaseSnapshot timebase, PipelineContourCertificate decodeCertificate,
			PipelineContourCertificate executeCertificate, PipelineContourCertificate retireCertificate)
			: Timebase(std::move(timebase)), DecodePublicationCertificate(std::move(decodeCertificate)),
			  ExecuteCompletionCertificate(std::move(executeCertificate)), RetireVisibilityCertificate(std::move(retireCertificate)) {}

		int CoreId = 0;
		unsigned long long LiveInstructionPointer = 0;
		unsigned long long CycleCount = 0;
		std::wstring CurrentState;
		Processor::CPU_Core::CorePowerState CurrentPowerState = static_cast<Processor::CPU_Core::CorePowerState>(0);
		unsigned int CurrentPerformanceLevel = 0;
		bool IsStalled = false;
		bool HasExceptions = false;
		int ActiveVirtualThreadId = 0;
		std::vector<unsigned long long> VirtualThreadLivePcs;
		std::vector<unsigned long long> VirtualThreadCommittedPcs;
		std::vector<bool> VirtualThreadStalled;
		std::vector<std::wstring> VirtualThreadStallReasons;
		std::vector<unsigned long long> ActiveVirtualThreadRegisters;
		int L1VliwBundleCount = 0;
		int L1DataEntryCount = 0;
		int L2VliwBundleCount = 0;
		int L2DataEntryCount = 0;
		bool PipelineEnabled = false;
		CoreTimebaseSnapshot Timebase;
		double PipelineIPC = 0.0;
		double PipelineEfficiency = 0.0;
		unsigned long long PipelineCycleCount = 0;
		unsigned long long PipelineStallCycles = 0;
		unsigned long long PipelineInstructionsRetired = 0;
		unsigned long long PipelineBranchMispredicts = 0;
		unsigned long long PipelineDataHazards = 0;
		unsigned long long PipelineMemoryStalls = 0;
		unsigned long long PipelineForwardingEvents = 0;
		unsigned long long PipelineControlHazards = 0;
		unsigned long long PipelineWAWHazards = 0;
		unsigned long long PipelineLoadUseBubbles = 0;
		unsigned char PipelineBundleSlotIndex = 0;
		YAKSys_Hybrid_CPU::Core::DecodedBundleStateOwnerKind DecodedBundleStateOwnerKind = static_cast<YAKSys_Hybrid_CPU::Core::DecodedBundleStateOwnerKind>(0);
		unsigned long long DecodedBundleStateEpoch = 0;
		unsigned long long DecodedBundleStateVersion = 0;
		YAKSys_Hybrid_CPU::Core::DecodedBundleStateKind DecodedBundleStateKind = static_cast<YAKSys_Hybrid_CPU::Core::DecodedBundleStateKind>(0);
		YAKSys_Hybrid_CPU::Core::DecodedBundleStateOrigin DecodedBundleStateOrigin = static_cast<YAKSys_Hybrid_CPU::Core::DecodedBundleStateOrigin>(0);
		unsigned long long DecodedBundlePc = 0;
		unsigned char DecodedBundleValidMask = 0;
		unsigned char DecodedBundleNopMask = 0;
		bool DecodedBundleHasCanonicalDecode = false;
		bool DecodedBundleHasCanonicalLegality = false;
		bool DecodedBundleHasDecodeFault = false;
		PipelineContourCertificate DecodePublicationCertificate;
		PipelineContourCertificate ExecuteCompletionCertificate;
		PipelineContourCertificate RetireVisibilityCertificate;
		unsigned long long VectorLength = 0;
		unsigned char VectorTailAgnostic = 0;
		unsigned char VectorMaskAgnostic = 0;
	};

	/// <summary>
	/// Pod controller snapshot.
	/// </summary>
	class PodSnapshot
	{
	public:
		int PodIndex = 0;
		int PodX = 0;
		int PodY = 0;
		unsigned short PodId = 0;
		unsigned long long DomainCertificate = 0;
		int PowerGatedCores = 0;
	};

	/// <summary>
	/// GRLB (Global Resource Lock Bitmap) snapshot.
	/// 128 bits divided into 4 banks of 32 bits each.
	/// </summary>
	class GrlbSnapshot
	{
	public:
		int CoreId = 0;
		unsigned int Bank0 = 0; // Bits 0-31: Register read/write groups
		unsigned int Bank1 = 0; // Bits 32-63: Memory domains + LSU + DMA 0-3
		unsigned int Bank2 = 0; // Bits 64-95: Extended GRLB (DMA 4-7, Stream, Accel)
		unsigned int Bank3 = 0; // Bits 96-127: Reserved / extended domains
		unsigned long long StructuralStalls = 0;
	};

	/// <summary>
	/// FSP Power Controller snapshot.
	/// </summary>
	class FspPowerSnapshot
	{
	public:
		int CoreId = 0;
		int PodIndex = 0;
		int CurrentGatedCount = 0;
		unsigned long long TotalGatedCycles = 0;
		unsigned long long GateTransitions = 0;
		bool IsClockGated = false;
	};

	/// <summary>
	/// Memory subsystem performance snapshot.
	/// </summary>
	class MemorySubsystemSnapshot
	{
	public:
		int NumBanks = 0;
		long long TotalBursts = 0;
		long long BankConflicts = 0;
		long long StallCycles = 0;
		long long TotalBytesTransferred = 0;
		int CurrentQueuedRequests = 0;
		int MaxQueueDepth = 0;
		double AverageBurstLength = 0.0;
		double AverageWaitCycles = 0.0;
		double BurstEfficiency = 0.0;
		bool IsChannelOverloaded = false;
		std::wstring ArbitrationPolicy;
	};

	/// <summary>
	/// Memory configuration snapshot.
	/// </summary>
	class MemoryConfigSnapshot
	{
	public:
		int NumBanks = 0;
		int BankWidthBytes = 0;
		int NumMemoryPorts = 0;
		double BankBandwidthGBps = 0.0;
		int DmaThresholdBytes = 0;
		int AxiBoundary = 0;
		int MaxBurstLength = 0;
		int PortSwitchingPenalty = 0;
		int RowBufferHitLatency = 0;
		int RowBufferMissLatency = 0;
		int RowBufferSize = 0;
	};

	/// <summary>
	/// Compiler state snapshot for multithreaded compilation.
	/// </summary>
	class CompilerStateSnapshot
	{
	public:
		bool IsAvailable = false;
		std::wstring AvailabilityReason;
		bool IsCompiling = false;
		int ThreadCount = 0;
		int TotalInstructionsCompiled = 0;
		int BarriersInserted = 0;
		int CrossThreadDependencies = 0;
		double BundleUtilization = 0.0;
	};

	/// <summary>
	/// Per-thread compiler context state snapshot.
	/// </summary>
	class ThreadCompilerStateSnapshot
	{
	public:
		bool IsAvailable = false;
		std::wstring AvailabilityReason;
		int VirtualThreadId = 0;
		int InstructionCount = 0;
		unsigned long long DomainTag = 0;
		int RegistersAllocated = 0;
		bool HasPendingBarrier = false;
	};

	/// <summary>
	/// Dependency graph snapshot for cross-thread dependencies.
	/// </summary>
	class DependencyGraphSnapshot
	{
	public:
		bool IsAvailable = false;
		std::wstring AvailabilityReason;
		std::vector<std::vector<bool>> DependencyMatrix; // 4x4 matrix: dependsOn[i,j] = VT-i depends on VT-j
		int MemoryAccessCount = 0;
		bool HasCyclicDependency = false;
		int TotalDependencies = 0;
	};

	/// <summary>
	/// Barrier scheduler snapshot.
	/// </summary>
	class BarrierSchedulerSnapshot
	{
	public:
		bool IsAvailable = false;
		std::wstring AvailabilityReason;
		int BarriersInserted = 0;
		int ActiveBarrierCount = 0;
		unsigned long long MaxBarrierTimeout = 0;
		int CoalescedBarriers = 0;
	};

//	#endregion
}

namespace HybridCPU_ISE::Machine
{
	/// <summary>
	/// Detached, read-only diagnostic projection of one CPU core. It contains no
	/// live runtime owner reference and cannot be written back as a core facade.
	/// </summary>
	class CpuCoreDiagnosticSnapshot final : public std::enable_shared_from_this<CpuCoreDiagnosticSnapshot>
	{
	private:
		const unsigned int CoreId;
		const Processor::CPU_Core::FlagsRegister CoreFlagsRegister;
		const int CallStackDepth;
		const unsigned long long CallStackTop;
		const int InterruptStackDepth;
		const unsigned long long InterruptStackTop;
		const unsigned long long StructuralStalls;
		const bool HasExceptions;
		const int L1VliwBundleCount;
		const int L1DataEntryCount;
		const int L2VliwBundleCount;
		const int L2DataEntryCount;
		const unsigned long long PowerStateCsrValue;
		const unsigned long long PerformanceLevelCsrValue;
		const unsigned long long VectorLength;
		const unsigned char VectorTailAgnostic;
		const unsigned char VectorMaskAgnostic;
		const YAKSys_Hybrid_CPU::Core::ReplayPhaseMetrics ReplayPhaseMetrics;
		const YAKSys_Hybrid_CPU::Core::SchedulerPhaseMetrics SchedulerPhaseMetrics;

		const std::vector<unsigned long long> _committedPcs;
		const std::vector<unsigned long long> _architecturalRegisters;
		const std::vector<bool> _virtualThreadStalled;
		const std::vector<PipelineState> _virtualThreadPipelineStates;
		const std::vector<Processor::CPU_Core::FPExceptionContext> _threadFpContexts;
		const std::vector<unsigned int> _grlbBanks;
		const Processor::CPU_Core::PipelineObservationSnapshot _pipelineObservation;

	public:
	public:
		CpuCoreDiagnosticSnapshot(unsigned int coreId, std::vector<unsigned long long> &committedPcs, std::vector<unsigned long long> &architecturalRegisters, std::vector<bool> &virtualThreadStalled, std::vector<PipelineState> &virtualThreadPipelineStates, std::vector<Processor::CPU_Core::FPExceptionContext> &threadFpContexts, Processor::CPU_Core::FlagsRegister coreFlagsRegister, int callStackDepth, unsigned long long callStackTop, int interruptStackDepth, unsigned long long interruptStackTop, std::vector<unsigned int> &grlbBanks, unsigned long long structuralStalls, bool hasExceptions, int l1VliwBundleCount, int l1DataEntryCount, int l2VliwBundleCount, int l2DataEntryCount, unsigned long long powerStateCsrValue, unsigned long long performanceLevelCsrValue, unsigned long long vectorLength, unsigned char vectorTailAgnostic, unsigned char vectorMaskAgnostic, Processor::CPU_Core::PipelineObservationSnapshot pipelineObservation, YAKSys_Hybrid_CPU::Core::ReplayPhaseMetrics replayPhaseMetrics, YAKSys_Hybrid_CPU::Core::SchedulerPhaseMetrics schedulerPhaseMetrics);

	public:
		unsigned int getCoreId() const;
		Processor::CPU_Core::FlagsRegister getCoreFlagsRegister() const;
		int getCallStackDepth() const;
		unsigned long long getCallStackTop() const;
		int getInterruptStackDepth() const;
		unsigned long long getInterruptStackTop() const;
		unsigned long long getStructuralStalls() const;
		bool getHasExceptions() const;
		int getL1VliwBundleCount() const;
		int getL1DataEntryCount() const;
		int getL2VliwBundleCount() const;
		int getL2DataEntryCount() const;
		unsigned long long getPowerStateCsrValue() const;
		unsigned long long getPerformanceLevelCsrValue() const;
		unsigned long long getVectorLength() const;
		unsigned char getVectorTailAgnostic() const;
		unsigned char getVectorMaskAgnostic() const;
		YAKSys_Hybrid_CPU::Core::ReplayPhaseMetrics getReplayPhaseMetrics() const;
		YAKSys_Hybrid_CPU::Core::SchedulerPhaseMetrics getSchedulerPhaseMetrics() const;

		unsigned long long ReadActiveLivePc();
		int ReadActiveVirtualThreadId();
		Processor::CPU_Core::PipelineControl GetPipelineControl();
		YAKSys_Hybrid_CPU::Core::ReplayPhaseMetrics GetReplayPhaseMetrics();
		YAKSys_Hybrid_CPU::Core::SchedulerPhaseMetrics GetSchedulerPhaseMetrics();
		unsigned long long ReadCommittedPc(int virtualThreadId);
		unsigned long long ReadArch(int virtualThreadId, int registerId);
		bool ReadVirtualThreadStalled(int virtualThreadId);
		Processor::CPU_Core::FPExceptionContext ReadThreadFpContext(int virtualThreadId);
		bool HasAnyVirtualThreadPipelineState(PipelineState state);
		std::vector<unsigned int> GetGrlbBanks();

		Processor::CPU_Core::PipelineObservationSnapshot GetPipelineObservationSnapshot();

		static std::shared_ptr<CpuCoreDiagnosticSnapshot> Capture(std::shared_ptr<Processor::CPU_Core> core);
	};
}
