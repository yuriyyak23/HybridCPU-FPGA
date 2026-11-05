#pragma once


#include <span>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class MicroOp;
	/// <summary>
	/// Physical lane class taxonomy for typed-slot scheduling.
	/// This enum is the canonical repository-facing description of the fixed W=8 topology:
	/// ALU lanes 0..3, LSU lanes 4..5, DMA/stream lane 6, and the aliased branch/system lane 7.
	/// Each value denotes a functional class of execution lanes in that typed bundle model.
	/// <para>
	/// HLS design note: encoded as 3-bit field (byte backing for C# convenience).
	/// Synthesizes to a 3-bit register / comparator — 3 flip-flops, 1-LUT decode depth.
	/// </para>
	/// </summary>
	enum class SlotClass : unsigned char
	{
		/// <summary>Lanes 0–3: scalar/vector ALU-capable lanes.</summary>
		AluClass = 0,

		/// <summary>Lanes 4–5: load/store unit lanes.</summary>
		LsuClass = 1,

		/// <summary>Lane 6: DMA/stream placement lane; operation semantics live on the MicroOp.</summary>
		DmaStreamClass = 2,

		/// <summary>Lane 7 (aliased): branch/control-flow lane.</summary>
		BranchControl = 3,

		/// <summary>Lane 7 (aliased): system-singleton lane (CSR, Halt, PortIO).</summary>
		SystemSingleton = 4,

		/// <summary>Lane 6: MatrixTile memory stream transport, distinct from DMA/DSC semantics.</summary>
		MatrixTileStreamClass = 5,

		/// <summary>No specific lane affinity (NOP, generic, or uninitialized).</summary>
		Unclassified = 7
	};

	/// <summary>
	/// Indicates whether a micro-operation is pinned to a specific physical lane
	/// or is flexible within its <see cref="SlotClass"/> lane set.
	/// <para>
	/// HLS design note: 1-bit field. Zero additional LUT when stored as register bit.
	/// </para>
	/// </summary>
	enum class SlotPinningKind : unsigned char
	{
		/// <summary>
		/// Operation may be placed in any lane belonging to its <see cref="SlotClass"/>.
		/// Scheduler picks a free lane from the class lane mask.
		/// </summary>
		ClassFlexible = 0,

		/// <summary>
		/// Operation is pinned to <see cref="MicroOp.Placement"/>.<see cref="SlotPlacementMetadata.PinnedLaneId"/>.
		/// Scheduler must place it in that exact physical lane.
		/// </summary>
		HardPinned = 1
	};

	/// <summary>
	/// Structured reject reasons for the two-stage typed-slot admission pipeline.
	/// Stage A (Class Admission) and Stage B (Lane Materialization) each produce
	/// distinct reject codes for telemetry (Phase 08) and diagnostics.
	/// <para>
	/// HLS design note: 5-bit enum (byte backing). Stored in diagnostic register,
	/// not on critical timing path.
	/// </para>
	/// </summary>
	enum class TypedSlotRejectReason : unsigned char
	{
		/// <summary>No rejection — candidate was admitted and placed.</summary>
		None = 0,

		// ── Stage A rejects ──────────────────────────────────────────

		/// <summary>
		/// Compiler's original bundle already exceeds class capacity.
		/// This is a structural/compiler fault — the bundle was packed
		/// with more ops of this class than physical lanes exist.
		/// Detected when class capacity is exhausted before any FSP inject
		/// in the current scheduling pass (<c>currentPassInjections == 0</c>).
		/// </summary>
		StaticClassOvercommit = 1,

		/// <summary>
		/// Class capacity was exhausted dynamically during intra-cycle FSP
		/// injection. Earlier injected ops in the same scheduling pass
		/// consumed remaining class capacity. This is normal runtime
		/// behavior, NOT a compiler bug.
		/// </summary>
		DynamicClassExhaustion = 2,

		/// <summary>
		/// Certificate / CanInject() safety-mask conflict.
		/// Shared resource overlap detected by <see cref="BundleResourceCertificate4Way"/>.
		/// </summary>
		ResourceConflict = 3,

		/// <summary>
		/// Domain-tag isolation violation (DomainTag &amp; CsrMemDomainCert mismatch).
		/// </summary>
		DomainReject = 4,

		/// <summary>
		/// Scoreboard conflict — SuppressLsu memory-wall gate blocked a memory op.
		/// </summary>
		ScoreboardReject = 5,

		/// <summary>
		/// Bank-pending scoreboard conflict — outstanding MSHR entry on the same
		/// memory bank for this virtual thread.
		/// </summary>
		BankPendingReject = 6,

		/// <summary>
		/// Sampled hardware load/store budget exhausted for the live widened lane4..5 LSU subset,
		/// including the direction-aware read/write turnaround split.
		/// </summary>
		HardwareBudgetReject = 7,

		/// <summary>
		/// Speculation budget exhausted — cross-thread memory op blocked
		/// because the budget counter reached zero.
		/// </summary>
		SpeculationBudgetReject = 8,

		/// <summary>
		/// Dedicated assist-only quota rejected an architecturally invisible memory assist.
		/// </summary>
		AssistQuotaReject = 9,

		/// <summary>
		/// Explicit assist widened-owner backpressure rejected an architecturally invisible
		/// memory assist before it could borrow shared outer-cap/MSHR/SRF pressure.
		/// </summary>
		AssistBackpressureReject = 10,

		// ── Stage B rejects ──────────────────────────────────────────

		/// <summary>
		/// Hard-pinned lane already occupied by another operation.
		/// </summary>
		PinnedLaneConflict = 11,

		/// <summary>
		/// A hard-pinned raw placement did not represent a physical lane
		/// before Stage B could use it for an occupancy shift or index.
		/// This is an owner-local admission denial, not a lane conflict or
		/// architectural fault winner.
		/// </summary>
		InvalidPinnedLane = 13,

		/// <summary>
		/// All class-eligible lanes are occupied — no free lane available
		/// for a <see cref="SlotPinningKind.ClassFlexible"/> candidate.
		/// </summary>
		LateBindingConflict = 12,

		// ── Policy defers (not hard rejects) ─────────────────────────

		/// <summary>
		/// Candidate deferred by fairness-credit policy (not a hard reject).
		/// </summary>
		FairnessDeferred = 20
	};

	/// <summary>
	/// Maps <see cref="SlotClass"/> values to physical lane bitmasks for the canonical W=8 topology.
	/// The fixed lane map is ALU[0..3], LSU[4..5], DMA/stream[6], Branch/System[7].
	/// Provides capacity queries and aliased-lane detection (BranchControl / SystemSingleton share lane 7).
	/// <para>
	/// HLS design note: all methods are pure combinational (ROM-table or mux). No dynamic allocation.
	/// GetLaneMask → 8-bit ROM indexed by 3-bit SlotClass → 8 × 3 = 24 LUT1 equivalent.
	/// PopCount(8-bit) → 4-LUT chain, 2 depth.
	/// </para>
	/// </summary>
	class SlotClassLaneMap final
	{
		// Lane mask constants for W=8 default topology.
		// Bit N set → physical lane N can host ops of that class.
	private:
		static constexpr unsigned char AluMask = 0b0000'1111; // lanes 0-3
		static constexpr unsigned char LsuMask = 0b0011'0000; // lanes 4-5
		static constexpr unsigned char DmaStreamMask = 0b0100'0000; // lane 6
		static constexpr unsigned char MatrixTileStreamMask = 0b0100'0000; // lane 6 (aliased physical carrier)
		static constexpr unsigned char BranchMask = 0b1000'0000; // lane 7
		static constexpr unsigned char SystemMask = 0b1000'0000; // lane 7 (aliased with Branch)
		static constexpr unsigned char UnclassifiedMask = 0b1111'1111; // any lane (placeholder)

		/// <summary>
		/// Returns the physical lane bitmask for the given <see cref="SlotClass"/>.
		/// Each set bit indicates a lane that can host operations of that class.
		/// </summary>
		/// <param name="slotClass">The slot class to query.</param>
		/// <returns>8-bit lane mask (bit N → lane N is eligible).</returns>
	public:
		static unsigned char GetLaneMask(SlotClass slotClass);

		/// <summary>
		/// Returns the number of physical lanes available for the given <see cref="SlotClass"/>.
		/// Equivalent to PopCount of <see cref="GetLaneMask"/>.
		/// </summary>
		/// <param name="slotClass">The slot class to query.</param>
		/// <returns>Lane count (1–8).</returns>
		static int GetClassCapacity(SlotClass slotClass);

		/// <summary>
		/// Returns the set of <see cref="SlotClass"/> values that share at least one physical lane
		/// with the given class (i.e., are aliased on the same hardware lane).
		/// For the default W=8 topology, BranchControl and SystemSingleton both map to lane 7.
		/// </summary>
		/// <param name="slotClass">The slot class to query.</param>
		/// <returns>
		/// A read-only span of co-aliased classes (excluding <paramref name="slotClass"/> itself).
		/// Empty if no aliasing exists.
		/// </returns>
		static std::span<const SlotClass> GetAliasedClasses(SlotClass slotClass);

		/// <summary>
		/// Returns <see langword="true"/> if the given <see cref="SlotClass"/> shares physical lanes
		/// with another class (aliased-lane condition).
		/// </summary>
		/// <param name="slotClass">The slot class to query.</param>
		/// <returns><see langword="true"/> for BranchControl and SystemSingleton; otherwise <see langword="false"/>.</returns>
		static bool HasAliasedLanes(SlotClass slotClass);
	};

	/// <summary>
	/// Typed-slot facts communicated between compiler and runtime.
	/// Compiler sets these during bundle formation (Phase 8);
	/// runtime reads them during class-admission and lane-binding.
	/// <para>
	/// HLS design note: fixed-size — 8 × 3-bit SlotClass + 8-bit pinning mask +
	/// 2 × 32-bit counts + 5 × 8-bit class counts ≈ 80 flip-flops, no dynamic allocation.
	/// </para>
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct TypedSlotBundleFacts
	class TypedSlotBundleFacts
	{
	private:
		SlotClass Slot0Class = static_cast<SlotClass>(0);
		SlotClass Slot1Class = static_cast<SlotClass>(0);
		SlotClass Slot2Class = static_cast<SlotClass>(0);
		SlotClass Slot3Class = static_cast<SlotClass>(0);
		SlotClass Slot4Class = static_cast<SlotClass>(0);
		SlotClass Slot5Class = static_cast<SlotClass>(0);
		SlotClass Slot6Class = static_cast<SlotClass>(0);
		SlotClass Slot7Class = static_cast<SlotClass>(0);
		unsigned char PinningKindMask = 0;
		int FlexibleOpCount = 0;
		int PinnedOpCount = 0;
		unsigned char AluCount = 0;
		unsigned char LsuCount = 0;
		unsigned char DmaStreamCount = 0;
		unsigned char MatrixTileStreamCount = 0;
		unsigned char BranchControlCount = 0;
		unsigned char SystemSingletonCount = 0;

		/// <summary>Required slot class for bundle slot 0.</summary>
	public:
		SlotClass getSlot0Class() const;
		void setSlot0Class(SlotClass value);

		/// <summary>Required slot class for bundle slot 1.</summary>
		SlotClass getSlot1Class() const;
		void setSlot1Class(SlotClass value);

		/// <summary>Required slot class for bundle slot 2.</summary>
		SlotClass getSlot2Class() const;
		void setSlot2Class(SlotClass value);

		/// <summary>Required slot class for bundle slot 3.</summary>
		SlotClass getSlot3Class() const;
		void setSlot3Class(SlotClass value);

		/// <summary>Required slot class for bundle slot 4.</summary>
		SlotClass getSlot4Class() const;
		void setSlot4Class(SlotClass value);

		/// <summary>Required slot class for bundle slot 5.</summary>
		SlotClass getSlot5Class() const;
		void setSlot5Class(SlotClass value);

		/// <summary>Required slot class for bundle slot 6.</summary>
		SlotClass getSlot6Class() const;
		void setSlot6Class(SlotClass value);

		/// <summary>Required slot class for bundle slot 7.</summary>
		SlotClass getSlot7Class() const;
		void setSlot7Class(SlotClass value);

		/// <summary>
		/// Per-slot pinning kind mask (bit per slot: 0 = flexible, 1 = pinned).
		/// Bit N corresponds to slot N.
		/// </summary>
		unsigned char getPinningKindMask() const;
		void setPinningKindMask(unsigned char value);

		/// <summary>Total class-flexible ops in this bundle.</summary>
		int getFlexibleOpCount() const;
		void setFlexibleOpCount(int value);

		/// <summary>Total hard-pinned ops in this bundle.</summary>
		int getPinnedOpCount() const;
		void setPinnedOpCount(int value);

		/// <summary>Number of ALU-class ops in the bundle (compiler structural budget).</summary>
		unsigned char getAluCount() const;
		void setAluCount(unsigned char value);

		/// <summary>Number of LSU-class ops in the bundle (compiler structural budget).</summary>
		unsigned char getLsuCount() const;
		void setLsuCount(unsigned char value);

		/// <summary>Number of DMA/Stream-class ops in the bundle (compiler structural budget).</summary>
		unsigned char getDmaStreamCount() const;
		void setDmaStreamCount(unsigned char value);

		/// <summary>Number of MatrixTile stream-class ops in the bundle.</summary>
		unsigned char getMatrixTileStreamCount() const;
		void setMatrixTileStreamCount(unsigned char value);

		/// <summary>Number of BranchControl-class ops in the bundle (compiler structural budget).</summary>
		unsigned char getBranchControlCount() const;
		void setBranchControlCount(unsigned char value);

		/// <summary>Number of SystemSingleton-class ops in the bundle (compiler structural budget).</summary>
		unsigned char getSystemSingletonCount() const;
		void setSystemSingletonCount(unsigned char value);

		/// <summary>
		/// Returns <see langword="true"/> when all counts are zero,
		/// indicating no typed-slot facts were provided (legacy / pre-Phase 8 bundle).
		/// <c>default(TypedSlotBundleFacts).IsEmpty == true</c>.
		/// </summary>
		bool isEmpty() const;

		/// <summary>
		/// Gets the per-slot <see cref="SlotClass"/> by index (0–7).
		/// </summary>
		SlotClass GetSlotClass(int index) const;

		/// <summary>
		/// Returns whether slot at <paramref name="index"/> is hard-pinned (bit set in <see cref="PinningKindMask"/>).
		/// </summary>
		bool IsSlotPinned(int index) const;

		/// <summary>
		/// Builds <see cref="TypedSlotBundleFacts"/> from a live VLIW bundle.
		/// Pure function — scans the array, counts per-class ops, and encodes pinning mask.
		/// </summary>
		/// <param name="bundle">The VLIW bundle (nullable slots, up to W=8).</param>
		/// <param name="bundleWidth">Number of slots to scan (default 8).</param>
		/// <returns>Fully populated facts struct.</returns>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static TypedSlotBundleFacts FromBundle(MicroOp?[] bundle, int bundleWidth = 8)
		static TypedSlotBundleFacts FromBundle(std::vector<std::shared_ptr<MicroOp>> &bundle, int bundleWidth = 8);
	};

	/// <summary>
	/// Cross-validation report generated when runtime rejects an op
	/// that compiler declared structurally admissible.
	/// Distinguishes structural mismatches (compiler bugs) from
	/// dynamic rejects (expected runtime behavior).
	/// <para>
	/// HLS design note: diagnostic-only struct, not on critical timing path.
	/// </para>
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AgreementViolationReport
	class AgreementViolationReport
	{
	private:
		TypedSlotBundleFacts CompilerFacts;
		TypedSlotRejectReason RuntimeReject = static_cast<TypedSlotRejectReason>(0);
		bool IsStructuralMismatch = false;

		/// <summary>What compiler declared about the bundle.</summary>
	public:
		TypedSlotBundleFacts getCompilerFacts() const;
		void setCompilerFacts(TypedSlotBundleFacts value);

		/// <summary>What runtime rejected and why.</summary>
		TypedSlotRejectReason getRuntimeReject() const;
		void setRuntimeReject(TypedSlotRejectReason value);

		/// <summary>
		/// <see langword="true"/> if this is a structural mismatch (compiler bug):
		/// <see cref="TypedSlotRejectReason.StaticClassOvercommit"/> or
		/// <see cref="TypedSlotRejectReason.PinnedLaneConflict"/>.
		/// <see langword="false"/> for dynamic rejects (expected runtime behavior).
		/// </summary>
		bool isStructuralMismatch() const;
		void setStructuralMismatch(bool value);

		/// <summary>
		/// Classifies a runtime reject reason as structural (compiler bug) or dynamic (expected).
		/// </summary>
		static bool IsStructuralReject(TypedSlotRejectReason reason);

		/// <summary>
		/// Classifies a runtime reject reason as dynamic (expected runtime behavior).
		/// </summary>
		static bool IsDynamicReject(TypedSlotRejectReason reason);
	};
}
