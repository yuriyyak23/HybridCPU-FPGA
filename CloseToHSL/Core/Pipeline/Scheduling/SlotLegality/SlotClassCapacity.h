#pragma once

#include "SlotClassDefinitions.h"

#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Per-cycle class-capacity snapshot for all slot classes.
	/// Tracks how many lanes of each class are occupied vs total available
	/// in the current VLIW bundle.
	/// <para>
	/// This is a derived summary layered on top of the physical slot view —
	/// it does not replace slot-level occupancy but enables O(1) class-level
	/// admission queries for FSP and scheduling logic.
	/// </para>
	/// <para>
	/// HLS design note: 6 × (3-bit occupied + 3-bit total) = 36 flip-flops.
	/// Fits in a single CLB. Written once per cycle, read by class-admission MUX.
	/// </para>
	/// </summary>
	class SlotClassCapacityState
	{
		// Per-class counters: [occupied, total].
		// Total is derived from SlotClassLaneMap at init; occupied is updated per-cycle.

		/// <summary>Number of ALU lanes currently occupied.</summary>
	public:
		unsigned char AluOccupied = 0;

		/// <summary>Total ALU lanes available (from lane map).</summary>
		unsigned char AluTotal = 0;

		/// <summary>Number of LSU lanes currently occupied.</summary>
		unsigned char LsuOccupied = 0;

		/// <summary>Total LSU lanes available (from lane map).</summary>
		unsigned char LsuTotal = 0;

		/// <summary>Number of DMA/Stream lanes currently occupied.</summary>
		unsigned char DmaStreamOccupied = 0;

		/// <summary>Total DMA/Stream lanes available (from lane map).</summary>
		unsigned char DmaStreamTotal = 0;

		/// <summary>Number of MatrixTile stream lane claims currently occupied.</summary>
		unsigned char MatrixTileStreamOccupied = 0;

		/// <summary>Total MatrixTile stream lanes available.</summary>
		unsigned char MatrixTileStreamTotal = 0;

		/// <summary>Number of BranchControl lanes currently occupied.</summary>
		unsigned char BranchControlOccupied = 0;

		/// <summary>Total BranchControl lanes available (from lane map).</summary>
		unsigned char BranchControlTotal = 0;

		/// <summary>Number of SystemSingleton lanes currently occupied.</summary>
		unsigned char SystemSingletonOccupied = 0;

		/// <summary>Total SystemSingleton lanes available (from lane map).</summary>
		unsigned char SystemSingletonTotal = 0;

		/// <summary>
		/// Check if there is free capacity in the given slot class.
		/// Alias-aware: if the class shares a physical lane with another class
		/// (e.g., BranchControl and SystemSingleton both map to lane 7),
		/// capacity is considered free only if no aliased class has occupied
		/// the shared lane(s).
		/// <para>HLS: 3-bit comparator + optional aliased-occupancy check.</para>
		/// </summary>
		/// <param name="slotClass">The slot class to query.</param>
		/// <returns><see langword="true"/> if at least one lane is free for this class.</returns>
		bool HasFreeCapacity(SlotClass slotClass) const;

		/// <summary>
		/// Get remaining free capacity for a slot class (no alias adjustment).
		/// <para>HLS: 3-bit subtractor.</para>
		/// </summary>
		/// <param name="slotClass">The slot class to query.</param>
		/// <returns>Number of free lanes (total − occupied). May be zero or negative if over-counted.</returns>
		int GetFreeCapacity(SlotClass slotClass) const;

		/// <summary>
		/// Get raw occupied count for a slot class (no alias adjustment).
		/// </summary>
		int GetOccupied(SlotClass slotClass) const;

		/// <summary>
		/// Increment the occupied counter for the given slot class.
		/// Called after a successful placement (inject/steal).
		/// <para>HLS: 3-bit adder, negligible gate cost.</para>
		/// </summary>
		/// <param name="slotClass">The slot class whose counter to increment.</param>
		void IncrementOccupancy(SlotClass slotClass);

		/// <summary>
		/// Initialize total capacities from <see cref="SlotClassLaneMap"/>.
		/// Called once at scheduler reset or configuration change.
		/// </summary>
		void InitializeFromLaneMap();

		/// <summary>
		/// Reset all occupied counters to zero (start of new bundle).
		/// Totals are preserved.
		/// <para>HLS: 5 × wire-reset to 0.</para>
		/// </summary>
		void ResetOccupancy();
	};

	/// <summary>
	/// Static helpers for computing <see cref="SlotClassCapacityState"/> from a VLIW bundle.
	/// </summary>
	class SlotClassCapacity final
	{
		/// <summary>
		/// Scan a VLIW bundle and compute per-class occupancy.
		/// <para>HLS: 8-iteration combinational scan (1 comparator per slot).</para>
		/// </summary>
		/// <param name="bundle">The VLIW bundle (up to <paramref name="bundleWidth"/> slots).</param>
		/// <param name="bundleWidth">Number of slots in the bundle (default 8).</param>
		/// <returns>Fully initialized capacity state with occupancy counts.</returns>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static SlotClassCapacityState ComputeFromBundle(MicroOp?[] bundle, int bundleWidth = 8)
		static SlotClassCapacityState ComputeFromBundle(std::vector<std::shared_ptr<MicroOp>> &bundle, int bundleWidth = 8);
	};
}
