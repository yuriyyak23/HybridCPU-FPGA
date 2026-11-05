#pragma once

#include "../SlotLegality/SlotClassCapacity.h"
#include <array>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Retained compatibility power controller for donor-idleness gating.
	///
	/// <para>Monitors inter-core donor nomination patterns and gates clocks for idle donor threads.
	/// The type keeps its legacy FSP name for retained compatibility, but the live
	/// semantics are donor-idleness gating after work donation.</para>
	///
	/// <para><b>Microarchitectural contract:</b></para>
	/// <list type="bullet">
	///   <item>Gating decision uses hysteresis: thread must be idle for
	///         <see cref="IDLE_HYSTERESIS_CYCLES"/> consecutive cycles before gating.</item>
	///   <item>Wakeup latency: 1 cycle (just re-enable clock AND-gate).</item>
	///   <item>No architectural state is lost — pipeline stages are gated, not flushed.</item>
	///   <item>Execute/MEM/WB remain active for commit of previously stolen ops.</item>
	/// </list>
	///
	/// <para><b>HLS characteristics:</b></para>
	/// <list type="bullet">
	///   <item>16 × 1-bit clock enable → 16 AND-gates on clock tree input
	///         (standard ICG cell pattern).</item>
	///   <item>Hysteresis counters: 16 × 4-bit → 64 bits total → 1 LUTRAM.</item>
	///   <item>Wakeup: single-cycle — deassert AND-gate on clock, no pipeline flush.</item>
	///   <item>Estimated power savings: ~30% dynamic power per gated thread
	///         (Fetch + Decode + I-cache access disabled).</item>
	///   <item>Zero heap allocation — <see cref="InlineArray"/>-backed fixed storage.</item>
	///   <item>Combinatorial depth: 1 level (comparator + mux per thread).</item>
	///   <item>Estimated area: ~80 LUT (counters + comparators + gate logic).</item>
	/// </list>
	/// </summary>
	class FspPowerController
	{
	private:
		unsigned long long TotalGatedCycles = 0;
		unsigned long long GateTransitions = 0;
		unsigned long long ClassCapacityEarlyGateCycles = 0;

		/// <summary>Number of hardware threads per Pod.</summary>
		static constexpr int MAX_THREADS = 16;

		/// <summary>
		/// Hysteresis threshold: thread must be idle for N consecutive cycles
		/// before gating. Prevents thrashing on short idle periods.
		/// HLS: 4-bit comparator per thread (threshold fits in 4 bits).
		/// </summary>
	public:
		static constexpr int IDLE_HYSTERESIS_CYCLES = 8;

		/// <summary>
		/// Fraction of per-thread dynamic power saved by Fetch/Decode gating.
		/// Based on typical VLIW pipeline power breakdown: Fetch ~15%, Decode ~15%.
		/// </summary>
	private:
		static constexpr double FETCH_DECODE_POWER_FRACTION = 0.30;

		// ── HLS-compatible storage (InlineArray, zero heap) ─────────

		/// <summary>Per-thread idle cycle counter (4-bit range sufficient for hysteresis ≤ 15).</summary>
	private:
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [InlineArray(MAX_THREADS)] private struct IdleCounterArray
		using IdleCounterArray = std::array<int, MAX_THREADS>;

		/// <summary>Per-thread clock gate state (true = gated/Fetch+Decode OFF).</summary>
	private:
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [InlineArray(MAX_THREADS)] private struct GateStateArray
		using GateStateArray = std::array<bool, MAX_THREADS>;

	private:
		IdleCounterArray _idleCounters;
		GateStateArray _gateStates;

		// ── Statistics ──────────────────────────────────────────────

		/// <summary>Total thread-cycles spent in gated state (across all threads).</summary>
	public:
		unsigned long long getTotalGatedCycles() const;
	private:
		void setTotalGatedCycles(unsigned long long value);

		/// <summary>Number of gate/ungate transitions (for thrashing detection).</summary>
	public:
		unsigned long long getGateTransitions() const;
	private:
		void setGateTransitions(unsigned long long value);

		/// <summary>
		/// Phase 06: Number of cycles where class-capacity exhaustion triggered
		/// early power-gating because the compatibility-era densification path has no remaining class-flexible admission opportunity.
		/// <para>HLS: 64-bit diagnostic counter — not on critical path.</para>
		/// </summary>
	public:
		unsigned long long getClassCapacityEarlyGateCycles() const;
	private:
		void setClassCapacityEarlyGateCycles(unsigned long long value);

		/// <summary>Number of threads currently gated (snapshot).</summary>
	public:
		int getCurrentGatedCount() const;

		// ── Core API ────────────────────────────────────────────────

		/// <summary>
		/// Update idle counters based on the visible inter-core donor-ready mask.
		/// Called every cycle from <see cref="PodController.BeginCycle"/>.
		///
		/// <para><b>Logic per thread:</b></para>
		/// <list type="bullet">
		///   <item>If port is valid (thread has work) → reset idle counter, ungate immediately.</item>
		///   <item>If port is invalid (thread donated everything / idle) → increment counter.</item>
		///   <item>If counter ≥ <see cref="IDLE_HYSTERESIS_CYCLES"/> → gate the thread.</item>
		/// </list>
		///
		/// HLS: 16 parallel comparators + AND-gate updates, single cycle.
		/// </summary>
		/// <param name="readyMask">16-bit visible nomination ready mask from <see cref="MicroOpScheduler"/>.</param>
		void UpdateFromFsp(unsigned short readyMask);

		/// <summary>
		/// Phase 06: Update idle counters with class-capacity-aware early power-gating.
		///
		/// <para>Extends <see cref="UpdateFromFsp"/> with an additional gating signal:
		/// when <paramref name="classCapacity"/> shows no free capacity in any
		/// class-flexible class (ALU, LSU), the compatibility-era densification path has no useful admission opportunity left.
		/// In this state all threads with valid ports are still treated as idle
		/// for power-gating purposes.</para>
		///
		/// <para><b>Logic:</b></para>
		/// <list type="bullet">
		///   <item>If class-capacity exhausted → override port validity to false for all threads
		///         (no class-flexible lanes remain for densification, so threads are effectively idle).</item>
		///   <item>Otherwise → delegate to <see cref="UpdateFromFsp"/>.</item>
		/// </list>
		///
		/// HLS: 2 additional comparators (ALU free, LSU free) → 1 AND-gate → override mux.
		/// </summary>
		/// <param name="readyMask">16-bit visible nomination ready mask from <see cref="MicroOpScheduler"/>.</param>
		/// <param name="classCapacity">Current class-capacity snapshot from MicroOpScheduler.</param>
		void UpdateFromFspWithClassCapacity(unsigned short readyMask, SlotClassCapacityState classCapacity);

		/// <summary>
		/// Phase 06: Check if all class-flexible slot classes have exhausted capacity.
		/// When true, no class-flexible densification opportunity remains.
		/// <para>
		/// Checks ALU and LSU classes (the two multi-lane class-flexible classes).
		/// Single-lane classes (DmaStream, Branch, System) are not checked because
		/// they are typically HardPinned and do not benefit from class-flexible densification.
		/// </para>
		/// <para>HLS: 2 comparators (ALU free, LSU free) → 1 NOR gate.</para>
		/// </summary>
		inline static bool IsClassCapacityExhausted(SlotClassCapacityState classCapacity);

		/// <summary>
		/// Check if a thread's Fetch/Decode should be clock-gated.
		/// True = skip Fetch/Decode for this thread (power saving mode).
		/// Execute/MEM/WB remain active for commit of stolen ops.
		/// HLS: single AND-gate read, zero combinatorial depth.
		/// </summary>
		/// <param name="threadId">Hardware thread index (0–15).</param>
		/// <returns>True if thread is clock-gated.</returns>
		inline bool IsThreadGated(int threadId) const;

		/// <summary>
		/// Force-wake a thread (interrupt, new work arrival, barrier release).
		/// Immediate ungate with 1-cycle wakeup latency.
		/// HLS: single register write + counter clear.
		/// </summary>
		/// <param name="threadId">Hardware thread index (0–15).</param>
		inline void WakeThread(int threadId);

		/// <summary>
		/// Force-wake all threads (global event, e.g., full barrier release or reset).
		/// </summary>
		void WakeAll();

		/// <summary>
		/// Get estimated power savings ratio (0.0–1.0) over the observation window.
		/// Based on the fraction of thread-cycles saved by Fetch/Decode gating.
		/// </summary>
		/// <param name="totalCycles">Total simulation cycles elapsed.</param>
		/// <returns>Estimated fraction of total power saved (0.0 = no savings).</returns>
		double GetPowerSavingsRatio(unsigned long long totalCycles) const;

		/// <summary>
		/// Reset all state (init or full pipeline flush).
		/// </summary>
		void Reset();
	};
}
