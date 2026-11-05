#pragma once

#include "ReplaySnapshot.h"
#include <limits>
#include <optional>
#include <memory>

// ─────────────────────────────────────────────────────────────────────────────
// HybridCPU ISA v4 — Diagnostics / Tracing
// Phase 11: Deterministic Replay and Trace Integration
// ─────────────────────────────────────────────────────────────────────────────

namespace YAKSys_Hybrid_CPU::Core
{
	class BundleMetadata;

	/// <summary>
	/// Evaluates whether a full <see cref="ReplaySnapshot"/> should be captured
	/// at the current bundle boundary.
	/// <para>
	/// Snapshot policy (priority order):
	/// <list type="number">
	///   <item><see cref="BundleMetadata.IsReplayAnchor"/> = <c>true</c> → always
	///     capture (<see cref="SnapshotTrigger.AnchorHint"/>).</item>
	///   <item>Bundle serial is a multiple of <see cref="PeriodicSnapshotInterval"/>
	///     → capture as periodic fallback (<see cref="SnapshotTrigger.Periodic"/>).</item>
	///   <item>Pipeline FSM transition occurred at this bundle serial
	///     → capture (<see cref="SnapshotTrigger.FsmTransition"/>).</item>
	///   <item>Otherwise → no snapshot.</item>
	/// </list>
	/// </para>
	/// <para>
	/// The <c>IsReplayAnchor</c> hint is a best-effort optimization from the compiler —
	/// the ISE must always capture periodic snapshots regardless of compiler hints.
	/// </para>
	/// </summary>
	class ReplayAnchorEvaluator final : public std::enable_shared_from_this<ReplayAnchorEvaluator>
	{
		/// <summary>
		/// Bundle serial period for periodic fallback snapshots.
		/// One snapshot is captured every <see cref="PeriodicSnapshotInterval"/> bundles.
		/// </summary>
	public:
		static constexpr unsigned long long PeriodicSnapshotInterval = 256;

	private:
		unsigned long long _lastFsmTransitionBundleSerial = std::numeric_limits<unsigned long long>::max();

		/// <summary>
		/// Notify the evaluator that a pipeline FSM state transition occurred
		/// at the given bundle serial. This will cause a snapshot to be captured
		/// at that bundle boundary.
		/// </summary>
		/// <param name="bundleSerial">Bundle serial at which the FSM transition occurred.</param>
	public:
		void NotifyFsmTransition(unsigned long long bundleSerial);

		/// <summary>
		/// Evaluate whether a <see cref="ReplaySnapshot"/> should be captured for the
		/// current bundle.
		/// </summary>
		/// <param name="meta">Bundle metadata for the current bundle.</param>
		/// <param name="bundleSerial">Monotonically increasing bundle execution counter.</param>
		/// <returns>
		/// The <see cref="SnapshotTrigger"/> indicating why a snapshot should be captured,
		/// or <c>null</c> if no snapshot is required.
		/// </returns>
		std::optional<SnapshotTrigger> ShouldCaptureSnapshot(std::shared_ptr<BundleMetadata> meta, unsigned long long bundleSerial);

		/// <summary>
		/// Reset the FSM transition tracking (e.g., after a hard reset).
		/// </summary>
		void Reset();
	};
}
