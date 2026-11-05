#pragma once
#include <vector>
#include <array>

#include <memory>

namespace HybridCPU_ISE::CloseToHSL::Memory::MMU
{
	/// <summary>
	/// Hardware Page Table Walker (PTW) — autonomous FSM that walks
	/// 2-level page tables on TLB miss, eliminating costly software trap handlers.
	///
	/// <para><b>Microarchitectural contract:</b></para>
	/// <list type="bullet">
	///   <item>On TLB miss: stalls requesting thread, starts walk.</item>
	///   <item>Walk uses dedicated DMA port (no contention with data DMA).</item>
	///   <item>On success: fills TLB, unstalls thread, replays access.</item>
	///   <item>On fault: triggers page fault exception on thread.</item>
	/// </list>
	///
	/// <para><b>HLS characteristics:</b></para>
	/// <list type="bullet">
	///   <item>7-state FSM → 3-bit state register → 1 LUT chain per transition.</item>
	///   <item>Dedicated AXI4-Lite master port (address + 64-bit data), not shared with data DMA.</item>
	///   <item>Walk latency: 2 × <see cref="MEMORY_READ_LATENCY"/> = 8 cycles worst-case.</item>
	///   <item>Pipelined throughput: 1 walk / 8 cycles.</item>
	///   <item>Queue depth: <see cref="MAX_PENDING_WALKS"/> entries (expandable for MSHR-like logic).</item>
	///   <item>Zero heap allocation — <see cref="InlineArray"/>-backed fixed storage.</item>
	///   <item>Combinatorial depth: 2 levels (state decode + next-state mux).</item>
	///   <item>Estimated area: ~120 LUT + 1 BRAM (for pending queue, if expanded).</item>
	/// </list>
	/// </summary>
	class PageTableWalker
	{
	private:
		unsigned long long WalksCompleted = 0;
		unsigned long long WalksFaulted = 0;
		unsigned long long TotalWalkCycles = 0;

		// ── FSM states ──────────────────────────────────────────────

		/// <summary>
		/// PTW FSM states (3-bit encoding in hardware).
		/// </summary>
	public:
		enum class PtwState : unsigned char
		{
			/// <summary>Idle — no walk in progress.</summary>
			Idle = 0,

			/// <summary>Reading Page Directory Entry (level 1) — issuing DMA read.</summary>
			ReadPDE = 1,

			/// <summary>Waiting for PDE memory response (MEMORY_READ_LATENCY cycles).</summary>
			WaitPDE = 2,

			/// <summary>Reading Page Table Entry (level 2) — issuing DMA read.</summary>
			ReadPTE = 3,

			/// <summary>Waiting for PTE memory response (MEMORY_READ_LATENCY cycles).</summary>
			WaitPTE = 4,

			/// <summary>Walk complete — result ready for TLB fill.</summary>
			Complete = 5,

			/// <summary>Fault — page not present or permission denied.</summary>
			Fault = 6
		};

		// ── Walk request descriptor ─────────────────────────────────

		/// <summary>
		/// Walk request queued by IOMMU on TLB miss.
		/// HLS: 168 bits (VA 64 + ThreadId 32 + DomainId 32 + IsWrite 1 + Valid 1 + pad).
		/// </summary>
	public:
		class WalkRequest
		{
			/// <summary>Virtual address that caused TLB miss.</summary>
		public:
			unsigned long long VirtualAddress = 0;

			/// <summary>Hardware thread ID (0–15) that issued the access.</summary>
			int ThreadId = 0;

			/// <summary>Memory domain / ASID for page table root lookup.</summary>
			int DomainId = 0;

			/// <summary>True if the original access was a write (for permission check).</summary>
			bool IsWrite = false;

			/// <summary>Valid bit — entry is active in the queue.</summary>
			bool Valid = false;
		};

		// ── HLS-compatible pending queue (InlineArray, fixed 4 entries) ─

		/// <summary>Max pending walk requests (MSHR-like, expandable).</summary>
	private:
		static constexpr int MAX_PENDING_WALKS = 4;

	private:
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [InlineArray(MAX_PENDING_WALKS)] private struct PendingWalkArray
		using PendingWalkArray = std::array<WalkRequest, MAX_PENDING_WALKS>;

		// ── Timing constants ────────────────────────────────────────

		/// <summary>
		/// Cycles to read one level of page table via dedicated DMA port.
		/// Matches AXI4-Lite single-beat latency (header + data + response).
		/// </summary>
	private:
		static constexpr int MEMORY_READ_LATENCY = 4;

		// ── Instance state ──────────────────────────────────────────

		PtwState _state = static_cast<PtwState>(0);
		WalkRequest _currentRequest;

		// Intermediate walk state
		unsigned long long _pdeAddress = 0;
		unsigned long long _pdeValue = 0;
		unsigned long long _pteAddress = 0;
		int _waitCycles = 0;

		// Pending request queue
		PendingWalkArray _pendingQueue;
		int _pendingHead = 0;
		int _pendingCount = 0;

		// ── Statistics ──────────────────────────────────────────────

		/// <summary>Number of walks completed successfully (TLB filled).</summary>
	public:
		unsigned long long getWalksCompleted() const;
	private:
		void setWalksCompleted(unsigned long long value);

		/// <summary>Number of walks that ended in page fault.</summary>
	public:
		unsigned long long getWalksFaulted() const;
	private:
		void setWalksFaulted(unsigned long long value);

		/// <summary>Total cycles spent in non-Idle state (walk overhead).</summary>
	public:
		unsigned long long getTotalWalkCycles() const;
	private:
		void setTotalWalkCycles(unsigned long long value);

		// ── Public properties ───────────────────────────────────────

		/// <summary>True if PTW is busy with a walk.</summary>
	public:
		bool isBusy() const;

		/// <summary>Current FSM state (for diagnostics / TraceSink).</summary>
		PtwState getCurrentState() const;

		/// <summary>Thread ID currently stalled by PTW, or -1 if idle.</summary>
		int getStalledThreadId() const;

		/// <summary>Number of pending requests waiting in queue.</summary>
		int getPendingCount() const;

		// ── Walk result (returned by AdvanceCycle) ──────────────────

		/// <summary>
		/// Result of a single PTW FSM cycle advancement.
		/// </summary>
	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct WalkResult
		class WalkResult
		{
			/// <summary>True if walk finished (success or fault) this cycle.</summary>
		public:
			const bool Done;

			/// <summary>True if walk ended in page fault (only valid when Done=true).</summary>
			const bool Faulted;

			/// <summary>Thread ID that requested this walk (-1 if no event).</summary>
			const int ThreadId;

			/// <summary>Translated physical address (only valid when Done=true and Faulted=false).</summary>
			const unsigned long long PhysicalAddress;

			/// <summary>Permission bits from PTE (bit 1=Read, bit 2=Write).</summary>
			const unsigned char Permissions;

			/// <summary>Virtual address that was walked (for TLB insertion by caller).</summary>
			const unsigned long long VirtualAddress;

			/// <summary>Domain ID for TLB insertion.</summary>
			const int DomainId;

			WalkResult(bool done, bool faulted, int threadId, unsigned long long physAddr, unsigned char perms, unsigned long long virtualAddress, int domainId);

			/// <summary>No-event result (idle cycle).</summary>
			static const WalkResult NoEvent;

			WalkResult() : Done(false), Faulted(false), ThreadId(0), PhysicalAddress(0), Permissions(0), VirtualAddress(0), DomainId(0)
			{
			}
		};

		// ── Public API ──────────────────────────────────────────────

		/// <summary>
		/// Submit walk request (called by IOMMU on TLB miss).
		/// If PTW is idle, starts immediately. Otherwise queues the request.
		/// </summary>
		/// <param name="virtualAddress">Virtual address that missed TLB.</param>
		/// <param name="threadId">Requesting hardware thread (0–15).</param>
		/// <param name="domainId">Memory domain / ASID for page table root.</param>
		/// <param name="isWrite">True if the original access was a write.</param>
		/// <returns>True if accepted (started or queued), false if queue full.</returns>
	public:
		bool SubmitWalk(unsigned long long virtualAddress, int threadId, int domainId, bool isWrite);

		/// <summary>
		/// Advance PTW FSM by one clock cycle.
		/// Called from MemorySubsystem.AdvanceCycles() or IOMMU tick.
		///
		/// <para><b>HLS:</b> single always_ff block, 3-bit state register,
		/// combinatorial next-state logic depth ≤ 2.</para>
		/// </summary>
		/// <param name="pageDirectory">
		/// Reference to IOMMU page directory for synchronous page table reads.
		/// In hardware this would be the dedicated PTW DMA port response.
		/// </param>
		/// <returns>Walk result for this cycle.</returns>
		WalkResult AdvanceCycle(std::shared_ptr<std::vector<std::shared_ptr<std::vector<unsigned long long>>>> pageDirectory);

		/// <summary>
		/// Reset PTW state (on IOMMU init or global TLB flush).
		/// </summary>
		void Reset();

		// ── Internal helpers ────────────────────────────────────────

		/// <summary>
		/// Begin a walk for the current request.
		/// Calculates PDE address from domain-indexed page table root.
		/// </summary>
	private:
		void BeginWalk();

		/// <summary>
		/// Try to dequeue next pending request and start walk.
		/// Returns NoEvent if nothing pending.
		/// HLS: single-cycle dequeue with head pointer increment.
		/// </summary>
		WalkResult TryDequeue();
	};
}
