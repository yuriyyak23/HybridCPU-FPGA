#pragma once
namespace YAKSys_Hybrid_CPU::Core
{
	enum class SystemEventKind : unsigned char
	{
		/// <summary>FENCE вЂ" data-memory ordering barrier.</summary>
		Fence,
		/// <summary>FENCE.I вЂ" instruction-cache flush and memory ordering barrier.</summary>
		FenceI,
		/// <summary>ECALL вЂ" environment call trap.</summary>
		Ecall,
		/// <summary>EBREAK вЂ" debug breakpoint trap.</summary>
		Ebreak,
		/// <summary>MRET вЂ" return from machine-mode trap handler.</summary>
		Mret,
		/// <summary>SRET вЂ" return from supervisor-mode trap handler.</summary>
		Sret,
		/// <summary>WFI вЂ" wait-for-interrupt low-power stall.</summary>
		Wfi,
		/// <summary>WFE РІР‚вЂќ wait-for-event stall owned by the pipeline FSM.</summary>
		Wfe,
		/// <summary>SEV РІР‚вЂќ wake event owned by the pipeline FSM.</summary>
		Sev,
		/// <summary>YIELD РІР‚вЂќ cooperative SMT/VT yield point.</summary>
		Yield,
		/// <summary>POD_BARRIER РІР‚вЂќ pod-scoped SMT/VT barrier.</summary>
		PodBarrier,
		/// <summary>VT_BARRIER РІР‚вЂќ VT-scoped barrier.</summary>
		VtBarrier
	};

	// в"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђ
	// C17 (Checklist): SystemEventOrderGuarantee вЂ" drain/flush/order contract
	// в"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђ

	/// <summary>
	/// Describes the pipeline drain / memory-order guarantee required by a system
	/// event instruction before it may complete.
	///
	/// C17: System events carry mandatory drain/flush/order guarantees rather than
	/// being treated as ordinary ALU-like micro-ops.
	///
	/// <para>Guarantees are additive: higher values subsume lower ones.</para>
	/// </summary>
	enum class SystemEventOrderGuarantee : unsigned char
	{
		/// <summary>
		/// No ordering requirement beyond normal in-order commit.
		/// Used for instructions that carry no side effects (should be rare for system ops).
		/// </summary>
		None = 0,

		/// <summary>
		/// All preceding memory stores must be visible to all agents before the
		/// event completes.  Maps to the RISC-V FENCE predecessor/successor contract
		/// (FENCE W, W).
		/// C17: Memory-ordering drain for data fences.
		/// </summary>
		DrainStores = 1,

		/// <summary>
		/// All preceding memory operations (loads + stores) must complete before the
		/// event and no subsequent memory operations may start until after the event.
		/// Maps to FENCE R+W, R+W.
		/// C17: Full memory ordering barrier.
		/// </summary>
		DrainMemory = 2,

		/// <summary>
		/// All in-flight micro-ops must retire (pipeline fully drained) and the
		/// instruction cache must be flushed before any new instruction is fetched.
		/// Required for FENCE.I and all privilege-level transitions.
		/// C17: Full pipeline flush including I-cache.
		/// </summary>
		FlushPipeline = 3,

		/// <summary>
		/// Full pipeline flush plus architectural state serialisation.
		/// All writes visible; all preceding exceptions taken before the event handler
		/// takes effect.  Required for ECALL, EBREAK, MRET, SRET.
		/// C17: Strongest ordering вЂ" trap-entry / trap-return serialization contract.
		/// </summary>
		FullSerialTrapBoundary = 4
	};

}
