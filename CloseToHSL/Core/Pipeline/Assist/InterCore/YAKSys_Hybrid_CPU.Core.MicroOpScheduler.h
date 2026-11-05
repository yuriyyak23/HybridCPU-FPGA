#pragma once

#include "../../Certificates/ReplayPhaseSubstrate.Interfaces.h"
#include "../Runtime/AssistRuntime.h"
#include "../../Certificates/ReplayPhaseSubstrate.h"
#include "../../MicroOps/Types/MicroOp.h"
#include "../../../../Processor/Core/YAKSys_Hybrid_CPU.PodController.h"
namespace YAKSys_Hybrid_CPU::Memory { class MemorySubsystem; class MemoryRequestToken; }
namespace HybridCPU_ISE::CloseToHSL::Memory::Banks { class BankArbitrator; }
#include "../../Certificates/BundleResourceCertificate.h"
#include "../../Safety/ResourceBitset.h"
#include "../../Certificates/ReplayPhaseSubstrate.Implementations.h"
#include "../../Certificates/YAKSys_Hybrid_CPU.Core.BundleResourceCertificate4Way.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
namespace YAKSys_Hybrid_CPU::Core { class AssistMicroOp; }
#include "../../Scheduling/MicroOpScheduler.h"
#include "../../../Memory/IHardwareOccupancyInput.h"
#include "../../Scheduling/PostStageBIssuedAttempt.h"
#include "../../Certificates/BundleResourceCertificate4Way.h"
#include "../../Scheduling/SlotLegality/SlotClassCapacity.h"
#include "../../Safety/SafetyVerifier.Types.h"

#include "../../Scheduling/MicroOpScheduler.ShadowOracle.h"
#include "../../Core/RuntimeClusterAdmissionPreparation.BundleIssuePacket.h"
#include "../../Scheduling/SlotPlacementMetadata.h"
#include "../../../Runtime/Events/Hypercalls/DomainHypercallCanonicalComposition.h"
#include "../../Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include "../../Core/RuntimeClusterAdmissionPreparation.IssuePacketLane.h"
namespace YAKSys_Hybrid_CPU::Core { class VmxMicroOp; }
#include "../../../Runtime/Events/Hypercalls/VirtualizationOperandSnapshot.h"
#include "../../../Runtime/Events/VmRead/VmReadScalarDeliveryCanonicalComposition.h"
#include "../../../Runtime/Events/VmRead/GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition.h"
#include "../../../Runtime/Events/VmRead/MemoryOwnedVmReadScalarDeliveryCanonicalComposition.h"
#include "../../../Runtime/Events/VmRead/CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition.h"
#include "../../../Runtime/Events/VmRead/CompletionSecondStageVmReadScalarDeliveryCanonicalComposition.h"
#include "../../../Runtime/Events/VmRead/PinBasedControlsVmReadScalarDeliveryCanonicalComposition.h"
#include "../../Scheduling/Smt/MicroOpScheduler.ResearchVirtualizationCanonicalComposition.h"
#include "../../Certificates/SafetyMask128.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <array>
#include <span>
#include <bit>
#include <deque>
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <any>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../../../../tangible_rectangular_vectors.h"

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { struct ResearchVirtualizationCanonicalCompositionResult; class ResearchVirtualizationCanonicalIssueComposition; }

using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;
using namespace YAKSys_Hybrid_CPU::Core::Memory;



using namespace YAKSys_Hybrid_CPU::Core::Decoder;



using namespace YAKSys_Hybrid_CPU::Memory;





using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;



namespace YAKSys_Hybrid_CPU::Core
{
    class MicroOpScheduler;
    class InterCoreAssistNomination final : public std::enable_shared_from_this<InterCoreAssistNomination>
    {
    private:
        const std::shared_ptr<MicroOpScheduler> owner;
        const int coreId;
    public:
        InterCoreAssistNomination(std::shared_ptr<MicroOpScheduler> owner, int coreId);
        void Withdraw();
    };
	/// <summary>
	/// Scoreboard entry type for distinguishing DMA from Outstanding Load/Store entries.
	/// HLS: 2-bit enum → 2 flip-flops per entry.
	/// </summary>
		/// <summary>Slot is free (no pending operation).</summary>

		/// <summary>DMA transaction pending (legacy behavior).</summary>

		/// <summary>Outstanding Load pending (MSHR tracking).</summary>

		/// <summary>Outstanding Store pending (store-buffer tracking).</summary>

	/// <summary>
	/// Micro-operation scheduler for Formally Safe Packing (FSP).
	///
	/// Manages 16 nomination ports (one per core in a Pod), a priority encoder
	/// for slot stealing, and per-VT scoreboards for DMA/MSHR tracking.
	///
	/// HLS design constraints:
	/// - Fixed-size arrays only (no dynamic allocation, no List/Dictionary)
	/// - Deterministic iteration order (ascending core ID = priority encoder)
	/// - All state fits in register file / LUTRAM (no BRAM for control path)
	/// - Single-cycle combinational paths for Nominate / TryStealSlot
	///
	/// Refactoring Pt. 1 integration:
	/// - globalHardwareMask (from MemorySubsystem.GetHardwareOccupancyMask128)
	///   is merged into legality checker conflict checks during PackBundle,
	///   replacing the ad-hoc SuppressLsu boolean bypass.
	///
	/// Refactoring Pt. 3 integration:
	/// - Universal Scoreboard: per-VT type-tagged entries track DMA, Outstanding Loads
	///   and Outstanding Stores. Memory bank IDs in scoreboard allow FSP and pipeline
	///   ID-stage to detect bank-level conflicts, preventing Load/Store injection when
	///   the target bank has in-flight MSHR entries for the same virtual thread.
	/// </summary>
	/// <summary>
	/// Reason category for oracle-gap decomposition.
	/// Each bucket explains why the real scheduler missed an oracle-legal slot.
	/// </summary>
		/// <summary>No gap — real scheduler matched oracle packing.</summary>

		/// <summary>Gap caused by replay-stable donor mask restricting injectable slots.</summary>

		/// <summary>Gap caused by fairness/credit or TDM ordering skipping a ready candidate.</summary>

		/// <summary>Gap caused by legality-layer conservatism (safety mask false reject).</summary>

		/// <summary>Gap caused by domain-isolation or kernel-to-user enforcement.</summary>

		/// <summary>Gap caused by speculation budget exhaustion.</summary>

		/// <summary>Phase 07: Gap caused by class-capacity template divergence between runs.</summary>

	/// <summary>
	/// Per-cycle oracle gap record: captures the difference between oracle and real packing.
	/// </summary>

		/// <summary>Scheduling cycle when this gap was observed.</summary>

		/// <summary>Number of slots the oracle reference contour packed.</summary>

		/// <summary>Number of slots the real scheduler packed.</summary>

		/// <summary>Primary reason bucket for the gap.</summary>

		/// <summary>Gap magnitude (oracle - real). Zero or positive.</summary>

	/// <summary>
	/// Structured counterexample: a specific scheduling scenario where the real scheduler
	/// missed an oracle-legal injection and the reason can be localized.
	/// </summary>

		/// <summary>Cycle where the counterexample was observed.</summary>

		/// <summary>Reason bucket classifying the false reject or missed opportunity.</summary>

		/// <summary>Number of slots missed in this counterexample.</summary>

		/// <summary>Human-readable description of the divergence shape.</summary>


	/// <summary>
	/// Aggregated oracle-gap summary across all scheduling cycles.
	/// </summary>

		/// <summary>Total slots packed by oracle across all analyzed cycles.</summary>

		/// <summary>Total slots packed by real scheduler across all analyzed cycles.</summary>

		/// <summary>Gap slots attributed to donor/replay mask restrictions.</summary>

		/// <summary>Gap slots attributed to fairness/TDM ordering.</summary>

		/// <summary>Gap slots attributed to legality-layer conservatism.</summary>

		/// <summary>Gap slots attributed to domain-isolation enforcement.</summary>

		/// <summary>Gap slots attributed to speculation budget exhaustion.</summary>

		/// <summary>Number of scheduling cycles where oracle packed more than real.</summary>

		/// <summary>Total scheduling cycles analyzed.</summary>

		/// <summary>Total gap (oracle - real) across all categories.</summary>

		/// <summary>Oracle efficiency rate: what fraction of oracle slots the real scheduler captured.</summary>

		/// <summary>Gap rate: fraction of oracle slots missed by the real scheduler.</summary>



	/// <summary>
	/// Phase 5: Shadow Oracle Scheduler — legality-preserving reference contour.
	///
	/// This partial class adds a diagnostic shadow-oracle path that estimates
	/// what a stronger scheduler could have packed under the same bundle and
	/// safety contract, but without replay-stable donor mask restrictions,
	/// fairness ordering, or speculation budget limits.
	///
	/// The oracle contour is opt-in, comparison-only, and never drives
	/// production scheduling decisions.
	/// </summary>
	/// <summary>
	/// TEST-ONLY partial extension for MicroOpScheduler.
	///
	/// Purpose: Enables performance testing without requiring real multi-threaded execution.
	/// This code is ONLY for testing and does NOT modify ISA, encoding, or production behavior.
	///
	/// Design rationale:
	/// - Tests need to simulate background thread nomination without complex threading
	/// - Production code remains unchanged (main MicroOpScheduler.cs)
	/// - TestMode flag gates all test-specific behavior
	/// - No changes to instruction encoding, VLIW slots, or ISA semantics
	///
	/// Created: 2026-03-02
	/// For: Performance test infrastructure (testPerfPlan.md Iteration 2)
	/// </summary>
	/// <summary>
	/// Default-off TESTING-only composition of the Phase 36 probe at the canonical
	/// issue/materialization seam. It owns no production dispatch, completion or retire
	/// authority; it can only consume a live E1 supplied by that seam.
	/// </summary>










	/// <summary>
	/// Phase 06 SMT eligibility helpers.
	/// Scheduler consumes an explicit runnable VT mask from the FSM-facing caller
	/// rather than deriving its own competing wait/wake truth model.
	/// </summary>
    class MicroOpScheduler : public ILegalityCertificateCacheTelemetrySink, public std::enable_shared_from_this<MicroOpScheduler>
	{
		class BundleOpportunityState;
		class ClassTemplateAdmissionState;
    public:
        struct SmtEligibilitySnapshot final {
            unsigned char RequestedMask = 0, NormalizedMask = 0, ReadyPortMask = 0, VisibleReadyMask = 0, MaskedReadyMask = 0;
            int getEligibleCount() const noexcept { return std::popcount(static_cast<unsigned int>(NormalizedMask)); }
            int getReadyPortCount() const noexcept { return std::popcount(static_cast<unsigned int>(ReadyPortMask)); }
            int getVisibleReadyCount() const noexcept { return std::popcount(static_cast<unsigned int>(VisibleReadyMask)); }
            int getMaskedReadyCount() const noexcept { return std::popcount(static_cast<unsigned int>(MaskedReadyMask)); }
            bool getHasMaskedReadyCandidates() const noexcept { return MaskedReadyMask != 0; }
        };
    private:
        enum class RejectionType;
	private:
		bool AssistInjectionEnabled = true;
		std::shared_ptr<AssistMemoryQuota> AssistMemoryQuotaPolicy = std::make_shared<AssistMemoryQuota>(AssistMemoryQuota::getDefault());
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::AssistBackpressurePolicy> AssistBackpressurePolicy = std::make_shared<YAKSys_Hybrid_CPU::Core::AssistBackpressurePolicy>(YAKSys_Hybrid_CPU::Core::AssistBackpressurePolicy::getDefault());
		long long AssistNominationCount = 0;
		long long AssistInjectionsCount = 0;
		long long AssistRejects = 0;
		long long AssistBoundaryRejects = 0;
		long long AssistInvalidations = 0;
		long long AssistInterCoreNominations = 0;
		long long AssistInterCoreInjections = 0;
		long long AssistInterCoreRejects = 0;
		long long AssistInterCoreDomainRejects = 0;
		long long AssistInterCorePodLocalInjections = 0;
		long long AssistInterCoreCrossPodInjections = 0;
		long long AssistInterCorePodLocalRejects = 0;
		long long AssistInterCoreCrossPodRejects = 0;
		long long AssistInterCorePodLocalDomainRejects = 0;
		long long AssistInterCoreCrossPodDomainRejects = 0;
		long long AssistInterCoreSameVtVectorInjects = 0;
		long long AssistInterCoreDonorVtVectorInjects = 0;
		long long AssistInterCoreSameVtVectorWritebackInjects = 0;
		long long AssistInterCoreDonorVtVectorWritebackInjects = 0;
		long long AssistInterCoreLane6DefaultStoreDonorPrefetchInjects = 0;
		long long AssistInterCoreLane6HotLoadDonorPrefetchInjects = 0;
		long long AssistInterCoreLane6HotStoreDonorPrefetchInjects = 0;
		long long AssistInterCoreLane6DonorPrefetchInjects = 0;
		long long AssistInterCoreLane6ColdStoreLdsaInjects = 0;
		long long AssistInterCoreLane6LdsaInjects = 0;
		long long AssistQuotaRejects = 0;
		long long AssistQuotaIssueRejects = 0;
		long long AssistQuotaLineRejects = 0;
		long long AssistQuotaLinesReserved = 0;
		long long AssistBackpressureRejects = 0;
		long long AssistBackpressureOuterCapRejects = 0;
		long long AssistBackpressureMshrRejects = 0;
		long long AssistBackpressureDmaSrfRejects = 0;
		long long AssistDonorPrefetchInjects = 0;
		long long AssistLdsaInjects = 0;
		long long AssistVdsaInjects = 0;
		long long AssistSameVtInjects = 0;
		long long AssistDonorVtInjects = 0;
		AssistInvalidationReason LastAssistInvalidationReason = AssistInvalidationReason::None;
		unsigned long long LastAssistOwnershipSignature = 0;
		long long FairnessStarvationEvents = 0;
		bool CreditFairnessEnabled = true;
		bool EnableLoopPhaseSampling = false;
		long long BankPressureAvoidanceCount = 0;
		bool BankPressureTieBreakEnabled = true;
		int SpeculationBudgetMax = SCOREBOARD_SLOTS / 2;
		long long SpeculationBudgetExhaustionEvents = 0;
		long long PeakConcurrentSpeculativeOps = 0;
		bool SpeculationBudgetEnabled = true;
		long long FspPipelineLatencyCycles = 0;
		long long TotalSchedulerCycles = 0;
		long long SuccessfulInjectionsCount = 0;
		long long SmtInjectionsCount = 0;
		long long SmtRejectionsCount = 0;
		long long ReplayAwareCycles = 0;
		long long PhaseCertificateReadyHits = 0;
		long long PhaseCertificateReadyMisses = 0;
		long long EstimatedPhaseCertificateChecksSaved = 0;
		long long PhaseCertificateInvalidations = 0;
		long long PhaseCertificateMutationInvalidations = 0;
		long long PhaseCertificatePhaseMismatchInvalidations = 0;
		ReplayPhaseInvalidationReason LastPhaseCertificateInvalidationReason = static_cast<ReplayPhaseInvalidationReason>(0);
		long long DeterminismReferenceOpportunitySlots = 0;
		long long DeterminismReplayEligibleSlots = 0;
		long long DeterminismMaskedSlots = 0;
		long long DeterminismEstimatedLostSlots = 0;
		long long DeterminismConstrainedCycles = 0;
		long long DomainIsolationProbeAttempts = 0;
		long long DomainIsolationBlockedAttempts = 0;
		long long DomainIsolationCrossDomainBlocks = 0;
		long long DomainIsolationKernelToUserBlocks = 0;
		long long SerializingBoundaryRejects = 0;
		long long ClassCapacityRejects = 0;
		long long LaneConflictRejects = 0;
		long long TypedSlotResourceConflictRejects = 0;
		long long SmtOwnerContextGuardRejects = 0;
		long long SmtDomainGuardRejects = 0;
		long long SmtBoundaryGuardRejects = 0;
		long long SmtSharedResourceCertificateRejects = 0;
		long long SmtRegisterGroupCertificateRejects = 0;
		long long SmtLegalityRejectByAluClass = 0;
		long long SmtLegalityRejectByLsuClass = 0;
		long long SmtLegalityRejectByDmaStreamClass = 0;
		long long SmtLegalityRejectByMatrixTileStreamClass = 0;
		long long SmtLegalityRejectByBranchControl = 0;
		long long SmtLegalityRejectBySystemSingleton = 0;
		long long TypedSlotScoreboardRejects = 0;
		long long TypedSlotBankPendingRejects = 0;
		long long BankPendingRejectBank0 = 0;
		long long BankPendingRejectBank1 = 0;
		long long BankPendingRejectBank2 = 0;
		long long BankPendingRejectBank3 = 0;
		long long BankPendingRejectBank4 = 0;
		long long BankPendingRejectBank5 = 0;
		long long BankPendingRejectBank6 = 0;
		long long BankPendingRejectBank7 = 0;
		long long BankPendingRejectBank8 = 0;
		long long BankPendingRejectBank9 = 0;
		long long BankPendingRejectBank10 = 0;
		long long BankPendingRejectBank11 = 0;
		long long BankPendingRejectBank12 = 0;
		long long BankPendingRejectBank13 = 0;
		long long BankPendingRejectBank14 = 0;
		long long BankPendingRejectBank15 = 0;
		long long MemoryClusteringEvents = 0;
		long long TypedSlotSpeculationBudgetRejects = 0;
		long long TypedSlotHardwareBudgetRejects = 0;
		long long TypedSlotAssistQuotaRejects = 0;
		long long TypedSlotAssistBackpressureRejects = 0;
		long long LaneReuseHits = 0;
		long long LaneReuseMisses = 0;
		long long ClassTemplateReuseHits = 0;
		long long ClassTemplateInvalidations = 0;
		long long TypedSlotFastPathAccepts = 0;
		long long StaticClassOvercommitRejects = 0;
		long long DynamicClassExhaustionRejects = 0;
		long long PinnedLaneConflicts = 0;
		long long InvalidPinnedLaneRejects = 0;
		long long LateBindingConflicts = 0;
		long long TypedSlotDomainRejects = 0;
		long long TypedSlotStandardPathAccepts = 0;
		long long TotalLaneBindings = 0;
		long long ClassTemplateDomainInvalidations = 0;
		long long ClassTemplateCapacityMismatchInvalidations = 0;
		long long NopDueToPinnedConstraint = 0;
		long long NopDueToNoClassCapacity = 0;
		long long NopDueToResourceConflict = 0;
		long long NopDueToDynamicState = 0;
		long long NopAvoided = 0;
		long long AluClassInjects = 0;
		long long LsuClassInjects = 0;
		long long DmaStreamClassInjects = 0;
		long long MatrixTileStreamClassInjects = 0;
		long long BranchControlInjects = 0;
		long long CertificateRejectByAluClass = 0;
		long long CertificateRejectByLsuClass = 0;
		long long CertificateRejectByDmaStreamClass = 0;
		long long CertificateRejectByMatrixTileStreamClass = 0;
		long long CertificateRejectByBranchControl = 0;
		long long CertificateRejectBySystemSingleton = 0;
		long long RejectionsVT0 = 0;
		long long RejectionsVT1 = 0;
		long long RejectionsVT2 = 0;
		long long RejectionsVT3 = 0;
		long long RegGroupConflictsVT0 = 0;
		long long RegGroupConflictsVT1 = 0;
		long long RegGroupConflictsVT2 = 0;
		long long RegGroupConflictsVT3 = 0;
		long long HardPinnedInjects = 0;
		long long ClassFlexibleInjects = 0;
		long long BankConflictsCount = 0;
		bool SuppressLsu = false;
		long long MemoryWallSuppressionsCount = 0;
		long long SuccessfulSpeculativeSteals = 0;
		long long FaultedSpeculativeSteals = 0;
		long long RejectedInjectionsCount = 0;
		long long RollbackEventsCount = 0;
		bool EnableShadowOracle = false;
		bool EnableRf06MemoryContractDifferential = false;
		bool TestMode = false;
		std::vector<std::deque<std::shared_ptr<MicroOp>>> TestNominationQueues;
		std::wstring LastRejectionReason = L"";
		long long TestRejectedDueToResourceConflict = 0;
		long long TestRejectedDueToMemoryConflict = 0;
		long long TestRejectedDueToSafetyMask = 0;
		long long TestRejectedDueToFullBundle = 0;
		long long MshrScoreboardStalls = 0;
		std::optional<std::shared_ptr<DomainHypercallCompositionResult>> LastExactVirtualizationCompositionResult;
		std::optional<std::shared_ptr<VmReadScalarDeliveryResult>> LastVmReadScalarDeliveryResult;
		std::optional<std::shared_ptr<ResearchVirtualizationCanonicalCompositionResult>> LastResearchVirtualizationCanonicalCompositionResult;
		bool PipelinedFspEnabled = false;
		bool TypedSlotEnabled = false;
		long long EligibilityMaskedCycles = 0;
		long long EligibilityMaskedReadyCandidates = 0;

	public:
		void NominateInterCoreAssistCandidate(int coreId, AssistInterCoreTransport transport);

		// Exact port custody for withdrawal only. Identical replacement value
		// transports must never inherit the prior producer's cleanup identity.
	public:
		using InterCoreAssistNomination = ::YAKSys_Hybrid_CPU::Core::InterCoreAssistNomination;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal InterCoreAssistNomination? NominateInterCoreAssistCandidateOwned(int coreId, AssistInterCoreTransport transport)
		std::shared_ptr<InterCoreAssistNomination> NominateInterCoreAssistCandidateOwned(int coreId, AssistInterCoreTransport transport);

		void ClearInterCoreAssistNominationPorts();

		bool IsCoreStalled(int coreId);

		void ClearInterCoreAssistNominationPort(int coreId);

		void ReconcileInterCoreAssistTransportOwnerSnapshot(int coreId, unsigned short podId, bool ownerSnapshotValid, PodController::AssistOwnerSnapshot const ownerSnapshot);

		bool TryPeekInterCoreAssistTransport(int coreId, AssistInterCoreTransport &transport);

		bool TryConsumeInterCoreAssistTransport(int coreId, AssistInterCoreTransport &transport);

	private:
		inline static bool IsInterCoreAssistTransportStale(unsigned short donorPodId, int donorCoreId, bool ownerSnapshotValid, PodController::AssistOwnerSnapshot const ownerSnapshot, AssistInterCoreTransport const transport);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static bool IsInterCoreAssistTransportStale(PodController? donorPod, int donorCoreId, in AssistInterCoreTransport transport)
		inline static bool IsInterCoreAssistTransportStale(std::shared_ptr<PodController> donorPod, int donorCoreId, AssistInterCoreTransport const transport);

		inline bool TryStealPodLocalInterCoreAssistTransport(int requestingCoreId, unsigned short requestingPodId, unsigned long long requestedDomainTag, AssistInterCoreTransport &transport);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private bool TryStealInterCoreAssistTransport(int requestingCoreId, ushort requestingPodId, ulong requestedDomainTag, out AssistInterCoreTransport transport, PodController?[]? pods = null)
		inline bool TryStealInterCoreAssistTransport(int requestingCoreId, unsigned short requestingPodId, unsigned long long requestedDomainTag, AssistInterCoreTransport &transport, std::shared_ptr<std::vector<std::shared_ptr<PodController>>> pods = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private int TryInjectInterCoreAssistCandidates(MicroOp[] result, ref BundleResourceCertificate bundleCert, ref BundleOpportunityState opportunityState, int ownerVirtualThreadId, int requestingCoreId, ushort requestingPodId, int bundleOwnerContextId, ulong bundleOwnerDomainTag, ulong requestedDomainTag, ulong assistRuntimeEpoch, ResourceBitset globalResourceLocks, int currentPassInjections, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null, System.Nullable<PodController>[]? pods = null)
		int TryInjectInterCoreAssistCandidates(std::vector<std::shared_ptr<MicroOp>> &result, std::shared_ptr<BundleResourceCertificate> &bundleCert, BundleOpportunityState &opportunityState, int ownerVirtualThreadId, int requestingCoreId, unsigned short requestingPodId, int bundleOwnerContextId, unsigned long long bundleOwnerDomainTag, unsigned long long requestedDomainTag, unsigned long long assistRuntimeEpoch, ResourceBitset globalResourceLocks, int currentPassInjections, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr, std::shared_ptr<std::vector<std::shared_ptr<PodController>>> pods = nullptr);


// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private bool TryStealCrossPodAssistTransport(ushort requestingPodId, ulong requestedDomainTag, out AssistInterCoreTransport transport, PodController?[]? pods = null)
		inline bool TryStealCrossPodAssistTransport(unsigned short requestingPodId, unsigned long long requestedDomainTag, AssistInterCoreTransport &transport, std::shared_ptr<std::vector<std::shared_ptr<PodController>>> pods = nullptr);


		static constexpr int AssistBundleQuota = 1;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly MicroOp?[] _smtAssistPorts = new MicroOp?[SMT_WAYS];
		std::vector<std::shared_ptr<MicroOp>> _smtAssistPorts = std::vector<std::shared_ptr<MicroOp>>(SMT_WAYS);
		std::vector<bool> _smtAssistPortValid = std::vector<bool>(SMT_WAYS);
		std::vector<AssistInterCoreTransport> _interCoreAssistPorts = std::vector<AssistInterCoreTransport>(NUM_PORTS);
		std::vector<bool> _interCoreAssistPortValid = std::vector<bool>(NUM_PORTS);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly InterCoreAssistNomination?[] _interCoreAssistNominationOwners = new InterCoreAssistNomination?[NUM_PORTS];
		std::vector<std::shared_ptr<InterCoreAssistNomination>> _interCoreAssistNominationOwners = std::vector<std::shared_ptr<InterCoreAssistNomination>>(NUM_PORTS);
		friend InterCoreAssistNomination;

	public:
		bool getAssistInjectionEnabled() const;
		void setAssistInjectionEnabled(bool value);

		AssistMemoryQuota getAssistMemoryQuotaPolicy() const;
		void setAssistMemoryQuotaPolicy(AssistMemoryQuota value);

		YAKSys_Hybrid_CPU::Core::AssistBackpressurePolicy getAssistBackpressurePolicy() const;
		void setAssistBackpressurePolicy(YAKSys_Hybrid_CPU::Core::AssistBackpressurePolicy value);

		long long getAssistNominationCount() const;
	private:
		void setAssistNominationCount(long long value);

	public:
		long long getAssistInjectionsCount() const;
	private:
		void setAssistInjectionsCount(long long value);

	public:
		long long getAssistRejects() const;
	private:
		void setAssistRejects(long long value);

	public:
		long long getAssistBoundaryRejects() const;
	private:
		void setAssistBoundaryRejects(long long value);

	public:
		long long getAssistInvalidations() const;
	private:
		void setAssistInvalidations(long long value);

	public:
		long long getAssistInterCoreNominations() const;
	private:
		void setAssistInterCoreNominations(long long value);

	public:
		long long getAssistInterCoreInjections() const;
	private:
		void setAssistInterCoreInjections(long long value);

	public:
		long long getAssistInterCoreRejects() const;
	private:
		void setAssistInterCoreRejects(long long value);

	public:
		long long getAssistInterCoreDomainRejects() const;
	private:
		void setAssistInterCoreDomainRejects(long long value);

	public:
		long long getAssistInterCorePodLocalInjections() const;
	private:
		void setAssistInterCorePodLocalInjections(long long value);

	public:
		long long getAssistInterCoreCrossPodInjections() const;
	private:
		void setAssistInterCoreCrossPodInjections(long long value);

	public:
		long long getAssistInterCorePodLocalRejects() const;
	private:
		void setAssistInterCorePodLocalRejects(long long value);

	public:
		long long getAssistInterCoreCrossPodRejects() const;
	private:
		void setAssistInterCoreCrossPodRejects(long long value);

	public:
		long long getAssistInterCorePodLocalDomainRejects() const;
	private:
		void setAssistInterCorePodLocalDomainRejects(long long value);

	public:
		long long getAssistInterCoreCrossPodDomainRejects() const;
	private:
		void setAssistInterCoreCrossPodDomainRejects(long long value);

	public:
		long long getAssistInterCoreSameVtVectorInjects() const;
	private:
		void setAssistInterCoreSameVtVectorInjects(long long value);

	public:
		long long getAssistInterCoreDonorVtVectorInjects() const;
	private:
		void setAssistInterCoreDonorVtVectorInjects(long long value);

	public:
		long long getAssistInterCoreSameVtVectorWritebackInjects() const;
	private:
		void setAssistInterCoreSameVtVectorWritebackInjects(long long value);

	public:
		long long getAssistInterCoreDonorVtVectorWritebackInjects() const;
	private:
		void setAssistInterCoreDonorVtVectorWritebackInjects(long long value);

	public:
		long long getAssistInterCoreLane6DefaultStoreDonorPrefetchInjects() const;
	private:
		void setAssistInterCoreLane6DefaultStoreDonorPrefetchInjects(long long value);

	public:
		long long getAssistInterCoreLane6HotLoadDonorPrefetchInjects() const;
	private:
		void setAssistInterCoreLane6HotLoadDonorPrefetchInjects(long long value);

	public:
		long long getAssistInterCoreLane6HotStoreDonorPrefetchInjects() const;
	private:
		void setAssistInterCoreLane6HotStoreDonorPrefetchInjects(long long value);

	public:
		long long getAssistInterCoreLane6DonorPrefetchInjects() const;
	private:
		void setAssistInterCoreLane6DonorPrefetchInjects(long long value);

	public:
		long long getAssistInterCoreLane6ColdStoreLdsaInjects() const;
	private:
		void setAssistInterCoreLane6ColdStoreLdsaInjects(long long value);

	public:
		long long getAssistInterCoreLane6LdsaInjects() const;
	private:
		void setAssistInterCoreLane6LdsaInjects(long long value);

	public:
		long long getAssistQuotaRejects() const;
	private:
		void setAssistQuotaRejects(long long value);

	public:
		long long getAssistQuotaIssueRejects() const;
	private:
		void setAssistQuotaIssueRejects(long long value);

	public:
		long long getAssistQuotaLineRejects() const;
	private:
		void setAssistQuotaLineRejects(long long value);

	public:
		long long getAssistQuotaLinesReserved() const;
	private:
		void setAssistQuotaLinesReserved(long long value);

	public:
		long long getAssistBackpressureRejects() const;
	private:
		void setAssistBackpressureRejects(long long value);

	public:
		long long getAssistBackpressureOuterCapRejects() const;
	private:
		void setAssistBackpressureOuterCapRejects(long long value);

	public:
		long long getAssistBackpressureMshrRejects() const;
	private:
		void setAssistBackpressureMshrRejects(long long value);

	public:
		long long getAssistBackpressureDmaSrfRejects() const;
	private:
		void setAssistBackpressureDmaSrfRejects(long long value);

	public:
		long long getAssistDonorPrefetchInjects() const;
	private:
		void setAssistDonorPrefetchInjects(long long value);

	public:
		long long getAssistLdsaInjects() const;
	private:
		void setAssistLdsaInjects(long long value);

	public:
		long long getAssistVdsaInjects() const;
	private:
		void setAssistVdsaInjects(long long value);

	public:
		long long getAssistSameVtInjects() const;
	private:
		void setAssistSameVtInjects(long long value);

	public:
		long long getAssistDonorVtInjects() const;
	private:
		void setAssistDonorVtInjects(long long value);

	public:
		AssistInvalidationReason getLastAssistInvalidationReason() const;
	private:
		void setLastAssistInvalidationReason(AssistInvalidationReason value);

	public:
		unsigned long long getLastAssistOwnershipSignature() const;
	private:
		void setLastAssistOwnershipSignature(unsigned long long value);

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void NominateAssistCandidate(int vtId, MicroOp? op)
		void NominateAssistCandidate(int vtId, std::shared_ptr<MicroOp> op);

		void WithdrawAssistCandidate(int vtId, std::shared_ptr<MicroOp> expected);

		void ClearAssistNominationPorts();

		void InvalidateAssistNominationState(AssistInvalidationReason reason);

	private:
		int CountReadyAssistCandidates(int ownerVirtualThreadId);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private int TryInjectAssistCandidates(MicroOp[] result, ref BundleResourceCertificate4Way bundleCert, ref SmtBundleMetadata4Way bundleMetadata, ref BoundaryGuardState boundaryGuard, ref ClassTemplateAdmissionState templateState, ref BundleOpportunityState opportunityState, ref byte bundleOccupancy, int ownerVirtualThreadId, int currentPassInjections, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		int TryInjectAssistCandidates(std::vector<std::shared_ptr<MicroOp>> &result, BundleResourceCertificate4Way &bundleCert, SmtBundleMetadata4Way &bundleMetadata, BoundaryGuardState &boundaryGuard, ClassTemplateAdmissionState &templateState, BundleOpportunityState &opportunityState, unsigned char &bundleOccupancy, int ownerVirtualThreadId, int currentPassInjections, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);

		void RecordAssistReject(std::shared_ptr<MicroOp> candidate, TypedSlotRejectReason rejectReason);

		bool TryReserveAssistQuota(std::shared_ptr<AssistMicroOp> assistMicroOp, AssistMemoryQuotaState &assistQuotaState);

		void RecordAssistInjection(std::shared_ptr<MicroOp> candidate);

		void RecordAssistInterCoreInjection(std::shared_ptr<AssistMicroOp> assistMicroOp);

		void RecordAssistInterCoreReject(AssistInterCoreTransport const transport, unsigned short requestingPodId, bool isDomainReject = false);


		// ── TDM (Time-DIV Multiplexing) ─────────────────────────

		/// <summary>Global cycle counter for TDM arbitration.</summary>
	public:
		unsigned long long GlobalCycleCounter = 0;

		/// <summary>TDM period config (N-th cycle dedicated to background thread).</summary>
		static constexpr int TDM_PERIOD = 64;

		// ── Constants ────────────────────────────────────────────────

		/// <summary>Number of nomination ports (one per core in a Pod).</summary>
	private:
		static constexpr int NUM_PORTS = 16;

		/// <summary>Number of SMT virtual threads per physical core.</summary>
		static constexpr int SMT_WAYS = 4;

		/// <summary>Number of scoreboard slots per virtual thread.</summary>
		static constexpr int SCOREBOARD_SLOTS = 8;

		// ── Nomination Ports ─────────────────────────────────────────

		/// <summary>
		/// Live nomination ports: each core writes its candidate here.
		/// HLS: 16-entry register file, written by core logic, read by arbiter.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly MicroOp?[] _ports = new MicroOp?[NUM_PORTS];
		std::vector<std::shared_ptr<MicroOp>> _ports = std::vector<std::shared_ptr<MicroOp>>(NUM_PORTS);

		/// <summary>
		/// Port validity flags (true = port has a valid candidate).
		/// Separate from null check for HLS: 16-bit register, 1 bit per core.
		/// </summary>
		std::vector<bool> _portValid = std::vector<bool>(NUM_PORTS);

		/// <summary>
		/// Latched ready mask from the previous cycle's visible inter-core nominations.
		/// Consumers that only need donor visibility read this transport rather than
		/// depending on shadow copies of the raw nomination port arrays.
		/// HLS: 16-bit pipeline register.
		/// </summary>
		unsigned short _latchedInterCoreNominationReadyMask = 0;

		/// <summary>
		/// Per-core stall flags. When a core is stalled its nominations are annulled.
		/// HLS: 16-bit register.
		/// </summary>
		std::vector<bool> _coreStalled = std::vector<bool>(NUM_PORTS);

		// ── SMT Nomination Ports (Intra-Core 4-Way) ──────────────────

		/// <summary>
		/// Per-virtual-thread SMT nomination ports for intra-core scheduling.
		/// HLS: 4-entry register file, indexed by VirtualThreadId.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly MicroOp?[] _smtPorts = new MicroOp?[SMT_WAYS];
		std::vector<std::shared_ptr<MicroOp>> _smtPorts = std::vector<std::shared_ptr<MicroOp>>(SMT_WAYS);
		std::vector<bool> _smtPortValid = std::vector<bool>(SMT_WAYS);

		// ── Phase 2A: Credit-Based Deterministic Fairness ────────────

		/// <summary>
		/// Maximum credit a VT can accumulate before capping.
		/// HLS: constant, compiled into comparator threshold.
		/// </summary>
		static constexpr int FAIRNESS_CREDIT_CAP = 16;
		static constexpr int LoopPhaseActivationThreshold = 8;
		static constexpr int LoopPhaseSampleCapacity = 16;
		static constexpr int LoopPhaseExportLimit = 8;
		static constexpr int LoopPhaseHeatPruneThreshold = 32;

		/// <summary>
		/// Per-VT fairness credit counters. Each cycle without injection
		/// accumulates credit; each successful injection spends one credit.
		/// Ranking uses descending credit order (highest credit = first chance).
		/// Tie-break: ascending vtId (deterministic).
		/// HLS: 4 × 16-bit register file = 64 bits → one LUTRAM.
		/// </summary>
		std::vector<int> _fairnessCredits = std::vector<int>(SMT_WAYS);

		/// <summary>
		/// Per-VT successful injection counter for fairness telemetry.
		/// HLS: 4 × 64-bit counters (diagnostic only, not in critical path).
		/// </summary>
		std::vector<long long> _perVtInjections = std::vector<long long>(SMT_WAYS);

		/// <summary>Number of starvation events detected (VT had valid candidate but was skipped for >FAIRNESS_CREDIT_CAP consecutive cycles).</summary>
	public:
		long long getFairnessStarvationEvents() const;
	private:
		void setFairnessStarvationEvents(long long value);

		/// <summary>Whether credit-based fairness ranking is enabled. Default: true (Phase 2).</summary>
	public:
		bool getCreditFairnessEnabled() const;
		void setCreditFairnessEnabled(bool value);

		/// <summary>
		/// Enables per-loop replay/class-capacity sampling for compiler-facing telemetry export.
		/// Default is <see langword="false"/> so existing replay paths stay unchanged unless explicitly opted in.
		/// </summary>
		bool getEnableLoopPhaseSampling() const;
		void setEnableLoopPhaseSampling(bool value);

		// ── Phase 2B: Bank-Pressure-Aware FSP ────────────────────────

		/// <summary>Number of injection decisions where bank-pressure tie-break selected a lower-pressure candidate.</summary>
		long long getBankPressureAvoidanceCount() const;
	private:
		void setBankPressureAvoidanceCount(long long value);

		/// <summary>Whether bank-pressure tie-breaking is enabled for memory-op candidates. Default: true (Phase 2).</summary>
	public:
		bool getBankPressureTieBreakEnabled() const;
		void setBankPressureTieBreakEnabled(bool value);

		// ── Phase 2C: Speculation Budget ─────────────────────────────

		/// <summary>
		/// Maximum number of concurrent speculative operations allowed.
		/// HLS: 4-bit constant → comparator threshold.
		/// </summary>
		int getSpeculationBudgetMax() const;
		void setSpeculationBudgetMax(int value);

		/// <summary>
		/// Current speculation budget (decremented on speculative inject, incremented on commit/squash).
		/// HLS: 4-bit register → 4 flip-flops.
		/// </summary>
	private:
		int _speculationBudget = 0;

		/// <summary>Number of speculative injections blocked because budget was exhausted.</summary>
	public:
		long long getSpeculationBudgetExhaustionEvents() const;
	private:
		void setSpeculationBudgetExhaustionEvents(long long value);

		/// <summary>Peak number of concurrent speculative operations observed.</summary>
	public:
		long long getPeakConcurrentSpeculativeOps() const;
	private:
		void setPeakConcurrentSpeculativeOps(long long value);

		/// <summary>Whether speculation budget enforcement is enabled. Default: true (Phase 2).</summary>
	public:
		bool getSpeculationBudgetEnabled() const;
		void setSpeculationBudgetEnabled(bool value);

		// ── Scoreboard (DMA / Outstanding Loads / Outstanding Stores) ──

		/// <summary>
		/// Global scoreboard: tracks pending targets (register IDs or DMA channels).
		/// _scoreboard[slot] = targetId that is pending, or -1 if slot is free.
		/// HLS: 8-entry register file (8 × 16-bit target ID + 8 × 1-bit valid).
		/// </summary>
	private:
		std::vector<int> _scoreboard = std::vector<int>(SCOREBOARD_SLOTS);

		/// <summary>
		/// Per-VT scoreboard for 4-way SMT isolation.
		/// _smtScoreboard[vt, slot] = targetId pending for that VT, or -1 if free.
		/// HLS: 4 × 8-entry register file = 32 entries total.
		/// </summary>
// C# TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in C#:
// ORIGINAL LINE: private readonly int[,] _smtScoreboard = new int[SMT_WAYS, SCOREBOARD_SLOTS];
		std::vector<std::vector<int>> _smtScoreboard = RectangularVectors::rectangularIntVector(SMT_WAYS, SCOREBOARD_SLOTS);

		/// <summary>
		/// Per-VT scoreboard entry type (Refactoring Pt. 3: Universal Scoreboard).
		/// _smtScoreboardType[vt, slot] = entry classification (Free/Dma/OutstandingLoad/OutstandingStore).
		/// HLS: 4 × 8 × 2-bit register file = 64 bits total.
		/// </summary>
// C# TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in C#:
// ORIGINAL LINE: private readonly ScoreboardEntryType[,] _smtScoreboardType = new ScoreboardEntryType[SMT_WAYS, SCOREBOARD_SLOTS];
		std::vector<std::vector<ScoreboardEntryType>> _smtScoreboardType = RectangularVectors::rectangularScoreboardEntryTypeVector(SMT_WAYS, SCOREBOARD_SLOTS);

		/// <summary>
		/// Per-VT scoreboard bank ID (Refactoring Pt. 3: bank-level conflict detection).
		/// _smtScoreboardBankId[vt, slot] = memory bank ID (0–15) for Load/Store entries, or -1 if N/A.
		/// HLS: 4 × 8 × 5-bit register file (4-bit bank ID + 1 valid bit) = 160 bits total.
		/// </summary>
// C# TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in C#:
// ORIGINAL LINE: private readonly int[,] _smtScoreboardBankId = new int[SMT_WAYS, SCOREBOARD_SLOTS];
		std::vector<std::vector<int>> _smtScoreboardBankId = RectangularVectors::rectangularIntVector(SMT_WAYS, SCOREBOARD_SLOTS);

		// Sampled hardware-occupancy view for the current packing pass.
		// The structural mask remains on the existing legality path, while the explicit
		// memory-budget state closes the widened lane4..5 LSU load/store contract.
		// A shared mixed ceiling remains as the conservative compatibility guardrail,
		// while split load/store credits now follow the memory-side turnaround model.
		// Projected outstanding-memory state keeps pack-time per-VT saturation aligned
		// with the real ID→EX scoreboard allocator, including pre-existing DMA occupancy.
		HardwareOccupancySnapshot128 _hardwareOccupancySnapshot = HardwareOccupancySnapshot128::getPermissive();
		unsigned char _remainingHardwareMemoryIssueBudget = 2;
		unsigned int _consumedHardwareMemoryBudgetByBank = 0;
		unsigned char _remainingHardwareLoadIssueBudget = 2;
		unsigned char _remainingHardwareStoreIssueBudget = 2;
		unsigned int _consumedHardwareLoadBudgetByBank = 0;
		unsigned int _consumedHardwareStoreBudgetByBank = 0;
		unsigned short _bundleLocalOutstandingStoreBankMask = 0;
		std::vector<unsigned char> _projectedOutstandingMemoryCountByVt = std::vector<unsigned char>(SMT_WAYS);
		std::vector<unsigned char> _projectedOutstandingMemoryCapacityByVt = std::vector<unsigned char>(SMT_WAYS);

		// ── 2-Stage Pipelined FSP (HLS Timing Closure §1) ───────────

		/// <summary>
		/// FSP pipeline stage enum for 2-stage arbitration.
		/// SCHED1: Nomination and source capture.
		/// SCHED2: Intersection &amp; priority-encoded commit.
		/// HLS: 1-bit flip-flop per scheduler instance.
		/// </summary>
	public:
		enum class FspPipelineStage : unsigned char
		{
			/// <summary>Cycle 1: Capture viable donor VT identities from SMT ports.</summary>
			SCHED1 = 0,

			/// <summary>Cycle 2: Intersect masks, priority-encode, commit to scoreboard.</summary>
			SCHED2 = 1
		};

		/// <summary>
		/// Pipeline register entry between SCHED1 and SCHED2.
		/// The exact nominated candidate reference is part of the stage-boundary
		/// state. SCHED2 must never reinterpret a replacement in the same VT port
		/// as the candidate admitted by SCHED1.
		/// </summary>
	private:
		class FspSnapshotFingerprint
		{
		private:
			unsigned long long Low = 0;
			unsigned long long High = 0;

		public:
			FspSnapshotFingerprint(unsigned long long low, unsigned long long high);

			unsigned long long getLow() const;

			unsigned long long getHigh() const;

			bool Equals(FspSnapshotFingerprint other) const ;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
			bool Equals(std::any obj) const;

			int GetHashCode() const;

			FspSnapshotFingerprint() = default;
		};

		class FspPipelineRegister
		{
			/// <summary>True if this register holds a valid candidate.</summary>
		public:
			bool Valid = false;

			/// <summary>Virtual thread ID of the candidate (0–3).</summary>
			int VirtualThreadId = 0;

			/// <summary>Exact candidate nominated at SCHED1.</summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MicroOp? Candidate;
			std::shared_ptr<MicroOp> Candidate;

			/// <summary>Identity template paired with that exact nomination.</summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public PostStageBIdentityTemplate? IdentityTemplate;
			std::shared_ptr<PostStageBIdentityTemplate> IdentityTemplate;

			/// <summary>
			/// Immutable scheduling fingerprint captured with the nomination.
			/// Reference equality alone cannot detect mutation of the same carrier
			/// between SCHED1 and SCHED2.
			/// </summary>
			FspSnapshotFingerprint CandidateFingerprint;

			/// <summary>Memory bank ID if candidate is Load/Store, -1 otherwise.</summary>

			/// <summary>True if candidate is a memory operation (Load/Store).</summary>

			// Phase 06: typed-slot metadata (3-bit class + 1-bit pinning + 3-bit lane = 7 flip-flops per entry)

			/// <summary>Required slot class of the candidate (Phase 01 taxonomy).</summary>

			/// <summary>Pinning kind of the candidate (ClassFlexible or HardPinned).</summary>

			/// <summary>Pinned lane ID for HardPinned candidates (0–7), 0 for ClassFlexible.</summary>
		};

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: private readonly struct FspSnapshotFingerprint : IEquatable<FspSnapshotFingerprint>


	private:
		class LoopPhaseTracker final : public std::enable_shared_from_this<LoopPhaseTracker>
		{
		private:
			const unsigned long long LoopPcAddress;
			int PhaseEntryCount = 0;

			std::vector<ClassCapacityTemplate> _samples = std::vector<ClassCapacityTemplate>(MicroOpScheduler::LoopPhaseSampleCapacity);
			int _sampleCount = 0;
			int _nextSampleIndex = 0;

		public:
			LoopPhaseTracker(unsigned long long loopPcAddress, int phaseEntryCount);

			unsigned long long getLoopPcAddress() const;

			int getPhaseEntryCount() const;
		private:
			void setPhaseEntryCount(int value);

		public:
			int getIterationsSampled() const;

			void SetPhaseEntryCount(int phaseEntryCount);

			void AddSample(ClassCapacityTemplate template_Keyword);

			std::shared_ptr<LoopPhaseClassProfile> BuildProfile();

		private:
			ClassCapacityTemplate GetOrderedSample(int sampleIndex);

			static double Square(double value);
		};

		/// <summary>
		/// SCHED1→SCHED2 pipeline register bank: one entry per SMT way.
		/// Written by PipelineFspStage1_Nominate, read by PipelineFspStage2_Intersect.
		/// HLS: 4-entry register file, D-flip-flop bank (clocked on pipeline edge).
		/// </summary>
	private:
		std::vector<FspPipelineRegister> _fspPipelineReg = std::vector<FspPipelineRegister>(SMT_WAYS);
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, int> _loopPhaseEntryCounts = new();
		std::unordered_map<unsigned long long, int> _loopPhaseEntryCounts = std::unordered_map<unsigned long long, int>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, LoopPhaseTracker> _loopPhaseTrackers = new();
		std::unordered_map<unsigned long long, std::shared_ptr<LoopPhaseTracker>> _loopPhaseTrackers = std::unordered_map<unsigned long long, std::shared_ptr<LoopPhaseTracker>>();

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: private readonly struct BundleOpportunityState
		class BundleOpportunityState
		{
		private:
			unsigned char OccupancyMask;
			unsigned char EmptySlotMask;
			unsigned char ReferenceOpportunityMask;

		public:
			BundleOpportunityState(unsigned char occupancyMask, unsigned char emptySlotMask, unsigned char referenceOpportunityMask);

			unsigned char getOccupancyMask() const;

			unsigned char getEmptySlotMask() const;

			unsigned char getReferenceOpportunityMask() const;

			int getReferenceOpportunityCount() const;

			inline int CountReplayEligibleReferenceOpportunities(unsigned char replayAwareMask) const;

			inline bool IsReplayEligibleReferenceOpportunity(int slotIndex, unsigned char replayAwareMask) const;

			inline int FindNextInjectableEmptySlot(int startIndex, bool hasStableDonorStructure, unsigned char stableDonorMask) const;

			inline BundleOpportunityState WithOccupiedSlot(int slotIndex) const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static BundleOpportunityState Create(IReadOnlyList<MicroOp?> bundle)
			inline static BundleOpportunityState Create(const std::vector<std::shared_ptr<MicroOp>> &bundle);

		private:
			inline static int FindNextSlotInMask(unsigned char slotMask, int startIndex);

			BundleOpportunityState() = default;
		};

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: private readonly struct SmtNominationState
		class SmtNominationState
		{
		private:
			unsigned char ReadyMask;

		public:
			SmtNominationState(unsigned char readyMask);

			unsigned char getReadyMask() const;

			bool getHasReadyBackgroundCandidates() const;

			inline bool IsReadyCandidate(int virtualThreadId) const;

			inline bool IsReadyNonOwnerCandidate(int virtualThreadId, int ownerVirtualThreadId) const;

			inline int CountReadyCandidatesExcluding(int ownerVirtualThreadId) const;

			inline SmtNominationState WithoutCandidate(int virtualThreadId) const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static SmtNominationState Create(MicroOp?[] ports, bool[] validPorts)
			inline static SmtNominationState Create(std::vector<std::shared_ptr<MicroOp>> &ports, std::vector<bool> &validPorts);

			SmtNominationState() = default;
		};

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: private readonly struct InterCoreNominationSnapshot
		class InterCoreNominationSnapshot
		{
		private:
			const unsigned short ReadyMask;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly MicroOp?[]? _candidates;
			std::vector<std::shared_ptr<MicroOp>> _candidates;

		public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public InterCoreNominationSnapshot(ushort readyMask, MicroOp?[] candidates)
			InterCoreNominationSnapshot(unsigned short readyMask, std::vector<std::shared_ptr<MicroOp>> &candidates);

			unsigned short getReadyMask() const;

			inline bool IsReadyCandidate(int coreId) const;

			inline bool TryGetCandidate(int coreId, std::shared_ptr<MicroOp> &candidate) const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static InterCoreNominationSnapshot Capture(MicroOp?[] ports, bool[] validPorts)
			static InterCoreNominationSnapshot Capture(std::vector<std::shared_ptr<MicroOp>> &ports, std::vector<bool> &validPorts);

			static InterCoreNominationSnapshot Capture(std::vector<std::deque<std::shared_ptr<MicroOp>>> &nominationQueues);

			InterCoreNominationSnapshot() = default;
		};

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: private readonly struct SmtNominationSnapshot
		class SmtNominationSnapshot
		{
		private:
			SmtNominationState NominationState;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly MicroOp?[]? _candidates;
			std::vector<std::shared_ptr<MicroOp>> _candidates;

		public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SmtNominationSnapshot(SmtNominationState nominationState, MicroOp?[] candidates)
			SmtNominationSnapshot(SmtNominationState nominationState, std::vector<std::shared_ptr<MicroOp>> &candidates);

			SmtNominationState getNominationState() const;

			inline bool TryGetCandidate(int virtualThreadId, std::shared_ptr<MicroOp> &candidate) const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static SmtNominationSnapshot Capture(MicroOp?[] ports, SmtNominationState nominationState)
			static SmtNominationSnapshot Capture(std::vector<std::shared_ptr<MicroOp>> &ports, SmtNominationState nominationState);

			SmtNominationSnapshot() : NominationState(SmtNominationState(0)), _candidates(4) {}
		};

	private:
		class ClassTemplateAdmissionState
		{
		private:
			bool TemplateHit = false;

			TemplateBudget _templateBudget;

			ClassTemplateAdmissionState(bool templateHit, TemplateBudget templateBudget);

		public:
			bool getTemplateHit() const;

			TemplateBudget getTemplateBudgetSnapshot() const;

			static ClassTemplateAdmissionState Create(TemplateBudget templateBudget);

			inline bool TryConsumeFastPathBudget(SlotClass slotClass);

			ClassTemplateAdmissionState() = default;
		};

		/// <summary>
		/// Current FSP pipeline stage (alternates each cycle when pipelined mode is active).
		/// </summary>
	private:
		FspPipelineStage _fspCurrentStage = FspPipelineStage::SCHED1;
		unsigned long long _lastLoopPhaseEntryEpochId = 0;
		unsigned long long _lastLoopPhaseEntryPc = 0;
		unsigned long long _lastLoopPhaseSampleEpochId = 0;
		unsigned long long _lastLoopPhaseSamplePc = 0;

		/// <summary>
		/// Owner VT ID latched during SCHED1 for use in SCHED2 (Phase 03: TryClassAdmission).
		/// HLS: 2-bit register, clocked on SCHED1 edge.
		/// </summary>
		int _fspOwnerVirtualThreadId = 0;

		FspSnapshotFingerprint _fspBundleFingerprint;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly MicroOp?[] _fspBundleReferences = new MicroOp?[8];
		std::vector<std::shared_ptr<MicroOp>> _fspBundleReferences = std::vector<std::shared_ptr<MicroOp>>(8);
		BundleResourceCertificateIdentity4Way _fspBundleCertificateIdentity;
		SmtBundleMetadata4Way _fspBundleMetadata;
		BoundaryGuardState _fspBoundaryGuard;
		ReplayPhaseContext _fspReplayPhase;

		/// <summary>Number of cycles added by the 2-stage FSP pipeline (diagnostic).</summary>
	public:
		long long getFspPipelineLatencyCycles() const;
	private:
		void setFspPipelineLatencyCycles(long long value);

		// ── Safety Verifier ──────────────────────────────────────────

		// ── Statistics ───────────────────────────────────────────────

		/// <summary>Number of PackBundle calls (one per scheduling cycle).</summary>
	public:
		long long getTotalSchedulerCycles() const;
	private:
		void setTotalSchedulerCycles(long long value);

		/// <summary>Number of successfully injected (stolen) operations.</summary>
	public:
		long long getSuccessfulInjectionsCount() const;
	private:
		void setSuccessfulInjectionsCount(long long value);

		/// <summary>Intra-core SMT successful injections.</summary>
	public:
		long long getSmtInjectionsCount() const;
	private:
		void setSmtInjectionsCount(long long value);

		/// <summary>Intra-core SMT rejections (safety mask conflict).</summary>
	public:
		long long getSmtRejectionsCount() const;
	private:
		void setSmtRejectionsCount(long long value);

		ReplayPhaseContext _currentReplayPhase;

	public:
		ReplayPhaseContext getCurrentReplayPhase() const;

		/// <summary>
		/// Runtime-local legality service for inter-core/SMT decision paths and adjacent diagnostics.
		/// </summary>
	private:
		const std::shared_ptr<IRuntimeLegalityService> _runtimeLegalityService;

		/// <summary>
		/// Internal E1 issuer/validator available only when the canonical runtime
		/// legality service is installed. Public/custom legality services cannot mint it.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly IVirtualizationAdmissionService? _virtualizationAdmissionService;
		const std::shared_ptr<IVirtualizationAdmissionService> _virtualizationAdmissionService;

		/// <summary>Number of scheduling cycles executed while a replay phase was active.</summary>
	public:
		long long getReplayAwareCycles() const;
	private:
		void setReplayAwareCycles(long long value);

		/// <summary>Number of legality checks that could reuse a phase certificate template.</summary>
	public:
		long long getPhaseCertificateReadyHits() const;
	private:
		void setPhaseCertificateReadyHits(long long value);

		/// <summary>Number of legality checks that had replay context but no reusable template.</summary>
	public:
		long long getPhaseCertificateReadyMisses() const;
	private:
		void setPhaseCertificateReadyMisses(long long value);

		/// <summary>Estimated number of repeated legality checks avoided by template reuse.</summary>
	public:
		long long getEstimatedPhaseCertificateChecksSaved() const;
	private:
		void setEstimatedPhaseCertificateChecksSaved(long long value);

		/// <summary>Number of explicit phase-certificate invalidations.</summary>
	public:
		long long getPhaseCertificateInvalidations() const;
	private:
		void setPhaseCertificateInvalidations(long long value);

		/// <summary>Number of phase-certificate invalidations caused by bundle mutation.</summary>
	public:
		long long getPhaseCertificateMutationInvalidations() const;
	private:
		void setPhaseCertificateMutationInvalidations(long long value);

		/// <summary>Number of phase-certificate invalidations caused by phase mismatch.</summary>
	public:
		long long getPhaseCertificatePhaseMismatchInvalidations() const;
	private:
		void setPhaseCertificatePhaseMismatchInvalidations(long long value);

		/// <summary>Most recent phase-certificate invalidation reason.</summary>
	public:
		ReplayPhaseInvalidationReason getLastPhaseCertificateInvalidationReason() const;
	private:
		void setLastPhaseCertificateInvalidationReason(ReplayPhaseInvalidationReason value);

		/// <summary>Reference empty/NOP slots observed before replay-stable masking is applied.</summary>
	public:
		long long getDeterminismReferenceOpportunitySlots() const;
	private:
		void setDeterminismReferenceOpportunitySlots(long long value);

		/// <summary>Replay-stable empty slots that remained eligible after deterministic masking.</summary>
	public:
		long long getDeterminismReplayEligibleSlots() const;
	private:
		void setDeterminismReplayEligibleSlots(long long value);

		/// <summary>Empty slots that an unconstrained contour could consider but replay-stable masking rejected.</summary>
	public:
		long long getDeterminismMaskedSlots() const;
	private:
		void setDeterminismMaskedSlots(long long value);

		/// <summary>Bounded estimate of lost injections caused by deterministic replay constraints.</summary>
	public:
		long long getDeterminismEstimatedLostSlots() const;
	private:
		void setDeterminismEstimatedLostSlots(long long value);

		/// <summary>Scheduling cycles where deterministic replay constraints removed at least one otherwise empty slot.</summary>
	public:
		long long getDeterminismConstrainedCycles() const;
	private:
		void setDeterminismConstrainedCycles(long long value);

		/// <summary>Domain-isolation probes observed while screening or verifying injected candidates.</summary>
	public:
		long long getDomainIsolationProbeAttempts() const;
	private:
		void setDomainIsolationProbeAttempts(long long value);

		/// <summary>Domain-isolation probes blocked by policy.</summary>
	public:
		long long getDomainIsolationBlockedAttempts() const;
	private:
		void setDomainIsolationBlockedAttempts(long long value);

		/// <summary>Blocked probes caused by disjoint non-kernel domain tags.</summary>
	public:
		long long getDomainIsolationCrossDomainBlocks() const;
	private:
		void setDomainIsolationCrossDomainBlocks(long long value);

		/// <summary>Blocked probes caused by kernel-to-user isolation enforcement.</summary>
	public:
		long long getDomainIsolationKernelToUserBlocks() const;
	private:
		void setDomainIsolationKernelToUserBlocks(long long value);

		// ── Phase 04: Serialising-Event Epoch State ───────────────────

		/// <summary>
		/// Monotonically increasing epoch counter, bumped once per
		/// <see cref="NotifySerializingCommit"/> call (G34).
		/// HLS: 32-bit up-counter, written only on serialising commit.
		/// </summary>
		long long _serializingEpochCounter = 0;

		/// <summary>
		/// Number of epoch boundaries created by <see cref="NotifySerializingCommit"/>.
		/// Exposed for telemetry; equals the total number of serialising instructions committed.
		/// </summary>
	public:
		long long getSerializingEpochCount() const;

		/// <summary>
		/// Number of <see cref="PackBundleIntraCoreSmt"/> calls short-circuited because
		/// the owner bundle contained a <see cref="Arch.SerializationClass.FullSerial"/>
		/// or <see cref="Arch.SerializationClass.VmxSerial"/> operation (G33).
		/// </summary>
		long long getSerializingBoundaryRejects() const;
	private:
		void setSerializingBoundaryRejects(long long value);

		// ── Phase 02: Class-Capacity Vacancy Model ───────────────────

		/// <summary>
		/// Per-cycle class-capacity snapshot, updated at the start of each
		/// PackBundle / PackBundleIntraCoreSmt cycle.
		/// Used by class-admission pre-checks (Phase 03+).
		/// <para>HLS: 36-flip-flop register, written once per cycle.</para>
		/// </summary>
		SlotClassCapacityState _classCapacity;

		// ── Phase 03: Two-Stage Admission Telemetry ─────────────────

		/// <summary>Number of class-capacity rejections (StaticClassOvercommit + DynamicClassExhaustion).</summary>
	public:
		long long getClassCapacityRejects() const;
	private:
		void setClassCapacityRejects(long long value);

		/// <summary>Number of lane-conflict rejections (PinnedLaneConflict + LateBindingConflict).</summary>
	public:
		long long getLaneConflictRejects() const;
	private:
		void setLaneConflictRejects(long long value);

		/// <summary>Number of resource-conflict rejections from Stage A (CanInject failed).</summary>
	public:
		long long getTypedSlotResourceConflictRejects() const;
	private:
		void setTypedSlotResourceConflictRejects(long long value);

		/// <summary>Number of SMT owner-context guard-plane rejects hidden behind ResourceConflict.</summary>
	public:
		long long getSmtOwnerContextGuardRejects() const;
	private:
		void setSmtOwnerContextGuardRejects(long long value);

		/// <summary>Number of SMT domain guard-plane rejects hidden behind ResourceConflict.</summary>
	public:
		long long getSmtDomainGuardRejects() const;
	private:
		void setSmtDomainGuardRejects(long long value);

		/// <summary>Number of SMT boundary guard-plane rejects.</summary>
	public:
		long long getSmtBoundaryGuardRejects() const;
	private:
		void setSmtBoundaryGuardRejects(long long value);

		/// <summary>Number of SMT shared-resource certificate rejects hidden behind ResourceConflict.</summary>
	public:
		long long getSmtSharedResourceCertificateRejects() const;
	private:
		void setSmtSharedResourceCertificateRejects(long long value);

		/// <summary>Number of SMT register-group certificate rejects hidden behind ResourceConflict.</summary>
	public:
		long long getSmtRegisterGroupCertificateRejects() const;
	private:
		void setSmtRegisterGroupCertificateRejects(long long value);

		/// <summary>Most recent SMT legality reject kind observed by typed-slot admission.</summary>
	public:
		RejectKind getLastSmtLegalityRejectKind() const;

		/// <summary>Most recent SMT legality authority source observed by typed-slot admission.</summary>
		LegalityAuthoritySource getLastSmtLegalityAuthoritySource() const;

		/// <summary>SMT legality rejections attributed to ALU-class candidates.</summary>
		long long getSmtLegalityRejectByAluClass() const;
	private:
		void setSmtLegalityRejectByAluClass(long long value);

		/// <summary>SMT legality rejections attributed to LSU-class candidates.</summary>
	public:
		long long getSmtLegalityRejectByLsuClass() const;
	private:
		void setSmtLegalityRejectByLsuClass(long long value);

		/// <summary>SMT legality rejections attributed to DMA/Stream-class candidates.</summary>
	public:
		long long getSmtLegalityRejectByDmaStreamClass() const;
	private:
		void setSmtLegalityRejectByDmaStreamClass(long long value);
	public:
		long long getSmtLegalityRejectByMatrixTileStreamClass() const;
	private:
		void setSmtLegalityRejectByMatrixTileStreamClass(long long value);

		/// <summary>SMT legality rejections attributed to Branch/Control candidates.</summary>
	public:
		long long getSmtLegalityRejectByBranchControl() const;
	private:
		void setSmtLegalityRejectByBranchControl(long long value);

		/// <summary>SMT legality rejections attributed to SystemSingleton candidates.</summary>
	public:
		long long getSmtLegalityRejectBySystemSingleton() const;
	private:
		void setSmtLegalityRejectBySystemSingleton(long long value);

		/// <summary>
		/// Number of scoreboard rejections from Stage A
		/// (legacy SuppressLsu or predicted per-VT scoreboard saturation).
		/// </summary>
	public:
		long long getTypedSlotScoreboardRejects() const;
	private:
		void setTypedSlotScoreboardRejects(long long value);

		/// <summary>Number of bank-pending rejections from Stage A.</summary>
	public:
		long long getTypedSlotBankPendingRejects() const;
	private:
		void setTypedSlotBankPendingRejects(long long value);

	public:
		long long getBankPendingRejectBank0() const;
	private:
		void setBankPendingRejectBank0(long long value);

	public:
		long long getBankPendingRejectBank1() const;
	private:
		void setBankPendingRejectBank1(long long value);

	public:
		long long getBankPendingRejectBank2() const;
	private:
		void setBankPendingRejectBank2(long long value);

	public:
		long long getBankPendingRejectBank3() const;
	private:
		void setBankPendingRejectBank3(long long value);

	public:
		long long getBankPendingRejectBank4() const;
	private:
		void setBankPendingRejectBank4(long long value);

	public:
		long long getBankPendingRejectBank5() const;
	private:
		void setBankPendingRejectBank5(long long value);

	public:
		long long getBankPendingRejectBank6() const;
	private:
		void setBankPendingRejectBank6(long long value);

	public:
		long long getBankPendingRejectBank7() const;
	private:
		void setBankPendingRejectBank7(long long value);

	public:
		long long getBankPendingRejectBank8() const;
	private:
		void setBankPendingRejectBank8(long long value);

	public:
		long long getBankPendingRejectBank9() const;
	private:
		void setBankPendingRejectBank9(long long value);

	public:
		long long getBankPendingRejectBank10() const;
	private:
		void setBankPendingRejectBank10(long long value);

	public:
		long long getBankPendingRejectBank11() const;
	private:
		void setBankPendingRejectBank11(long long value);

	public:
		long long getBankPendingRejectBank12() const;
	private:
		void setBankPendingRejectBank12(long long value);

	public:
		long long getBankPendingRejectBank13() const;
	private:
		void setBankPendingRejectBank13(long long value);

	public:
		long long getBankPendingRejectBank14() const;
	private:
		void setBankPendingRejectBank14(long long value);

	public:
		long long getBankPendingRejectBank15() const;
	private:
		void setBankPendingRejectBank15(long long value);

	public:
		long long getMemoryClusteringEvents() const;
	private:
		void setMemoryClusteringEvents(long long value);

		/// <summary>Number of speculation-budget rejections from Stage A.</summary>
	public:
		long long getTypedSlotSpeculationBudgetRejects() const;
	private:
		void setTypedSlotSpeculationBudgetRejects(long long value);

		/// <summary>Number of sampled hardware-budget rejections for widened LSU load/store injection.</summary>
	public:
		long long getTypedSlotHardwareBudgetRejects() const;
	private:
		void setTypedSlotHardwareBudgetRejects(long long value);

		/// <summary>Number of dedicated assist-quota rejections.</summary>
	public:
		long long getTypedSlotAssistQuotaRejects() const;
	private:
		void setTypedSlotAssistQuotaRejects(long long value);

		/// <summary>Number of explicit assist backpressure rejections.</summary>
	public:
		long long getTypedSlotAssistBackpressureRejects() const;
	private:
		void setTypedSlotAssistBackpressureRejects(long long value);

		// ── Phase 04: Deterministic Lane Chooser ─────────────────────

		/// <summary>
		/// Per-class previous-lane record for Tier 2 replay lane reuse.
		/// Indexed by SlotClass (cast to byte). Reset on replay phase change.
		/// HLS: 6 × 4-bit register (3-bit lane + 1-bit valid) = 24 flip-flops.
		/// </summary>
		std::vector<int> _previousPhaseLane = std::vector<int>(8);

		/// <summary>
		/// Epoch ID of the last replay phase that recorded lane assignments.
		/// Used to detect epoch transitions and reset <see cref="_previousPhaseLane"/>.
		/// </summary>
		unsigned long long _previousPhaseEpochId = 0;

		/// <summary>Number of Tier 2 lane reuse hits (replay phase, previous lane available).</summary>
	public:
		long long getLaneReuseHits() const;
	private:
		void setLaneReuseHits(long long value);

		/// <summary>Number of Tier 2 lane reuse misses (fallback to Tier 1).</summary>
	public:
		long long getLaneReuseMisses() const;
	private:
		void setLaneReuseMisses(long long value);

		// ── Phase 07: Class-Level Replay Templates ───────────────────

		/// <summary>
		/// Cached class-capacity template for replay reuse (Level 1, stable).
		/// Captures per-class free capacity pattern at first successful injection in replay epoch.
		/// HLS: 15 flip-flops (5 × 3-bit values).
		/// </summary>
		ClassCapacityTemplate _classCapacityTemplate;

		/// <summary>
		/// Whether <see cref="_classCapacityTemplate"/> holds a valid captured template.
		/// HLS: 1 flip-flop.
		/// </summary>
		bool _classTemplateValid = false;

		/// <summary>
		/// Domain scope ID at the time the class template was captured.
		/// Template is invalidated when current domain differs.
		/// HLS: 8-bit register.
		/// </summary>
		int _classTemplateDomainId = 0;

		/// <summary>Number of Level 1 class-template reuse hits (template compatible with current capacity).</summary>
	public:
		long long getClassTemplateReuseHits() const;
	private:
		void setClassTemplateReuseHits(long long value);

		/// <summary>Number of class-template invalidations (domain boundary, capacity mismatch, expired).</summary>
	public:
		long long getClassTemplateInvalidations() const;
	private:
		void setClassTemplateInvalidations(long long value);

		/// <summary>Number of fast-path accepts where template budget confirmed class capacity.</summary>
	public:
		long long getTypedSlotFastPathAccepts() const;
	private:
		void setTypedSlotFastPathAccepts(long long value);

		// ── Phase 08: Typed-Slot Telemetry ─────────────────────────

		/// <summary>Disaggregated: compiler bundle exceeds class capacity (sub-count of ClassCapacityRejects).</summary>
	public:
		long long getStaticClassOvercommitRejects() const;
	private:
		void setStaticClassOvercommitRejects(long long value);

		/// <summary>Disaggregated: intra-cycle typed-slot densification exhaustion (sub-count of ClassCapacityRejects).</summary>
	public:
		long long getDynamicClassExhaustionRejects() const;
	private:
		void setDynamicClassExhaustionRejects(long long value);

		/// <summary>Disaggregated: hard-pinned lane occupied (sub-count of LaneConflictRejects).</summary>
	public:
		long long getPinnedLaneConflicts() const;
	private:
		void setPinnedLaneConflicts(long long value);

		/// <summary>Disaggregated: hard-pinned raw payload was not a LaneId.</summary>
	public:
		long long getInvalidPinnedLaneRejects() const;
	private:
		void setInvalidPinnedLaneRejects(long long value);

		/// <summary>Disaggregated: all class lanes occupied (sub-count of LaneConflictRejects).</summary>
	public:
		long long getLateBindingConflicts() const;
	private:
		void setLateBindingConflicts(long long value);

		/// <summary>Domain isolation gating reject.</summary>
	public:
		long long getTypedSlotDomainRejects() const;
	private:
		void setTypedSlotDomainRejects(long long value);

		/// <summary>Admission via full pipeline (complement to TypedSlotFastPathAccepts).</summary>
	public:
		long long getTypedSlotStandardPathAccepts() const;
	private:
		void setTypedSlotStandardPathAccepts(long long value);

		/// <summary>Total successful late lane bindings.</summary>
	public:
		long long getTotalLaneBindings() const;
	private:
		void setTotalLaneBindings(long long value);

		/// <summary>Template invalidated by domain boundary (sub-count of ClassTemplateInvalidations).</summary>
	public:
		long long getClassTemplateDomainInvalidations() const;
	private:
		void setClassTemplateDomainInvalidations(long long value);

		/// <summary>Template invalidated by capacity mismatch (sub-count of ClassTemplateInvalidations).</summary>
	public:
		long long getClassTemplateCapacityMismatchInvalidations() const;
	private:
		void setClassTemplateCapacityMismatchInvalidations(long long value);

		/// <summary>NOPs caused by hard-pinned lane unavailability.</summary>
	public:
		long long getNopDueToPinnedConstraint() const;
	private:
		void setNopDueToPinnedConstraint(long long value);

		/// <summary>NOPs caused by class-capacity exhaustion.</summary>
	public:
		long long getNopDueToNoClassCapacity() const;
	private:
		void setNopDueToNoClassCapacity(long long value);

		/// <summary>NOPs caused by SafetyMask conflict.</summary>
	public:
		long long getNopDueToResourceConflict() const;
	private:
		void setNopDueToResourceConflict(long long value);

		/// <summary>NOPs caused by dynamic runtime state.</summary>
	public:
		long long getNopDueToDynamicState() const;
	private:
		void setNopDueToDynamicState(long long value);

		/// <summary>NOPs avoided thanks to class-flexible placement.</summary>
	public:
		long long getNopAvoided() const;
	private:
		void setNopAvoided(long long value);

		/// <summary>Successful injections into ALU-class lanes.</summary>
	public:
		long long getAluClassInjects() const;
	private:
		void setAluClassInjects(long long value);

		/// <summary>Successful injections into LSU-class lanes.</summary>
	public:
		long long getLsuClassInjects() const;
	private:
		void setLsuClassInjects(long long value);

		/// <summary>Successful injections into DMA/Stream-class lanes.</summary>
	public:
		long long getDmaStreamClassInjects() const;
	private:
		void setDmaStreamClassInjects(long long value);
	public:
		long long getMatrixTileStreamClassInjects() const;
	private:
		void setMatrixTileStreamClassInjects(long long value);

		/// <summary>Injections into Branch/Control lanes.</summary>
	public:
		long long getBranchControlInjects() const;
	private:
		void setBranchControlInjects(long long value);

		/// <summary>Certificate rejections attributed to ALU-class candidates.</summary>
	public:
		long long getCertificateRejectByAluClass() const;
	private:
		void setCertificateRejectByAluClass(long long value);

		/// <summary>Certificate rejections attributed to LSU-class candidates.</summary>
	public:
		long long getCertificateRejectByLsuClass() const;
	private:
		void setCertificateRejectByLsuClass(long long value);

		/// <summary>Certificate rejections attributed to DMA/Stream-class candidates.</summary>
	public:
		long long getCertificateRejectByDmaStreamClass() const;
	private:
		void setCertificateRejectByDmaStreamClass(long long value);
	public:
		long long getCertificateRejectByMatrixTileStreamClass() const;
	private:
		void setCertificateRejectByMatrixTileStreamClass(long long value);

		/// <summary>Certificate rejections attributed to Branch/Control candidates.</summary>
	public:
		long long getCertificateRejectByBranchControl() const;
	private:
		void setCertificateRejectByBranchControl(long long value);

		/// <summary>Certificate rejections attributed to SystemSingleton candidates.</summary>
	public:
		long long getCertificateRejectBySystemSingleton() const;
	private:
		void setCertificateRejectBySystemSingleton(long long value);

		/// <summary>Register-group certificate conflicts observed for VT0.</summary>
	public:
		long long getCertificateRegGroupConflictVT0() const;

		/// <summary>Register-group certificate conflicts observed for VT1.</summary>
		long long getCertificateRegGroupConflictVT1() const;

		/// <summary>Register-group certificate conflicts observed for VT2.</summary>
		long long getCertificateRegGroupConflictVT2() const;

		/// <summary>Register-group certificate conflicts observed for VT3.</summary>
		long long getCertificateRegGroupConflictVT3() const;

		/// <summary>Total scheduler rejections observed for VT0.</summary>
		long long getRejectionsVT0() const;
	private:
		void setRejectionsVT0(long long value);

		/// <summary>Total scheduler rejections observed for VT1.</summary>
	public:
		long long getRejectionsVT1() const;
	private:
		void setRejectionsVT1(long long value);

		/// <summary>Total scheduler rejections observed for VT2.</summary>
	public:
		long long getRejectionsVT2() const;
	private:
		void setRejectionsVT2(long long value);

		/// <summary>Total scheduler rejections observed for VT3.</summary>
	public:
		long long getRejectionsVT3() const;
	private:
		void setRejectionsVT3(long long value);

		/// <summary>Register-group certificate conflicts observed for VT0.</summary>
	public:
		long long getRegGroupConflictsVT0() const;
	private:
		void setRegGroupConflictsVT0(long long value);

		/// <summary>Register-group certificate conflicts observed for VT1.</summary>
	public:
		long long getRegGroupConflictsVT1() const;
	private:
		void setRegGroupConflictsVT1(long long value);

		/// <summary>Register-group certificate conflicts observed for VT2.</summary>
	public:
		long long getRegGroupConflictsVT2() const;
	private:
		void setRegGroupConflictsVT2(long long value);

		/// <summary>Register-group certificate conflicts observed for VT3.</summary>
	public:
		long long getRegGroupConflictsVT3() const;
	private:
		void setRegGroupConflictsVT3(long long value);

		/// <summary>Injections of hard-pinned ops.</summary>
	public:
		long long getHardPinnedInjects() const;
	private:
		void setHardPinnedInjects(long long value);

		/// <summary>Injections of class-flexible ops.</summary>
	public:
		long long getClassFlexibleInjects() const;
	private:
		void setClassFlexibleInjects(long long value);

		/// <summary>
		/// Get the previous lane used for a class in the current replay phase.
		/// </summary>
		inline int GetPreviousPhaseLane(SlotClass slotClass);

		/// <summary>
		/// Record the lane selected for a class in the current replay phase.
		/// Called after successful TryMaterializeLane for ClassFlexible ops.
		/// </summary>
		inline void RecordPhaseLane(SlotClass slotClass, int lane);

		/// <summary>
		/// Reset all per-class previous-lane records to invalid (-1).
		/// Called on replay phase deactivation or epoch transition.
		/// </summary>
		inline void ResetPreviousPhaseLanes();

		// ── Phase 07: Class Template Capture / Match / Invalidation ──

		/// <summary>
		/// Phase 07: Capture a class-capacity template from current capacity state.
		/// Called on first successful typed-slot injection in an active replay epoch.
		/// Gated behind <see cref="TypedSlotEnabled"/>.
		/// <para>HLS: 5 × 3-bit latch writes — single cycle.</para>
		/// </summary>
		inline void CaptureClassTemplate(int domainScopeId);

		inline static int ResolveClassTemplateDomainScopeId(SmtBundleMetadata4Way bundleMetadata);

		inline void RecordLoopPhaseEntry(ReplayPhaseContext phase);

		inline void RecordLoopPhaseSample();

		void PromoteLoopPhaseTracker(unsigned long long loopPcAddress, int phaseEntryCount);

		void PruneLoopPhaseHeat();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private IReadOnlyList<LoopPhaseClassProfile>? BuildLoopPhaseProfiles()
		const std::vector<std::shared_ptr<LoopPhaseClassProfile>> BuildLoopPhaseProfiles();

		/// <summary>
		/// Phase 07: Check if class template matches current state and build
		/// explicit per-cycle template-admission state for the decrement-aware fast path.
		/// <para>HLS: 5 × 3-bit comparators + 1 comparator (domain) = ~6 LUTs.</para>
		/// </summary>
		inline ClassTemplateAdmissionState PrepareClassTemplateAdmissionState(int currentDomainId);

		/// <summary>
		/// Phase 07: Invalidate the class-capacity template.
		/// <para>HLS: 1-bit clear + diagnostic counter increment.</para>
		/// </summary>
		inline void InvalidateClassTemplate(ReplayPhaseInvalidationReason reason);

		// ── Constructor ──────────────────────────────────────────────

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MicroOpScheduler(IRuntimeLegalityService? runtimeLegalityService = null)
		MicroOpScheduler(std::shared_ptr<IRuntimeLegalityService> runtimeLegalityService = nullptr);

		/// <summary>
		/// Quick pre-check: is there class capacity for a candidate operation?
		/// Called before full CanInject() to avoid expensive mask checks
		/// when the class is already full.
		/// <para>HLS: single 3-bit comparator (1 LUT layer).</para>
		/// </summary>
		/// <param name="candidate">The micro-operation to check capacity for.</param>
		/// <returns><see langword="true"/> if the candidate's required slot class has free capacity.</returns>
		inline bool HasClassCapacityFor(std::shared_ptr<MicroOp> candidate);

		/// <summary>
		/// Gets the current class-capacity snapshot (read-only copy for diagnostics/testing).
		/// </summary>
		SlotClassCapacityState GetClassCapacitySnapshot();

		/// <summary>
		/// Returns the total number of successful injections attributed to one virtual thread.
		/// This is a production diagnostics surface consumed by telemetry export.
		/// </summary>
		long long GetPerVtInjectionCount(int vtId);

	private:
		inline InterCoreNominationSnapshot CaptureInterCoreNominationSnapshot();

		inline void ConsumeInterCoreNomination(int coreId);

		inline SmtNominationSnapshot CaptureSmtNominationSnapshot(SmtNominationState nominationState);


		// ══════════════════════════════════════════════════════════════
		//  SMT Nomination Port Management
		// ══════════════════════════════════════════════════════════════

		/// <summary>
		/// Clear all SMT nomination ports.
		/// Called at the start of each cycle to prepare for fresh nominations.
		/// HLS: 4-bit register clear.
		/// </summary>
	public:
		void ClearSmtNominationPorts();

		// ══════════════════════════════════════════════════════════════
		//  Bank Arbitration Integration
		// ══════════════════════════════════════════════════════════════

		/// <summary>
		/// Execution result enum for ScheduleWithArbitration.
		/// </summary>
	public:
		enum class ExecutionResult
		{
			/// <summary>Operation scheduled successfully, no conflicts.</summary>
			Success,
			/// <summary>Bank conflict detected, operation must stall.</summary>
			Stall,
			/// <summary>Invalid operation (null or unsupported).</summary>
			Failed
		};

		/// <summary>Number of bank conflicts detected during arbitration.</summary>
	public:
		long long getBankConflictsCount() const;
	private:
		void setBankConflictsCount(long long value);

		/// <summary>
		/// Schedule a micro-operation with bank arbitration.
		/// For memory operations, checks bank availability via BankArbitrator.
		/// Non-memory operations always succeed.
		///
		/// HLS: combinational path — bank CAM lookup + comparator.
		/// </summary>
		/// <param name="op">Micro-operation to schedule</param>
		/// <param name="arbitrator">Bank arbitrator for conflict detection</param>
		/// <returns>Execution result: Success, Stall, or Failed</returns>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExecutionResult ScheduleWithArbitration(MicroOp? op, BankArbitrator arbitrator)
		ExecutionResult ScheduleWithArbitration(std::shared_ptr<MicroOp> op, std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Banks::BankArbitrator> arbitrator);

		// ══════════════════════════════════════════════════════════════
		//  Memory Wall Suppression (Legacy SuppressLsu compat)
		// ══════════════════════════════════════════════════════════════

		/// <summary>
		/// When true, LSU (Load/Store) operations from background VTs are suppressed
		/// during PackBundleIntraCoreSmt. Replaced in production by GRLB hardware
		/// occupancy mask, but retained for test backward compatibility.
		/// </summary>
		bool getSuppressLsu() const;
		void setSuppressLsu(bool value);

		/// <summary>Number of LSU operations suppressed due to SuppressLsu flag.</summary>
		long long getMemoryWallSuppressionsCount() const;
	private:
		void setMemoryWallSuppressionsCount(long long value);

		// ══════════════════════════════════════════════════════════════
		//  Speculative FSP
		// ══════════════════════════════════════════════════════════════

		/// <summary>Number of speculative steals that completed successfully.</summary>
	public:
		long long getSuccessfulSpeculativeSteals() const;
	private:
		void setSuccessfulSpeculativeSteals(long long value);

		/// <summary>Number of speculative steals that faulted and were squashed.</summary>
	public:
		long long getFaultedSpeculativeSteals() const;
	private:
		void setFaultedSpeculativeSteals(long long value);

		/// <summary>Number of rejected injection attempts.</summary>
	public:
		long long getRejectedInjectionsCount() const;
	private:
		void setRejectedInjectionsCount(long long value);

		/// <summary>Number of rollback events (resource mask restoration).</summary>
	public:
		long long getRollbackEventsCount() const;
	private:
		void setRollbackEventsCount(long long value);

		/// <summary>
		/// Process faulted speculative operations in a bundle.
		/// Clears faulted/speculative flags and increments FaultedSpeculativeSteals counter.
		///
		/// HLS: 8-slot scan with conditional flag reset.
		/// </summary>
		/// <param name="bundle">VLIW bundle to scan for faulted ops</param>
	public:
		void ProcessFaultedOperations(std::shared_ptr<std::vector<std::shared_ptr<MicroOp>>> bundle);

		/// <summary>
		/// Phase 2C: Release one unit of speculation budget (on speculative commit or squash).
		/// HLS: 4-bit increment with saturation at SpeculationBudgetMax.
		/// </summary>
		void ReleaseSpeculationBudget();

		// ══════════════════════════════════════════════════════════════
		//  Barrier Management
		// ══════════════════════════════════════════════════════════════

		/// <summary>
		/// Barrier entries: (vtId, instructionIndex, threadMask).
		/// HLS: fixed-size array of barrier descriptors (max 16).
		/// </summary>
	private:
// C# TO C++ CONVERTER TASK: Tuple fields are not converted by C# to C++ Converter:
		struct BarrierEntry { int vtId = 0, instrIndex = 0; std::vector<int> threads; };
		std::array<BarrierEntry, 16> _barriers{};
		int _barrierCount = 0;

		/// <summary>Number of active barriers.</summary>
	public:
		int getBarrierCount() const;

		/// <summary>
		/// Add a manual barrier for a virtual thread at a given instruction index.
		/// </summary>
		void AddManualBarrier(int vtId, int instructionIndex, std::vector<int> &threads);

		/// <summary>
		/// Optimize barriers by merging adjacent ones (within 5 instructions).
		/// </summary>
		void OptimizeBarriers();

		/// <summary>
		/// Generate a 16-bit affinity mask from thread IDs.
		/// Each thread ID sets a corresponding bit in the mask.
		/// </summary>
		unsigned short GenerateAffinityMask(std::vector<int> &threads);


		// ── Phase 5: Shadow Oracle State ─────────────────────────────

		/// <summary>
		/// Enable shadow oracle reference contour for Phase 5 diagnostics.
		/// When true, each PackBundleIntraCoreSmt call also computes a reference
		/// packing result for gap analysis.
		/// </summary>
		bool getEnableShadowOracle() const;
		void setEnableShadowOracle(bool value);

		/// <summary>
		/// RF-06.4b opt-in differential assertion. When enabled, the existing
		/// ShadowOracle memory predicate is evaluated once from the legacy
		/// carrier and once from the immutable memory contract projection.
		/// The flag is diagnostic-only and defaults to false; it never drives
		/// production admission or packing.
		/// </summary>
		bool getEnableRf06MemoryContractDifferential() const;
		void setEnableRf06MemoryContractDifferential(bool value);

	private:
		long long _oracleTotalSlots = 0;
		long long _oracleRealSlots = 0;
		long long _oracleDonorRestrictionGap = 0;
		long long _oracleFairnessOrderingGap = 0;
		long long _oracleLegalityConservatismGap = 0;
		long long _oracleDomainIsolationGap = 0;
		long long _oracleSpeculationBudgetGap = 0;
		long long _oracleCyclesWithGap = 0;
		long long _oracleTotalCyclesAnalyzed = 0;

		/// <summary>Maximum number of counterexamples retained (bounded buffer).</summary>
		static constexpr int MAX_COUNTEREXAMPLES = 64;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<OracleCounterexample> _counterexamples = new(MAX_COUNTEREXAMPLES);
		std::vector<OracleCounterexample> _counterexamples;

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: private readonly struct ShadowOracleCycleSnapshot
		class ShadowOracleCycleSnapshot
		{
		private:
			SmtNominationSnapshot NominationSnapshot;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly MicroOp?[]? _bundleSlots;
			std::vector<std::shared_ptr<MicroOp>> _bundleSlots;

		public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ShadowOracleCycleSnapshot(MicroOp?[] bundleSlots, SmtNominationSnapshot nominationSnapshot)
			ShadowOracleCycleSnapshot(std::vector<std::shared_ptr<MicroOp>> &bundleSlots, SmtNominationSnapshot nominationSnapshot);

			SmtNominationSnapshot getNominationSnapshot() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public bool TryGetBundleSlot(int slotIndex, out MicroOp? bundleOp)
			inline bool TryGetBundleSlot(int slotIndex, std::shared_ptr<MicroOp> &bundleOp) const;

			static ShadowOracleCycleSnapshot Capture(std::vector<std::shared_ptr<MicroOp>> &bundle, SmtNominationSnapshot nominationSnapshot);

			ShadowOracleCycleSnapshot() = default;
		};

	private:
		inline ShadowOracleCycleSnapshot CaptureShadowOracleCycleSnapshot(std::vector<std::shared_ptr<MicroOp>> &bundle, SmtNominationState nominationState);

		/// <summary>
		/// Get the aggregated oracle-gap summary for Phase 5 evidence rendering.
		/// </summary>
	public:
		OracleGapSummary GetOracleGapSummary();

		/// <summary>
		/// Get bounded counterexample records for Phase 5 refinement guidance.
		/// </summary>
		const std::vector<OracleCounterexample> GetCounterexamples();

		/// <summary>
		/// Compute the shadow oracle reference packing for the current bundle.
		///
		/// The oracle ignores:
		/// - Replay-stable donor mask (considers ALL empty slots)
		/// - Fairness credit ordering (tries all candidates)
		/// - Speculation budget limits
		///
		/// The oracle preserves:
		/// - Bundle size (8 slots)
		/// - Legality and safety masks (BundleResourceCertificate4Way)
		/// - Domain isolation enforcement
		/// - Structural resource limits
		///
		/// Returns the number of candidates the oracle could have packed.
		/// </summary>
	private:
		inline int ComputeShadowOraclePackCount(ShadowOracleCycleSnapshot cycleSnapshot, int ownerVirtualThreadId);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private void AssertRf06MemoryContractDifferential(MicroOp candidate, int candidateVirtualThreadId, ushort localOutstandingStoreBankMask, byte remainingHardwareMemoryIssueBudget, uint consumedHardwareMemoryBudgetByBank, byte remainingHardwareLoadIssueBudget, uint consumedHardwareLoadBudgetByBank, byte remainingHardwareStoreIssueBudget, uint consumedHardwareStoreBudgetByBank, IReadOnlyList<byte> projectedOutstandingMemoryCount, IReadOnlyList<byte> projectedOutstandingMemoryCapacity)
		inline void AssertRf06MemoryContractDifferential(std::shared_ptr<MicroOp> candidate, int candidateVirtualThreadId, unsigned short localOutstandingStoreBankMask, unsigned char remainingHardwareMemoryIssueBudget, unsigned int consumedHardwareMemoryBudgetByBank, unsigned char remainingHardwareLoadIssueBudget, unsigned int consumedHardwareLoadBudgetByBank, unsigned char remainingHardwareStoreIssueBudget, unsigned int consumedHardwareStoreBudgetByBank, const std::vector<unsigned char> &projectedOutstandingMemoryCount, const std::vector<unsigned char> &projectedOutstandingMemoryCapacity);

		inline void AssertRf06MemoryContractProjection(std::shared_ptr<MicroOp> candidate);

		inline bool HasOracleHardwareMemoryBudget(std::shared_ptr<MicroOp> candidate, unsigned char remainingHardwareMemoryIssueBudget, unsigned int consumedHardwareMemoryBudgetByBank, unsigned char remainingHardwareLoadIssueBudget, unsigned int consumedHardwareLoadBudgetByBank, unsigned char remainingHardwareStoreIssueBudget, unsigned int consumedHardwareStoreBudgetByBank, int memoryBankId);

		inline static bool HasOracleDirectionalHardwareBudget(unsigned char remainingDirectionalIssueBudget, unsigned int consumedDirectionalBudgetByBank, unsigned char bankBudget, int memoryBankId);

		/// <summary>
		/// Record the oracle gap for a single scheduling cycle and emit counterexamples.
		/// Called at the end of PackBundleIntraCoreSmt when shadow oracle is enabled.
		/// </summary>
		inline void RecordOracleGap(ShadowOracleCycleSnapshot cycleSnapshot, int ownerVirtualThreadId, int realInjectedCount);

		/// <summary>
		/// Classify the primary reason for oracle gap based on current scheduler state.
		/// </summary>
		inline OracleGapCategory ClassifyGapCategory(ShadowOracleCycleSnapshot cycleSnapshot, int ownerVirtualThreadId);


//		#region Test-Only Fields

		/// <summary>
		/// TEST-ONLY: Enable test mode for synthetic nomination.
		/// When true, scheduler-side inter-core nomination helpers rebuild a synthetic
		/// visible nomination snapshot from <see cref="TestNominationQueues"/> instead of
		/// reading the live inter-core nomination ports directly.
		/// Default: false (production behavior unchanged).
		/// </summary>
	public:
		bool getTestMode() const;
		void setTestMode(bool value);

		/// <summary>
		/// TEST-ONLY: Synthetic nomination queues for each virtual thread [0..3].
		/// Used only when TestMode == true. Scheduler-side inter-core helpers expose the
		/// head of each queue as a synthetic visible nomination source without requiring
		/// threaded producers or live core-port writes.
		/// </summary>
		std::vector<std::deque<std::shared_ptr<MicroOp>>> getTestNominationQueues() const;
	private:
		void setTestNominationQueues(const std::vector<std::deque<std::shared_ptr<MicroOp>>> &value);

		/// <summary>
		/// TEST-ONLY: Detailed rejection reason for last failed injection.
		/// Helps tests understand why a candidate was rejected.
		/// </summary>
	public:
		std::wstring getLastRejectionReason() const;
	private:
		void setLastRejectionReason(const std::wstring &value);

		/// <summary>
		/// TEST-ONLY: Counter for rejections due to resource conflicts.
		/// Incremented when CanInject() returns false due to register/resource overlap.
		/// </summary>
	public:
		long long getTestRejectedDueToResourceConflict() const;
	private:
		void setTestRejectedDueToResourceConflict(long long value);

		/// <summary>
		/// TEST-ONLY: Counter for rejections due to memory conflicts.
		/// Incremented when CanInject() returns false due to memory domain overlap.
		/// </summary>
	public:
		long long getTestRejectedDueToMemoryConflict() const;
	private:
		void setTestRejectedDueToMemoryConflict(long long value);

		/// <summary>
		/// TEST-ONLY: Counter for rejections due to safety mask conflicts.
		/// Incremented when the runtime legality checker rejects due to mask overlap.
		/// </summary>
	public:
		long long getTestRejectedDueToSafetyMask() const;
	private:
		void setTestRejectedDueToSafetyMask(long long value);

		/// <summary>
		/// TEST-ONLY: Counter for rejections due to full bundle (no stealable slots).
		/// </summary>
	public:
		long long getTestRejectedDueToFullBundle() const;
	private:
		void setTestRejectedDueToFullBundle(long long value);

//		#endregion

//		#region Test-Only Methods

		/// <summary>
		/// TEST-ONLY: Initialize test mode infrastructure.
		/// Call this before using TestMode in tests.
		/// </summary>
	public:
		void InitializeTestMode();

		/// <summary>
		/// TEST-ONLY: Enqueue a micro-op for a specific virtual thread.
		/// The head element of each queue becomes visible through the synthetic
		/// inter-core nomination snapshot used by scheduler test mode.
		/// </summary>
		/// <param name="vtId">Virtual thread ID (0-3)</param>
		/// <param name="op">Micro-op to nominate</param>
		void TestEnqueueMicroOp(int vtId, std::shared_ptr<MicroOp> op);

		/// <summary>
		/// TEST-ONLY: Record rejection reason for diagnostics.
		/// </summary>
	private:
		void TestRecordRejection(const std::wstring &reason, RejectionType type);

		/// <summary>
		/// TEST-ONLY: Reset all test-specific counters.
		/// </summary>
	public:
		void ResetTestCounters();

		/// <summary>
		/// TEST-ONLY: Get count of pending nominations across all VTs.
		/// </summary>
		int TestGetPendingNominationCount();

		/// <summary>
		/// TEST-ONLY: Clear all nomination queues.
		/// </summary>
		void TestClearNominationQueues();

		/// <summary>
		/// TEST-ONLY: Get the current fairness credit value for a virtual thread.
		/// </summary>
		int TestGetFairnessCredit(int vtId);

		/// <summary>
		/// TEST-ONLY: Set the fairness credit for a virtual thread (for seeding tests).
		/// </summary>
		void TestSetFairnessCredit(int vtId, int credit);

		/// <summary>
		/// TEST-ONLY: Get per-VT injection count for fairness distribution analysis.
		/// </summary>
		long long TestGetPerVtInjections(int vtId);

		/// <summary>
		/// TEST-ONLY: Get per-VT rejection count for Phase 3 telemetry assertions.
		/// </summary>
		long long TestGetPerVtRejections(int vtId);

		/// <summary>
		/// TEST-ONLY: Get per-VT register-group conflict count for Phase 3 telemetry assertions.
		/// </summary>
		long long TestGetPerVtRegGroupConflicts(int vtId);

		/// <summary>
		/// TEST-ONLY: Get the current speculation budget level.
		/// </summary>
		int TestGetSpeculationBudget();

		/// <summary>
		/// TEST-ONLY: Set the speculation budget level (for seeding tests).
		/// </summary>
		void TestSetSpeculationBudget(int budget);

		/// <summary>
		/// TEST-ONLY: Seed Phase 1 replay/certificate metrics so runtime trace emission can exercise realistic evidence paths.
		/// </summary>
		void TestSetPhaseMetrics(SchedulerPhaseMetrics metrics);

		/// <summary>
		/// TEST-ONLY: Get the current exported loop-phase profiles.
		/// </summary>
		const std::vector<std::shared_ptr<LoopPhaseClassProfile>> TestGetLoopPhaseProfiles();

		/// <summary>
		/// TEST-ONLY: Read the currently published replay-phase context.
		/// </summary>
		ReplayPhaseContext TestGetReplayPhaseContext();

//		#endregion

//		#region Test-Only Phase 03 Accessors

		/// <summary>
		/// TEST-ONLY: Expose TryClassAdmission for direct unit testing.
		/// </summary>
		bool TestTryClassAdmission(std::shared_ptr<MicroOp> candidate, BundleResourceCertificate4Way cert4Way, std::shared_ptr<std::vector<std::shared_ptr<MicroOp>>> workingBundle, int ownerVirtualThreadId, int currentPassInjections, TypedSlotRejectReason &rejectReason, bool templateHitThisCycle = false, TemplateBudget templateBudget = YAKSys_Hybrid_CPU::Core::TemplateBudget());

		/// <summary>
		/// TEST-ONLY: Expose TryMaterializeLane for direct unit testing.
		/// </summary>
		bool TestTryMaterializeLane(std::shared_ptr<MicroOp> candidate, unsigned char bundleOccupancy, int &selectedLane, TypedSlotRejectReason &rejectReason);

		/// <summary>
		/// TEST-ONLY: Expose ComputeBundleOccupancy for unit testing.
		/// </summary>
		static unsigned char TestComputeBundleOccupancy(std::vector<std::shared_ptr<MicroOp>> &bundle);

		/// <summary>
		/// TEST-ONLY: Set internal class-capacity state (for seeding tests).
		/// </summary>
		void TestSetClassCapacity(SlotClassCapacityState state);

//		#endregion

//		#region Test-Only Phase 04 Accessors

		/// <summary>
		/// TEST-ONLY: Get the previous-lane record for a class (for Tier 2 testing).
		/// </summary>
		int TestGetPreviousPhaseLane(SlotClass slotClass);

		/// <summary>
		/// TEST-ONLY: Set the previous-lane record for a class (for seeding Tier 2 tests).
		/// </summary>
		void TestSetPreviousPhaseLane(SlotClass slotClass, int lane);

		/// <summary>
		/// TEST-ONLY: Reset all per-class previous-lane records (for test isolation).
		/// </summary>
		void TestResetPreviousPhaseLanes();

//		#endregion

//		#region Test-Only Phase 06 Accessors

		/// <summary>
		/// TEST-ONLY: Get the last <see cref="CertificateRejectDetail"/> captured
		/// by the diagnostics path in <see cref="TryClassAdmission"/>.
		/// </summary>
		CertificateRejectDetail TestGetLastCertificateRejectDetail();

		/// <summary>
		/// TEST-ONLY: Get the last <see cref="TypedSlotRejectClassification"/> computed
		/// by <see cref="RecordTypedSlotReject(TypedSlotRejectReason, MicroOp)"/>.
		/// </summary>
		TypedSlotRejectClassification TestGetLastRejectClassification();

		/// <summary>
		/// TEST-ONLY: Get the placement carried by the exact FSP SCHED1 candidate.
		/// </summary>
// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		internal (bool Valid, SlotPlacementMetadata Placement) TestGetFspPipelinePlacement(int vt)
//		{
//			if ((uint)vt >= SMT_WAYS)
//				return default;
//
//			ref FspPipelineRegister reg = ref _fspPipelineReg[vt];
//			if (!reg.Valid)
//			{
//				return default;
//			}
//
//			System.Nullable<MicroOp> candidate = reg.Candidate;
//			if (candidate == nullptr)
//			{
//				return (true, SlotPlacementMetadata.Default with { RequiredSlotClass = SlotClass.Unclassified, PinningKind = SlotPinningKind.ClassFlexible });
//			}
//
//			return (true, candidate.Placement);
//		}

		/// <summary>
		/// TEST-ONLY: Returns the most recent eligibility snapshot captured while
		/// building an SMT nomination state from an explicit runnable VT mask.
		/// </summary>
		std::shared_ptr<SmtEligibilitySnapshot> TestGetLastSmtEligibilitySnapshot();

//		#endregion

//		#region Test-Only Phase 07 Accessors

		/// <summary>
		/// TEST-ONLY: Get the current class-capacity template.
		/// </summary>
		ClassCapacityTemplate TestGetClassCapacityTemplate();

		/// <summary>
		/// TEST-ONLY: Set the class-capacity template (for seeding tests).
		/// </summary>
		void TestSetClassCapacityTemplate(ClassCapacityTemplate template_Keyword);

		/// <summary>
		/// TEST-ONLY: Get whether the class-capacity template is valid.
		/// </summary>
		bool TestGetClassTemplateValid();

		/// <summary>
		/// TEST-ONLY: Set whether the class-capacity template is valid (for seeding tests).
		/// </summary>
		void TestSetClassTemplateValid(bool valid);

		/// <summary>
		/// TEST-ONLY: Get the domain scope ID associated with the class template.
		/// </summary>
		int TestGetClassTemplateDomainId();

		/// <summary>
		/// TEST-ONLY: Set the domain scope ID for the class template (for seeding tests).
		/// </summary>
		void TestSetClassTemplateDomainId(int domainId);

		/// <summary>
		/// TEST-ONLY: Build explicit class-template admission state for the current scheduler state.
		/// Returns whether the class-template fast path is active for <paramref name="currentDomainId"/>.
		/// </summary>
		bool TestTryPrepareClassTemplateAdmissionState(int currentDomainId, TemplateBudget &templateBudget);

//		#endregion

//		#region Test-Only Phase 08 Accessors

		/// <summary>
		/// TEST-ONLY: Invoke RecordTypedSlotInject for unit testing.
		/// </summary>
		void TestRecordTypedSlotInject(std::shared_ptr<MicroOp> candidate, int lane);

		/// <summary>
		/// TEST-ONLY: Invoke RecordTypedSlotReject (base overload) for unit testing.
		/// </summary>
		void TestRecordTypedSlotReject(TypedSlotRejectReason reason);

		/// <summary>
		/// TEST-ONLY: Record a bank-pending reject with a specific bank ID.
		/// </summary>
		void TestRecordBankPendingReject(int memoryBankId);

		/// <summary>
		/// TEST-ONLY: Record a decode-side memory clustering event.
		/// </summary>
		void TestRecordMemoryClusteringEvent();

		/// <summary>
		/// TEST-ONLY: Invoke metadata-aware reject accounting with a seeded certificate detail.
		/// </summary>
		void TestRecordTypedSlotReject(TypedSlotRejectReason reason, std::shared_ptr<MicroOp> candidate, CertificateRejectDetail certificateRejectDetail);

		/// <summary>
		/// TEST-ONLY: Invoke InvalidateClassTemplate for unit testing.
		/// </summary>
		void TestInvalidateClassTemplate(ReplayPhaseInvalidationReason reason);

		/// <summary>
		/// TEST-ONLY: Check whether a valid candidate has been nominated to the given
		/// inter-core FSP port (indexed by core ID, 0–15).
		/// Rebuilt from an explicit inter-core nomination snapshot.
		/// </summary>
		/// <param name="coreId">Core nomination port to query (0–15).</param>
		/// <returns><c>true</c> if the port has a valid candidate.</returns>
		bool HasNominatedCandidate(int coreId);

//		#endregion

//		#region Test-Only Enums

		/// <summary>
		/// TEST-ONLY: Rejection reason categories for diagnostics.
		/// </summary>
	private:
		enum class RejectionType
		{
			ResourceConflict,
			MemoryConflict,
			SafetyMask,
			FullBundle
		};

//		#endregion


		//  Phase 03: Two-Stage Admission Pipeline
		// ══════════════════════════════════════════════════════════════

		// ── Phase 06: Last certificate reject detail for diagnostics ──

		/// <summary>
		/// Last certificate-layer detail captured by the most recent
		/// <see cref="LegalityDecision"/> consumed by <see cref="TryClassAdmission"/>.
		/// Reset each admission attempt.
		/// <para>HLS: 2-bit diagnostic register — not on critical path.</para>
		/// </summary>
	private:
		CertificateRejectDetail _lastCertificateRejectDetail = static_cast<CertificateRejectDetail>(0);

		/// <summary>
		/// Last checker-owned reject kind captured by the most recent SMT legality
		/// decision consumed by typed-slot admission.
		/// </summary>
		RejectKind _lastSmtLegalityRejectKind = static_cast<RejectKind>(0);

		/// <summary>
		/// Last checker-owned authority source captured by the most recent SMT
		/// legality decision consumed by typed-slot admission.
		/// </summary>
		LegalityAuthoritySource _lastSmtLegalityAuthoritySource = static_cast<LegalityAuthoritySource>(0);

		/// <summary>
		/// Last <see cref="TypedSlotRejectClassification"/> produced by
		/// <see cref="RecordTypedSlotReject(TypedSlotRejectReason, MicroOp)"/>.
		/// Available for Phase 08 telemetry and diagnostics.
		/// <para>HLS: diagnostic register bank — not on critical path.</para>
		/// </summary>
		TypedSlotRejectClassification _lastRejectClassification;

		/// <summary>
		/// Stage A: check whether the candidate is admissible at class level.
		/// This stage does not select a physical lane. It only decides whether the
		/// operation may advance to Stage B under the current capacity, legality,
		/// and dynamic-state envelope.
		/// <para>
		/// Gate order: A1 (class-capacity, O(1)) → A2 (explicit legality decision, O(128-bit AND))
		/// → A3 (outer-cap dynamic gates, O(8 comparators)). Fail-fast from cheap to expensive.
		/// </para>
		/// <para>
		/// Gate A2 consumes <see cref="IRuntimeLegalityService.EvaluateSmtLegality(ReplayPhaseContext, BundleResourceCertificate4Way, SmtBundleMetadata4Way, BoundaryGuardState, MicroOp)"/>
		/// and preserves <see cref="CertificateRejectDetail"/> via the returned <see cref="LegalityDecision"/>.
		/// The legality-service substage is itself ordered guard-plane first, then
		/// replay/certificate authority.
		/// </para>
		/// <para>HLS: ~3 LUT layers combinational (fits in SCHED1 pipeline stage).</para>
		/// </summary>
		/// <param name="candidate">Candidate micro-operation.</param>
		/// <param name="cert4Way">Current bundle certificate (ref for read-only access).</param>
		/// <param name="bundleMetadata">Explicit runtime bundle metadata transport.</param>
		/// <param name="boundaryGuard">Explicit runtime boundary guard transport.</param>
		/// <param name="ownerVirtualThreadId">Bundle owner VT (for speculation budget check).</param>
		/// <param name="currentPassInjections">Number of successful injections so far in this pass.</param>
		/// <param name="rejectReason">Output: reason for rejection, if any.</param>
		/// <returns><see langword="true"/> if class-admitted.</returns>
		inline bool TryClassAdmission(std::shared_ptr<MicroOp> candidate, BundleResourceCertificate4Way &cert4Way, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard, ClassTemplateAdmissionState &templateState, int ownerVirtualThreadId, int currentPassInjections, TypedSlotRejectReason &rejectReason);

		/// <summary>
		/// Stage B: materialize one concrete physical lane for an admitted candidate.
		/// Called only after <see cref="TryClassAdmission"/> returns <see langword="true"/>.
		/// <para>
		/// HardPinned ops use <see cref="MicroOp.Placement"/>.<see cref="SlotPlacementMetadata.PinnedLaneId"/> directly.
		/// ClassFlexible ops intersect the class lane mask with free lanes
		/// and apply <see cref="DeterministicLaneChooser.SelectWithReplayHint"/>
		/// (Tier 1: lowest-free, Tier 2: replay lane reuse).
		/// </para>
		/// <para>
		/// Stage B does not widen legality. It only resolves the final lane from the
		/// already admitted class and replay/topology constraints.
		/// </para>
		/// <para>HLS: ~2 LUT layers combinational (fits in SCHED2 pipeline stage).</para>
		/// </summary>
		/// <param name="candidate">Class-admitted candidate.</param>
		/// <param name="bundleOccupancy">Bitmask of occupied physical slots (bit N → slot N occupied).</param>
		/// <param name="selectedLane">Output: chosen physical lane (0–7).</param>
		/// <param name="rejectReason">Output: reason for rejection, if any.</param>
		/// <returns><see langword="true"/> if a lane was successfully materialized.</returns>
	private:
		enum class LaneMaterializationRole : unsigned char
		{
			Opportunistic,
			Foreground
		};

	private:
		inline bool TryMaterializeLane(std::shared_ptr<MicroOp> candidate, unsigned char bundleOccupancy, int &selectedLane, TypedSlotRejectReason &rejectReason, LaneMaterializationRole role = LaneMaterializationRole::Opportunistic, unsigned char permittedLanes = std::numeric_limits<unsigned char>::max());

		inline unsigned char GetReplayAwareStealMask(unsigned char stealMask, unsigned char donorMask);

		/// <summary>
		/// Legacy TDM-only ranking (used when CreditFairnessEnabled is false).
		/// </summary>
		inline static int GetRankedVirtualThread(int scanIndex, bool forceTdm);

		/// <summary>
		/// Phase 2A: Build credit-ranked VT order for the current scheduling cycle.
		/// Returns a fixed-size 4-element array sorted by descending credit, tie-break ascending vtId.
		/// HLS: 4-input comparator tree → 2-level MUX, ≤2 LUT depth.
		/// </summary>
		inline void GetCreditRankedOrder(int ownerVirtualThreadId, std::span<int> ranked);

		/// <summary>
		/// Phase 2A: Accumulate credit for VTs that had valid candidates but were not injected this cycle.
		/// </summary>
		inline void AccumulateFairnessCredits(SmtNominationState nominationState, int ownerVirtualThreadId, std::span<const bool> wasInjected);

		/// <summary>
		/// Prime the pack-local memory issue state from live scoreboard truth.
		/// This folds sampled widened load/store budget, bundle-local store-bank overlay,
		/// and projected per-VT outstanding-memory saturation into one deterministic pass.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private void PrepareProjectedMemoryIssuePass(System.Collections.Generic.IReadOnlyList<MicroOp?> bundle)
			inline void PrepareProjectedMemoryIssuePass(const std::vector<std::shared_ptr<MicroOp>> &bundle);

		inline void InitializeProjectedOutstandingMemoryState();

		inline bool HasProjectedOutstandingMemoryCapacity(bool isMemoryOp, int virtualThreadId);

		inline bool HasHardwareMemoryBudget(std::shared_ptr<MicroOp> candidate, int memoryBankId);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private void ConsumeProjectedMemoryIssueStateIfNeeded(MicroOp? candidate)
		inline void ConsumeProjectedMemoryIssueStateIfNeeded(std::shared_ptr<MicroOp> candidate);

		inline bool HasSharedHardwareMemoryBudget(int memoryBankId);

		inline bool HasDirectionalHardwareMemoryBudget(std::shared_ptr<MicroOp> candidate, int memoryBankId);

		inline static bool HasDirectionalHardwareBudget(unsigned char remainingDirectionalIssueBudget, unsigned int consumedDirectionalBudgetByBank, unsigned char bankBudget, int memoryBankId);

		inline static unsigned char SaturateScoreboardCount(int value);

		inline static void IncrementProjectedOutstandingMemoryCount(std::vector<unsigned char> &projectedOutstandingCounts, int virtualThreadId);

		inline bool HasBundleLocalOutstandingStoreBankPending(int bankId);

		inline void MarkBundleLocalOutstandingStoreBank(int bankId);

		inline unsigned char GetConsumedHardwareMemoryBudget(int bankId);

		inline void IncrementConsumedHardwareMemoryBudget(int bankId);

		inline void IncrementConsumedHardwareLoadBudget(int bankId);

		inline void IncrementConsumedHardwareStoreBudget(int bankId);

		inline static unsigned char GetPackedConsumedHardwareBudget(unsigned int consumedPackedBudgetByBank, int bankId);

		inline static unsigned int IncrementPackedConsumedHardwareBudget(unsigned int consumedPackedBudgetByBank, int bankId);

		/// <summary>
		/// Phase 2B: Compute bank pressure score for a memory-op candidate.
		/// Score = number of outstanding memory entries targeting the same bank across all VTs.
		/// Lower score = less contention = preferred.
		/// HLS: bank-indexed scan across 4×8 scoreboard entries.
		/// </summary>
		inline int GetBankPressureScore(int bankId);

		inline bool TryPassOuterCap(std::shared_ptr<MicroOp> candidate, int virtualThreadId, bool isMemoryOp, int memoryBankId, int bundleOwnerVtId, TypedSlotRejectReason &rejectReason);

		/// <summary>
		/// Returns true when the bundle contains a serialising event
		/// (<see cref="Arch.SerializationClass.FullSerial"/> or
		/// <see cref="Arch.SerializationClass.VmxSerial"/>) that forbids cross-domain
		/// FSP injection at this bundle boundary (G33 — Deterministic Legality Alignment).
		/// <para>
		/// Published into the SMT guard-plane key; the verifier now owns the rejection.
		/// </para>
		/// </summary>
		inline bool IsSmtBundleBlockedByBoundaryGuard(BundleResourceCertificate4Way bundleCert, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard);

		/// <summary>
		/// Evaluate SMT legality through the explicit verifier-owned decision seam and
		/// consume cache-owned replay telemetry without reinterpreting reuse semantics locally.
		/// <para>HLS: same hot-path cost as the old bool helpers; only the authority surface changed.</para>
		/// </summary>
		inline LegalityDecision EvaluateSmtLegality(BundleResourceCertificate4Way bundleCert, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard, std::shared_ptr<MicroOp> candidate);

		inline void InvalidatePhaseCertificateTemplates(ReplayPhaseInvalidationReason reason, bool invalidateInterCore = true, bool invalidateFourWay = true);

	public:
		bool TryInvalidatePhaseCertificateTemplatesForMemoryFootprintOverlap(std::shared_ptr<MicroOp> writer, std::shared_ptr<MicroOp> replayEvidence);



		// Foreground is not an SMT donor. Reuse the same Stage A/B owners without
		// inventing a donor VT or changing the nomination ports / deferred pipeline.
		bool TryIssueForegroundScalar(std::shared_ptr<MicroOp> candidate, std::shared_ptr<PostStageBIdentityTemplate> template_Keyword, TypedSlotRejectReason &rejection);


// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal bool TryIssuePacketOwnerScalars(BundleIssuePacket packet, IReadOnlyList<WorkingBundleEntry> selected, byte executableMask, int ownerVt, byte currentSlot, out TypedSlotRejectReason rejection, out byte issuedWorkingMask)
		bool TryIssuePacketOwnerScalars(BundleIssuePacket packet, const std::vector<std::shared_ptr<WorkingBundleEntry>> &selected, unsigned char executableMask, int ownerVt, unsigned char currentSlot, TypedSlotRejectReason &rejection, unsigned char &issuedWorkingMask);


	private:
		AssistBackpressureSnapshot CaptureAssistBackpressureSnapshot();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private bool TryReserveAssistBackpressure(AssistMicroOp assistMicroOp, ref AssistBackpressureState assistBackpressureState, MemorySubsystem? memSub = null)
		bool TryReserveAssistBackpressure(std::shared_ptr<AssistMicroOp> assistMicroOp, AssistBackpressureState &assistBackpressureState, std::shared_ptr<MemorySubsystem> memSub = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool CanAllocateAssistDmaSrf(AssistMicroOp assistMicroOp, AssistStreamRegisterPartitionPolicy partitionPolicy, MemorySubsystem? memSub = null)
		static bool CanAllocateAssistDmaSrf(std::shared_ptr<AssistMicroOp> assistMicroOp, AssistStreamRegisterPartitionPolicy partitionPolicy, std::shared_ptr<MemorySubsystem> memSub = nullptr);


		// ══════════════════════════════════════════════════════════════
		//  PackBundle — Inter-Core FSP
		// ══════════════════════════════════════════════════════════════

		/// <summary>
		/// Pack a VLIW bundle with FSP slot stealing.
		/// Scans the bundle for NOP / null / stealable slots and fills them
		/// with nominated candidates that pass legality-checker checks.
		///
		/// Supports both direct test calls (4-param) and PodController calls (7-param).
		/// The globalResourceLocks parameter is forwarded from GRLB for structural
		/// conflict detection. domainCertificate gates Singularity-style isolation.
		/// </summary>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public MicroOp[] PackBundle(System.Collections.Generic.IReadOnlyList<MicroOp?> originalBundle, int currentThreadId, bool stealEnabled, byte stealMask, ResourceBitset globalResourceLocks = default, byte donorMask = 0, ulong domainCertificate = 0, int localCoreId = -1, ushort assistPodId = ushort.MaxValue, int bundleOwnerContextId = -1, ulong bundleOwnerDomainTag = 0, ulong assistRuntimeEpoch = 0, System.Nullable<YAKSys_Hybrid_CPU.Memory.MemorySubsystem> memSub = null, System.Nullable<PodController>[]? pods = null)
		std::vector<std::shared_ptr<MicroOp>> PackBundle(const std::vector<std::shared_ptr<MicroOp>> &originalBundle, int currentThreadId, bool stealEnabled, unsigned char stealMask, ResourceBitset globalResourceLocks = YAKSys_Hybrid_CPU::Core::ResourceBitset(), unsigned char donorMask = 0, unsigned long long domainCertificate = 0, int localCoreId = -1, unsigned short assistPodId = std::numeric_limits<unsigned short>::max(), int bundleOwnerContextId = -1, unsigned long long bundleOwnerDomainTag = 0, unsigned long long assistRuntimeEpoch = 0, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr, std::shared_ptr<std::vector<std::shared_ptr<PodController>>> pods = nullptr);



		// ══════════════════════════════════════════════════════════════
		//  2-Stage Pipelined FSP (HLS Timing Closure §1)
		// ══════════════════════════════════════════════════════════════

		/// <summary>
		/// SCHED1: Nomination and source capture.
		///
		/// Captures the exact ready non-owner candidate reference. No legality or
		/// placement work is performed here; SCHED2 rejects a replaced VT port
		/// instead of issuing a different operation under this nomination.
		///
		/// HLS: 4-way ready-mask fanout → 4 × D-flip-flop writes.
		/// Single-cycle, minimal LUT depth.
		/// </summary>
		/// <param name="ownerVirtualThreadId">VT that owns the current bundle.</param>
	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private void PipelineFspStage1_Nominate(int ownerVirtualThreadId, SmtNominationState nominationState, IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate4Way bundleMask, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard)
		inline void PipelineFspStage1_Nominate(int ownerVirtualThreadId, SmtNominationState nominationState, const std::vector<std::shared_ptr<MicroOp>> &bundle, BundleResourceCertificate4Way bundleMask, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard);
		std::pair<BundleResourceCertificate4Way, int> PipelineFspStage2_Intersect(std::vector<std::shared_ptr<MicroOp>> &bundle, BundleResourceCertificate4Way bundleMask, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard, int nextEmptySlot);

		/// <summary>
		/// SCHED2: Intersection &amp; priority-encoded commit.
		///
		/// Reads the pipeline register bank from SCHED1, performs two-stage
		/// admission (Phase 03): TryClassAdmission → TryMaterializeLane → Commit.
		/// SCHED1 latched the exact candidate. SCHED2 requires the live port to
		/// retain that same reference before evaluating admission.
		///
		/// HLS: 4-iteration loop, each with Stage A (~3 LUT) + Stage B (~2 LUT).
		/// Single-cycle with parallel reduction tree.
		/// </summary>
		/// <param name="bundle">Working copy of VLIW bundle (8 slots)</param>
		/// <param name="bundleMask">Cumulative safety mask of existing bundle ops.</param>
		/// <param name="nextEmptySlot">First empty slot index (or -1). Retained for API compat.</param>
		/// <returns>Updated (bundleMask, nextEmptySlot) after injections.</returns>
// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		public (BundleResourceCertificate4Way mask, int nextSlot) PipelineFspStage2_Intersect(MicroOp[] bundle, BundleResourceCertificate4Way bundleMask, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard, int nextEmptySlot)
//		{
//			if (_fspOwnerVirtualThreadId != bundleMetadata.OwnerVirtualThreadId || !FspBundleReferencesMatch(bundle) || !_fspBundleFingerprint.Equals(ComputeFspBundleFingerprint(bundle)) || !_fspBundleCertificateIdentity.Equals(bundleMask.StructuralIdentity) || !_fspBundleMetadata.Equals(bundleMetadata) || !_fspBoundaryGuard.Equals(boundaryGuard) || !ReplayPhaseMatches(_fspReplayPhase, _currentReplayPhase))
//			{
//				_fspCurrentStage = FspPipelineStage.SCHED1;
//				return (bundleMask, nextEmptySlot);
//			}
//
//			if (IsSmtBundleBlockedByBoundaryGuard(bundleMask, bundleMetadata, boundaryGuard))
//			{
//				_fspCurrentStage = FspPipelineStage.SCHED1;
//				return (bundleMask, nextEmptySlot);
//			}
//
//			_runtimeLegalityService.PrepareSmt(_currentReplayPhase, bundleMask, bundleMetadata, boundaryGuard);
//
//			BundleOpportunityState opportunityState = BundleOpportunityState.Create(bundle);
//			int templateDomainScopeId = ResolveClassTemplateDomainScopeId(bundleMetadata);
//			ClassTemplateAdmissionState templateState = TypedSlotEnabled ? PrepareClassTemplateAdmissionState(templateDomainScopeId) : default;
//			byte bundleOccupancy = opportunityState.OccupancyMask;
//			int realInjectedCount = 0;
//			PrepareProjectedMemoryIssuePass(bundle);
//
//			for (int vt = 0; vt < SMT_WAYS; vt++)
//			{
//				if (!_fspPipelineReg[vt].Valid)
//					continue;
//
//				ref FspPipelineRegister pipelineEntry = ref _fspPipelineReg[vt];
//				int candidateVt = pipelineEntry.VirtualThreadId;
//				System.Nullable<MicroOp> candidate = pipelineEntry.Candidate;
//				if (candidate == nullptr || !ReferenceEquals(_smtPorts[candidateVt], candidate) || !ReferenceEquals(candidate.PostStageBIdentityTemplate, pipelineEntry.IdentityTemplate) || candidate.PostStageBIssuedAttempt != nullptr || !pipelineEntry.CandidateFingerprint.Equals(ComputeFspCandidateFingerprint(candidate)))
//				{
//					continue;
//				}
//
//				// RF-06.4b diagnostic seam: FSP SCHED2 accepts only a carrier
//				// whose bank/direction/footprint can be represented by the
//				// same immutable memory capability used by ShadowOracle.
//				// This is opt-in and does not alter the production decision.
//				AssertRf06MemoryContractProjection(candidate);
//
//				if (TypedSlotEnabled)
//				{
//					// Phase 06: Two-stage class-admission + lane-materialization path
//					if (!TryClassAdmission(candidate, ref bundleMask, bundleMetadata, boundaryGuard, ref templateState, _fspOwnerVirtualThreadId, realInjectedCount, out TypedSlotRejectReason rejectA))
//					{
//						RecordTypedSlotReject(rejectA, candidate);
//						continue;
//					}
//
//					if (!TryMaterializeLane(candidate, bundleOccupancy, out int lane, out TypedSlotRejectReason rejectB))
//					{
//						RecordTypedSlotReject(rejectB, candidate);
//						continue;
//					}
//
//					// Commit
//					bundle[lane] = candidate;
//					MaterializePostStageBIssuedAttempt(candidate, pipelineEntry.IdentityTemplate, lane, bundleMetadata);
//					candidate.IsFspInjected = true;
//					bundleMask.AddOperation(candidate);
//					opportunityState = opportunityState.WithOccupiedSlot(lane);
//					bundleOccupancy |= (byte)(1 << lane);
//					SmtInjectionsCount++;
//					realInjectedCount++;
//					_perVtInjections[candidateVt]++;
//					RecordTypedSlotInject(candidate, lane);
//					bundleMetadata = bundleMetadata.WithOperation(candidate);
//					boundaryGuard = boundaryGuard.WithOperation(candidate);
//					_runtimeLegalityService.RefreshSmtAfterMutation(_currentReplayPhase, bundleMask, bundleMetadata, boundaryGuard);
//
//					_classCapacity.IncrementOccupancy(candidate.Placement.RequiredSlotClass);
//					ConsumeProjectedMemoryIssueStateIfNeeded(candidate);
//
//					if (candidate.Placement.PinningKind == SlotPinningKind.ClassFlexible)
//					{
//						RecordPhaseLane(candidate.Placement.RequiredSlotClass, lane);
//					}
//
//					// Phase 07: Capture class template on first successful typed-slot injection (pipelined)
//					if (_currentReplayPhase.IsActive && !_classTemplateValid)
//					{
//						CaptureClassTemplate(templateDomainScopeId);
//					}
//				}
//				else
//				{
//					// Legacy path: exact slot search + CanInject
//					int slot = ResolveNextInjectableSlot(opportunityState, 0);
//					if (slot < 0)
//						continue;
//
//					LegalityDecision legalityDecision = EvaluateSmtLegality(bundleMask, bundleMetadata, boundaryGuard, candidate);
//					if (!legalityDecision.IsAllowed)
//					{
//						RecordPerVtRejection(candidate.VirtualThreadId);
//						SmtRejectionsCount++;
//						continue;
//					}
//
//					int candidateBankId = candidate is LoadStoreMicroOp ls ? ls.MemoryBankId : -1;
//					if (!TryPassOuterCap(candidate, candidate.VirtualThreadId, candidate.IsMemoryOp, candidateBankId, _fspOwnerVirtualThreadId, out _))
//					{
//						RecordPerVtRejection(candidate.VirtualThreadId);
//						continue;
//					}
//
//					// Commit (legacy)
//					bundle[slot] = candidate;
//					candidate.IsFspInjected = true;
//					bundleMask.AddOperation(candidate);
//					opportunityState = opportunityState.WithOccupiedSlot(slot);
//					bundleOccupancy = opportunityState.OccupancyMask;
//					SmtInjectionsCount++;
//					realInjectedCount++;
//					_perVtInjections[candidateVt]++;
//					RecordTypedSlotInject(candidate, slot);
//					bundleMetadata = bundleMetadata.WithOperation(candidate);
//					boundaryGuard = boundaryGuard.WithOperation(candidate);
//					_runtimeLegalityService.RefreshSmtAfterMutation(_currentReplayPhase, bundleMask, bundleMetadata, boundaryGuard);
//					ConsumeProjectedMemoryIssueStateIfNeeded(candidate);
//				}
//
//				// Consume the SMT port
//				_smtPorts[candidateVt] = nullptr;
//				_smtPortValid[candidateVt] = false;
//			}
//
//			// Update nextEmptySlot for API compatibility
//			nextEmptySlot = ResolveNextInjectableSlot(opportunityState, 0);
//
//			_fspCurrentStage = FspPipelineStage.SCHED1;
//			return (bundleMask, nextEmptySlot);
//		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private void RebuildFspStageContext(IReadOnlyList<MicroOp?> bundle, int ownerVirtualThreadId, out BundleResourceCertificate4Way bundleMask, out SmtBundleMetadata4Way bundleMetadata, out BoundaryGuardState boundaryGuard)
		void RebuildFspStageContext(const std::vector<std::shared_ptr<MicroOp>> &bundle, int ownerVirtualThreadId, BundleResourceCertificate4Way &bundleMask, SmtBundleMetadata4Way &bundleMetadata, BoundaryGuardState &boundaryGuard);

		static bool ReplayPhaseMatches(ReplayPhaseContext expected, ReplayPhaseContext actual);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private void CaptureFspBundleReferences(IReadOnlyList<MicroOp?> bundle)
		void CaptureFspBundleReferences(const std::vector<std::shared_ptr<MicroOp>> &bundle);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private bool FspBundleReferencesMatch(IReadOnlyList<MicroOp?> bundle)
		bool FspBundleReferencesMatch(const std::vector<std::shared_ptr<MicroOp>> &bundle);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static FspSnapshotFingerprint ComputeFspBundleFingerprint(IReadOnlyList<MicroOp?> bundle)
		static FspSnapshotFingerprint ComputeFspBundleFingerprint(const std::vector<std::shared_ptr<MicroOp>> &bundle);

		static FspSnapshotFingerprint ComputeFspCandidateFingerprint(std::shared_ptr<MicroOp> candidate);

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private static (ulong Low, ulong High) CreateFspFingerprintSeed()
//		{
//			(14695981039346656037UL, 7809847782465536322UL);
//		}

		static void AddFspPlacementFingerprint(unsigned long long &low, unsigned long long &high, SlotPlacementMetadata placement);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void AddFspRegisterListFingerprint(ref ulong low, ref ulong high, IReadOnlyList<int> values)
		static void AddFspRegisterListFingerprint(unsigned long long &low, unsigned long long &high, const std::vector<int> &values);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void AddFspRangeListFingerprint(ref ulong low, ref ulong high, IReadOnlyList<(ulong Address, ulong Length)> ranges)
		static void AddFspRangeListFingerprint(unsigned long long &low, unsigned long long &high, const std::vector<std::pair<unsigned long long, unsigned long long>> &ranges);

		static void AddFspFingerprintValue(unsigned long long &low, unsigned long long &high, unsigned long long value);

		/// <summary>
		/// Get the latched inter-core nomination ready mask for FspPowerController.
		/// HLS: direct wire tap on the latched ready-mask register.
		/// </summary>
	public:
		unsigned short GetLatchedInterCoreNominationReadyMask();




		// ══════════════════════════════════════════════════════════════
		//  Nomination API
		// ══════════════════════════════════════════════════════════════

		/// <summary>
		/// Nominate a micro-operation from a core for potential FSP injection.
		/// Control-flow and null operations are silently ignored.
		/// Nominations from stalled cores are annulled.
		///
		/// HLS: single-cycle write to nomination port register.
		/// </summary>
		/// <param name="coreId">Core index within Pod (0–15)</param>
		/// <param name="op">Candidate micro-operation (may be null)</param>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public void Nominate(int coreId, MicroOp? op)
		inline void Nominate(int coreId, std::shared_ptr<MicroOp> op);

		/// <summary>
		/// Nominate an SMT candidate from a virtual thread for intra-core scheduling.
		/// HLS: single-cycle write to VT nomination port.
		/// </summary>
		/// <param name="vtId">Virtual thread ID (0–3)</param>
		/// <param name="op">Candidate micro-operation</param>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public void NominateSmtCandidate(int vtId, MicroOp? op)
		void NominateSmtCandidate(int vtId, std::shared_ptr<MicroOp> op);

		// The synchronous RF-06 caller owns only this exact nomination. A
		// rejected/throwing route must not issue later from a retained port,
		// and cleanup must preserve a replacement or foreign VT nomination.
		void WithdrawSmtCandidate(int vtId, std::shared_ptr<MicroOp> expected);

		/// <summary>
		/// Try to steal a slot from any nominated core.
		/// Priority encoder: scans ports 0→15, returns first valid candidate
		/// that passes domain filtering. Consumes the port on success.
		///
		/// HLS: 16-input priority encoder with domain-tag comparator.
		/// </summary>
		/// <param name="requestingCoreId">Core requesting the steal (excluded from search)</param>
		/// <param name="requestedDomainTag">Domain tag filter (0 = no filtering)</param>
		/// <returns>Stolen MicroOp, or null if none available</returns>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public MicroOp? TryStealSlot(int requestingCoreId, ulong requestedDomainTag)
		inline std::shared_ptr<MicroOp> TryStealSlot(int requestingCoreId, unsigned long long requestedDomainTag);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private MicroOp? TryStealSlot(int requestingCoreId, ulong requestedDomainTag, InterCoreNominationSnapshot nominationSnapshot)
		inline std::shared_ptr<MicroOp> TryStealSlot(int requestingCoreId, unsigned long long requestedDomainTag, InterCoreNominationSnapshot nominationSnapshot);

		/// <summary>
		/// Mark a core as stalled (annuls its nominations) or unstalled.
		/// HLS: single-bit write to stalled register.
		/// </summary>
	public:
		inline void SetCoreStalled(int coreId, bool stalled);

		/// <summary>
		/// Latch the currently visible inter-core nomination state for cycle-local consumers.
		/// Called at start of scheduling cycle by PodController.BeginCycle().
		/// HLS: 16 × D-flip-flop transfer (one clock edge).
		/// </summary>
		void LatchNominations();

		/// <summary>
		/// Clear all live nomination ports for fresh nominations.
		/// Called after LatchNominations by PodController.BeginCycle().
		/// HLS: 16-bit register clear.
		/// </summary>
		void ClearNominationPorts();



		// ══════════════════════════════════════════════════════════════
		//  Global Scoreboard (DMA / Outstanding Loads)
		// ══════════════════════════════════════════════════════════════

		/// <summary>
		/// Mark a target (register ID, DMA channel, etc.) as pending in the scoreboard.
		/// Returns the slot index used, or -1 if scoreboard is full.
		///
		/// HLS: 8-entry CAM scan + first-free allocation.
		/// </summary>
		/// <param name="targetId">Resource target to mark pending</param>
		/// <param name="ownerThreadId">Thread that owns this pending entry</param>
		/// <param name="currentCycle">Cycle when entry was created</param>
		/// <returns>Allocated slot index (0–7), or -1 if full</returns>
		inline int SetScoreboardPending(int targetId, int ownerThreadId, long long currentCycle);

		/// <summary>
		/// Check if a target is pending in the global scoreboard.
		/// HLS: 8-entry CAM compare.
		/// </summary>
		inline bool IsScoreboardPending(int targetId);

		/// <summary>
		/// Clear a specific scoreboard entry by slot index.
		/// HLS: single register write.
		/// </summary>
		inline void ClearScoreboardEntry(int slotIndex);

		/// <summary>
		/// Clear all scoreboard entries (flush on pipeline reset / context switch).
		/// HLS: 8-entry register clear.
		/// </summary>
		inline void ClearScoreboard();

		// ══════════════════════════════════════════════════════════════
		//  Per-VT Scoreboard (4-Way SMT Isolation)
		// ══════════════════════════════════════════════════════════════

		/// <summary>
		/// Mark a target as pending in the per-VT scoreboard (legacy DMA-typed entry).
		/// Returns the slot index used, or -1 if scoreboard is full.
		///
		/// HLS: per-VT 8-entry CAM.
		/// </summary>
		inline int SetSmtScoreboardPending(int targetId, int virtualThreadId, long long currentCycle);

		/// <summary>
		/// Mark a target as pending with explicit entry type and optional bank ID (Refactoring Pt. 3).
		/// Returns the slot index used, or -1 if scoreboard is full.
		///
		/// HLS: per-VT 8-entry CAM scan + first-free allocation + 3-field write.
		/// </summary>
		int SetSmtScoreboardPendingTyped(int targetId, int virtualThreadId, long long currentCycle, ScoreboardEntryType entryType, int bankId);

		/// <summary>
		/// Check if a target is pending in a specific VT's scoreboard.
		/// HLS: 8-entry CAM per VT.
		/// </summary>
		inline bool IsSmtScoreboardPending(int targetId, int virtualThreadId);

		/// <summary>
		/// Clear a specific entry in the per-VT scoreboard.
		/// HLS: 3-register write (targetId + type + bankId).
		/// </summary>
		void ClearSmtScoreboardEntry(int virtualThreadId, int slotIndex);

		/// <summary>
		/// Clear all entries in all VT scoreboards.
		/// HLS: 32-entry register clear (4 VTs × 8 slots × 3 fields).
		/// </summary>
		inline void ClearSmtScoreboard();

		// ══════════════════════════════════════════════════════════════
		//  Universal Scoreboard Queries (Refactoring Pt. 3)
		// ══════════════════════════════════════════════════════════════

		/// <summary>
		/// Check if any outstanding load (MSHR entry) targets the specified memory bank
		/// for a given virtual thread. Used by FSP and pipeline ID-stage to prevent
		/// Load/Store injection when the bank has in-flight MSHR entries.
		///
		/// HLS: 8-entry CAM scan with type + bankId comparator = O(8) fixed.
		/// </summary>
		bool IsBankPendingForVT(int bankId, int virtualThreadId);

		/// <summary>
		/// Check if any outstanding load/store targets the specified memory bank
		/// across ALL virtual threads. Used for inter-VT bank conflict detection
		/// during Pod-level FSP injection.
		///
		/// HLS: 4 × 8-entry CAM scan = 32 comparisons, single-cycle with parallel reduction.
		/// </summary>
		inline bool IsBankPendingGlobal(int bankId);

		/// <summary>
		/// Count outstanding memory entries (Load + Store) for a given virtual thread.
		/// Used for MSHR saturation detection and back-pressure signaling.
		///
		/// HLS: 8-entry type-match scan.
		/// </summary>
		int GetOutstandingMemoryCount(int virtualThreadId);

		/// <summary>
		/// Count free scoreboard slots for a given virtual thread using the same
		/// slot allocator truth as <see cref="SetSmtScoreboardPendingTyped"/>.
		/// </summary>
	private:
		inline int GetFreeSmtScoreboardSlotCount(int virtualThreadId);

		/// <summary>Number of MSHR/bank scoreboard stalls detected during scheduling.</summary>
	public:
		long long getMshrScoreboardStalls() const;
	private:
		void setMshrScoreboardStalls(long long value);



// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private DomainHypercallCanonicalComposition? _exactVirtualizationComposition;
		std::shared_ptr<DomainHypercallCanonicalComposition> _exactVirtualizationComposition;

	public:
		std::optional<std::shared_ptr<DomainHypercallCompositionResult>> getLastExactVirtualizationCompositionResult() const;
	private:
		void setLastExactVirtualizationCompositionResult(const std::optional<std::shared_ptr<DomainHypercallCompositionResult>> &value);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SafetyVerifier? ExactVirtualizationCanonicalVerifier
	public:
		std::shared_ptr<SafetyVerifier> getExactVirtualizationCanonicalVerifier() const;

		bool getHasExactVirtualizationComposition() const;

		void ConfigureExactVirtualizationComposition(std::shared_ptr<DomainHypercallCanonicalComposition> composition);

		void DisableExactVirtualizationComposition();

	private:
// C# TO C++ CONVERTER TASK: C++ does not support 'partial' methods:
// ORIGINAL LINE: partial void OnCanonicalVirtualizationOperandMaterialized(BundleIssuePacket issuePacket, IssuePacketLane issueLane, SmtBundleMetadata4Way bundleMetadata, VmxMicroOp carrier, SafetyVerifier.VirtualizationAdmissionCertificate e1, VirtualizationOperandSnapshot operand)
		void OnCanonicalVirtualizationOperandMaterialized(BundleIssuePacket issuePacket, IssuePacketLane issueLane, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VirtualizationAdmissionCertificate> e1, std::shared_ptr<VirtualizationOperandSnapshot> operand);


// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private VmReadScalarDeliveryCanonicalComposition? _exactVmReadScalarDelivery;
		std::shared_ptr<VmReadScalarDeliveryCanonicalComposition> _exactVmReadScalarDelivery;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition? _exactGuestPcSpFlagsVmReadScalarDelivery;
		std::shared_ptr<GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition> _exactGuestPcSpFlagsVmReadScalarDelivery;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MemoryOwnedVmReadScalarDeliveryCanonicalComposition? _exactMemoryOwnedVmReadScalarDelivery;
		std::shared_ptr<MemoryOwnedVmReadScalarDeliveryCanonicalComposition> _exactMemoryOwnedVmReadScalarDelivery;

	public:
		std::optional<std::shared_ptr<VmReadScalarDeliveryResult>> getLastVmReadScalarDeliveryResult() const;
	private:
		void setLastVmReadScalarDeliveryResult(const std::optional<std::shared_ptr<VmReadScalarDeliveryResult>> &value);
	public:
		bool getHasExactVmReadScalarDelivery() const;
		bool getHasExactGuestPcSpFlagsVmReadScalarDelivery() const;
		bool getHasExactMemoryOwnedVmReadScalarDelivery() const;

		void ConfigureExactVmReadScalarDelivery(std::shared_ptr<VmReadScalarDeliveryCanonicalComposition> composition);

		void DisableExactVmReadScalarDelivery();

		void ConfigureExactGuestPcSpFlagsVmReadScalarDelivery(std::shared_ptr<GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition> composition);

		void DisableExactGuestPcSpFlagsVmReadScalarDelivery();

		void ConfigureExactMemoryOwnedVmReadScalarDelivery(std::shared_ptr<MemoryOwnedVmReadScalarDeliveryCanonicalComposition> composition);

		void DisableExactMemoryOwnedVmReadScalarDelivery();

		bool TryPrepareVmReadScalarAfterCanonicalValueRead(BundleIssuePacket issuePacket, IssuePacketLane issueLane, unsigned long long fieldSelector, unsigned long long restoreGeneration);

		bool TryPrepareCompletionReasonQualificationVmReadAfterCanonicalValueRead(std::shared_ptr<CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition> composition, IssuePacketLane issueLane, unsigned long long fieldSelector, unsigned long long restoreGeneration);

		bool TryPrepareCompletionSecondStageVmReadAfterCanonicalValueRead(std::shared_ptr<CompletionSecondStageVmReadScalarDeliveryCanonicalComposition> composition, IssuePacketLane issueLane, unsigned long long fieldSelector, unsigned long long restoreGeneration);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool TryPreparePinBasedControlsVmReadAfterCanonicalValueRead(PinBasedControlsVmReadScalarDeliveryCanonicalComposition? composition, IssuePacketLane issueLane, ulong fieldSelector, ulong restoreGeneration)
		bool TryPreparePinBasedControlsVmReadAfterCanonicalValueRead(std::shared_ptr<PinBasedControlsVmReadScalarDeliveryCanonicalComposition> composition, IssuePacketLane issueLane, unsigned long long fieldSelector, unsigned long long restoreGeneration);


	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ResearchVirtualizationCanonicalIssueComposition? _researchVirtualizationCanonicalComposition;
		std::shared_ptr<ResearchVirtualizationCanonicalIssueComposition> _researchVirtualizationCanonicalComposition;

	public:
		std::optional<std::shared_ptr<ResearchVirtualizationCanonicalCompositionResult>> getLastResearchVirtualizationCanonicalCompositionResult() const;
	private:
		void setLastResearchVirtualizationCanonicalCompositionResult(const std::optional<std::shared_ptr<ResearchVirtualizationCanonicalCompositionResult>> &value);

	public:
		void EnableResearchVirtualizationCanonicalComposition(std::shared_ptr<ResearchVirtualizationCanonicalIssueComposition> composition);

		void DisableResearchVirtualizationCanonicalComposition();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SafetyVerifier? ResearchVirtualizationCanonicalVerifierForTesting
		std::shared_ptr<SafetyVerifier> getResearchVirtualizationCanonicalVerifierForTesting() const;

	private:
// C# TO C++ CONVERTER TASK: C++ does not support 'partial' methods:
// ORIGINAL LINE: partial void OnCanonicalVirtualizationAdmissionMaterialized(BundleIssuePacket issuePacket, IssuePacketLane issueLane, SmtBundleMetadata4Way bundleMetadata, VmxMicroOp carrier, SafetyVerifier.VirtualizationAdmissionCertificate e1)
		void OnCanonicalVirtualizationAdmissionMaterialized(BundleIssuePacket issuePacket, IssuePacketLane issueLane, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VirtualizationAdmissionCertificate> e1);


		// ══════════════════════════════════════════════════════════════
		//  PackBundleIntraCoreSmt — Intra-Core 4-Way SMT
		// ══════════════════════════════════════════════════════════════

		/// <summary>
		/// Pack a VLIW bundle with intra-core SMT slot stealing.
		/// Scans SMT nomination ports (VT 0–3) and injects candidates from
		/// other virtual threads into empty/NOP slots, subject to safety mask checks.
		///
		/// Supports two modes:
		/// - **Single-cycle** (default): Original combinational path for functional correctness.
		/// - **2-stage pipelined**: Call PipelineFspStage1_Nominate() on cycle N,
		///   then PipelineFspStage2_Intersect() on cycle N+1 for reduced LUT depth.
		///   This mode is used when PipelinedFspEnabled is true.
		///
		/// HLS: 4-iteration loop (one per VT), each with a safety comparator.
		/// </summary>
		/// <summary>
		/// Pack a VLIW bundle with intra-core SMT slot stealing using an explicit
		/// FSM-provided runnable VT mask for the nomination snapshot.
		/// </summary>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public MicroOp[] PackBundleIntraCoreSmt(System.Collections.Generic.IReadOnlyList<MicroOp?> bundle, int ownerVirtualThreadId, int localCoreId, byte eligibleVirtualThreadMask, System.Nullable<YAKSys_Hybrid_CPU.Memory.MemorySubsystem> memSub = null)
		std::vector<std::shared_ptr<MicroOp>> PackBundleIntraCoreSmt(const std::vector<std::shared_ptr<MicroOp>> &bundle, int ownerVirtualThreadId, int localCoreId, unsigned char eligibleVirtualThreadMask, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);

	private:
		inline void MaterializePostStageBIssuedAttempt(std::shared_ptr<MicroOp> candidate, int lane, SmtBundleMetadata4Way bundleMetadata);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private void MaterializePostStageBIssuedAttempt(MicroOp candidate, PostStageBIdentityTemplate? template, int lane, SmtBundleMetadata4Way bundleMetadata)
		inline void MaterializePostStageBIssuedAttempt(std::shared_ptr<MicroOp> candidate, std::shared_ptr<PostStageBIdentityTemplate> template_Keyword, int lane, SmtBundleMetadata4Way bundleMetadata);

		/// <summary>
		/// Materializes E1 only after the canonical issue packet has selected a live
		/// foreground lane. VMX is a serial/SystemSingleton carrier and therefore is
		/// not expected to arrive through SMT donor injection.
		/// </summary>
	public:
		bool TryAttachVirtualizationAdmissionAfterCanonicalLaneMaterialization(BundleIssuePacket issuePacket, IssuePacketLane issueLane);

		bool TryAttachVirtualizationOperandSnapshotAfterCanonicalValueRead(BundleIssuePacket issuePacket, IssuePacketLane issueLane, unsigned long long rs1Value, unsigned long long restoreGeneration);

// C# TO C++ CONVERTER TASK: C++ does not support 'partial' methods:
// ORIGINAL LINE: partial void OnCanonicalVirtualizationOperandMaterialized(BundleIssuePacket issuePacket, IssuePacketLane issueLane, SmtBundleMetadata4Way bundleMetadata, VmxMicroOp carrier, SafetyVerifier.VirtualizationAdmissionCertificate e1, VirtualizationOperandSnapshot operand);
//		partial void OnCanonicalVirtualizationOperandMaterialized(BundleIssuePacket issuePacket, IssuePacketLane issueLane, SmtBundleMetadata4Way bundleMetadata, VmxMicroOp carrier, SafetyVerifier.VirtualizationAdmissionCertificate e1, VirtualizationOperandSnapshot operand);

// C# TO C++ CONVERTER TASK: C++ does not support 'partial' methods:
// ORIGINAL LINE: partial void OnCanonicalVirtualizationAdmissionMaterialized(BundleIssuePacket issuePacket, IssuePacketLane issueLane, SmtBundleMetadata4Way bundleMetadata, VmxMicroOp carrier, SafetyVerifier.VirtualizationAdmissionCertificate e1);
//		partial void OnCanonicalVirtualizationAdmissionMaterialized(BundleIssuePacket issuePacket, IssuePacketLane issueLane, SmtBundleMetadata4Way bundleMetadata, VmxMicroOp carrier, SafetyVerifier.VirtualizationAdmissionCertificate e1);

		/// <summary>
		/// Enable 2-stage pipelined FSP arbitration for HLS timing closure.
		/// When true, PackBundleIntraCoreSmt uses SCHED1/SCHED2 pipeline stages
		/// instead of single-cycle combinational path.
		/// Default: false (single-cycle, backwards compatible).
		/// </summary>
		bool getPipelinedFspEnabled() const;
		void setPipelinedFspEnabled(bool value);

		/// <summary>
		/// Master toggle for typed-slot admission pipeline.
		/// When false: scheduler uses legacy physical-slot path (Unclassified + exact slot search).
		/// When true: scheduler uses two-stage class-admission + lane-materialization.
		/// Default: false (backward compatible; opt-in for typed-slot rollout).
		/// <para>
		/// Takes effect on the next scheduling cycle. Disabling immediately reverts to legacy path.
		/// When disabled, all Phase 08 typed-slot counters remain at 0 for clean A/B comparison.
		/// </para>
		/// </summary>
		bool getTypedSlotEnabled() const;
		void setTypedSlotEnabled(bool value);

		/// <summary>
		/// Publish current replay-phase context to the scheduler.
		/// </summary>
		void SetReplayPhaseContext(ReplayPhaseContext phase, bool invalidateAssistOnDeactivate = true);

		/// <summary>
		/// Publish the deterministic hardware-occupancy snapshot for the current packing pass.
		/// Callers should set this once before invoking PackBundle/PackBundleIntraCoreSmt.
		/// </summary>
		void SetHardwareOccupancySnapshot(HardwareOccupancySnapshot128 snapshot);

		/// <summary>
		/// Notify the scheduler that a serialising instruction has been committed to
		/// architectural state (G34 — Deterministic Legality Alignment).
		///
		/// <para>
		/// Each call creates a new serialising epoch boundary:
		/// <list type="bullet">
		///   <item>Increments the serialising epoch counter.</item>
		///   <item>Invalidates all phase-certificate templates (inter-core and 4-way)
		///   so that no replay-phase certificate spans a serialising event (G35/G36).</item>
		///   <item>Invalidates the class-capacity template with reason
		///   <see cref="ReplayPhaseInvalidationReason.SerializingEvent"/>.</item>
		/// </list>
		/// </para>
		///
		/// <para>
		/// Must be called by the commit stage after retiring a
		/// <see cref="Arch.SerializationClass.FullSerial"/> or
		/// <see cref="Arch.SerializationClass.VmxSerial"/> instruction.
		/// </para>
		/// </summary>
		void NotifySerializingCommit(bool invalidateAssist = true);

		/// <summary>
		/// Get Phase 1 replay/certificate telemetry from the scheduler.
		/// </summary>
		SchedulerPhaseMetrics GetPhaseMetrics();

		void RecordLegalityCertificateCacheHit(long long estimatedChecksSaved) override;

		void RecordLegalityCertificateCacheMiss() override;

		void RecordLegalityCertificateCacheInvalidation(ReplayPhaseInvalidationReason reason) override;

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private LegalityDecision EvaluateInterCoreLegality(IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCert, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask = default)
		inline LegalityDecision EvaluateInterCoreLegality(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCert, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask = YAKSys_Hybrid_CPU::Core::SafetyMask128());

		inline int CountPendingInterCoreCandidates(unsigned long long requestedDomainTag);

		inline int CountPendingInterCoreCandidates(InterCoreNominationSnapshot nominationSnapshot, unsigned long long requestedDomainTag);

		inline static int CountPendingSmtCandidates(SmtNominationState nominationState, int ownerVirtualThreadId);

		inline void RecordDeterminismTaxWindow(int referenceSlots, int replayEligibleSlots, int readyCandidates);

		inline void RecordDomainIsolationProbe(DomainIsolationProbeResult probe);

		/// <summary>
		/// Record a typed-slot rejection for telemetry (Phase 03).
		/// Increments the appropriate per-reason counter and preserves
		/// backward-compatible <see cref="SmtRejectionsCount"/>.
		/// <para>
		/// Note: the outer-cap gate already increments SmtRejectionsCount
		/// for ScoreboardReject, BankPendingReject, HardwareBudgetReject,
		/// and SpeculationBudgetReject.
		/// This method only increments SmtRejectionsCount for reject types that
		/// do not originate from TryPassOuterCap.
		/// </para>
		/// <para>HLS: diagnostic counter bank — not on critical path.</para>
		/// </summary>
		inline void RecordTypedSlotReject(TypedSlotRejectReason reason);

		inline void RecordCertificateRejectByClass(SlotClass slotClass);

		inline void RecordSmtLegalityRejectByClass(SlotClass slotClass);

		inline void RecordCertificateRegisterGroupConflict(int virtualThreadId);

		inline void RecordSmtLegalityRejectBreakdown();

		inline void RecordPerVtRejection(int virtualThreadId);

		inline void RecordPerVtRegGroupConflict(int virtualThreadId);

		/// <summary>
		/// Phase 06: Extended reject recording with candidate metadata.
		/// Computes <see cref="TypedSlotRejectClassification"/> via
		/// the runtime legality service seam for structured diagnostics.
		/// Falls back to <see cref="RecordTypedSlotReject(TypedSlotRejectReason)"/>
		/// for counter updates.
		/// <para>HLS: diagnostic path — not on critical timing path.</para>
		/// </summary>
		inline void RecordTypedSlotReject(TypedSlotRejectReason reason, std::shared_ptr<MicroOp> candidate);

		inline void RecordBankPendingReject(int memoryBankId);

	public:
		void RecordMemoryClusteringEvent();

		/// <summary>
		/// Phase 08: Record a successful typed-slot injection with per-class distribution.
		/// Called in commit path after successful typed-slot lane materialization.
		/// <para>HLS: diagnostic counter bank — not on critical path.</para>
		/// </summary>
	private:
		inline void RecordTypedSlotInject(std::shared_ptr<MicroOp> candidate, int lane);

		inline int ResolveNextInjectableSlot(BundleOpportunityState opportunityState, int startIndex);

		/// <summary>
		/// Compute an 8-bit bitmask of physically occupied slots in a VLIW bundle.
		/// Bit N is set when <paramref name="bundle"/>[N] is not null.
		/// <para>HLS: 8-bit wire-OR of non-null predicates — zero LUT depth.</para>
		/// </summary>
		inline static unsigned char ComputeBundleOccupancy(std::vector<std::shared_ptr<MicroOp>> &bundle);

		// ══════════════════════════════════════════════════════════════


		/// <summary>
		/// Eligibility snapshot for the most recent SMT nomination pass.
		/// Diagnostics-only contour: exposes how the FSM-provided runnable mask
		/// intersected with ready SMT donor ports.
		/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//		public readonly record struct SmtEligibilitySnapshot(byte RequestedMask, byte NormalizedMask, byte ReadyPortMask, byte VisibleReadyMask, byte MaskedReadyMask)
//		{
//			public int EligibleCount
//			{
//				get
//				{
//					return BitOperations.PopCount((uint)NormalizedMask);
//				}
//			}
//
//			public int ReadyPortCount
//			{
//				get
//				{
//					return BitOperations.PopCount((uint)ReadyPortMask);
//				}
//			}
//
//			public int VisibleReadyCount
//			{
//				get
//				{
//					return BitOperations.PopCount((uint)VisibleReadyMask);
//				}
//			}
//
//			public int MaskedReadyCount
//			{
//				get
//				{
//					return BitOperations.PopCount((uint)MaskedReadyMask);
//				}
//			}
//
//			public bool HasMaskedReadyCandidates
//			{
//				get
//				{
//					return MaskedReadyMask != 0;
//				}
//			}
//		}

	public:
		static unsigned char getAllEligibleVirtualThreadMask();

	private:
		std::shared_ptr<SmtEligibilitySnapshot> _lastSmtEligibilitySnapshot;

	public:
		long long getEligibilityMaskedCycles() const;
	private:
		void setEligibilityMaskedCycles(long long value);

	public:
		long long getEligibilityMaskedReadyCandidates() const;
	private:
		void setEligibilityMaskedReadyCandidates(long long value);

		inline static unsigned char NormalizeEligibleVirtualThreadMask(unsigned char eligibleVirtualThreadMask);

		inline SmtNominationState CreateEligibleSmtNominationState(unsigned char eligibleVirtualThreadMask);

		inline void RecordSmtEligibilitySnapshot(std::shared_ptr<SmtEligibilitySnapshot> snapshot);

		inline SmtNominationSnapshot CaptureSmtNominationSnapshot(unsigned char eligibleVirtualThreadMask);
	};
}
