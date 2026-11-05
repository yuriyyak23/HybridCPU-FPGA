#pragma once

#include "../Pipeline/Certificates/SafetyMask128.h"

// ─────────────────────────────────────────────────────────────────────────────
// HybridCPU ISA v4 — Hardware Occupancy Input Contract
// K55 (Checklist): GetHardwareOccupancyMask128 as external legality input
//                  with a deterministic sampling contract.
// ─────────────────────────────────────────────────────────────────────────────


namespace YAKSys_Hybrid_CPU::Core::Memory
{
	/// <summary>
	/// Deterministic sampled view of hardware backpressure for one scheduling cycle.
	/// The structural mask remains the compatibility projection used by existing
	/// legality paths, while the explicit memory-budget fields provide honest budget
	/// truth for the live widened lane4..5 LSU load/store subset.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct HardwareOccupancySnapshot128
	class HardwareOccupancySnapshot128
	{
	private:
		SafetyMask128 OverloadedResources;
		long long SamplingCycle;
		unsigned long long SamplingEpoch;
		unsigned char MemoryIssueBudget;
		unsigned short MemoryBankBudgetAtLeastOneMask;
		unsigned short MemoryBankBudgetAtLeastTwoMask;
		unsigned char ReadIssueBudget;
		unsigned short ReadBankBudgetAtLeastOneMask;
		unsigned short ReadBankBudgetAtLeastTwoMask;
		unsigned char WriteIssueBudget;
		unsigned short WriteBankBudgetAtLeastOneMask;
		unsigned short WriteBankBudgetAtLeastTwoMask;

	public:
		static HardwareOccupancySnapshot128 getPermissive();

		HardwareOccupancySnapshot128(SafetyMask128 overloadedResources, long long samplingCycle, unsigned long long samplingEpoch, unsigned char memoryIssueBudget, unsigned short memoryBankBudgetAtLeastOneMask, unsigned short memoryBankBudgetAtLeastTwoMask, unsigned char readIssueBudget, unsigned short readBankBudgetAtLeastOneMask, unsigned short readBankBudgetAtLeastTwoMask, unsigned char writeIssueBudget, unsigned short writeBankBudgetAtLeastOneMask, unsigned short writeBankBudgetAtLeastTwoMask);

		/// <summary>
		/// Compatibility structural projection consumed by the existing mask-shaped legality path.
		/// </summary>
		SafetyMask128 getOverloadedResources() const;

		/// <summary>
		/// Memory subsystem cycle captured by the sample.
		/// </summary>
		long long getSamplingCycle() const;

		/// <summary>
		/// Monotonic sampling epoch used to keep repeated reads idempotent within one cycle.
		/// </summary>
		unsigned long long getSamplingEpoch() const;

		/// <summary>
		/// Remaining globally admissible load/store injections for the live widened LSU subset.
		/// Saturated to 0..2 because the current executable widened subset exposes only two LSU lanes.
		/// </summary>
		unsigned char getMemoryIssueBudget() const;

		/// <summary>
		/// Bit i set when bank i can accept at least one additional load/store request
		/// without crossing the sampled threshold.
		/// </summary>
		unsigned short getMemoryBankBudgetAtLeastOneMask() const;

		/// <summary>
		/// Bit i set when bank i can accept at least two additional load/store requests
		/// without crossing the sampled threshold.
		/// </summary>
		unsigned short getMemoryBankBudgetAtLeastTwoMask() const;

		/// <summary>
		/// Deterministic sampled read-credit projection for the live widened LSU subset.
		/// This now incorporates the memory-side per-port read/write turnaround signal
		/// while still living under the same shared mixed queue-pressure ceiling.
		/// </summary>
		unsigned char getReadIssueBudget() const;

		/// <summary>
		/// Bit i set when bank i can accept at least one additional widened read request
		/// under the current sampled queue-pressure plus per-port turnaround model.
		/// </summary>
		unsigned short getReadBankBudgetAtLeastOneMask() const;

		/// <summary>
		/// Bit i set when bank i can accept at least two additional widened read requests
		/// under the current sampled queue-pressure plus per-port turnaround model.
		/// </summary>
		unsigned short getReadBankBudgetAtLeastTwoMask() const;

		/// <summary>
		/// Deterministic sampled write-credit projection for the live widened LSU subset.
		/// This now incorporates the memory-side per-port read/write turnaround signal
		/// while still living under the same shared mixed queue-pressure ceiling.
		/// </summary>
		unsigned char getWriteIssueBudget() const;

		/// <summary>
		/// Bit i set when bank i can accept at least one additional widened write request
		/// under the current sampled queue-pressure plus per-port turnaround model.
		/// </summary>
		unsigned short getWriteBankBudgetAtLeastOneMask() const;

		/// <summary>
		/// Bit i set when bank i can accept at least two additional widened write requests
		/// under the current sampled queue-pressure plus per-port turnaround model.
		/// </summary>
		unsigned short getWriteBankBudgetAtLeastTwoMask() const;

		unsigned char GetMemoryBudgetForBank(int bankId) const;

		unsigned char GetReadBudgetForBank(int bankId) const;

		unsigned char GetWriteBudgetForBank(int bankId) const;

	private:
		inline static unsigned char GetBudgetForBank(unsigned short bankBudgetAtLeastOneMask, unsigned short bankBudgetAtLeastTwoMask, int bankId);

		/// <summary>
		/// Compatibility alias for the older store-only naming.
		/// It now follows the sampled write-side turnaround projection.
		/// </summary>
	public:
		unsigned char getStoreIssueBudget() const;

		/// <summary>
		/// Compatibility alias for the older store-only naming.
		/// </summary>
		unsigned short getStoreBankBudgetAtLeastOneMask() const;

		/// <summary>
		/// Compatibility alias for the older store-only naming.
		/// </summary>
		unsigned short getStoreBankBudgetAtLeastTwoMask() const;

		inline unsigned char GetStoreBudgetForBank(int bankId) const;

		HardwareOccupancySnapshot128() = default;
	};

	/// <summary>
	/// Contract for the external legality input that reports overloaded hardware
	/// resources (bank congestion, LSU channel saturation) to the pipeline
	/// scheduler.
	///
	/// K55: <see cref="GetHardwareOccupancyMask128"/> is defined as an
	/// <em>external legality input</em> with a deterministic sampling contract.
	///
	/// Sampling contract:
	/// <list type="number">
	///   <item>
	///     The method must be called <em>at most once per bundle-slot evaluation</em>.
	///     Callers must cache the returned mask for the duration of a single FSP
	///     packing pass.  Multiple calls within the same pass are forbidden because
	///     bank-queue depths can change between samples.
	///   </item>
	///   <item>
	///     The returned mask represents the hardware state at the <em>start</em> of
	///     the current scheduling cycle.  It must not reflect speculative in-flight
	///     requests that have not yet been committed to the bank queues.
	///   </item>
	///   <item>
	///     A zero mask (<see cref="SafetyMask128.Zero"/>) is a valid result and
	///     means "no resource is currently overloaded."  Schedulers must treat it
	///     as unconditionally permissive (no additional stall).
	///   </item>
	///   <item>
	///     Implementations must be <em>idempotent</em> across calls in the same
	///     clock cycle — repeated calls must return the same mask until
	///     <see cref="AdvanceSamplingEpoch"/> is called.
	///   </item>
	/// </list>
	///
	/// K56: The occupancy mask represents <em>scheduling backpressure only</em>.
	/// It must not encode trap semantics, memory ordering guarantees, or any
	/// other information outside the structural resource domain.
	/// </summary>
	class IHardwareOccupancyInput
	{
		/// <summary>
		/// Returns the canonical deterministic occupancy snapshot for the current cycle.
		/// Callers should prefer this method when they need honest budget truth for the
		/// live widened LSU load/store subset instead of a coarse overloaded/not-overloaded mask.
		/// </summary>
	public:
		virtual HardwareOccupancySnapshot128 GetHardwareOccupancySnapshot128() = 0;

		/// <summary>
		/// Returns a 128-bit safety mask encoding the currently overloaded hardware
		/// resources.  Set bits indicate resources that are too busy to accept
		/// additional micro-ops in the current scheduling cycle.
		///
		/// The mask layout matches <see cref="ResourceBitset"/>:
		/// <list type="bullet">
		///   <item>Bits 48–50 (Low): LSU Load (48), Store (49), Atomic (50) channels.</item>
		///   <item>Bits 51–62 (Low): DMA / Stream / Accelerator channels.</item>
		///   <item>High word: extended GRLB channels (future hardware).</item>
		/// </list>
		///
		/// Compatibility projection from <see cref="GetHardwareOccupancySnapshot128"/>.
		/// Sampling contract: see <see cref="IHardwareOccupancyInput"/> class remarks.
		/// </summary>
		virtual SafetyMask128 GetHardwareOccupancyMask128() = 0;

		/// <summary>
		/// Advances the sampling epoch.  Must be called once per clock cycle,
		/// after all bundle-packing passes for that cycle are complete.
		///
		/// Implementations use this to snapshot fresh bank-queue depth counters
		/// so that <see cref="GetHardwareOccupancyMask128"/> remains idempotent
		/// within a single cycle (sampling contract item 4).
		///
		/// Callers: pipeline tick driver / MemorySubsystem.AdvanceCycles().
		/// </summary>
		virtual void AdvanceSamplingEpoch() = 0;
	};
}
