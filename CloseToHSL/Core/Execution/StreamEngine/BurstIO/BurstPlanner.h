#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include <format>
#include <cassert>

namespace YAKSys_Hybrid_CPU::Execution
{
	/// <summary>
	/// Burst segment descriptor for AXI4-compliant memory transfers.
	/// Represents a single contiguous burst that respects hardware constraints.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct BurstSegment
	class BurstSegment
	{
		/// <summary>
		/// Starting address of this burst segment.
		/// </summary>
	public:
		const unsigned long long Address;

		/// <summary>
		/// Length of this burst segment in bytes.
		/// </summary>
		const int Length;

		BurstSegment(unsigned long long address, int length);

		BurstSegment() : Address(0), Length(0)
		{
		}
	};

	/// <summary>
	/// Burst timing descriptor for AXI4 transactions (Phase 3)
	/// Models realistic cycle-level timing for burst operations
	/// </summary>
	class BurstTiming
	{
		/// <summary>
		/// Address phase cycles
		/// </summary>
	public:
		int HeaderCycles = 0;

		/// <summary>
		/// Data beat cycles
		/// </summary>
		int DataCycles = 0;

		/// <summary>
		/// Response phase cycles
		/// </summary>
		int ResponseCycles = 0;

		/// <summary>
		/// Unaligned access overhead
		/// </summary>
		int AlignmentPenalty = 0;

		/// <summary>
		/// Total cycles (sum of above)
		/// </summary>
		int TotalCycles = 0;

		/// <summary>
		/// Number of data beats
		/// </summary>
		int DataBeats = 0;

		/// <summary>
		/// Bytes per beat
		/// </summary>
		int BeatSize = 0;
	};

	/// <summary>
	/// BurstPlanner: Pure mathematical burst planning logic.
	///
	/// Responsibilities:
	/// - Split large transfers at 4KB page boundaries
	/// - Respect AXI4 burst length limits (256 beats max)
	/// - Deterministic: same input → same burst pattern
	/// - Zero state, zero allocation (HLS-friendly)
	///
	/// Does NOT:
	/// - Perform actual memory access
	/// - Depend on IOMMU/DMA backend
	/// - Manage device state
	///
	/// Design philosophy:
	/// - Separation of concerns: planning vs. execution
	/// - Testable: burst math can be verified independently
	/// - Reusable: backend-agnostic planning
	/// </summary>
	class BurstPlanner final
	{
		/// <summary>
		/// AXI4 page size constraint: bursts cannot cross 4KB boundaries.
		/// This is a hardware limitation imposed by memory controllers.
		/// </summary>
	private:
		static constexpr unsigned long long PAGE_SIZE = 4096;

		/// <summary>
		/// Plan burst segments for a contiguous memory transfer.
		/// Splits the transfer into AXI4-compliant segments that:
		/// 1. Do not cross 4KB page boundaries
		/// 2. Respect hardware burst length limits
		///
		/// Algorithm:
		/// - Iterate through transfer from baseAddr to baseAddr+totalBytes
		/// - At each step, compute bytes remaining in current page
		/// - Emit segment for min(bytes_in_page, remaining_bytes)
		/// - Advance to next page boundary if needed
		///
		/// Complexity: O(total_bytes / PAGE_SIZE)
		/// Memory: O(1) per iteration (iterator pattern)
		///
		/// Invariant: [Invariant("Burst segments never cross 4KB boundaries")]
		/// Each returned segment is guaranteed to fit within a single 4KB page.
		/// </summary>
		/// <param name="baseAddr">Starting address of transfer</param>
		/// <param name="totalBytes">Total number of bytes to transfer</param>
		/// <returns>Iterator of burst segments (no allocation)</returns>
	public:
		static std::vector<BurstSegment> Plan(unsigned long long baseAddr, unsigned long long totalBytes);

		/// <summary>
		/// Plan burst segments for 2D memory transfers.
		/// Optimizes row-major 2D array access by identifying contiguous rows
		/// and planning bursts for each row independently.
		///
		/// Algorithm:
		/// - For each row, if columns are contiguous (colStride == elementSize):
		///   - Plan burst for entire row using Plan()
		/// - Otherwise, fall back to element-by-element access
		///
		/// Optimization: When rowStride == rowLength * colStride, entire 2D region
		/// is contiguous and can be treated as 1D.
		///
		/// Complexity: O(rows * segments_per_row)
		/// </summary>
		/// <param name="baseAddr">Base address of 2D array</param>
		/// <param name="elementCount">Total number of elements</param>
		/// <param name="elementSize">Size of each element in bytes</param>
		/// <param name="rowLength">Number of elements per row</param>
		/// <param name="rowStride">Byte pitch between rows</param>
		/// <param name="colStride">Byte stride between columns</param>
		/// <returns>Iterator of burst segments optimized for 2D access</returns>
		static std::vector<BurstSegment> Plan2D(unsigned long long baseAddr, unsigned long long elementCount, int elementSize, unsigned int rowLength, unsigned short rowStride, unsigned short colStride);

		/// <summary>
		/// Plan burst segments for indexed memory access (gather/scatter).
		/// Analyzes index array to identify contiguous runs and group them into bursts.
		///
		/// Algorithm:
		/// - Sort indices to identify contiguous runs
		/// - For each contiguous run, plan bursts using Plan()
		/// - Return mapping of original index position to burst segment
		///
		/// Optimization heuristic:
		/// - If >80% of accesses are contiguous, use burst planning
		/// - Otherwise, fall back to element-by-element access
		///
		/// Note: This method performs index analysis, which has O(n log n) complexity
		/// due to sorting. Use only when n is large enough to benefit from bursting.
		///
		/// Complexity: O(n log n) for sorting + O(runs * segments_per_run)
		/// </summary>
		/// <param name="baseAddr">Base address of source/dest array</param>
		/// <param name="elementSize">Size of each element in bytes</param>
		/// <param name="indices">Array of indices (element or byte offsets)</param>
		/// <param name="indexIsByteOffset">True if indices are byte offsets, false if element indices</param>
		/// <returns>Iterator of burst segments for indexed access</returns>
		static std::vector<BurstSegment> PlanIndexed(unsigned long long baseAddr, int elementSize, std::vector<unsigned long long> &indices, bool indexIsByteOffset);

		/// <summary>
		/// Calculate detailed AXI4 burst timing (Phase 3)
		/// Models realistic cycle-level timing including alignment penalties
		/// </summary>
		/// <param name="address">Starting address</param>
		/// <param name="size">Transfer size in bytes</param>
		/// <param name="isRead">Read or write operation</param>
		/// <returns>Detailed timing breakdown</returns>
		static BurstTiming CalculateBurstTiming(unsigned long long address, int size, bool isRead);

		/// <summary>
		/// Calculate alignment penalty for unaligned accesses (Phase 3)
		/// </summary>
		/// <param name="alignment">Byte offset from aligned boundary</param>
		/// <param name="size">Transfer size</param>
		/// <returns>Penalty in cycles</returns>
	private:
		static int CalculateAlignmentPenalty(int alignment, int size);

		/// <summary>
		/// Count 4KB page fragments for burst transaction (Phase 3)
		/// </summary>
		/// <param name="address">Starting address</param>
		/// <param name="size">Transfer size</param>
		/// <returns>Number of 4KB fragments</returns>
		static int Count4KBFragments(unsigned long long address, int size);

		/// <summary>
		/// Calculate total latency for burst sequence (Phase 3)
		/// </summary>
		/// <param name="segments">Burst segments</param>
		/// <param name="isRead">Read or write operation</param>
		/// <returns>Total cycles</returns>
	public:
		static int CalculateBurstSequenceLatency(std::vector<BurstSegment> &segments, bool isRead);
	};
}
