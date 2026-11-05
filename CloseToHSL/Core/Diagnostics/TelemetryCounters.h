#pragma once

#include "V4TraceEvent.h"
#include <unordered_map>
#include <vector>
#include <memory>

// ─────────────────────────────────────────────────────────────────────────────
// HybridCPU ISA v4 — Diagnostics / Tracing
// Phase 11: Deterministic Replay and Trace Integration
// ─────────────────────────────────────────────────────────────────────────────

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Per-VT and global diagnostic telemetry counters for HybridCPU ISA v4.
	/// <para>
	/// Tracks architecturally significant event counts for telemetry export and
	/// counter CSR population (Cycle, BundleRet, InstrRet, VmExitCnt, BarrierCnt,
	/// StealCnt, ReplayCnt).
	/// </para>
	/// <para>
	/// This class is NOT thread-safe. Callers must serialize access when updating
	/// counters from multiple VTs in the same simulation step.
	/// </para>
	/// </summary>
	class TelemetryCounters final : public std::enable_shared_from_this<TelemetryCounters>
	{
	private:
		unsigned long long CycleCount = 0;
		unsigned long long BundleRetiredCount = 0;
		unsigned long long InstrRetiredCount = 0;
		unsigned long long VmExitCount = 0;
		unsigned long long BarrierCount = 0;
		unsigned long long StealCount = 0;
		unsigned long long ReplayCount = 0;

		/// <summary>Number of supported VTs (matches HybridCPU SMT width).</summary>
	public:
		static constexpr int VtCount = 4;

		// ─── Global counters ─────────────────────────────────────────────────

		/// <summary>Total cycles elapsed since reset.</summary>
		unsigned long long getCycleCount() const;
	private:
		void setCycleCount(unsigned long long value);

		/// <summary>Total bundles retired across all VTs.</summary>
	public:
		unsigned long long getBundleRetiredCount() const;
	private:
		void setBundleRetiredCount(unsigned long long value);

		/// <summary>Total instructions retired across all VTs.</summary>
	public:
		unsigned long long getInstrRetiredCount() const;
	private:
		void setInstrRetiredCount(unsigned long long value);

		/// <summary>Total VM exits (VMLAUNCH/VMRESUME → VMEXIT transitions).</summary>
	public:
		unsigned long long getVmExitCount() const;
	private:
		void setVmExitCount(unsigned long long value);

		/// <summary>Total barrier operations (POD_BARRIER + VT_BARRIER) across all VTs.</summary>
	public:
		unsigned long long getBarrierCount() const;
	private:
		void setBarrierCount(unsigned long long value);

		/// <summary>Total FSP slot pilfering operations across all VTs.</summary>
	public:
		unsigned long long getStealCount() const;
	private:
		void setStealCount(unsigned long long value);

		/// <summary>Total replay operations executed from snapshots.</summary>
	public:
		unsigned long long getReplayCount() const;
	private:
		void setReplayCount(unsigned long long value);

		// ─── Per-VT counters ─────────────────────────────────────────────────

		std::vector<unsigned long long> _amoDwordCountPerVt = std::vector<unsigned long long>(VtCount);
		std::vector<unsigned long long> _amoWordCountPerVt = std::vector<unsigned long long>(VtCount);
		std::vector<unsigned long long> _lrCountPerVt = std::vector<unsigned long long>(VtCount);
		std::vector<unsigned long long> _scSuccessCountPerVt = std::vector<unsigned long long>(VtCount);
		std::vector<unsigned long long> _scFailCountPerVt = std::vector<unsigned long long>(VtCount);
		std::vector<unsigned long long> _instrCountPerVt = std::vector<unsigned long long>(VtCount);

		// ─── Increment APIs ──────────────────────────────────────────────────

		/// <summary>Advance the cycle counter by one.</summary>
	public:
		void IncrementCycle();

		/// <summary>Advance the cycle counter by <paramref name="delta"/> cycles.</summary>
		void IncrementCycle(unsigned long long delta);

		/// <summary>Increment retired bundle count.</summary>
		void IncrementBundleRetired();

		/// <summary>Increment retired instruction count.</summary>
		void IncrementInstrRetired();

		/// <summary>
		/// Increment retired instruction count for a specific VT.
		/// </summary>
		/// <param name="vtId">VT ID (0–3).</param>
		void IncrementInstrRetiredForVt(unsigned char vtId);

		/// <summary>Increment VM exit count.</summary>
		void IncrementVmExit();

		/// <summary>
		/// Increment barrier count (for POD_BARRIER or VT_BARRIER).
		/// </summary>
		void IncrementBarrier();

		/// <summary>Increment FSP steal/pilfer count.</summary>
		void IncrementSteal();

		/// <summary>Increment replay operation count.</summary>
		void IncrementReplay();

		/// <summary>
		/// Increment AMO*_D (64-bit atomic) count for the given VT.
		/// </summary>
		/// <param name="vtId">VT ID (0–3).</param>
		void IncrementAmoDwordCount(unsigned char vtId);

		/// <summary>
		/// Increment AMO*_W (32-bit atomic) count for the given VT.
		/// </summary>
		/// <param name="vtId">VT ID (0–3).</param>
		void IncrementAmoWordCount(unsigned char vtId);

		/// <summary>
		/// Increment LR (load-reserved) count for the given VT.
		/// </summary>
		/// <param name="vtId">VT ID (0–3).</param>
		void IncrementLrCount(unsigned char vtId);

		/// <summary>
		/// Increment SC success count for the given VT.
		/// </summary>
		/// <param name="vtId">VT ID (0–3).</param>
		void IncrementScSuccess(unsigned char vtId);

		/// <summary>
		/// Increment SC failure count for the given VT.
		/// </summary>
		/// <param name="vtId">VT ID (0–3).</param>
		void IncrementScFail(unsigned char vtId);

		// ─── Per-VT read APIs ────────────────────────────────────────────────

		/// <summary>Total AMO*_D operations for VT <paramref name="vtId"/>.</summary>
		unsigned long long GetAmoDwordCount(unsigned char vtId);

		/// <summary>Total AMO*_W operations for VT <paramref name="vtId"/>.</summary>
		unsigned long long GetAmoWordCount(unsigned char vtId);

		/// <summary>Total LR operations for VT <paramref name="vtId"/>.</summary>
		unsigned long long GetLrCount(unsigned char vtId);

		/// <summary>Total SC successes for VT <paramref name="vtId"/>.</summary>
		unsigned long long GetScSuccessCount(unsigned char vtId);

		/// <summary>Total SC failures for VT <paramref name="vtId"/>.</summary>
		unsigned long long GetScFailCount(unsigned char vtId);

		/// <summary>Total instructions retired by VT <paramref name="vtId"/>.</summary>
		unsigned long long GetInstrCountForVt(unsigned char vtId);

		/// <summary>
		/// Total AMO*_D operations across all VTs.
		/// </summary>
		unsigned long long GetTotalAmoDwordCount();

		// ─── Counter dispatch from v4 trace events ───────────────────────────

		/// <summary>
		/// Update telemetry counters from a <see cref="V4TraceEvent"/>.
		/// Called by the v4 trace event recording path to keep counters in sync.
		/// </summary>
		/// <param name="evt">The trace event to account for.</param>
		void ApplyTraceEvent(V4TraceEvent evt);

		/// <summary>Reset all counters to zero.</summary>
		void Reset();

		/// <summary>
		/// Export all counters as a CSR snapshot compatible with
		/// <see cref="ReplaySnapshot.CsrSnapshot"/>.
		/// </summary>
		const std::unordered_map<unsigned short, long long> ExportAsCsrSnapshot();
	};
}
