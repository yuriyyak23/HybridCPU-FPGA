#pragma once

#include "HazardEffectKind.h"

namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }
#include "../Pipeline/Scheduling/SlotPlacementMetadata.h"
#include "../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../Pipeline/Safety/ResourceBitset.h"
#include "DecodedInstructionBundle.h"
namespace YAKSys_Hybrid_CPU::Core::Legality { class BundleLegalityDescriptor; }
namespace YAKSys_Hybrid_CPU::Core { class TrapMicroOp; }
namespace YAKSys_Hybrid_CPU::Core { struct MicroOpAdmissionMetadata; }
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <optional>
#include <format>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class SlotHazardQueryResult; }

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Decode-structural severity classification for a dependency pair within a bundle.
	/// Determined solely from register lists, slot class, and memory bank intent вЂ"
	/// never from runtime state, pipeline occupancy, or scheduling policy.
	/// This is a triage hint for the runtime, not a final admission decision.
	/// </summary>
	enum class HazardTriageClass : unsigned char
	{
		/// The slot pair has no bundle-local dependency that would prevent parallel issue.
		Safe = 0,

		/// Decode sees a potential conflict, but runtime may resolve it
		/// (e.g., WAR between scalar slots, cross-class memory bank divergence).
		NeedsRuntimeCheck = 1,

		/// Decode is certain that parallel issue is impossible for this pair
		/// (e.g., RAW or WAW between two scalar-cluster-internal slots).
		HardReject = 2
	};

	/// <summary>
	/// Dominant effect type for a decoded bundle-local hazard.
	/// This is additive metadata for compiler-facing analysis and does not alter runtime admission.
	/// </summary>
	

	/// <summary>
	/// Typed classification for a slot's relationship to the scalar cluster target.
	/// Provides explicit contract for downstream consumers without policy leakage.
	/// </summary>
	enum class ScalarLaneTargetClass : unsigned char
	{
		/// Slot is not eligible for the scalar cluster (memory, vector, control, system, DMA, or empty/NOP).
		NotEligible = 0,

		/// <summary>
		/// Slot is structurally eligible for the scalar cluster but excluded from the current
		/// prepared group due to bundle-local hazards or the 4-way ceiling.
		/// </summary>
		Eligible = 1,

		/// <summary>
		/// Slot is eligible and admitted into the prepared scalar issue group (within the 4-way ceiling).
		/// </summary>
		Prepared = 2
	};

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum DecodedBundleFlags : ushort
	enum class DecodedBundleFlags : int
	{
		None = 0,
		HasValidSlots = 1 << 0,
		HasEmptyOrNopSlots = 1 << 1,
		HasControlFlow = 1 << 2,
		HasMemoryOps = 1 << 3,
		HasVectorOps = 1 << 4,
		HasRegisterWrites = 1 << 5,
		HasPinnedOps = 1 << 6,
		HasCrossThreadSpan = 1 << 7
	};

	enum class DecodedBundleStateKind : unsigned char
	{
		Empty = 0,
		Canonical = 1,
		ForegroundMutated = 2,
		Replay = 3,
		DecodeFault = 4
	};

	enum class DecodedBundleStateOrigin : unsigned char
	{
		None = 0,
		Reset = 1,
		CanonicalDecode = 2,
		DecodeFallbackTrap = 3,
		ReplayBundleLoad = 4,
		ForegroundBundlePublication = 5,
		SingleSlotMutation = 7,
		ClearMaskMutation = 8,
		FspPacking = 9
	};

	enum class DecodedBundleStateOwnerKind : unsigned char
	{
		None = 0,
		BaseRuntimePublication = 1,
		DerivedIssuePlanPublication = 2
	};

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct DecodedBundleSlotDescriptor
	class DecodedBundleSlotDescriptor
	{
	private:
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> MicroOp;
		unsigned char SlotIndex;
		int VirtualThreadId;
		int OwnerThreadId;
		unsigned int OpCode;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<int> ReadRegisters;
		std::vector<int> ReadRegisters;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<int> WriteRegisters;
		std::vector<int> WriteRegisters;
		bool WritesRegister;
		bool IsMemoryOp;
		bool IsControlFlow;
		SlotPlacementMetadata Placement;
		int MemoryBankIntent;
		bool IsFspInjected;
		bool IsEmptyOrNop;
		bool IsVectorOp;

		static inline const SlotPlacementMetadata EmptySlotPlacement = SlotPlacementMetadata {.RequiredSlotClass = SlotClass::Unclassified, .PinningKind = SlotPinningKind::HardPinned, .PinnedLaneId = 0, .DomainTag = 0};

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DecodedBundleSlotDescriptor Create(byte slotIndex, MicroOp? microOp, System.Nullable<bool> isVectorOpOverride = null)
		static DecodedBundleSlotDescriptor Create(unsigned char slotIndex, std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> microOp, const std::optional<bool> &isVectorOpOverride = std::nullopt);

	private:
		static const std::vector<int> NormalizeWriteRegisters(std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> microOp, std::shared_ptr<MicroOpAdmissionMetadata> admission);

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public DecodedBundleSlotDescriptor(MicroOp microOp, byte slotIndex, int virtualThreadId, int ownerThreadId, uint opCode, IReadOnlyList<int> readRegisters, IReadOnlyList<int> writeRegisters, bool writesRegister, bool isMemoryOp, bool isControlFlow, SlotPlacementMetadata placement, int memoryBankIntent, bool isFspInjected, bool isEmptyOrNop, bool isVectorOp = false)
		DecodedBundleSlotDescriptor(std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> microOp, unsigned char slotIndex, int virtualThreadId, int ownerThreadId, unsigned int opCode, const std::vector<int> &readRegisters, const std::vector<int> &writeRegisters, bool writesRegister, bool isMemoryOp, bool isControlFlow, SlotPlacementMetadata placement, int memoryBankIntent, bool isFspInjected, bool isEmptyOrNop, bool isVectorOp = false);

		std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> getMicroOp() const;
		unsigned char getSlotIndex() const;
		int getVirtualThreadId() const;
		int getOwnerThreadId() const;
		unsigned int getOpCode() const;
		const std::vector<int> getReadRegisters() const;
		const std::vector<int> getWriteRegisters() const;
		bool getWritesRegister() const;
		bool isMemoryOp() const;
		bool isControlFlow() const;
		SlotPlacementMetadata getPlacement() const;
		int getMemoryBankIntent() const;
		bool isFspInjected() const;
		bool isEmptyOrNop() const;
		bool isVectorOp() const;
		bool isValid() const;

		SlotPlacementMetadata GetRuntimeAdmissionPlacement() const;

		bool GetRuntimeAdmissionIsMemoryOp() const;

		bool GetRuntimeAdmissionIsControlFlow() const;

		bool GetRuntimeAdmissionWritesRegister() const;

		const std::vector<int> GetRuntimeAdmissionReadRegisters() const;

		const std::vector<int> GetRuntimeAdmissionWriteRegisters() const;

		int GetRuntimeExecutionVirtualThreadId() const;

		int GetRuntimeExecutionOwnerThreadId() const;

		unsigned int GetRuntimeExecutionOpCode() const;

		unsigned long long GetRuntimeAdmissionDomainTag() const;

		bool GetRuntimeExecutionIsFspInjected() const;

		bool GetRuntimeExecutionIsEmptyOrNop() const;

		int GetRuntimeExecutionMemoryBankIntent() const;

		/// <summary>
		/// Whether this slot is structurally eligible for the 4-way scalar cluster from decode facts alone.
		/// Mirrors the decode-side eligibility rule: non-empty, non-memory, non-control-flow, non-vector, AluClass.
		/// This is a preparatory hint, not a final admission decision.
		/// </summary>
		bool getCanTargetScalarCluster() const;

		/// <summary>
		/// Classify this slot's relationship to the scalar cluster target given the bundle-wide prepared mask.
		/// Returns <see cref="ScalarLaneTargetClass.NotEligible"/> if the slot cannot target the scalar cluster,
		/// <see cref="ScalarLaneTargetClass.Prepared"/> if it was admitted into the prepared group,
		/// or <see cref="ScalarLaneTargetClass.Eligible"/> if it is structurally eligible but excluded
		/// from the current prepared group (due to hazards or the 4-way ceiling).
		/// </summary>
		ScalarLaneTargetClass ClassifyScalarLaneTarget(unsigned char preparedMask) const;

		/// <summary>
		/// Returns <see langword="true"/> when the two register sets share at least one register ID
		/// and both slots belong to the same virtual thread.
		/// <para>
		/// Blueprint В§7: "РЎРєРѕСЂСЂРµРєС‚РёСЂРѕРІР°С‚СЊ HasRegisterIntersection: РІРѕР·РјРѕР¶РЅРѕ, РґРµР»Р°С‚СЊ РµРіРѕ С‡Р°СЃС‚СЊСЋ
		/// SlotDescriptor, Р° РЅРµ СЃС‚Р°С‚РёС‡РµСЃРєРёРј РїСЂРёРІР°С‚РЅС‹Рј РјРµС‚РѕРґРѕРј."
		/// </para>
		/// </summary>
		/// <param name="left">First slot (provides its VT for thread-isolation check).</param>
		/// <param name="right">Second slot (provides its VT for thread-isolation check).</param>
		/// <param name="leftRegs">Register list from <paramref name="left"/> to inspect.</param>
		/// <param name="rightRegs">Register list from <paramref name="right"/> to inspect.</param>
		/// <returns><see langword="true"/> if the two slots are in the same VT and share at least one register.</returns>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static bool HasRegisterIntersection(in DecodedBundleSlotDescriptor left, in DecodedBundleSlotDescriptor right, IReadOnlyList<int> leftRegs, IReadOnlyList<int> rightRegs)
		static bool HasRegisterIntersection(DecodedBundleSlotDescriptor const left, DecodedBundleSlotDescriptor const right, const std::vector<int> &leftRegs, const std::vector<int> &rightRegs);

		DecodedBundleSlotDescriptor() = default;
	};

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct DecodedBundleTypedSlotFacts
	class DecodedBundleTypedSlotFacts
	{
	private:
		const unsigned char AluClassMask = 0;
		const unsigned char LsuClassMask = 0;
		const unsigned char DmaStreamClassMask = 0;
		const unsigned char MatrixTileStreamClassMask = 0;
		const unsigned char BranchControlMask = 0;
		const unsigned char SystemSingletonMask = 0;
		const unsigned char UnclassifiedMask = 0;
		const unsigned char PinnedSlotMask = 0;
		const unsigned char FlexibleSlotMask = 0;

	public:
		DecodedBundleTypedSlotFacts(unsigned char aluClassMask, unsigned char lsuClassMask, unsigned char dmaStreamClassMask, unsigned char matrixTileStreamClassMask, unsigned char branchControlMask, unsigned char systemSingletonMask, unsigned char unclassifiedMask, unsigned char pinnedSlotMask, unsigned char flexibleSlotMask);

		unsigned char getAluClassMask() const;
		unsigned char getLsuClassMask() const;
		unsigned char getDmaStreamClassMask() const;
		unsigned char getMatrixTileStreamClassMask() const;
		unsigned char getBranchControlMask() const;
		unsigned char getSystemSingletonMask() const;
		unsigned char getUnclassifiedMask() const;
		unsigned char getPinnedSlotMask() const;
		unsigned char getFlexibleSlotMask() const;

		int getAluClassCount() const;
		int getLsuClassCount() const;
		int getDmaStreamClassCount() const;
		int getMatrixTileStreamClassCount() const;
		int getBranchControlCount() const;
		int getSystemSingletonCount() const;
		int getUnclassifiedCount() const;

		DecodedBundleTypedSlotFacts() = default;
	};

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct DecodedBundleDependencySummary
	class DecodedBundleDependencySummary
	{
	private:
		unsigned long long ReadRegisterMask;
		unsigned long long WriteRegisterMask;
		ResourceBitset AggregateResourceMask;
		unsigned long long RawDependencyMask;
		unsigned long long WawDependencyMask;
		unsigned long long WarDependencyMask;
		unsigned long long ControlConflictMask;
		unsigned long long MemoryConflictMask;
		unsigned char ScalarClusterEligibleMask;

	public:
		DecodedBundleDependencySummary(unsigned long long readRegisterMask, unsigned long long writeRegisterMask, ResourceBitset aggregateResourceMask, unsigned long long rawDependencyMask, unsigned long long wawDependencyMask, unsigned long long warDependencyMask, unsigned long long controlConflictMask, unsigned long long memoryConflictMask, unsigned char scalarClusterEligibleMask, unsigned long long systemBarrierConflictMask = 0, unsigned long long pinnedLaneConflictMask = 0);

	private:
		unsigned long long _systemBarrierConflictMask;
		unsigned long long _pinnedLaneConflictMask;

	public:
		unsigned long long getReadRegisterMask() const;
		unsigned long long getWriteRegisterMask() const;
		ResourceBitset getAggregateResourceMask() const;

		/// <summary>
		/// Directed 8x8 RAW matrix encoded in a 64-bit mask.
		/// Bit <c>(sourceSlot * 8 + targetSlot)</c> is set when an earlier slot writes
		/// a register read by a later slot.
		/// </summary>
		unsigned long long getRawDependencyMask() const;

		/// <summary>
		/// Directed 8x8 WAW matrix encoded in a 64-bit mask.
		/// Bit <c>(sourceSlot * 8 + targetSlot)</c> is set when two slots target the
		/// same architectural destination in bundle order.
		/// </summary>
		unsigned long long getWawDependencyMask() const;

		/// <summary>
		/// Directed 8x8 WAR matrix encoded in a 64-bit mask.
		/// Bit <c>(sourceSlot * 8 + targetSlot)</c> is set when an earlier slot reads
		/// a register written by a later slot.
		/// </summary>
		unsigned long long getWarDependencyMask() const;

		/// <summary>
		/// Pairwise singleton conflict matrix for branch/system lane aliasing.
		/// Bit <c>(sourceSlot * 8 + targetSlot)</c> is set when both slots require the
		/// aliased singleton control lane.
		/// </summary>
		unsigned long long getControlConflictMask() const;

		/// <summary>
		/// Pairwise coarse memory-ordering conflict matrix.
		/// Bit <c>(sourceSlot * 8 + targetSlot)</c> is set when two bundle slots are both
		/// memory operations and decode-side facts indicate they should stay ordered.
		/// </summary>
		unsigned long long getMemoryConflictMask() const;

		/// <summary>
		/// Mask of slots that are obvious scalar-cluster candidates from decode facts alone.
		/// This is a preparatory hint for later wide scalar admission, not a final policy decision.
		/// </summary>
		unsigned char getScalarClusterEligibleMask() const;

		/// <summary>
		/// Directed per-slot hazard query: for a given slot within a scalar group,
		/// returns which peer slots are HardReject and which NeedsRuntimeCheck.
		/// Projects from the existing 8x8 dependency matrices using slot class context.
		/// </summary>
		/// <param name="slotIndex">The slot to query (0..7).</param>
		/// <param name="scalarGroupMask">Mask of slots in the scalar cluster group being evaluated.</param>
		SlotHazardQueryResult QuerySlotHazards(unsigned char slotIndex, unsigned char scalarGroupMask) const;

		HazardTriageClass QueryPairHazard(unsigned char slotIndex, unsigned char peerIndex, unsigned char scalarGroupMask, HazardEffectKind &dominantEffectKind) const;

		/// <summary>
		/// Compute a refined scalar-cluster readiness mask that blocks a slot only when it has
		/// a <see cref="HazardTriageClass.HardReject"/> peer within the scalar group.
		/// This replaces the overly conservative flat blocking used by <see cref="DecodedBundleAdmissionPrep.WideReadyScalarMask"/>.
		/// </summary>
		/// <param name="scalarCandidateMask">Scalar-eligible slots from decode facts.</param>
		unsigned char ComputeRefinedWideReadyScalarMask(unsigned char scalarCandidateMask) const;

		/// <summary>
		/// Classify the hazard severity for a dependency pair based on decode-structural facts.
		/// Per Phase 03 В§4.1 triage table.
		/// </summary>
	private:
		static HazardTriageClass ClassifyPairHazard(bool slotIsScalar, bool peerIsScalar, bool hasRaw, bool hasWaw, bool hasWar, bool hasMemory, bool hasControlFlow, bool hasSystemBarrier, bool hasPinnedLane, HazardEffectKind &dominantEffectKind);

		DecodedBundleDependencySummary() : ReadRegisterMask(0), WriteRegisterMask(0), AggregateResourceMask(), RawDependencyMask(0), WawDependencyMask(0), WarDependencyMask(0), ControlConflictMask(0), MemoryConflictMask(0), ScalarClusterEligibleMask(0), _systemBarrierConflictMask(0), _pinnedLaneConflictMask(0)
		{
		}
	};

	/// <summary>
	/// Result of a directed per-slot hazard query within the scalar cluster context.
	/// Contains peer masks classified by hazard severity.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct SlotHazardQueryResult
	class SlotHazardQueryResult
	{
	private:
		const unsigned char HardRejectPeers = 0;
		const unsigned char NeedsCheckPeers = 0;
		const HazardEffectKind DominantEffectKind = HazardEffectKind::RegisterData;

	public:
		SlotHazardQueryResult(unsigned char hardRejectPeers, unsigned char needsCheckPeers, HazardEffectKind dominantEffectKind = HazardEffectKind::RegisterData);

		/// <summary>
		/// Mask of peer slots with HardReject severity вЂ" parallel issue is impossible.
		/// </summary>
		unsigned char getHardRejectPeers() const;

		/// <summary>
		/// Mask of peer slots with NeedsRuntimeCheck severity вЂ" runtime may resolve the conflict.
		/// </summary>
		unsigned char getNeedsCheckPeers() const;

		/// <summary>
		/// Dominant hazard effect kind observed for this query.
		/// Defaults to <see cref="HazardEffectKind.RegisterData"/> when no hazard was classified.
		/// </summary>
		HazardEffectKind getDominantEffectKind() const;

		/// <summary>
		/// True when the queried slot has no HardReject peers and can potentially proceed to wide path.
		/// </summary>
		bool isWideCandidate() const;

		SlotHazardQueryResult() = default;
	};

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum DecodedBundleAdmissionFlags : ushort
	enum class DecodedBundleAdmissionFlags : int
	{
		None = 0,
		HasScalarClusterCandidates = 1 << 0,
		HasWideReadyScalarCandidates = 1 << 1,
		HasBundleLocalDependencies = 1 << 2,
		HasControlConflicts = 1 << 3,
		HasMemoryOrderingConstraints = 1 << 4,
		HasAuxiliaryClusterOps = 1 << 5,
		SuggestNarrowFallback = 1 << 6
	};

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct DecodedBundleAdmissionPrep
	class DecodedBundleAdmissionPrep
	{
	private:
		unsigned char ScalarCandidateMask;
		unsigned char WideReadyScalarMask;
		unsigned char AuxiliaryOpMask;
		unsigned char NarrowOnlyMask;
		DecodedBundleAdmissionFlags Flags;

	public:
		DecodedBundleAdmissionPrep(unsigned char scalarCandidateMask, unsigned char wideReadyScalarMask, unsigned char auxiliaryOpMask, unsigned char narrowOnlyMask, DecodedBundleAdmissionFlags flags);

		/// <summary>
		/// Scalar operations that can target the 4-way scalar cluster from decode facts alone.
		/// </summary>
		unsigned char getScalarCandidateMask() const;

		/// <summary>
		/// Scalar candidates that currently have no bundle-local blockers in the decode-side legality substrate.
		/// </summary>
		unsigned char getWideReadyScalarMask() const;

		/// <summary>
		/// Non-empty operations that belong to auxiliary paths (LSU, DMA, vector, branch/system)
		/// rather than the scalar-cluster target.
		/// </summary>
		unsigned char getAuxiliaryOpMask() const;

		/// <summary>
		/// Non-empty operations that currently require the narrow/reference path.
		/// Includes auxiliary operations and scalar candidates blocked by bundle-local hazards.
		/// </summary>
		unsigned char getNarrowOnlyMask() const;

		/// <summary>
		/// Advisory decode-side flags for narrow-vs-wide readiness.
		/// </summary>
		DecodedBundleAdmissionFlags getFlags() const;

		int getScalarCandidateCount() const;
		int getWideReadyScalarCount() const;
		bool getSuggestNarrowFallback() const;

		DecodedBundleAdmissionPrep() = default;
	};

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct DecodedBundleTransportFacts
	class DecodedBundleTransportFacts
	{
	private:
		unsigned long long PC;
		std::vector<DecodedBundleSlotDescriptor> Slots;
		unsigned char ValidMask;
		unsigned char NopMask;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<DecodedBundleDependencySummary> DependencySummary;
		std::optional<DecodedBundleDependencySummary> DependencySummary;
		DecodedBundleAdmissionPrep AdmissionPrep;
		DecodedBundleStateKind StateKind;
		DecodedBundleStateOrigin StateOrigin;

	public:
		DecodedBundleTransportFacts(unsigned long long pc, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char validMask, unsigned char nopMask, std::optional<DecodedBundleDependencySummary> &dependencySummary, DecodedBundleAdmissionPrep admissionPrep, DecodedBundleStateKind stateKind, DecodedBundleStateOrigin stateOrigin);

		unsigned long long getPC() const;
		std::vector<DecodedBundleSlotDescriptor> getSlots() const;
		unsigned char getValidMask() const;
		unsigned char getNopMask() const;
		unsigned char getValidNonEmptyMask() const;
		std::optional<DecodedBundleDependencySummary> getDependencySummary() const;
		DecodedBundleAdmissionPrep getAdmissionPrep() const;
		DecodedBundleStateKind getStateKind() const;
		DecodedBundleStateOrigin getStateOrigin() const;

		DecodedBundleTransportFacts() = default;
	};

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct BundleProgressState
	class BundleProgressState
	{
	private:
		unsigned long long BundlePc;
		unsigned char RemainingMask;
		unsigned char NextSlotIndex;
		unsigned char ConsumedMask;

	public:
		static constexpr unsigned char TerminalSlotIndex = 8;

		BundleProgressState(unsigned long long bundlePc, unsigned char remainingMask, unsigned char nextSlotIndex, unsigned char consumedMask = 0);

		unsigned long long getBundlePc() const;
		unsigned char getRemainingMask() const;
		unsigned char getNextSlotIndex() const;
		unsigned char getConsumedMask() const;
		bool getHasRemaining() const;

		bool Contains(unsigned char slotIndex) const;

		static BundleProgressState CreateEmpty(unsigned long long bundlePc = 0);

		static BundleProgressState CreateForCursor(unsigned long long bundlePc, unsigned char remainingMask, unsigned char currentSlotIndex, unsigned char consumedMask = 0);

		BundleProgressState ConsumeSlot(unsigned char slotIndex) const;

		BundleProgressState ConsumeMask(unsigned char consumedSlotMask, unsigned char currentSlotIndex) const;

		BundleProgressState RebaseAfterSourceRefresh(unsigned char currentValidMask) const;

	private:
		static unsigned char ResolveNextSlotIndexForCursor(unsigned char remainingMask, unsigned char currentSlotIndex);

		BundleProgressState() = default;
	};

	enum class DecodedBundleCanonicalValidity : unsigned char
	{
		None = 0,
		Canonical = 1,
		DecodeFault = 2
	};

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] internal enum DecodedBundleTransformKind : ushort
	enum class DecodedBundleTransformKind : int
	{
		None = 0,
		Reset = 1 << 0,
		CanonicalDecode = 1 << 1,
		DecodeFallbackTrap = 1 << 2,
		ReplayBundleLoad = 1 << 3,
		ForegroundBundlePublication = 1 << 4,
		SingleSlotMutation = 1 << 6,
		ClearMaskMutation = 1 << 7,
		FspPacking = 1 << 8
	};

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct DecodedBundleTransformHistory
	class DecodedBundleTransformHistory
	{
	private:
		DecodedBundleTransformKind AppliedTransforms = DecodedBundleTransformKind::None;
		unsigned char MutationDepth = 0;
		DecodedBundleStateOrigin LatestOrigin = DecodedBundleStateOrigin::None;

	public:
		// C# readonly structs have a zero-initialized default value; preserve that
		// value semantics for containing native state structs.
		DecodedBundleTransformHistory() = default;
		DecodedBundleTransformHistory(DecodedBundleTransformKind appliedTransforms, unsigned char mutationDepth, DecodedBundleStateOrigin latestOrigin);

		DecodedBundleTransformKind getAppliedTransforms() const;
		unsigned char getMutationDepth() const;
		DecodedBundleStateOrigin getLatestOrigin() const;

		bool Contains(DecodedBundleTransformKind transformKind) const;

		static DecodedBundleTransformHistory CreateBase(DecodedBundleStateOrigin origin);

		DecodedBundleTransformHistory Append(DecodedBundleStateOrigin origin) const;

	private:
		static bool CountsAsMutation(DecodedBundleStateOrigin origin);

		static DecodedBundleTransformKind ResolveTransformKind(DecodedBundleStateOrigin origin);

	};

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct DecodedBundleRuntimeState
	class DecodedBundleRuntimeState
	{
	private:
		DecodedBundleTransportFacts TransportFacts;
		std::shared_ptr<Decoder::DecodedInstructionBundle> CanonicalDecode;
		std::shared_ptr<Legality::BundleLegalityDescriptor> LegalityDescriptor;
		DecodedBundleCanonicalValidity CanonicalValidity = DecodedBundleCanonicalValidity::None;
		DecodedBundleTransformHistory TransformHistory;
		DecodedBundleStateOwnerKind StateOwnerKind = DecodedBundleStateOwnerKind::None;
		unsigned long long StateEpoch = 0;
		unsigned long long StateVersion = 0;
		unsigned long long LineageStateVersion = 0;

	public:
		// Preserve C# value-type default state semantics for containing result types.
		DecodedBundleRuntimeState() = default;
		DecodedBundleRuntimeState(DecodedBundleTransportFacts transportFacts, std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalDecode, std::shared_ptr<Legality::BundleLegalityDescriptor> legalityDescriptor, DecodedBundleTransformHistory transformHistory, DecodedBundleStateOwnerKind stateOwnerKind = DecodedBundleStateOwnerKind::None, unsigned long long stateEpoch = 0, unsigned long long stateVersion = 0, unsigned long long lineageStateVersion = 0);

		DecodedBundleTransportFacts getTransportFacts() const;
		std::shared_ptr<Decoder::DecodedInstructionBundle> getCanonicalDecode() const;
		std::shared_ptr<Legality::BundleLegalityDescriptor> getLegalityDescriptor() const;
		DecodedBundleCanonicalValidity getCanonicalValidity() const;
		DecodedBundleTransformHistory getTransformHistory() const;
		DecodedBundleStateOwnerKind getStateOwnerKind() const;
		unsigned long long getStateEpoch() const;
		unsigned long long getStateVersion() const;
		unsigned long long getLineageStateVersion() const;
		unsigned long long getBundlePc() const;
		unsigned long long getBundleSerial() const;
		DecodedBundleStateKind getStateKind() const;
		DecodedBundleStateOrigin getStateOrigin() const;
		bool getHasCanonicalDecode() const;
		bool getHasCanonicalLegality() const;
		bool getHasDecodeFault() const;

		bool CanPreserveCanonicalForPc(unsigned long long pc) const;

		DecodedBundleRuntimeState RepublishTransport(DecodedBundleTransportFacts const transportFacts) const;

		DecodedBundleRuntimeState ApplyFspPacking(DecodedBundleTransportFacts const transportFacts) const;

		DecodedBundleRuntimeState ApplySingleSlotMutation(DecodedBundleTransportFacts const transportFacts) const;

		DecodedBundleRuntimeState ApplyProgressProjection(DecodedBundleTransportFacts const transportFacts) const;

		DecodedBundleRuntimeState WithPublicationIdentity(DecodedBundleStateOwnerKind stateOwnerKind, unsigned long long stateEpoch, unsigned long long stateVersion, unsigned long long lineageStateVersion) const;

		static DecodedBundleRuntimeState CreateCanonical(std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalDecode, std::shared_ptr<Legality::BundleLegalityDescriptor> legalityDescriptor, DecodedBundleTransportFacts const transportFacts);

		static DecodedBundleRuntimeState CreateFallback(unsigned long long pc, DecodedBundleTransportFacts const transportFacts, unsigned long long bundleSerial = 0);

		static DecodedBundleRuntimeState CreateReplay(DecodedBundleTransportFacts const transportFacts, unsigned long long bundleSerial = 0);

		static DecodedBundleRuntimeState CreateTransportOnly(DecodedBundleTransportFacts const transportFacts, unsigned long long bundleSerial = 0, const std::optional<DecodedBundleTransformHistory> &transformHistory = std::nullopt);

		static DecodedBundleRuntimeState CreateEmpty(unsigned long long pc, unsigned long long bundleSerial = 0);

	private:
		static DecodedBundleCanonicalValidity ResolveCanonicalValidity(std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalDecode, std::shared_ptr<Legality::BundleLegalityDescriptor> legalityDescriptor);

		static void ValidateBundleIdentity(unsigned long long transportPc, std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalDecode, std::shared_ptr<Legality::BundleLegalityDescriptor> legalityDescriptor);

		DecodedBundleRuntimeState ApplyTransform(DecodedBundleTransportFacts const transportFacts, DecodedBundleStateOrigin transformOrigin) const;

		static void ValidatePublicationIdentity(DecodedBundleStateOwnerKind stateOwnerKind, unsigned long long stateEpoch, unsigned long long stateVersion, unsigned long long lineageStateVersion);

		static void ValidateExpectedOrigin(DecodedBundleTransportFacts const transportFacts, DecodedBundleStateOrigin expectedOrigin);

	};

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct DecodedBundleDerivedIssuePlanState
	class DecodedBundleDerivedIssuePlanState
	{
	private:
		bool IsActive;
		DecodedBundleRuntimeState RuntimeState;

	public:
		DecodedBundleDerivedIssuePlanState(bool isActive, DecodedBundleRuntimeState runtimeState);

		bool isActive() const;
		DecodedBundleRuntimeState getRuntimeState() const;
		DecodedBundleTransportFacts getTransportFacts() const;
		unsigned long long getBundlePc() const;
		unsigned long long getBundleSerial() const;
		unsigned long long getStateEpoch() const;
		unsigned long long getStateVersion() const;
		unsigned long long getLineageStateVersion() const;

		bool MatchesBundlePc(unsigned long long pc) const;

		void ValidateAgainstBaseRuntimeState(DecodedBundleRuntimeState const baseRuntimeState, const std::wstring &caller) const;

		static DecodedBundleDerivedIssuePlanState CreateEmpty(unsigned long long bundlePc = 0);

		static DecodedBundleDerivedIssuePlanState CreateActive(DecodedBundleRuntimeState const runtimeState);

		DecodedBundleDerivedIssuePlanState() = default;
	};

	class DecodedBundleSlotCarrierBuilder final
	{
	private:
		static constexpr int BundleWidth = 8;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildTransportFacts(ulong pc, IReadOnlyList<MicroOp?> carrierBundle, DecodedBundleStateKind stateKind = DecodedBundleStateKind.ForegroundMutated, DecodedBundleStateOrigin stateOrigin = DecodedBundleStateOrigin.ForegroundBundlePublication)
		static DecodedBundleTransportFacts BuildTransportFacts(unsigned long long pc, const std::vector<std::shared_ptr<MicroOp>> &carrierBundle, DecodedBundleStateKind stateKind = DecodedBundleStateKind::ForegroundMutated, DecodedBundleStateOrigin stateOrigin = DecodedBundleStateOrigin::ForegroundBundlePublication);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildTransportFacts(ulong pc, IReadOnlyList<MicroOp?> carrierBundle, System.Nullable<DecodedBundleDependencySummary> dependencySummary, DecodedBundleStateKind stateKind = DecodedBundleStateKind.ForegroundMutated, DecodedBundleStateOrigin stateOrigin = DecodedBundleStateOrigin.ForegroundBundlePublication)
		static DecodedBundleTransportFacts BuildTransportFacts(unsigned long long pc, const std::vector<std::shared_ptr<MicroOp>> &carrierBundle, std::optional<DecodedBundleDependencySummary> &dependencySummary, DecodedBundleStateKind stateKind = DecodedBundleStateKind::ForegroundMutated, DecodedBundleStateOrigin stateOrigin = DecodedBundleStateOrigin::ForegroundBundlePublication);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildTransportFacts(ulong pc, IReadOnlyList<MicroOp?> carrierBundle, Decoder.DecodedInstructionBundle canonicalBundle, System.Nullable<DecodedBundleDependencySummary> dependencySummary)
		static DecodedBundleTransportFacts BuildTransportFacts(unsigned long long pc, const std::vector<std::shared_ptr<MicroOp>> &carrierBundle, std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalBundle, std::optional<DecodedBundleDependencySummary> &dependencySummary);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildTransportFactsFromSingleSlotMutationContour(ulong pc, IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, byte slotIndex, MicroOp? microOp)
		static DecodedBundleTransportFacts BuildTransportFactsFromSingleSlotMutationContour(unsigned long long pc, const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, unsigned char slotIndex, std::shared_ptr<MicroOp> microOp);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildTransportFactsFromClearMaskMutationContour(ulong pc, IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, byte clearSlotMask)
		static DecodedBundleTransportFacts BuildTransportFactsFromClearMaskMutationContour(unsigned long long pc, const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, unsigned char clearSlotMask);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static DecodedBundleTransportFacts BuildTransportFactsFromSlotDescriptorProjection(ulong pc, IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, DecodedBundleStateKind stateKind = DecodedBundleStateKind.ForegroundMutated, DecodedBundleStateOrigin stateOrigin = DecodedBundleStateOrigin.ForegroundBundlePublication)
		static DecodedBundleTransportFacts BuildTransportFactsFromSlotDescriptorProjection(unsigned long long pc, const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, DecodedBundleStateKind stateKind = DecodedBundleStateKind::ForegroundMutated, DecodedBundleStateOrigin stateOrigin = DecodedBundleStateOrigin::ForegroundBundlePublication);

		static DecodedBundleTransportFacts BuildTransportFactsFromProgressProjection(DecodedBundleTransportFacts const baseTransportFacts, unsigned char remainingMask);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveTransportFacts(IReadOnlyList<MicroOp?> carrierBundle, out DecodedBundleSlotDescriptor[] slots, out byte validMask, out byte nopMask, out System.Nullable<DecodedBundleDependencySummary> dependencySummary, out DecodedBundleAdmissionPrep admissionPrep)
		static void ResolveTransportFacts(const std::vector<std::shared_ptr<MicroOp>> &carrierBundle, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char &validMask, unsigned char &nopMask, std::optional<DecodedBundleDependencySummary> &dependencySummary, DecodedBundleAdmissionPrep &admissionPrep);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveTransportFacts(IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, out DecodedBundleSlotDescriptor[] slots, out byte validMask, out byte nopMask, out System.Nullable<DecodedBundleDependencySummary> dependencySummary, out DecodedBundleAdmissionPrep admissionPrep)
		static void ResolveTransportFacts(const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char &validMask, unsigned char &nopMask, std::optional<DecodedBundleDependencySummary> &dependencySummary, DecodedBundleAdmissionPrep &admissionPrep);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveTransportFactSummary(IReadOnlyList<DecodedBundleSlotDescriptor> slots, byte validMask, byte nopMask, out System.Nullable<DecodedBundleDependencySummary> dependencySummary, out DecodedBundleAdmissionPrep admissionPrep)
		static void ResolveTransportFactSummary(const std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char validMask, unsigned char nopMask, std::optional<DecodedBundleDependencySummary> &dependencySummary, DecodedBundleAdmissionPrep &admissionPrep);

		static unsigned long long EncodeDependencyRegisterMaskBit(int registerId, unsigned char slotIndex, const std::wstring &accessKind);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveCarrierTransportFactsShape(IReadOnlyList<MicroOp?> carrierBundle, out DecodedBundleSlotDescriptor[] slots, out byte validMask, out byte nopMask)
		static void ResolveCarrierTransportFactsShape(const std::vector<std::shared_ptr<MicroOp>> &carrierBundle, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char &validMask, unsigned char &nopMask);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveCanonicalCarrierTransportFactsShape(IReadOnlyList<MicroOp?> carrierBundle, Decoder.DecodedInstructionBundle canonicalBundle, out DecodedBundleSlotDescriptor[] slots, out byte validMask, out byte nopMask)
		static void ResolveCanonicalCarrierTransportFactsShape(const std::vector<std::shared_ptr<MicroOp>> &carrierBundle, std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalBundle, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char &validMask, unsigned char &nopMask);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static DecodedBundleSlotDescriptor ResolveCanonicalCarrierTransportSlotDescriptor(byte slotIndex, MicroOp? microOp, Decoder.DecodedInstructionBundle canonicalBundle)
		static DecodedBundleSlotDescriptor ResolveCanonicalCarrierTransportSlotDescriptor(unsigned char slotIndex, std::shared_ptr<MicroOp> microOp, std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalBundle);

		static DecodedBundleSlotDescriptor BuildCanonicalTrapTransportSlotDescriptor(unsigned char slotIndex, std::shared_ptr<TrapMicroOp> trapMicroOp, std::shared_ptr<Decoder::DecodedInstruction> decodedSlot, bool isVectorOp);

		static unsigned long long ResolveCanonicalTransportDomainTag(std::shared_ptr<MicroOp> microOp, std::shared_ptr<Decoder::DecodedInstruction> decodedSlot);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool ResolveCanonicalExecutionVectorClassification(byte slotIndex, MicroOp? microOp, Decoder.DecodedInstructionBundle canonicalBundle)
		static bool ResolveCanonicalExecutionVectorClassification(unsigned char slotIndex, std::shared_ptr<MicroOp> microOp, std::shared_ptr<Decoder::DecodedInstructionBundle> canonicalBundle);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveTransportFactsShape(IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, out DecodedBundleSlotDescriptor[] slots, out byte validMask, out byte nopMask)
		static void ResolveTransportFactsShape(const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char &validMask, unsigned char &nopMask);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveTransportFactsShapeWithSingleSlotMutation(IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, byte replacementSlotIndex, in DecodedBundleSlotDescriptor replacementSlot, out DecodedBundleSlotDescriptor[] slots, out byte validMask, out byte nopMask)
		static void ResolveTransportFactsShapeWithSingleSlotMutation(const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, unsigned char replacementSlotIndex, DecodedBundleSlotDescriptor const replacementSlot, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char &validMask, unsigned char &nopMask);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ResolveTransportFactsShapeWithClearMaskMutation(IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors, byte clearSlotMask, out DecodedBundleSlotDescriptor[] slots, out byte validMask, out byte nopMask)
		static void ResolveTransportFactsShapeWithClearMaskMutation(const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors, unsigned char clearSlotMask, std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char &validMask, unsigned char &nopMask);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static DecodedBundleSlotDescriptor NormalizeSlotDescriptor(byte slotIndex, IReadOnlyList<DecodedBundleSlotDescriptor> slotDescriptors)
		static DecodedBundleSlotDescriptor NormalizeSlotDescriptor(unsigned char slotIndex, const std::vector<DecodedBundleSlotDescriptor> &slotDescriptors);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static DecodedBundleAdmissionPrep BuildAdmissionPrep(IReadOnlyList<DecodedBundleSlotDescriptor> slots, byte validMask, byte nopMask, DecodedBundleDependencySummary dependencySummary)
		static DecodedBundleAdmissionPrep BuildAdmissionPrep(const std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char validMask, unsigned char nopMask, DecodedBundleDependencySummary dependencySummary);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static byte ResolveAuthoritativeScalarCandidateMask(IReadOnlyList<DecodedBundleSlotDescriptor> slots, byte scalarClusterEligibleMask)
		static unsigned char ResolveAuthoritativeScalarCandidateMask(const std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char scalarClusterEligibleMask);

		static bool IsAuthoritativeScalarAdmissionCandidate(DecodedBundleSlotDescriptor const slot);

		static unsigned long long EncodeSlotPair(int sourceSlotIndex, int targetSlotIndex);

		static unsigned char ExtractBlockedTargetSlotMask(unsigned long long pairMask);
	};


}
