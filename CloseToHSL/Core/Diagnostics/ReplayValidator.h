#pragma once

#include "V4TraceEvent.h"
#include <string>
#include <vector>
#include <cmath>
#include <format>
#include <memory>

// ─────────────────────────────────────────────────────────────────────────────
// HybridCPU ISA v4 — Diagnostics / Tracing
// Phase 11: Deterministic Replay and Trace Integration
// ─────────────────────────────────────────────────────────────────────────────


namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Result of a deterministic replay validation.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ReplayValidationResult
	class ReplayValidationResult
	{
	private:
		const bool IsDeterministic;
		const int ComparedEventCount;
		const int DivergenceIndex;
		const std::wstring DivergenceDescription;

		/// <summary><c>true</c> if replay produced an identical event sequence.</summary>
	public:
		bool isDeterministic() const;

		/// <summary>Number of events compared.</summary>
		int getComparedEventCount() const;

		/// <summary>
		/// Index of the first divergent event in the original trace, or -1 if no divergence.
		/// </summary>
		int getDivergenceIndex() const;

		/// <summary>
		/// Human-readable description of the divergence, or null if deterministic.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public string? DivergenceDescription {get;}
		std::wstring getDivergenceDescription() const;

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ReplayValidationResult(bool isDeterministic, int comparedEventCount, int divergenceIndex, string? divergenceDescription)
		ReplayValidationResult(bool isDeterministic, int comparedEventCount, int divergenceIndex, const std::wstring &divergenceDescription);

		/// <summary>Create a successful (deterministic) result.</summary>
	public:
		static ReplayValidationResult Deterministic(int comparedEventCount);

		/// <summary>Create a divergent (non-deterministic) result.</summary>
		static ReplayValidationResult Diverged(int comparedEventCount, int divergenceIndex, const std::wstring &divergenceDescription);

		/// <inheritdoc/>
		std::wstring ToString() const;

		ReplayValidationResult()
			: IsDeterministic(false), ComparedEventCount(0), DivergenceIndex(0), DivergenceDescription(L"")
		{
		}
	};

	/// <summary>
	/// Validates that a replayed trace is identical to the original trace
	/// up to the comparison point.
	/// <para>
	/// The <see cref="ReplayValidator"/> is the canonical correctness test for
	/// deterministic execution — its pass/fail is a hard gate for Phase 12.
	/// </para>
	/// <para>
	/// Comparison semantics: two <see cref="V4TraceEvent"/> records are equal when
	/// <see cref="V4TraceEvent.Kind"/>, <see cref="V4TraceEvent.VtId"/>,
	/// <see cref="V4TraceEvent.FsmState"/>, and <see cref="V4TraceEvent.Payload"/>
	/// are all equal. <see cref="V4TraceEvent.BundleSerial"/> is compared only after
	/// offsetting by the difference between the original and replay start serials.
	/// </para>
	/// </summary>
	class ReplayValidator final : public std::enable_shared_from_this<ReplayValidator>
	{
		/// <summary>
		/// Compare two v4 trace event sequences and return a validation result.
		/// </summary>
		/// <param name="original">
		/// The original trace, starting at the first event after the snapshot anchor.
		/// </param>
		/// <param name="replay">
		/// The replay trace, starting at the first event after the snapshot was restored.
		/// </param>
		/// <returns>
		/// A <see cref="ReplayValidationResult"/> describing whether the replay was
		/// deterministic.
		/// </returns>
	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public ReplayValidationResult ValidateTrace(IReadOnlyList<V4TraceEvent> original, IReadOnlyList<V4TraceEvent> replay)
		ReplayValidationResult ValidateTrace(const std::vector<V4TraceEvent> &original, const std::vector<V4TraceEvent> &replay);

		/// <summary>
		/// Compare two trace sequences, but only up to <paramref name="maxBundleCount"/> bundles.
		/// </summary>
		/// <param name="original">Original trace.</param>
		/// <param name="replay">Replay trace.</param>
		/// <param name="maxBundleCount">
		/// Maximum number of bundles to compare (comparison stops when this many
		/// <see cref="TraceEventKind.BundleRetired"/> events have been seen in the original).
		/// </param>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public ReplayValidationResult ValidateTrace(IReadOnlyList<V4TraceEvent> original, IReadOnlyList<V4TraceEvent> replay, int maxBundleCount)
		ReplayValidationResult ValidateTrace(const std::vector<V4TraceEvent> &original, const std::vector<V4TraceEvent> &replay, int maxBundleCount);

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static List<V4TraceEvent> CropToBundles(IReadOnlyList<V4TraceEvent> trace, int maxBundles)
		static std::vector<V4TraceEvent> CropToBundles(const std::vector<V4TraceEvent> &trace, int maxBundles);
	};
}
