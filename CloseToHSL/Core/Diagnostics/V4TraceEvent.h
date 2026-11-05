#pragma once

#include "../Architecture/State/Pipeline/PipelineState.h"
#include "TraceEventKind.h"
#include <string>
#include <format>

// ─────────────────────────────────────────────────────────────────────────────
// HybridCPU ISA v4 — Diagnostics / Tracing
// Phase 11: Deterministic Replay and Trace Integration
// ─────────────────────────────────────────────────────────────────────────────

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Lightweight v4 trace event record.
	/// <para>
	/// Every architecturally observable event is recorded as a <see cref="V4TraceEvent"/>
	/// stamped with the bundle serial, VT ID, and pipeline FSM state at the time of the
	/// event. This record is append-only — no events may be retroactively modified.
	/// </para>
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct V4TraceEvent
	class V4TraceEvent
	{
	private:
		unsigned long long BundleSerial = 0;
		unsigned char VtId = 0;
		PipelineState FsmState = static_cast<PipelineState>(0);
		TraceEventKind Kind = static_cast<TraceEventKind>(0);
		unsigned long long Payload = 0;

		/// <summary>Monotonically increasing bundle execution counter at event time.</summary>
	public:
		unsigned long long getBundleSerial() const;
		void setBundleSerial(unsigned long long value);

		/// <summary>Virtual thread ID (0–3) that produced this event.</summary>
		unsigned char getVtId() const;
		void setVtId(unsigned char value);

		/// <summary>Pipeline FSM state at the time of this event.</summary>
		PipelineState getFsmState() const;
		void setFsmState(PipelineState value);

		/// <summary>Classification of this event.</summary>
		TraceEventKind getKind() const;
		void setKind(TraceEventKind value);

		/// <summary>
		/// Optional payload for events that carry additional data.
		/// <list type="bullet">
		///   <item><see cref="TraceEventKind.VmExit"/> — VM-exit reason code.</item>
		///   <item><see cref="TraceEventKind.FsmTransition"/> — encoded as
		///     <c>(FromState &lt;&lt; 8) | ToState</c>.</item>
		///   <item><see cref="TraceEventKind.CsrRead"/> / <see cref="TraceEventKind.CsrWrite"/>
		///     — CSR address.</item>
		///   <item>All other events: 0.</item>
		/// </list>
		/// </summary>
		unsigned long long getPayload() const;
		void setPayload(unsigned long long value);

		/// <summary>
		/// Create a <see cref="V4TraceEvent"/> with no payload.
		/// </summary>
		static V4TraceEvent Create(unsigned long long bundleSerial, unsigned char vtId, PipelineState fsmState, TraceEventKind kind);

		/// <summary>
		/// Create a <see cref="V4TraceEvent"/> with an opaque payload.
		/// </summary>
		static V4TraceEvent Create(unsigned long long bundleSerial, unsigned char vtId, PipelineState fsmState, TraceEventKind kind, unsigned long long payload);

		/// <inheritdoc/>
		std::wstring ToString() const;
	};
}
