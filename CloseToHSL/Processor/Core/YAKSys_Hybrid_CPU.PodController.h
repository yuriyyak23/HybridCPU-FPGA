#pragma once

#include "../../Memory/Cache/SharedPodCache.h"
#include "../../Core/Pipeline/Assist/Runtime/AssistRuntime.h"
namespace YAKSys_Hybrid_CPU::Core { class MicroOpScheduler; }
#include "../../Core/Pipeline/Scheduling/Fsp/FspPowerController.h"
namespace YAKSys_Hybrid_CPU::Memory { class MemorySubsystem; }
#include "../../Core/Pipeline/MicroOps/Types/MicroOp.h"
#include "../../Core/Pipeline/Safety/ResourceBitset.h"
#include <vector>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core;

using namespace HybridCPU_ISE::CloseToHSL::Memory::Cache;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU
{
	/// <summary>
	/// Pod Controller — orchestrates 16 CPU cores within a single Pod (tech.md §1).
	///
	/// Responsibilities:
	/// - Manage local FSP arbitration scope (intra-pod only, no inter-pod FSP)
	/// - Route local L2 bank accesses
	/// - Implement pod-level barrier synchronization (tech.md §4)
	/// - Provide XY coordinates for NoC addressing (tech.md §2)
	///
	/// HLS design constraints:
	/// - Fixed-size arrays only (no dynamic allocation)
	/// - Deterministic iteration order for all scans
	/// - All state fits in register file (no BRAM for control path)
	/// </summary>
	class PodController : public std::enable_shared_from_this<PodController>
	{
	private:
		const int PodX;
		const int PodY;
		unsigned int BarrierTimeoutThreshold = 10000;
		unsigned long long DomainCertificate = 0;
		const std::shared_ptr<SharedPodCache> L2Cache;

	public:
		bool TryPeekInterCoreAssistTransport(int localCoreId, AssistInterCoreTransport &transport);

		bool TryConsumeInterCoreAssistTransport(int localCoreId, AssistInterCoreTransport &transport);

		void ClearInterCoreAssistTransport(int localCoreId);


	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AssistOwnerSnapshot
		class AssistOwnerSnapshot
		{
		private:
			const bool IsPublished;
			const int VirtualThreadId;
			const int OwnerContextId;
			const unsigned long long DomainTag;
			const unsigned long long AssistEpochId;

		public:
			AssistOwnerSnapshot(int virtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned long long assistEpochId);
			AssistOwnerSnapshot(const AssistOwnerSnapshot &) = default;
			AssistOwnerSnapshot &operator=(const AssistOwnerSnapshot &other);

			bool isPublished() const;

			int getVirtualThreadId() const;

			int getOwnerContextId() const;

			unsigned long long getDomainTag() const;

			unsigned long long getAssistEpochId() const;

			bool isValid() const;

			bool Matches(int virtualThreadId, int ownerContextId, unsigned long long domainTag) const;

			AssistOwnerSnapshot() : IsPublished(false), VirtualThreadId(0), OwnerContextId(0), DomainTag(0), AssistEpochId(0) {}
		};

	private:
		std::vector<AssistOwnerSnapshot> _assistOwnerSnapshots = CreateEmptyAssistOwnerSnapshots();

	public:
		void PublishAssistOwnerSnapshot(int localCoreId, int virtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned long long assistEpochId = 0);

		void InvalidateAssistOwnerSnapshot(int localCoreId);

		bool TryGetAssistOwnerSnapshot(int localCoreId, AssistOwnerSnapshot &snapshot);

	private:
		static std::vector<AssistOwnerSnapshot> CreateEmptyAssistOwnerSnapshots();


		/// <summary>
		/// Number of cores per Pod (hardware constant)
		/// </summary>
	public:
		static constexpr int CORES_PER_POD = 16;

		/// <summary>
		/// Pod X coordinate in 2D-Mesh NoC (0–7 for 8×8 grid = 64 pods)
		/// </summary>
		int getPodX() const;

		/// <summary>
		/// Pod Y coordinate in 2D-Mesh NoC (0–7 for 8×8 grid = 64 pods)
		/// </summary>
		int getPodY() const;

		/// <summary>
		/// Composite Pod ID: (X &lt;&lt; 8) | Y, written to CSR POD_ID (0xB00)
		/// </summary>
		unsigned short getPodId() const;

		/// <summary>
		/// Pod-local FSP scheduler (manages nomination ports for 16 cores)
		/// </summary>
	private:
		const std::shared_ptr<MicroOpScheduler> _scheduler;

		/// <summary>
		/// Barrier synchronization: bitmask of cores that have reached the barrier.
		/// Bit i = 1 means core i has executed POD.BARRIER instruction.
		/// When (BarrierReached & mask) == mask, all participating cores are synchronized.
		/// HLS: single 16-bit register.
		/// </summary>
		unsigned short _barrierReachedMask = 0;

		/// <summary>
		/// Barrier timeout counter — increments each cycle while barrier is active.
		/// If it exceeds BarrierTimeoutThreshold, a deadlock warning is raised.
		/// </summary>
		unsigned int _barrierTimeoutCounter = 0;

		/// <summary>
		/// Maximum cycles to wait for barrier before triggering deadlock mitigation.
		/// Configurable via CSR, default = 10000 cycles.
		/// </summary>
	public:
		unsigned int getBarrierTimeoutThreshold() const;
		void setBarrierTimeoutThreshold(unsigned int value);

		/// <summary>
		/// Domain certificate for this Pod (Singularity-style isolation, tech.md §3).
		/// Used by TryStealSlot to filter nominations by domain membership.
		/// Written from CSR MEM_DOMAIN_CERT (0xB02).
		/// </summary>
		unsigned long long getDomainCertificate() const;
		void setDomainCertificate(unsigned long long value);

		/// <summary>
		/// L2 Shared Pod Cache — shared among 16 cores within this Pod (req.md §2).
		/// On L2 miss, the core's slots become FSP donor-eligible.
		/// </summary>
		std::shared_ptr<SharedPodCache> getL2Cache() const;

		/// <summary>
		/// FSP-driven power controller for Dark Silicon optimization (Plan 10).
		/// Monitors donation patterns and gates Fetch/Decode clocks for idle donors.
		/// HLS: 16 × 1-bit clock enable + 16 × 4-bit hysteresis counters.
		/// </summary>
		FspPowerController PowerController;

		PodController(int podX, int podY, std::shared_ptr<MicroOpScheduler> scheduler);

		/// <summary>
		/// Check if pod barrier has been reached by all cores in the affinity mask.
		/// Called by CPU_Core.FSM in WAIT_FOR_CLUSTER_SYNC state (tech.md §4).
		/// </summary>
		/// <param name="affinityMask">Bitmask of cores that must reach barrier (from CSR POD_AFFINITY_MASK)</param>
		/// <returns>True if all cores in mask have reached the barrier</returns>
		bool IsBarrierReached(unsigned short affinityMask);

		/// <summary>
		/// Signal that a core has reached the pod barrier.
		/// Called when a core executes POD.BARRIER instruction.
		/// </summary>
		/// <param name="localCoreId">Core index within Pod (0–15)</param>
		void SignalBarrier(int localCoreId);

		/// <summary>
		/// Reset barrier state. Called when barrier is released or on interrupt preemption.
		/// </summary>
		void ResetBarrier();

		/// <summary>
		/// Tick barrier timeout. Returns true if timeout has been reached (deadlock warning).
		/// Called each cycle while any core is in WAIT_FOR_CLUSTER_SYNC.
		/// </summary>
		bool TickBarrierTimeout();

		/// <summary>
		/// Begin a scheduling cycle: clear nomination ports for fresh nominations.
		/// Called at the top of each pipeline cycle before cores nominate candidates.
		/// Also advances L2 cache cycle counter, propagates miss-pending flags,
		/// and updates FSP power gating state (Plan 10).
		/// </summary>
		void BeginCycle();

		/// <summary>
		/// Pack a VLIW bundle with FSP slot stealing using the Pod's scheduler.
		/// This is the main entry point for FSP from the pipeline.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public MicroOp[] PackBundle(System.Collections.Generic.IReadOnlyList<MicroOp?> originalBundle, int currentThreadId, bool stealEnabled, byte stealMask, ResourceBitset globalResourceLocks = default, byte donorMask = 0, int localCoreId = -1, int bundleOwnerContextId = -1, ulong bundleOwnerDomainTag = 0, ulong assistRuntimeEpoch = 0, System.Nullable<MemorySubsystem> memSub = null, System.Nullable<PodController>[]? pods = null)
		std::vector<std::shared_ptr<MicroOp>> PackBundle(const std::vector<std::shared_ptr<MicroOp>> &originalBundle, int currentThreadId, bool stealEnabled, unsigned char stealMask, ResourceBitset globalResourceLocks = YAKSys_Hybrid_CPU::Core::ResourceBitset(), unsigned char donorMask = 0, int localCoreId = -1, int bundleOwnerContextId = -1, unsigned long long bundleOwnerDomainTag = 0, unsigned long long assistRuntimeEpoch = 0, std::shared_ptr<MemorySubsystem> memSub = nullptr, std::shared_ptr<std::vector<std::shared_ptr<PodController>>> pods = nullptr);

		/// <summary>
		/// Pack a VLIW bundle with intra-core SMT slot stealing using the Pod's scheduler.
		/// This path is used when runtime wants VT-local stealing rather than inter-core donation.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public MicroOp[] PackBundleIntraCoreSmt(System.Collections.Generic.IReadOnlyList<MicroOp?> originalBundle, int ownerVirtualThreadId, int localCoreId, byte eligibleVirtualThreadMask, System.Nullable<MemorySubsystem> memSub = null)
		std::vector<std::shared_ptr<MicroOp>> PackBundleIntraCoreSmt(const std::vector<std::shared_ptr<MicroOp>> &originalBundle, int ownerVirtualThreadId, int localCoreId, unsigned char eligibleVirtualThreadMask, std::shared_ptr<MemorySubsystem> memSub = nullptr);

		/// <summary>
		/// Get the underlying scheduler for direct access (test/debug).
		/// </summary>
		std::shared_ptr<MicroOpScheduler> getScheduler() const;
	};
}
