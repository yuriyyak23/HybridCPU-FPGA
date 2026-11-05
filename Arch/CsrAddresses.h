#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// HybridCPU ISA v4 — Canonical CSR Address Space
// Phase 08: CSR Layer
// ─────────────────────────────────────────────────────────────────────────────

namespace YAKSys_Hybrid_CPU::Arch
{
	/// <summary>
	/// HybridCPU ISA v4 canonical CSR address assignments.
	/// <para>
	/// CSR addresses are 12-bit (0x000–0xFFF).  This class defines all groups:
	/// <list type="bullet">
	///   <item>Supervisor-mode system registers (0x100–0x144)</item>
	///   <item>Machine-mode privilege/system registers (0x300–0x344)</item>
	///   <item>VT / SMT scheduling-visible registers (0x800–0x803)</item>
	///   <item>Capability / configuration registers (0x810–0x814)</item>
	///   <item>VMX virtualisation control registers (0x820–0x824)</item>
	///   <item>Pod / NoC cluster registers (0xB00–0xB03)</item>
	///   <item>Performance counters (0xC00–0xC06)</item>
	/// </list>
	/// </para>
	/// </summary>
	class CsrAddresses final
	{
		// ═══════════════════════════════════════════════════════════
		// Privilege / System — Machine-mode (0x300–0x344)
		// ═══════════════════════════════════════════════════════════

		/// <summary>Machine status register.</summary>
	public:
		static constexpr unsigned short Mstatus = 0x300;
		/// <summary>ISA and extensions.</summary>
		static constexpr unsigned short Misa = 0x301;
		/// <summary>Machine interrupt enable.</summary>
		static constexpr unsigned short Mie = 0x304;
		/// <summary>Machine trap-handler base address.</summary>
		static constexpr unsigned short Mtvec = 0x305;
		/// <summary>Machine scratch register.</summary>
		static constexpr unsigned short Mscratch = 0x340;
		/// <summary>Machine exception program counter.</summary>
		static constexpr unsigned short Mepc = 0x341;
		/// <summary>Machine trap cause.</summary>
		static constexpr unsigned short Mcause = 0x342;
		/// <summary>Machine bad address or instruction.</summary>
		static constexpr unsigned short Mtval = 0x343;
		/// <summary>Machine interrupt pending.</summary>
		static constexpr unsigned short Mip = 0x344;
		/// <summary>Machine power-state status/control CSR.</summary>
		static constexpr unsigned short MpowerState = 0x345;
		/// <summary>Machine performance-level status/control CSR.</summary>
		static constexpr unsigned short MperfLevel = 0x346;

		// ═══════════════════════════════════════════════════════════
		// Supervisor Mode — S-mode (0x100–0x144)
		// ═══════════════════════════════════════════════════════════

		/// <summary>Supervisor status register.</summary>
		static constexpr unsigned short Sstatus = 0x100;
		/// <summary>Supervisor interrupt enable.</summary>
		static constexpr unsigned short Sie = 0x104;
		/// <summary>Supervisor trap-handler base address.</summary>
		static constexpr unsigned short Stvec = 0x105;
		/// <summary>Supervisor scratch register.</summary>
		static constexpr unsigned short Sscratch = 0x140;
		/// <summary>Supervisor exception program counter.</summary>
		static constexpr unsigned short Sepc = 0x141;
		/// <summary>Supervisor trap cause.</summary>
		static constexpr unsigned short Scause = 0x142;
		/// <summary>Supervisor bad address or instruction.</summary>
		static constexpr unsigned short Stval = 0x143;
		/// <summary>Supervisor interrupt pending.</summary>
		static constexpr unsigned short Sip = 0x144;

		// ═══════════════════════════════════════════════════════════
		// VT / SMT Scheduling-Visible Architectural State (0x800–0x803)
		// ═══════════════════════════════════════════════════════════

		/// <summary>Current virtual thread ID (0–3 for 4-way SMT).</summary>
		static constexpr unsigned short VtId = 0x800;
		/// <summary>Virtual thread active mask (bitmask of active VTs).</summary>
		static constexpr unsigned short VtMask = 0x801;
		/// <summary>VT execution status (running/waiting/blocked).</summary>
		static constexpr unsigned short VtStatus = 0x802;
		/// <summary>VT-local trap cause.</summary>
		static constexpr unsigned short VtCause = 0x803;

		// ═══════════════════════════════════════════════════════════
		// Capability / Configuration (0x810–0x814)
		// ═══════════════════════════════════════════════════════════

		/// <summary>ISA capability bits (v4 surface indicator).</summary>
		static constexpr unsigned short IsaCaps = 0x810;
		/// <summary>Number of lanes and lane capability bits.</summary>
		static constexpr unsigned short LaneCaps = 0x811;
		/// <summary>Bundle width and bundle capability bits.</summary>
		static constexpr unsigned short BundleCaps = 0x812;
		/// <summary>SafetyMask / legality capability discovery.</summary>
		static constexpr unsigned short SafetyCaps = 0x813;
		/// <summary>Memory model capability bits.</summary>
		static constexpr unsigned short MemCaps = 0x814;

		// ═══════════════════════════════════════════════════════════
		// VMX (0x820–0x824)
		// ═══════════════════════════════════════════════════════════

		/// <summary>VMX enable/disable control. Set by VMXON/VMXOFF instruction plane.</summary>
		static constexpr unsigned short VmxEnable = 0x820;
		/// <summary>VMX capability discovery register.</summary>
		static constexpr unsigned short VmxCaps = 0x821;
		/// <summary>VMX control configuration register.</summary>
		static constexpr unsigned short VmxControl = 0x822;
		/// <summary>VMEXIT reason code (last exit).</summary>
		static constexpr unsigned short VmxExitReason = 0x823;
		/// <summary>VMEXIT qualification (additional exit information).</summary>
		static constexpr unsigned short VmxExitQual = 0x824;
		/// <summary>Vector exception mask control CSR.</summary>
		static constexpr unsigned short VexcpMask = 0x900;
		/// <summary>Vector exception priority control CSR.</summary>
		static constexpr unsigned short VexcpPri = 0x901;

		// ═══════════════════════════════════════════════════════════
		// Performance Counters (0xC00–0xC06)
		// ═══════════════════════════════════════════════════════════

		/// <summary>Cycle counter.</summary>
		static constexpr unsigned short Cycle = 0xC00;
		/// <summary>Retired instruction bundles.</summary>
		static constexpr unsigned short BundleRet = 0xC01;
		/// <summary>Retired individual instructions.</summary>
		static constexpr unsigned short InstrRet = 0xC02;
		/// <summary>VMEXIT event counter.</summary>
		static constexpr unsigned short VmExitCnt = 0xC03;
		/// <summary>Barrier instruction counter.</summary>
		static constexpr unsigned short BarrierCnt = 0xC04;
		/// <summary>FSP steal event counter.</summary>
		static constexpr unsigned short StealCnt = 0xC05;
		/// <summary>Deterministic replay diagnostics counter.</summary>
		static constexpr unsigned short ReplayCnt = 0xC06;
	};
}
