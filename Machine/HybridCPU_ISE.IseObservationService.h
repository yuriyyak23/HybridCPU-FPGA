#pragma once

#include "../CloseToHSL/Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "IseObservationService.h"
#include "IIseMachineStateSource.h"
#include "../CloseToHSL/Core/State/CoreTimebaseSnapshot.h"
#include "../NonRTL/Processor/Performance/YAKSys_Hybrid_CPU.PerformanceReport.h"
#include "../CloseToHSL/Core/Pipeline/Certificates/ReplayPhaseSubstrate.h"
#include "../CloseToHSL/Core/Pipeline/Certificates/ReplayPhaseSubstrate.Implementations.h"
#include "MachineStateSourceProvenance.h"
#include "../CloseToHSL/Processor/Core/YAKSys_Hybrid_CPU.PodController.h"
#include "../CloseToHSL/Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include <string>
#include <vector>
#include <any>
#include <optional>
#include <mutex>
#include <format>
#include <memory>
#include "../tangible_rectangular_vectors.h"

using namespace HybridCPU_ISE::Machine;
using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Memory;

using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;


namespace HybridCPU_ISE
{
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
	class IseObservationService final : public std::enable_shared_from_this<IseObservationService>
	{
	private:
		static inline const std::wstring CompilerTelemetryUnavailableReason = L"No live multithreaded compiler telemetry is published on the current runtime contour; only stored canonical compile artifacts remain available to specialized views.";

//		#region 4-Way SMT State Access

		/// <summary>
		/// Get live program counters for all 4 virtual threads in a core.
		/// </summary>
		/// <param name="coreId">Core ID (0-1023)</param>
		/// <returns>Array of 4 live PCs (one per VT)</returns>
	public:
		std::vector<unsigned long long> GetVirtualThreadLivePcs(int coreId);

		/// <summary>
		/// Get stall flags for all 4 virtual threads.
		/// </summary>
		/// <param name="coreId">Core ID (0-1023)</param>
		/// <returns>Array of 4 stall flags (true = stalled)</returns>
		std::vector<bool> GetVirtualThreadStalled(int coreId);

		/// <summary>
		/// Get stall reasons for all 4 virtual threads.
		/// </summary>
		/// <param name="coreId">Core ID (0-1023)</param>
		/// <returns>Array of 4 stall reason strings</returns>
		std::vector<std::wstring> GetVirtualThreadStallReasons(int coreId);

		/// <summary>
		/// Get exception contexts for all 4 virtual threads.
		/// </summary>
		/// <param name="coreId">Core ID (0-1023)</param>
		/// <returns>Array of 4 FP exception contexts</returns>
		std::vector<Processor::CPU_Core::FPExceptionContext> GetVirtualThreadExceptions(int coreId);

		/// <summary>
		/// Get register values for a specific virtual thread.
		/// Values are read through the unified architectural-state helpers.
		/// </summary>
		/// <param name="coreId">Core ID (0-1023)</param>
		/// <param name="vtId">Virtual Thread ID (0-3)</param>
		/// <returns>Array of 32 register values</returns>
		std::vector<unsigned long long> GetVirtualThreadRegisters(int coreId, int vtId);

		/// <summary>
		/// Get the active live instruction pointer for a core.
		/// </summary>
		/// <param name="coreId">Core ID (0-1023)</param>
		/// <returns>Active live PC</returns>
		unsigned long long GetLiveInstructionPointer(int coreId);

		/// <summary>
		/// Get the currently active virtual thread ID for a core.
		/// </summary>
		/// <param name="coreId">Core ID (0-1023)</param>
		/// <returns>Active VT ID (0-3)</returns>
		int GetActiveVirtualThreadId(int coreId);

//		#endregion

//		#region Core State Access

		StackFlagsSnapshot GetStackFlagsSnapshot(int coreId);

		/// <summary>
		/// Get basic core state snapshot.
		/// </summary>
		CoreStateSnapshot GetCoreState(int coreId);

		/// <summary>
		/// Get total number of cores in the processor.
		/// </summary>
		int GetTotalCores();

//		#endregion

//		#region Memory State Access

		/// <summary>
		/// Read memory bytes (thread-safe).
		/// </summary>
		/// <param name="address">Start address</param>
		/// <param name="length">Number of bytes to read</param>
		/// <returns>Copy of memory bytes</returns>
		std::vector<unsigned char> ReadMemory(unsigned long long address, int length);

//		#endregion

//		#region Private Helpers

	private:
		void ValidateCoreId(int coreId);

		static void ValidateCoreId(std::shared_ptr<IIseMachineStateSource> machineStateSource, int coreId);

		static void ValidateVtId(int vtId);

		static Processor::CPU_Core::PipelineObservationSnapshot ReadCoreObservationSnapshot(std::shared_ptr<CpuCoreDiagnosticSnapshot> core);

		static std::wstring DetermineStallReason(std::shared_ptr<CpuCoreDiagnosticSnapshot> core, int vtId, Processor::CPU_Core::PipelineObservationSnapshot const observation);

		static std::vector<unsigned long long> BuildVirtualThreadLivePcs(std::shared_ptr<CpuCoreDiagnosticSnapshot> core, int activeVtId, unsigned long long activeLivePc);

		static std::vector<unsigned long long> BuildVirtualThreadCommittedPcs(std::shared_ptr<CpuCoreDiagnosticSnapshot> core);

		static std::vector<bool> BuildVirtualThreadStalledSnapshot(std::shared_ptr<CpuCoreDiagnosticSnapshot> core, int activeVtId, Processor::CPU_Core::PipelineControl const pipeState);

		static std::vector<std::wstring> BuildVirtualThreadStallReasons(std::shared_ptr<CpuCoreDiagnosticSnapshot> core, std::vector<bool> &stalled, Processor::CPU_Core::PipelineObservationSnapshot const observation);

		static std::vector<unsigned long long> BuildVirtualThreadRegisters(std::shared_ptr<CpuCoreDiagnosticSnapshot> core, int vtId);

		static bool IsVirtualThreadMemoryStalled(std::shared_ptr<CpuCoreDiagnosticSnapshot> core, int vtId);

		static CoreTimebaseSnapshot BuildCoreTimebaseSnapshot(Processor::CPU_Core::PipelineControl const pipeState);

//		#endregion

//		#region Pod and NoC Access

		/// <summary>
		/// Get Pod ID for a given core (Pod coordinates encoded as (X << 8) | Y).
		/// </summary>
	public:
		unsigned short GetCorePodId(int coreId);

		/// <summary>
		/// Get Pod coordinates (X, Y) for a given core.
		/// </summary>
// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		public (int X, int Y) GetCorePodCoordinates(int coreId)
//		{
//			lock (_syncLock)
//			{
//				ValidateCoreId(coreId);
//				int podIndex = coreId / 16;
//				if ((uint)podIndex < (uint)GetPodCount())
//				{
//					System.Nullable<PodController> pod = GetPodOrNull(podIndex);
//					if (pod != nullptr)
//					{
//						return (pod.PodX, pod.PodY);
//					}
//				}
//				return (0, 0);
//			}
//		}

		/// <summary>
		/// Get Pod barrier synchronization state for a core's Pod.
		/// Returns true if the core is waiting at a Pod barrier.
		/// </summary>
		bool GetPodBarrierState(int coreId);

		/// <summary>
		/// Get Pod controller snapshot for a given Pod index.
		/// </summary>
		PodSnapshot GetPodSnapshot(int podIndex);

//		#endregion

//		#region GRLB (Global Resource Lock Bitmap) Access

		/// <summary>
		/// Get GRLB snapshot for a core.
		/// Returns the current state of all 128 resource lock bits.
		/// </summary>
		GrlbSnapshot GetGrlbSnapshot(int coreId);

		/// <summary>
		/// Check if a specific resource bit is locked in GRLB.
		/// </summary>
		/// <param name="coreId">Core ID</param>
		/// <param name="resourceBit">Resource bit index (0-127)</param>
		/// <returns>True if resource is locked</returns>
		bool IsGrlbResourceLocked(int coreId, int resourceBit);

//		#endregion

//		#region FSP Power Controller Access

		/// <summary>
		/// Get FSP power controller snapshot for a core's Pod.
		/// </summary>
		FspPowerSnapshot GetFspPowerSnapshot(int coreId);

//		#endregion

//		#region Memory Subsystem Access

		/// <summary>
		/// Get memory subsystem snapshot with bank statistics.
		/// </summary>
		MemorySubsystemSnapshot GetMemorySubsystemSnapshot();

		/// <summary>
		/// Get memory configuration details.
		/// </summary>
		MemoryConfigSnapshot GetMemoryConfig();

		/// <summary>
		/// Get total memory size in bytes.
		/// </summary>
		long long GetTotalMemorySize();

//		#endregion

//		#region Multithreaded Compiler State Access

		/// <summary>
		/// Get compilation telemetry for multithreaded compilation.
		/// Returns an explicit unavailable snapshot when the current runtime contour
		/// does not publish live compiler telemetry.
		/// </summary>
		CompilerStateSnapshot GetCompilerStateSnapshot();

		/// <summary>
		/// Get per-thread compiler context state for a specific virtual thread.
		/// Returns an explicit unavailable snapshot when no live compiler context is published.
		/// </summary>
		ThreadCompilerStateSnapshot GetThreadCompilerState(int vtId);

		/// <summary>
		/// Get dependency graph state showing cross-thread dependencies.
		/// Returns an explicit unavailable snapshot when no live compiler dependency view is published.
		/// </summary>
		DependencyGraphSnapshot GetDependencyGraph();

		/// <summary>
		/// Get barrier scheduler state showing inserted barriers and their locations.
		/// Returns an explicit unavailable snapshot when no live barrier scheduler view is published.
		/// </summary>
		BarrierSchedulerSnapshot GetBarrierSchedulerState();

	private:
		static CoreStateSnapshot BuildCoreStateSnapshot(std::shared_ptr<IIseMachineStateSource> machineStateSource, int coreId);

		static PodSnapshot BuildPodSnapshot(std::shared_ptr<IIseMachineStateSource> machineStateSource, int podIndex);

//		#endregion


	public:
		std::shared_ptr<PerformanceReport> GetPerformanceReport();

		ReplayPhaseMetrics GetReplayPhaseMetrics(int coreId);

		SchedulerPhaseMetrics GetSchedulerPhaseMetrics(int coreId);

		std::optional<std::shared_ptr<TypedSlotTelemetryProfile>> GetTypedSlotTelemetryProfile(int coreId, const std::wstring &programHash);

		std::wstring GetReplayToken();


	private:
		const std::shared_ptr<IIseMachineStateSource> _machineStateSource;
		const std::shared_ptr<std::recursive_mutex> _syncLock;

	public:
		IseObservationService(std::shared_ptr<IIseMachineStateSource> machineStateSource, std::shared_ptr<std::recursive_mutex> syncLock);

		MachineStateSourceProvenance getSourceProvenance() const;

	private:
		int GetCoreCount();

		static int GetCoreCount(std::shared_ptr<IIseMachineStateSource> machineStateSource);

		std::shared_ptr<CpuCoreDiagnosticSnapshot> GetCoreSnapshot(int coreId);

		static std::shared_ptr<CpuCoreDiagnosticSnapshot> GetCoreSnapshot(std::shared_ptr<IIseMachineStateSource> machineStateSource, int coreId);

		int GetPodCount();

		static int GetPodCount(std::shared_ptr<IIseMachineStateSource> machineStateSource);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private PodController? GetPodOrNull(int podIndex)
		std::shared_ptr<PodController> GetPodOrNull(int podIndex);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static PodController? GetPodOrNull(IIseMachineStateSource machineStateSource, int podIndex)
		static std::shared_ptr<PodController> GetPodOrNull(std::shared_ptr<IIseMachineStateSource> machineStateSource, int podIndex);

		std::vector<unsigned char> ReadMachineMemory(unsigned long long address, int length);

		static std::vector<unsigned char> ReadMachineMemory(std::shared_ptr<IIseMachineStateSource> machineStateSource, unsigned long long address, int length);

		long long GetTotalMachineMemorySize();

		static long long GetTotalMachineMemorySize(std::shared_ptr<IIseMachineStateSource> machineStateSource);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MemorySubsystem? GetMemorySubsystem()
		std::shared_ptr<MemorySubsystem> GetMemorySubsystem();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static MemorySubsystem? GetMemorySubsystem(IIseMachineStateSource machineStateSource)
		static std::shared_ptr<MemorySubsystem> GetMemorySubsystem(std::shared_ptr<IIseMachineStateSource> machineStateSource);
	};
}
