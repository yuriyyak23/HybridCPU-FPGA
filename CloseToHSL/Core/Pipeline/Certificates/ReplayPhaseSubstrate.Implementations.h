#pragma once
namespace YAKSys_Hybrid_CPU::Core { enum class RejectKind : unsigned char; enum class LegalityAuthoritySource : unsigned char; }
namespace YAKSys_Hybrid_CPU::Core::Diagnostics { struct LoopPhaseClassProfile; }

#include "ReplayPhaseSubstrate.Interfaces.h"
#include "BundleResourceCertificate.h"
#include "ReplayPhaseSubstrate.h"
#include "SafetyMask128.h"
namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }
#include "../Safety/SafetyVerifier.Types.h"
#include "YAKSys_Hybrid_CPU.Core.BundleResourceCertificate4Way.h"
#include "BundleResourceCertificate4Way.h"
#include "../Assist/Runtime/AssistRuntime.h"
#include <vector>
#include <any>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class BoundaryGuardState; }
namespace YAKSys_Hybrid_CPU::Core { class SmtBundleMetadata4Way; }
namespace YAKSys_Hybrid_CPU::Core { class PhaseCertificateTemplateKey4Way; }

using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;


namespace YAKSys_Hybrid_CPU::Core
{
	/// </summary>
	class LegalityCertificateCacheCoordinator final : public ILegalityCertificateCacheCoordinator
	{
	private:
		const std::shared_ptr<ILegalityCertificateCacheTelemetrySink> _telemetrySink;
		const std::shared_ptr<IInterCoreLegalityCertificateCache> _interCoreCache;
		const std::shared_ptr<ISmtLegalityCertificateCache4Way> _smtCache;

	public:
		LegalityCertificateCacheCoordinator(std::shared_ptr<ILegalityCertificateCacheTelemetrySink> telemetrySink, std::shared_ptr<IInterCoreLegalityCertificateCache> interCoreCache, std::shared_ptr<ISmtLegalityCertificateCache4Way> smtCache);

		inline void PrepareInterCore(ReplayPhaseKey phaseKey, std::shared_ptr<BundleResourceCertificate> certificate) override;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public LegalityDecision EvaluateInterCoreLegality(ILegalityChecker legalityChecker, ReplayPhaseKey phaseKey, IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCertificate, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask = default)
		inline LegalityDecision EvaluateInterCoreLegality(std::shared_ptr<ILegalityChecker> legalityChecker, ReplayPhaseKey phaseKey, const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCertificate, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask = YAKSys_Hybrid_CPU::Core::SafetyMask128()) override;

		inline void RefreshInterCoreAfterMutation(ReplayPhaseKey phaseKey, std::shared_ptr<BundleResourceCertificate> certificate) override;

		inline void PrepareSmt(ReplayPhaseKey phaseKey, BundleResourceCertificate4Way certificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard) override;

		inline LegalityDecision EvaluateSmtBoundaryGuard(std::shared_ptr<ILegalityChecker> legalityChecker, ReplayPhaseKey phaseKey, BundleResourceCertificate4Way bundleCertificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard) override;

		inline LegalityDecision EvaluateSmtLegality(std::shared_ptr<ILegalityChecker> legalityChecker, ReplayPhaseKey phaseKey, BundleResourceCertificate4Way bundleCertificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard, std::shared_ptr<MicroOp> candidate) override;

		inline void RefreshSmtAfterMutation(ReplayPhaseKey phaseKey, BundleResourceCertificate4Way certificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard) override;

		inline void InvalidatePhaseMismatch(ReplayPhaseContext phase) override;

		inline void Invalidate(ReplayPhaseInvalidationReason reason, bool invalidateInterCore = true, bool invalidateFourWay = true) override;

	private:
		inline void ApplyTelemetry(LegalityCertificateCacheTelemetry telemetry);

		inline static PhaseCertificateTemplateKey CreateInterCoreTemplateKey(ReplayPhaseKey phaseKey, std::shared_ptr<BundleResourceCertificate> certificate);

		inline static PhaseCertificateTemplateKey4Way CreateSmtTemplateKey(ReplayPhaseKey phaseKey, BundleResourceCertificate4Way certificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard);
	};

	/// <summary>
	/// Internal runtime-local legality service provisioning.
	/// Keeps scheduler construction on a single inter-core/SMT legality service seam
	/// without exposing checker/cache wiring as a public runtime contract.
	/// </summary>
	class RuntimeLegalityServiceFactory final
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static IRuntimeLegalityService CreateDefault(ILegalityCertificateCacheTelemetrySink telemetrySink, IInterCoreLegalityCertificateCache? interCoreCache = null, System.Nullable<ISmtLegalityCertificateCache4Way> smtCache = null, System.Nullable<ILegalityCertificateCacheCoordinator> legalityCertificateCacheCoordinator = null)
		static std::shared_ptr<IRuntimeLegalityService> CreateDefault(std::shared_ptr<ILegalityCertificateCacheTelemetrySink> telemetrySink, std::shared_ptr<IInterCoreLegalityCertificateCache> interCoreCache = nullptr, std::shared_ptr<ISmtLegalityCertificateCache4Way> smtCache = nullptr, std::shared_ptr<ILegalityCertificateCacheCoordinator> legalityCertificateCacheCoordinator = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static IRuntimeLegalityService CreateCompatibilityDefault(ILegalityCertificateCacheTelemetrySink telemetrySink, ILegalityChecker legalityChecker, IInterCoreLegalityCertificateCache? interCoreCache = null, System.Nullable<ISmtLegalityCertificateCache4Way> smtCache = null, System.Nullable<ILegalityCertificateCacheCoordinator> legalityCertificateCacheCoordinator = null)
		static std::shared_ptr<IRuntimeLegalityService> CreateCompatibilityDefault(std::shared_ptr<ILegalityCertificateCacheTelemetrySink> telemetrySink, std::shared_ptr<ILegalityChecker> legalityChecker, std::shared_ptr<IInterCoreLegalityCertificateCache> interCoreCache = nullptr, std::shared_ptr<ISmtLegalityCertificateCache4Way> smtCache = nullptr, std::shared_ptr<ILegalityCertificateCacheCoordinator> legalityCertificateCacheCoordinator = nullptr);
	};

	/// <summary>
	/// Internal runtime legality service for inter-core + SMT hot paths plus adjacent diagnostics.
	/// </summary>

	/// <summary>
	/// Explicit scheduler-facing cache seam for replay-stable inter-core legality reuse.
	/// Owns template lifecycle and emits cache telemetry instead of exposing raw
	/// template hit/miss semantics to scheduler helpers.
	/// </summary>
	class InterCoreLegalityCertificateCache : public IInterCoreLegalityCertificateCache
	{
	private:
		PhaseCertificateTemplate _template;

	public:
		bool isValid() const override;

		bool Matches(ReplayPhaseContext phase) override;

		void Prepare(PhaseCertificateTemplateKey templateKey, std::shared_ptr<BundleResourceCertificate> certificate) override;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public LegalityCertificateCacheEvaluation<LegalityDecision> EvaluateLegality(ILegalityChecker legalityChecker, IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask = default)
		LegalityCertificateCacheEvaluation<LegalityDecision> EvaluateLegality(std::shared_ptr<ILegalityChecker> legalityChecker, const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask = YAKSys_Hybrid_CPU::Core::SafetyMask128()) override;

		LegalityCertificateCacheTelemetry RefreshAfterMutation(PhaseCertificateTemplateKey templateKey, std::shared_ptr<BundleResourceCertificate> certificate) override;

		bool Invalidate() override;
	};

	/// <summary>
	/// Explicit SMT boundary mode carried by replay/template certificate keys.
	/// Keeps serializing-boundary scope out of implicit phase-only matching.
	/// </summary>
	enum class SmtReplayBoundaryMode : unsigned char
	{
		Open = 0,
		SerializingBundle = 1
	};

	/// <summary>
	/// Boundary guard context for SMT replay/template reuse.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct BoundaryGuardState : IEquatable<BoundaryGuardState>
	class BoundaryGuardState
	{
	private:
		long long SerializingEpochId{};
		SmtReplayBoundaryMode BoundaryMode{};

	public:
		BoundaryGuardState(long long serializingEpochId, SmtReplayBoundaryMode boundaryMode);

		static BoundaryGuardState Open(long long serializingEpochId);

		long long getSerializingEpochId() const;

		SmtReplayBoundaryMode getBoundaryMode() const;

		bool isValid() const;

		bool getBlocksSmtInjection() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public BoundaryGuardState WithOperation(MicroOp? op)
		BoundaryGuardState WithOperation(std::shared_ptr<MicroOp> op) const;

		bool Equals(BoundaryGuardState other) const ;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
		bool Equals(std::any obj) const;

		int GetHashCode() const;

		public:
        BoundaryGuardState() = default;
	};

	/// <summary>
	/// Explicit runtime bundle metadata for SMT replay/template and guard-plane checks.
	/// Tracks owner/domain scope incrementally as the scheduler builds the working bundle,
	/// so legality consumers do not need to reconstruct these facts from a live bundle scan.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct SmtBundleMetadata4Way : IEquatable<SmtBundleMetadata4Way>
	class SmtBundleMetadata4Way
	{
	private:
		int OwnerVirtualThreadId{};
		int OwnerContextId{};
		unsigned long long OwnerDomainTag{};
		unsigned long long BundleDomainXor{};
		unsigned long long BundleDomainSum{};
		int OperationCount{};

	public:
		SmtBundleMetadata4Way(int ownerVirtualThreadId, int ownerContextId, unsigned long long ownerDomainTag, unsigned long long bundleDomainXor, unsigned long long bundleDomainSum, int operationCount);

		static SmtBundleMetadata4Way Empty(int ownerVirtualThreadId);

		int getOwnerVirtualThreadId() const;

		int getOwnerContextId() const;

		unsigned long long getOwnerDomainTag() const;

		unsigned long long getBundleDomainXor() const;

		unsigned long long getBundleDomainSum() const;

		int getOperationCount() const;

		bool isValid() const;

		bool getHasKnownOwnerContext() const;

		bool getHasDomainRestriction() const;

		unsigned long long getBundleDomainShapeId() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SmtBundleMetadata4Way WithOperation(MicroOp? op)
		SmtBundleMetadata4Way WithOperation(std::shared_ptr<MicroOp> op) const;

		bool Equals(SmtBundleMetadata4Way other) const ;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
		bool Equals(std::any obj) const;

		int GetHashCode() const;

	private:
		static unsigned long long ComputeOperationDomainShape(std::shared_ptr<MicroOp> op);

		public:
        SmtBundleMetadata4Way() = default;
	};

	/// <summary>
	/// Explicit replay/template key for SMT bundle certificates.
	/// Replay epoch is only one part of the scope; boundary and owner/domain context
	/// are part of the key rather than hidden scheduler-side assumptions.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct PhaseCertificateTemplateKey4Way : IEquatable<PhaseCertificateTemplateKey4Way>
	class PhaseCertificateTemplateKey4Way
	{
	private:
		ReplayPhaseKey PhaseKey{};
		BundleResourceCertificateIdentity4Way CertificateIdentity{};
		SmtBundleMetadata4Way BundleMetadata{};
		BoundaryGuardState BoundaryGuard{};

	public:
		PhaseCertificateTemplateKey4Way(ReplayPhaseKey phaseKey, BundleResourceCertificateIdentity4Way certificateIdentity, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard);

		ReplayPhaseKey getPhaseKey() const;

		BundleResourceCertificateIdentity4Way getCertificateIdentity() const;

		SmtBundleMetadata4Way getBundleMetadata() const;

		BoundaryGuardState getBoundaryGuard() const;

		bool isValid() const;

		bool Equals(PhaseCertificateTemplateKey4Way other) const ;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
		bool Equals(std::any obj) const;

		int GetHashCode() const;

		public:
        PhaseCertificateTemplateKey4Way() = default;
	};

	/// <summary>
	/// Reusable certificate template for replay-stable 4-way SMT bundle packing.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct PhaseCertificateTemplate4Way
	class PhaseCertificateTemplate4Way
	{
	private:
		PhaseCertificateTemplateKey4Way TemplateKey{};
		BundleResourceCertificate4Way Certificate{};

	public:
		PhaseCertificateTemplate4Way(PhaseCertificateTemplateKey4Way templateKey, BundleResourceCertificate4Way certificate);

		PhaseCertificateTemplateKey4Way getTemplateKey() const;

		ReplayPhaseKey getPhaseKey() const;

		BundleResourceCertificate4Way getCertificate() const;

		BundleResourceCertificateIdentity4Way getCertificateIdentity() const;

		SmtBundleMetadata4Way getBundleMetadata() const;

		BoundaryGuardState getBoundaryGuard() const;

		bool isValid() const;

		bool Matches(ReplayPhaseContext phase) const;

		bool Matches(PhaseCertificateTemplateKey4Way templateKey) const;

		public:
        PhaseCertificateTemplate4Way() = default;
	};

	/// <summary>
	/// Explicit scheduler-facing cache seam for SMT legality certificate reuse.
	/// Owns replay-template lifecycle so the scheduler no longer treats the raw
	/// <see cref="PhaseCertificateTemplate4Way"/> as its local source of truth.
	/// </summary>
	class SmtLegalityCertificateCache4Way : public ISmtLegalityCertificateCache4Way
	{
	private:
		PhaseCertificateTemplate4Way _template;

	public:
		bool isValid() const override;

		bool Matches(ReplayPhaseContext phase) override;

		LegalityCertificateCacheEvaluation<LegalityDecision> EvaluateLegality(std::shared_ptr<ILegalityChecker> legalityChecker, BundleResourceCertificate4Way bundleCertificate, PhaseCertificateTemplateKey4Way liveTemplateKey, std::shared_ptr<MicroOp> candidate) override;

		void Prepare(PhaseCertificateTemplateKey4Way templateKey, BundleResourceCertificate4Way certificate) override;

		LegalityCertificateCacheTelemetry RefreshAfterMutation(PhaseCertificateTemplateKey4Way templateKey, BundleResourceCertificate4Way certificate) override;

		bool Invalidate() override;
	};

	/// <summary>
	/// Scheduler-side Phase 1 readiness telemetry.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct SchedulerPhaseMetrics
	class SchedulerPhaseMetrics
	{
	private:
		long long ReplayAwareCycles = 0;
		long long PhaseCertificateReadyHits = 0;
		long long PhaseCertificateReadyMisses = 0;
		long long EstimatedChecksSaved = 0;
		long long PhaseCertificateInvalidations = 0;
		long long PhaseCertificateMutationInvalidations = 0;
		long long PhaseCertificatePhaseMismatchInvalidations = 0;
		ReplayPhaseInvalidationReason LastCertificateInvalidationReason = static_cast<ReplayPhaseInvalidationReason>(0);
		long long DeterminismReferenceOpportunitySlots = 0;
		long long DeterminismReplayEligibleSlots = 0;
		long long DeterminismMaskedSlots = 0;
		long long DeterminismEstimatedLostSlots = 0;
		long long DeterminismConstrainedCycles = 0;
		long long DomainIsolationProbeAttempts = 0;
		long long DomainIsolationBlockedAttempts = 0;
		long long DomainIsolationCrossDomainBlocks = 0;
		long long DomainIsolationKernelToUserBlocks = 0;
		long long EligibilityMaskedCycles = 0;
		long long EligibilityMaskedReadyCandidates = 0;
		unsigned char LastEligibilityRequestedMask = 0;
		unsigned char LastEligibilityNormalizedMask = 0;
		unsigned char LastEligibilityReadyPortMask = 0;
		unsigned char LastEligibilityVisibleReadyMask = 0;
		unsigned char LastEligibilityMaskedReadyMask = 0;
		long long ClassTemplateReuseHits = 0;
		long long ClassTemplateInvalidations = 0;
		long long TypedSlotFastPathAccepts = 0;
		long long TypedSlotStandardPathAccepts = 0;
		long long TotalLaneBindings = 0;
		long long LaneReuseHits = 0;
		long long LaneReuseMisses = 0;
		long long AluClassInjects = 0;
		long long LsuClassInjects = 0;
		long long DmaStreamClassInjects = 0;
		long long MatrixTileStreamClassInjects = 0;
		long long BranchControlInjects = 0;
		long long HardPinnedInjects = 0;
		long long ClassFlexibleInjects = 0;
		long long NopAvoided = 0;
		long long NopDueToPinnedConstraint = 0;
		long long NopDueToNoClassCapacity = 0;
		long long NopDueToResourceConflict = 0;
		long long NopDueToDynamicState = 0;
		long long StaticClassOvercommitRejects = 0;
		long long DynamicClassExhaustionRejects = 0;
		long long PinnedLaneConflicts = 0;
		long long LateBindingConflicts = 0;
		long long TypedSlotDomainRejects = 0;
		long long SmtOwnerContextGuardRejects = 0;
		long long SmtDomainGuardRejects = 0;
		long long SmtBoundaryGuardRejects = 0;
		long long SmtSharedResourceCertificateRejects = 0;
		long long SmtRegisterGroupCertificateRejects = 0;
		RejectKind LastSmtLegalityRejectKind = static_cast<RejectKind>(0);
		LegalityAuthoritySource LastSmtLegalityAuthoritySource = static_cast<LegalityAuthoritySource>(0);
		long long SmtLegalityRejectByAluClass = 0;
		long long SmtLegalityRejectByLsuClass = 0;
		long long SmtLegalityRejectByDmaStreamClass = 0;
		long long SmtLegalityRejectByMatrixTileStreamClass = 0;
		long long SmtLegalityRejectByBranchControl = 0;
		long long SmtLegalityRejectBySystemSingleton = 0;
		long long ClassTemplateDomainInvalidations = 0;
		long long ClassTemplateCapacityMismatchInvalidations = 0;
		long long CertificateRejectByAluClass = 0;
		long long CertificateRejectByLsuClass = 0;
		long long CertificateRejectByDmaStreamClass = 0;
		long long CertificateRejectByMatrixTileStreamClass = 0;
		long long CertificateRejectByBranchControl = 0;
		long long CertificateRejectBySystemSingleton = 0;
		long long CertificateRegGroupConflictVT0 = 0;
		long long CertificateRegGroupConflictVT1 = 0;
		long long CertificateRegGroupConflictVT2 = 0;
		long long CertificateRegGroupConflictVT3 = 0;
		long long RejectionsVT0 = 0;
		long long RejectionsVT1 = 0;
		long long RejectionsVT2 = 0;
		long long RejectionsVT3 = 0;
		long long RegGroupConflictsVT0 = 0;
		long long RegGroupConflictsVT1 = 0;
		long long RegGroupConflictsVT2 = 0;
		long long RegGroupConflictsVT3 = 0;
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
		long long TypedSlotHardwareBudgetRejects = 0;
		long long TypedSlotAssistQuotaRejects = 0;
		long long TypedSlotAssistBackpressureRejects = 0;
		long long SerializingBoundaryRejects = 0;
		long long SerializingEpochCount = 0;
		long long AssistNominations = 0;
		long long AssistInjections = 0;
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
		AssistInvalidationReason LastAssistInvalidationReason = static_cast<AssistInvalidationReason>(0);
		unsigned long long LastAssistOwnershipSignature = 0;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private IReadOnlyList<LoopPhaseClassProfile> LoopPhaseProfiles;
		std::vector<std::shared_ptr<LoopPhaseClassProfile>> LoopPhaseProfiles;

	public:
		long long getReplayAwareCycles() const;
		void setReplayAwareCycles(long long value);

		long long getPhaseCertificateReadyHits() const;
		void setPhaseCertificateReadyHits(long long value);

		long long getPhaseCertificateReadyMisses() const;
		void setPhaseCertificateReadyMisses(long long value);

		long long getEstimatedChecksSaved() const;
		void setEstimatedChecksSaved(long long value);

		long long getPhaseCertificateInvalidations() const;
		void setPhaseCertificateInvalidations(long long value);

		long long getPhaseCertificateMutationInvalidations() const;
		void setPhaseCertificateMutationInvalidations(long long value);

		long long getPhaseCertificatePhaseMismatchInvalidations() const;
		void setPhaseCertificatePhaseMismatchInvalidations(long long value);

		ReplayPhaseInvalidationReason getLastCertificateInvalidationReason() const;
		void setLastCertificateInvalidationReason(ReplayPhaseInvalidationReason value);

		long long getDeterminismReferenceOpportunitySlots() const;
		void setDeterminismReferenceOpportunitySlots(long long value);

		long long getDeterminismReplayEligibleSlots() const;
		void setDeterminismReplayEligibleSlots(long long value);

		long long getDeterminismMaskedSlots() const;
		void setDeterminismMaskedSlots(long long value);

		long long getDeterminismEstimatedLostSlots() const;
		void setDeterminismEstimatedLostSlots(long long value);

		long long getDeterminismConstrainedCycles() const;
		void setDeterminismConstrainedCycles(long long value);

		long long getDomainIsolationProbeAttempts() const;
		void setDomainIsolationProbeAttempts(long long value);

		long long getDomainIsolationBlockedAttempts() const;
		void setDomainIsolationBlockedAttempts(long long value);

		long long getDomainIsolationCrossDomainBlocks() const;
		void setDomainIsolationCrossDomainBlocks(long long value);

		long long getDomainIsolationKernelToUserBlocks() const;
		void setDomainIsolationKernelToUserBlocks(long long value);

		long long getEligibilityMaskedCycles() const;
		void setEligibilityMaskedCycles(long long value);

		long long getEligibilityMaskedReadyCandidates() const;
		void setEligibilityMaskedReadyCandidates(long long value);

		unsigned char getLastEligibilityRequestedMask() const;
		void setLastEligibilityRequestedMask(unsigned char value);

		unsigned char getLastEligibilityNormalizedMask() const;
		void setLastEligibilityNormalizedMask(unsigned char value);

		unsigned char getLastEligibilityReadyPortMask() const;
		void setLastEligibilityReadyPortMask(unsigned char value);

		unsigned char getLastEligibilityVisibleReadyMask() const;
		void setLastEligibilityVisibleReadyMask(unsigned char value);

		unsigned char getLastEligibilityMaskedReadyMask() const;
		void setLastEligibilityMaskedReadyMask(unsigned char value);

		// ── Phase 08: Typed-Slot Telemetry ─────────────────────────

		long long getClassTemplateReuseHits() const;
		void setClassTemplateReuseHits(long long value);

		long long getClassTemplateInvalidations() const;
		void setClassTemplateInvalidations(long long value);

		long long getTypedSlotFastPathAccepts() const;
		void setTypedSlotFastPathAccepts(long long value);

		long long getTypedSlotStandardPathAccepts() const;
		void setTypedSlotStandardPathAccepts(long long value);

		long long getTotalLaneBindings() const;
		void setTotalLaneBindings(long long value);

		long long getLaneReuseHits() const;
		void setLaneReuseHits(long long value);

		long long getLaneReuseMisses() const;
		void setLaneReuseMisses(long long value);

		long long getAluClassInjects() const;
		void setAluClassInjects(long long value);

		long long getLsuClassInjects() const;
		void setLsuClassInjects(long long value);

		long long getDmaStreamClassInjects() const;
		void setDmaStreamClassInjects(long long value);
		long long getMatrixTileStreamClassInjects() const;
		void setMatrixTileStreamClassInjects(long long value);

		long long getBranchControlInjects() const;
		void setBranchControlInjects(long long value);

		long long getHardPinnedInjects() const;
		void setHardPinnedInjects(long long value);

		long long getClassFlexibleInjects() const;
		void setClassFlexibleInjects(long long value);

		long long getNopAvoided() const;
		void setNopAvoided(long long value);

		long long getNopDueToPinnedConstraint() const;
		void setNopDueToPinnedConstraint(long long value);

		long long getNopDueToNoClassCapacity() const;
		void setNopDueToNoClassCapacity(long long value);

		long long getNopDueToResourceConflict() const;
		void setNopDueToResourceConflict(long long value);

		long long getNopDueToDynamicState() const;
		void setNopDueToDynamicState(long long value);

		long long getStaticClassOvercommitRejects() const;
		void setStaticClassOvercommitRejects(long long value);

		long long getDynamicClassExhaustionRejects() const;
		void setDynamicClassExhaustionRejects(long long value);

		long long getPinnedLaneConflicts() const;
		void setPinnedLaneConflicts(long long value);

		long long getLateBindingConflicts() const;
		void setLateBindingConflicts(long long value);

		long long getTypedSlotDomainRejects() const;
		void setTypedSlotDomainRejects(long long value);

		long long getSmtOwnerContextGuardRejects() const;
		void setSmtOwnerContextGuardRejects(long long value);

		long long getSmtDomainGuardRejects() const;
		void setSmtDomainGuardRejects(long long value);

		long long getSmtBoundaryGuardRejects() const;
		void setSmtBoundaryGuardRejects(long long value);

		long long getSmtSharedResourceCertificateRejects() const;
		void setSmtSharedResourceCertificateRejects(long long value);

		long long getSmtRegisterGroupCertificateRejects() const;
		void setSmtRegisterGroupCertificateRejects(long long value);

		RejectKind getLastSmtLegalityRejectKind() const;
		void setLastSmtLegalityRejectKind(RejectKind value);

		LegalityAuthoritySource getLastSmtLegalityAuthoritySource() const;
		void setLastSmtLegalityAuthoritySource(LegalityAuthoritySource value);

		long long getSmtLegalityRejectByAluClass() const;
		void setSmtLegalityRejectByAluClass(long long value);

		long long getSmtLegalityRejectByLsuClass() const;
		void setSmtLegalityRejectByLsuClass(long long value);

		long long getSmtLegalityRejectByDmaStreamClass() const;
		void setSmtLegalityRejectByDmaStreamClass(long long value);
		long long getSmtLegalityRejectByMatrixTileStreamClass() const;
		void setSmtLegalityRejectByMatrixTileStreamClass(long long value);

		long long getSmtLegalityRejectByBranchControl() const;
		void setSmtLegalityRejectByBranchControl(long long value);

		long long getSmtLegalityRejectBySystemSingleton() const;
		void setSmtLegalityRejectBySystemSingleton(long long value);

		long long getClassTemplateDomainInvalidations() const;
		void setClassTemplateDomainInvalidations(long long value);

		long long getClassTemplateCapacityMismatchInvalidations() const;
		void setClassTemplateCapacityMismatchInvalidations(long long value);

		long long getCertificateRejectByAluClass() const;
		void setCertificateRejectByAluClass(long long value);

		long long getCertificateRejectByLsuClass() const;
		void setCertificateRejectByLsuClass(long long value);

		long long getCertificateRejectByDmaStreamClass() const;
		void setCertificateRejectByDmaStreamClass(long long value);
		long long getCertificateRejectByMatrixTileStreamClass() const;
		void setCertificateRejectByMatrixTileStreamClass(long long value);

		long long getCertificateRejectByBranchControl() const;
		void setCertificateRejectByBranchControl(long long value);

		long long getCertificateRejectBySystemSingleton() const;
		void setCertificateRejectBySystemSingleton(long long value);

		long long getCertificateRegGroupConflictVT0() const;
		void setCertificateRegGroupConflictVT0(long long value);

		long long getCertificateRegGroupConflictVT1() const;
		void setCertificateRegGroupConflictVT1(long long value);

		long long getCertificateRegGroupConflictVT2() const;
		void setCertificateRegGroupConflictVT2(long long value);

		long long getCertificateRegGroupConflictVT3() const;
		void setCertificateRegGroupConflictVT3(long long value);

		long long getRejectionsVT0() const;
		void setRejectionsVT0(long long value);

		long long getRejectionsVT1() const;
		void setRejectionsVT1(long long value);

		long long getRejectionsVT2() const;
		void setRejectionsVT2(long long value);

		long long getRejectionsVT3() const;
		void setRejectionsVT3(long long value);

		long long getRegGroupConflictsVT0() const;
		void setRegGroupConflictsVT0(long long value);

		long long getRegGroupConflictsVT1() const;
		void setRegGroupConflictsVT1(long long value);

		long long getRegGroupConflictsVT2() const;
		void setRegGroupConflictsVT2(long long value);

		long long getRegGroupConflictsVT3() const;
		void setRegGroupConflictsVT3(long long value);

		long long getBankPendingRejectBank0() const;
		void setBankPendingRejectBank0(long long value);

		long long getBankPendingRejectBank1() const;
		void setBankPendingRejectBank1(long long value);

		long long getBankPendingRejectBank2() const;
		void setBankPendingRejectBank2(long long value);

		long long getBankPendingRejectBank3() const;
		void setBankPendingRejectBank3(long long value);

		long long getBankPendingRejectBank4() const;
		void setBankPendingRejectBank4(long long value);

		long long getBankPendingRejectBank5() const;
		void setBankPendingRejectBank5(long long value);

		long long getBankPendingRejectBank6() const;
		void setBankPendingRejectBank6(long long value);

		long long getBankPendingRejectBank7() const;
		void setBankPendingRejectBank7(long long value);

		long long getBankPendingRejectBank8() const;
		void setBankPendingRejectBank8(long long value);

		long long getBankPendingRejectBank9() const;
		void setBankPendingRejectBank9(long long value);

		long long getBankPendingRejectBank10() const;
		void setBankPendingRejectBank10(long long value);

		long long getBankPendingRejectBank11() const;
		void setBankPendingRejectBank11(long long value);

		long long getBankPendingRejectBank12() const;
		void setBankPendingRejectBank12(long long value);

		long long getBankPendingRejectBank13() const;
		void setBankPendingRejectBank13(long long value);

		long long getBankPendingRejectBank14() const;
		void setBankPendingRejectBank14(long long value);

		long long getBankPendingRejectBank15() const;
		void setBankPendingRejectBank15(long long value);

		long long getMemoryClusteringEvents() const;
		void setMemoryClusteringEvents(long long value);

		long long getTypedSlotHardwareBudgetRejects() const;
		void setTypedSlotHardwareBudgetRejects(long long value);

		long long getTypedSlotAssistQuotaRejects() const;
		void setTypedSlotAssistQuotaRejects(long long value);

		long long getTypedSlotAssistBackpressureRejects() const;
		void setTypedSlotAssistBackpressureRejects(long long value);

		// ── Phase 04: Serialising-Event Epoch Telemetry ─────────────

		/// <summary>
		/// Number of <see cref="PackBundleIntraCoreSmt"/> calls that were short-circuited
		/// because the owner bundle contained a <see cref="Arch.SerializationClass.FullSerial"/>
		/// or <see cref="Arch.SerializationClass.VmxSerial"/> operation (G33).
		/// </summary>
		long long getSerializingBoundaryRejects() const;
		void setSerializingBoundaryRejects(long long value);

		/// <summary>
		/// Number of epoch boundaries created by <see cref="MicroOpScheduler.NotifySerializingCommit"/>
		/// calls (G34). Each call corresponds to one serialising instruction being committed.
		/// </summary>
		long long getSerializingEpochCount() const;
		void setSerializingEpochCount(long long value);

		long long getAssistNominations() const;
		void setAssistNominations(long long value);

		long long getAssistInjections() const;
		void setAssistInjections(long long value);

		long long getAssistRejects() const;
		void setAssistRejects(long long value);

		long long getAssistBoundaryRejects() const;
		void setAssistBoundaryRejects(long long value);

		long long getAssistInvalidations() const;
		void setAssistInvalidations(long long value);

		long long getAssistInterCoreNominations() const;
		void setAssistInterCoreNominations(long long value);

		long long getAssistInterCoreInjections() const;
		void setAssistInterCoreInjections(long long value);

		long long getAssistInterCoreRejects() const;
		void setAssistInterCoreRejects(long long value);

		long long getAssistInterCoreDomainRejects() const;
		void setAssistInterCoreDomainRejects(long long value);

		long long getAssistInterCorePodLocalInjections() const;
		void setAssistInterCorePodLocalInjections(long long value);

		long long getAssistInterCoreCrossPodInjections() const;
		void setAssistInterCoreCrossPodInjections(long long value);

		long long getAssistInterCorePodLocalRejects() const;
		void setAssistInterCorePodLocalRejects(long long value);

		long long getAssistInterCoreCrossPodRejects() const;
		void setAssistInterCoreCrossPodRejects(long long value);

		long long getAssistInterCorePodLocalDomainRejects() const;
		void setAssistInterCorePodLocalDomainRejects(long long value);

		long long getAssistInterCoreCrossPodDomainRejects() const;
		void setAssistInterCoreCrossPodDomainRejects(long long value);

		long long getAssistInterCoreSameVtVectorInjects() const;
		void setAssistInterCoreSameVtVectorInjects(long long value);

		long long getAssistInterCoreDonorVtVectorInjects() const;
		void setAssistInterCoreDonorVtVectorInjects(long long value);

		long long getAssistInterCoreSameVtVectorWritebackInjects() const;
		void setAssistInterCoreSameVtVectorWritebackInjects(long long value);

		long long getAssistInterCoreDonorVtVectorWritebackInjects() const;
		void setAssistInterCoreDonorVtVectorWritebackInjects(long long value);

		long long getAssistInterCoreLane6DefaultStoreDonorPrefetchInjects() const;
		void setAssistInterCoreLane6DefaultStoreDonorPrefetchInjects(long long value);

		long long getAssistInterCoreLane6HotLoadDonorPrefetchInjects() const;
		void setAssistInterCoreLane6HotLoadDonorPrefetchInjects(long long value);

		long long getAssistInterCoreLane6HotStoreDonorPrefetchInjects() const;
		void setAssistInterCoreLane6HotStoreDonorPrefetchInjects(long long value);

		long long getAssistInterCoreLane6DonorPrefetchInjects() const;
		void setAssistInterCoreLane6DonorPrefetchInjects(long long value);

		long long getAssistInterCoreLane6ColdStoreLdsaInjects() const;
		void setAssistInterCoreLane6ColdStoreLdsaInjects(long long value);

		long long getAssistInterCoreLane6LdsaInjects() const;
		void setAssistInterCoreLane6LdsaInjects(long long value);

		long long getAssistQuotaRejects() const;
		void setAssistQuotaRejects(long long value);

		long long getAssistQuotaIssueRejects() const;
		void setAssistQuotaIssueRejects(long long value);

		long long getAssistQuotaLineRejects() const;
		void setAssistQuotaLineRejects(long long value);

		long long getAssistQuotaLinesReserved() const;
		void setAssistQuotaLinesReserved(long long value);

		long long getAssistBackpressureRejects() const;
		void setAssistBackpressureRejects(long long value);

		long long getAssistBackpressureOuterCapRejects() const;
		void setAssistBackpressureOuterCapRejects(long long value);

		long long getAssistBackpressureMshrRejects() const;
		void setAssistBackpressureMshrRejects(long long value);

		long long getAssistBackpressureDmaSrfRejects() const;
		void setAssistBackpressureDmaSrfRejects(long long value);

		long long getAssistDonorPrefetchInjects() const;
		void setAssistDonorPrefetchInjects(long long value);

		long long getAssistLdsaInjects() const;
		void setAssistLdsaInjects(long long value);

		long long getAssistVdsaInjects() const;
		void setAssistVdsaInjects(long long value);

		long long getAssistSameVtInjects() const;
		void setAssistSameVtInjects(long long value);

		long long getAssistDonorVtInjects() const;
		void setAssistDonorVtInjects(long long value);

		AssistInvalidationReason getLastAssistInvalidationReason() const;
		void setLastAssistInvalidationReason(AssistInvalidationReason value);

		unsigned long long getLastAssistOwnershipSignature() const;
		void setLastAssistOwnershipSignature(unsigned long long value);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public IReadOnlyList<LoopPhaseClassProfile>? LoopPhaseProfiles {get;set;}
		const std::vector<std::shared_ptr<LoopPhaseClassProfile>> getLoopPhaseProfiles() const;
		void setLoopPhaseProfiles(const std::vector<std::shared_ptr<LoopPhaseClassProfile>> &value);
	};
}
