#pragma once

#include "../../MicroOps/Types/MicroOp.h"
#include "../../../Decoder/Rf09ReplayEntry.h"
#include "../../Certificates/ReplayPhaseSubstrate.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include <vector>
#include <optional>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// MicroOp Loop Buffer for decode-once-replay-N during strip-mining (req.md §3).
	///
	/// Purpose: Cache decoded MicroOps from a VLIW bundle so the core can replay
	/// them for thousands of iterations without re-fetching the 2048-bit bundle
	/// from L1-I cache each cycle. This is critical for streaming workloads where
	/// the same operation pattern repeats across data chunks (strip-mining).
	///
	/// Architecture:
	/// - Fixed-size buffer holding up to 8 decoded MicroOps (one VLIW bundle)
	/// - States: Empty → Loading → Active → Draining
	/// - When Active, the pipeline fetches from the Loop Buffer instead of L1-I
	/// - Replay-stable donor exposure: while replaying, stable donor-safe slots
	///   are summarized as bundle-donor capacity for the replay duration
	///
	/// HLS constraints:
	/// - Fixed-size arrays only (no heap allocation)
	/// - Deterministic state machine transitions
	/// - All state fits in register file
	/// </summary>
	class LoopBuffer
	{
	public:
		enum class BufferState : unsigned char;
	private:
		int ValidSlotCount = 0;
		BufferState State = static_cast<BufferState>(0);
		unsigned long long CachedPC = 0;
		unsigned long long RemainingIterations = 0;
		unsigned long long TotalReplays = 0;
		unsigned long long FetchSavings = 0;
		unsigned long long ReplayEpochCount = 0;
		unsigned long long TotalEpochLength = 0;
		unsigned long long StableDonorSlotSamples = 0;
		unsigned long long TotalReplaySlotSamples = 0;
		unsigned long long DeterministicTransitionCount = 0;

		/// <summary>
		/// Loop buffer state machine
		/// </summary>
	public:
		enum class BufferState : unsigned char
		{
			/// <summary>Buffer is empty, no cached bundle</summary>
			Empty = 0,

			/// <summary>Bundle is being loaded (decode in progress)</summary>
			Loading = 1,

			/// <summary>Buffer is active, replaying cached MicroOps</summary>
			Active = 2,

			/// <summary>Buffer is draining (last iteration, will transition to Empty)</summary>
			Draining = 3
		};

		/// <summary>
		/// Maximum number of MicroOps per VLIW bundle (8 slots × 256 bits)
		/// </summary>
	private:
		static constexpr int MAX_SLOTS = 8;

		/// <summary>
		/// HLS-compatible inline buffer for 8 MicroOp slot references.
		/// Maps to an 8-entry register file in hardware — no heap allocation.
		/// </summary>
#if defined(TESTING)
	private:
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [InlineArray(MAX_SLOTS)] private struct MicroOpSlotBuffer
		class MicroOpSlotBuffer
		{
		private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MicroOp? _element0;
			std::shared_ptr<MicroOp> _element0;
		};

		/// <summary>
		/// HLS-compatible inline buffer for 8 per-slot valid bits.
		/// Maps to an 8-bit register in hardware — no heap allocation.
		/// </summary>
	private:
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [InlineArray(MAX_SLOTS)] private struct SlotValidBuffer
		class SlotValidBuffer
		{
		private:
			bool _element0 = false;
		};

		/// <summary>
		/// Cached MicroOps from the decoded VLIW bundle.
		/// HLS: maps to an 8-entry register file of MicroOp references (inline, no heap).
		/// </summary>
	private:
		MicroOpSlotBuffer _slots;

		/// <summary>
		/// Per-slot valid bits. A slot may be null/NOP in the original bundle.
		/// HLS: maps to an 8-bit validity register (inline, no heap).
		/// </summary>
		SlotValidBuffer _slotValid;
#endif

		/// <summary>
		/// Immutable semantic entry selected by the bounded loop-placement
		/// nomination. Production replay never stores a MicroOp carrier.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private Decoder.ReplayEntry? _replayEntry;
		std::shared_ptr<Decoder::ReplayEntry> _replayEntry;

		/// <summary>
		/// Number of valid (non-null) slots in the cached bundle
		/// </summary>
	public:
		int getValidSlotCount() const;
	private:
		void setValidSlotCount(int value);

		/// <summary>
		/// Current buffer state
		/// </summary>
	public:
		BufferState getState() const;
	private:
		void setState(BufferState value);

		/// <summary>
		/// Program counter of the cached bundle (for PC matching on replay)
		/// </summary>
	public:
		unsigned long long getCachedPC() const;
	private:
		void setCachedPC(unsigned long long value);

		/// <summary>
		/// Remaining strip-mining iterations before buffer drains
		/// </summary>
	public:
		unsigned long long getRemainingIterations() const;
	private:
		void setRemainingIterations(unsigned long long value);

		/// <summary>
		/// Total iterations executed from this buffer (performance counter)
		/// </summary>
	public:
		unsigned long long getTotalReplays() const;
	private:
		void setTotalReplays(unsigned long long value);

		/// <summary>
		/// Total L1-I fetch savings (iterations that did not require re-fetch)
		/// </summary>
	public:
		unsigned long long getFetchSavings() const;
	private:
		void setFetchSavings(unsigned long long value);

		/// <summary>
		/// Number of replay epochs observed by the loop buffer.
		/// </summary>
	public:
		unsigned long long getReplayEpochCount() const;
	private:
		void setReplayEpochCount(unsigned long long value);

		/// <summary>
		/// Sum of replay-epoch lengths.
		/// </summary>
	public:
		unsigned long long getTotalEpochLength() const;
	private:
		void setTotalEpochLength(unsigned long long value);

		/// <summary>
		/// Sum of stable donor-slot samples across replay hits.
		/// </summary>
	public:
		unsigned long long getStableDonorSlotSamples() const;
	private:
		void setStableDonorSlotSamples(unsigned long long value);

		/// <summary>
		/// Total slot samples observed during replay hits.
		/// </summary>
	public:
		unsigned long long getTotalReplaySlotSamples() const;
	private:
		void setTotalReplaySlotSamples(unsigned long long value);

		/// <summary>
		/// Deterministic epoch transition count.
		/// </summary>
	public:
		unsigned long long getDeterministicTransitionCount() const;
	private:
		void setDeterministicTransitionCount(unsigned long long value);

		unsigned long long _currentEpochId = 0;
		unsigned long long _currentEpochReplayCount = 0;
		unsigned long long _currentEpochLength = 0;
		unsigned char _stableDonorMask = 0;
		ReplayPhaseInvalidationReason _lastInvalidationReason = static_cast<ReplayPhaseInvalidationReason>(0);
		std::optional<std::shared_ptr<Decoder::CanonicalDecodeContext>> _loadedDecodeContext;

		// Phase 07: Per-class donor capacity for class-level replay stability

		/// <summary>
		/// Number of ALU-class donor slots available in the cached replay bundle.
		/// More stable than exact slot positions for replay template matching.
		/// HLS: 3-bit counter.
		/// </summary>
		unsigned char _aluDonorCapacity = 0;

		/// <summary>
		/// Number of LSU-class donor slots available in the cached replay bundle.
		/// HLS: 3-bit counter.
		/// </summary>
		unsigned char _lsuDonorCapacity = 0;

		/// <summary>
		/// Number of DmaStream-class donor slots available in the cached replay bundle.
		/// HLS: 3-bit counter.
		/// </summary>
		unsigned char _dmaStreamDonorCapacity = 0;

		/// <summary>
		/// Number of BranchControl-class donor slots available in the cached replay bundle.
		/// HLS: 3-bit counter.
		/// </summary>
		unsigned char _branchControlDonorCapacity = 0;

		/// <summary>
		/// Number of SystemSingleton-class donor slots available in the cached replay bundle.
		/// HLS: 3-bit counter.
		/// </summary>
		unsigned char _systemSingletonDonorCapacity = 0;

		/// <summary>
		/// Initialize the loop buffer. Must be called once before use.
		/// HLS-compatible: no heap allocation, resets inline register state.
		/// </summary>
	public:
		void Initialize();

		/// <summary>
		/// Begin loading a decoded VLIW bundle into the loop buffer.
		/// Called by the Decode stage when a strip-mining loop is detected.
		/// </summary>
		/// <param name="pc">Program counter of the bundle being cached</param>
		/// <param name="totalIterations">Total strip-mining iterations expected</param>
#if defined(TESTING)
		void BeginLoad(unsigned long long pc, unsigned long long totalIterations, std::optional<std::shared_ptr<Decoder::CanonicalDecodeContext>> &decodeContext = std::nullopt);

		/// <summary>
		/// Store a decoded MicroOp into the buffer during the Loading phase.
		/// Called once per slot during the first decode pass.
		/// </summary>
		/// <param name="slotIndex">VLIW slot index (0–7)</param>
		/// <param name="op">Decoded MicroOp (null for empty/NOP slots)</param>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public void StoreSlot(int slotIndex, MicroOp? op)
		void StoreSlot(int slotIndex, std::shared_ptr<MicroOp> op);
#endif

		/// <summary>
		/// Begin a production semantic replay epoch. Mutable carriers are
		/// inspected only to preserve the existing donor-template summary and
		/// are never retained by the loop buffer.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public void BeginSemanticLoad(ulong pc, ulong totalIterations, Decoder.ReplayEntry replayEntry, System.Collections.Generic.IReadOnlyList<MicroOp?> liveCarriers, Decoder.CanonicalDecodeContext decodeContext)
		void BeginSemanticLoad(unsigned long long pc, unsigned long long totalIterations, std::shared_ptr<Decoder::ReplayEntry> replayEntry, const std::vector<std::shared_ptr<MicroOp>> &liveCarriers, std::shared_ptr<Decoder::CanonicalDecodeContext> decodeContext);

		/// <summary>
		/// Commit the load and transition to Active state.
		/// Called after all 8 slots have been stored.
		/// </summary>
		void CommitLoad();

		/// <summary>
		/// Attempt to replay the cached bundle for the current iteration.
		/// Copies cached MicroOps into the caller-provided target buffer on hit.
		/// HLS-compatible: reads from inline register file, no heap exposure.
		/// </summary>
		/// <param name="requestedPC">PC being fetched by the pipeline</param>
		/// <param name="target">Caller-provided buffer (min 8 elements) to receive cached MicroOps</param>
		/// <returns>True if loop buffer hit (target filled), false if miss (fetch from L1-I)</returns>
#if defined(TESTING)
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public bool TryReplay(ulong requestedPC, MicroOp?[] target, System.Nullable<Decoder.CanonicalDecodeContext> currentDecodeContext = null)
		bool TryReplay(unsigned long long requestedPC, std::vector<std::shared_ptr<MicroOp>> &target, std::optional<std::shared_ptr<Decoder::CanonicalDecodeContext>> &currentDecodeContext = std::nullopt);
#endif

		/// <summary>
		/// Return only the immutable semantic entry for a fully validated
		/// production replay hit. Placement PC nominates the bounded entry;
		/// full context equality and the frozen-entry integrity witness
		/// authorize serving.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public bool TryGetReplayEntry(ulong requestedPC, Decoder.CanonicalDecodeContext currentDecodeContext, out Decoder.ReplayEntry? replayEntry)
		bool TryGetReplayEntry(unsigned long long requestedPC, std::shared_ptr<Decoder::CanonicalDecodeContext> currentDecodeContext, std::shared_ptr<Decoder::ReplayEntry> &replayEntry);

		/// <summary>
		/// Complete the current replay cycle. If draining, transition to Empty.
		/// Called at the end of the pipeline cycle.
		/// </summary>
		void EndCycle();

		/// <summary>
		/// Invalidate the loop buffer (branch, interrupt, or strip-mining complete).
		/// HLS-compatible: resets inline register state without heap interaction.
		/// </summary>
		void Invalidate(ReplayPhaseInvalidationReason reason = ReplayPhaseInvalidationReason::Manual);

		/// <summary>
		/// Current replay phase context for phase-aware scheduling.
		/// </summary>
		ReplayPhaseContext getCurrentReplayPhase() const;

		/// <summary>
		/// Aggregate replay-phase metrics for Phase 1 observability.
		/// </summary>
		ReplayPhaseMetrics GetReplayPhaseMetrics();

		/// <summary>
		/// Check if a specific slot is replay-stable donor eligible.
		/// During Active replay, empty/NOP slots and donor-safe non-vector slots
		/// are exposed as stable donor capacity for the replay epoch.
		/// </summary>
		/// <param name="slotIndex">Slot to check (0–7)</param>
		/// <returns>True if the slot is part of the replay-stable donor mask.</returns>
		bool IsSlotDonorEligible(int slotIndex);

		/// <summary>
		/// Get the number of replay-stable donor slots in the cached bundle.
		/// This is the bounded donor-capacity surface exposed to replay-aware
		/// bundle densification during the active replay epoch.
		/// </summary>
		int getDonorEligibleSlotCount() const;

		/// <summary>
		/// Check if the loop buffer is currently active and replaying.
		/// </summary>
		bool isReplaying() const;

		/// <summary>
		/// Phase 07: Get per-class donor capacity for the cached replay bundle.
		/// Returns the number of donor-eligible (empty/NOP) slots that belong to each class.
		/// More stable across replay iterations than exact slot positions.
		/// </summary>
		/// <param name="slotClass">The slot class to query.</param>
		/// <returns>Number of donor slots available for the given class.</returns>
		int GetClassDonorCapacity(SlotClass slotClass) const;

#if defined(TESTING)
	private:
		unsigned char ComputeStableDonorMask();

		bool IsReplayStableDonorSlot(int slotIndex);
#endif

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static byte ComputeStableDonorMask(System.Collections.Generic.IReadOnlyList<MicroOp?> carriers)
		static unsigned char ComputeStableDonorMask(const std::vector<std::shared_ptr<MicroOp>> &carriers);

		void ConsumeReplayHit();

		/// <summary>
		/// Phase 07: Compute per-class donor capacity from stable donor mask and lane map.
		/// A donor slot at lane N belongs to the class that lane N is mapped to.
		/// </summary>
		void ComputeClassDonorCapacity();

		static int CountBits(unsigned char value);
	};
}
