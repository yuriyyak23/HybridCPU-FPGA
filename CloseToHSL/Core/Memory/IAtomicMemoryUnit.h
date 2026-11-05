#pragma once

namespace YAKSys_Hybrid_CPU::Core::Memory
{
	/// <summary>
	/// Atomic memory operation interface for the HybridCPU ISA v4 memory subsystem.
	/// <para>
	/// All implementations must guarantee atomicity with respect to:
	/// <list type="bullet">
	///   <item>Other AMO operations on the same address</item>
	///   <item>LR/SC reservations on the same address</item>
	///   <item>DMA transfers to the same region (if DMA is in-flight)</item>
	/// </list>
	/// </para>
	/// <para>
	/// Phase 07: This interface replaces the minimal <c>IAtomicMemoryBus</c>
	/// with a complete typed contract covering the full 64-bit atomic plane:
	/// LR/SC (word and doubleword), AMO*_W (9 word operations), and AMO*_D
	/// (9 doubleword operations).
	/// </para>
	/// </summary>
	class IAtomicMemoryUnit
	{
		// ── LR/SC ─────────────────────────────────────────────────────────────

		/// <summary>Load-Reserved Word: rd = sign_extend(mem32[address]); reserve address.</summary>
	public:
		virtual int LoadReserved32(unsigned long long address) = 0;

		/// <summary>Store-Conditional Word: if reservation holds mem32[address] = value, return 0 (success); else return 1 (fail).</summary>
		virtual int StoreConditional32(unsigned long long address, int value) = 0;

		/// <summary>Load-Reserved Doubleword: rd = mem64[address]; reserve address.</summary>
		virtual long long LoadReserved64(unsigned long long address) = 0;

		/// <summary>Store-Conditional Doubleword: if reservation holds mem64[address] = value, return 0 (success); else return 1 (fail).</summary>
		virtual long long StoreConditional64(unsigned long long address, long long value) = 0;

		// ── AMO Word (signed return, sign-extended to 64 bits by caller) ──────

		/// <summary>Atomic Swap Word: rd = mem32[addr]; mem32[addr] = value. Returns old value.</summary>
		virtual int AtomicSwap32(unsigned long long address, int value) = 0;

		/// <summary>Atomic Add Word: rd = mem32[addr]; mem32[addr] += value. Returns old value.</summary>
		virtual int AtomicAdd32(unsigned long long address, int value) = 0;

		/// <summary>Atomic XOR Word: rd = mem32[addr]; mem32[addr] ^= value. Returns old value.</summary>
		virtual int AtomicXor32(unsigned long long address, int value) = 0;

		/// <summary>Atomic AND Word: rd = mem32[addr]; mem32[addr] &amp;= value. Returns old value.</summary>
		virtual int AtomicAnd32(unsigned long long address, int value) = 0;

		/// <summary>Atomic OR Word: rd = mem32[addr]; mem32[addr] |= value. Returns old value.</summary>
		virtual int AtomicOr32(unsigned long long address, int value) = 0;

		/// <summary>Atomic MIN Word (signed): rd = mem32[addr]; mem32[addr] = min(mem32[addr], value). Returns old value.</summary>
		virtual int AtomicMinSigned32(unsigned long long address, int value) = 0;

		/// <summary>Atomic MAX Word (signed): rd = mem32[addr]; mem32[addr] = max(mem32[addr], value). Returns old value.</summary>
		virtual int AtomicMaxSigned32(unsigned long long address, int value) = 0;

		/// <summary>Atomic MIN Word (unsigned): rd = mem32[addr]; mem32[addr] = min(mem32[addr], value). Returns old value.</summary>
		virtual unsigned int AtomicMinUnsigned32(unsigned long long address, unsigned int value) = 0;

		/// <summary>Atomic MAX Word (unsigned): rd = mem32[addr]; mem32[addr] = max(mem32[addr], value). Returns old value.</summary>
		virtual unsigned int AtomicMaxUnsigned32(unsigned long long address, unsigned int value) = 0;

		// ── AMO Doubleword ────────────────────────────────────────────────────

		/// <summary>Atomic Swap Doubleword: rd = mem64[addr]; mem64[addr] = value. Returns old value.</summary>
		virtual long long AtomicSwap64(unsigned long long address, long long value) = 0;

		/// <summary>Atomic Add Doubleword: rd = mem64[addr]; mem64[addr] += value. Returns old value.</summary>
		virtual long long AtomicAdd64(unsigned long long address, long long value) = 0;

		/// <summary>Atomic XOR Doubleword: rd = mem64[addr]; mem64[addr] ^= value. Returns old value.</summary>
		virtual long long AtomicXor64(unsigned long long address, long long value) = 0;

		/// <summary>Atomic AND Doubleword: rd = mem64[addr]; mem64[addr] &amp;= value. Returns old value.</summary>
		virtual long long AtomicAnd64(unsigned long long address, long long value) = 0;

		/// <summary>Atomic OR Doubleword: rd = mem64[addr]; mem64[addr] |= value. Returns old value.</summary>
		virtual long long AtomicOr64(unsigned long long address, long long value) = 0;

		/// <summary>Atomic MIN Doubleword (signed): rd = mem64[addr]; mem64[addr] = min(mem64[addr], value). Returns old value.</summary>
		virtual long long AtomicMin64Signed(unsigned long long address, long long value) = 0;

		/// <summary>Atomic MAX Doubleword (signed): rd = mem64[addr]; mem64[addr] = max(mem64[addr], value). Returns old value.</summary>
		virtual long long AtomicMax64Signed(unsigned long long address, long long value) = 0;

		/// <summary>Atomic MIN Doubleword (unsigned): rd = mem64[addr]; mem64[addr] = min(mem64[addr], value). Returns old value.</summary>
		virtual unsigned long long AtomicMin64Unsigned(unsigned long long address, unsigned long long value) = 0;

		/// <summary>Atomic MAX Doubleword (unsigned): rd = mem64[addr]; mem64[addr] = max(mem64[addr], value). Returns old value.</summary>
		virtual unsigned long long AtomicMax64Unsigned(unsigned long long address, unsigned long long value) = 0;
	};
}
