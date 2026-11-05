#pragma once

#include "../Scheduling/SlotLegality/SlotClassCapacity.h"
#include "../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "BundleResourceCertificate.h"
#include <cmath>
#include <any>
#include <memory>




namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Canonical invalidation reasons for replay-phase aware runtime state.
	/// </summary>
	enum class ReplayPhaseInvalidationReason : unsigned char
	{
		None = 0,
		Completed = 1,
		PcMismatch = 2,
		Manual = 3,
		CertificateMutation = 4,
		PhaseMismatch = 5,
		InactivePhase = 6,

		// Phase 07: class-level template invalidation reasons
		DomainBoundary = 7,
		ClassCapacityMismatch = 8,
		ClassTemplateExpired = 9,

		// Phase 04: serialising-event epoch boundary
		SerializingEvent = 10,

		// DmaStreamCompute Task D: overlapping write footprint invalidates replay evidence.
		MemoryFootprintOverlap = 11,

		// DmaStreamCompute Task G: explicit replay evidence envelope mismatches.
		DmaStreamComputeDescriptorMismatch = 12,
		DmaStreamComputeDescriptorPayloadLost = 13,
		DmaStreamComputeCarrierMismatch = 14,
		DmaStreamComputeFootprintMismatch = 15,
		DmaStreamComputeOwnerDomainMismatch = 16,
		DmaStreamComputeCertificateInputMismatch = 17,
		DmaStreamComputeTokenEvidenceMismatch = 18,
		DmaStreamComputeLanePlacementMismatch = 19,
		DmaStreamComputeIncompleteEvidence = 20
	};

	/// <summary>
	/// Class-level replay template: captures per-class free capacity pattern
	/// rather than exact physical slot positions.
	/// More stable across replay iterations where lane drift is expected.
	/// HLS: 5 × 3-bit values = 15 flip-flops.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ClassCapacityTemplate : IEquatable<ClassCapacityTemplate>
	class ClassCapacityTemplate
	{
	private:
		unsigned char AluFree{};
		unsigned char LsuFree{};
		unsigned char DmaStreamFree{};
		unsigned char MatrixTileStreamFree{};
		unsigned char BranchControlFree{};
		unsigned char SystemSingletonFree{};

		/// <summary>Free ALU-class lanes at template capture time.</summary>
	public:
		unsigned char getAluFree() const;

		/// <summary>Free LSU-class lanes at template capture time.</summary>
		unsigned char getLsuFree() const;

		/// <summary>Free DmaStream-class lanes at template capture time.</summary>
		unsigned char getDmaStreamFree() const;

		unsigned char getMatrixTileStreamFree() const;

		/// <summary>Free BranchControl-class lanes at template capture time.</summary>
		unsigned char getBranchControlFree() const;

		/// <summary>Free SystemSingleton-class lanes at template capture time.</summary>
		unsigned char getSystemSingletonFree() const;

		/// <summary>
		/// Construct a class-capacity template from the current capacity state.
		/// Clamps negative free counts to 0 (possible when over-counted).
		/// </summary>
		ClassCapacityTemplate(SlotClassCapacityState state);

		/// <summary>
		/// Check if this template is compatible with current class capacity.
		/// Compatible = for every class, current free ≥ template free.
		/// </summary>
		bool IsCompatibleWith(SlotClassCapacityState current) const;

		/// <summary>
		/// Get the captured free capacity for a specific slot class.
		/// </summary>
		inline int GetCapturedFree(SlotClass slotClass) const;

		bool Equals(ClassCapacityTemplate other) const ;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
		bool Equals(std::any obj) const;

		int GetHashCode() const;

	private:
		inline static unsigned char ClampToByte(int value);

		public:
        ClassCapacityTemplate() = default;
	};

	/// <summary>
	/// Mutable per-class budget for decrement-aware template fast path.
	/// Each successful injection decrements the budget for the consumed class.
	/// When a class budget reaches 0, the fast path is disabled for that class.
	/// HLS: 5 × 3-bit counters = 15 flip-flops.
	/// </summary>
	class TemplateBudget
	{
	private:
		unsigned char _aluRemaining = 0;
		unsigned char _lsuRemaining = 0;
		unsigned char _dmaStreamRemaining = 0;
		unsigned char _matrixTileStreamRemaining = 0;
		unsigned char _branchControlRemaining = 0;
		unsigned char _systemSingletonRemaining = 0;

		/// <summary>
		/// Initialize the budget from a class-capacity template.
		/// </summary>
	public:
		TemplateBudget(ClassCapacityTemplate template_Keyword);

		/// <summary>
		/// Get remaining budget for a slot class.
		/// </summary>
		int GetRemaining(SlotClass slotClass) const;

		/// <summary>
		/// Decrement the budget for a slot class after a successful injection.
		/// </summary>
		void Decrement(SlotClass slotClass);

		TemplateBudget() = default;
	};

	/// <summary>
	/// Stable identity of a replay epoch that can key future certificate reuse.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ReplayPhaseKey : IEquatable<ReplayPhaseKey>
	class ReplayPhaseKey
	{
	private:
		unsigned long long EpochId{};
		unsigned long long CachedPc{};
		unsigned char StableDonorMask{};
		int ValidSlotCount{};
		ClassCapacityTemplate ClassTemplate{};
		int DomainScopeId{};

	public:
		ReplayPhaseKey(unsigned long long epochId, unsigned long long cachedPc, unsigned char stableDonorMask, int validSlotCount);

		/// <summary>
		/// Phase 07: Extended constructor with class-capacity template and domain scope.
		/// </summary>
		ReplayPhaseKey(unsigned long long epochId, unsigned long long cachedPc, unsigned char stableDonorMask, int validSlotCount, ClassCapacityTemplate classTemplate, int domainScopeId);

		unsigned long long getEpochId() const;

		unsigned long long getCachedPc() const;

		unsigned char getStableDonorMask() const;

		int getValidSlotCount() const;

		/// <summary>Phase 07: class-level capacity template for stable replay reuse.</summary>
		ClassCapacityTemplate getClassTemplate() const;

		/// <summary>Phase 07: domain scope identifier for template isolation.</summary>
		int getDomainScopeId() const;

		bool isValid() const;

		bool Equals(ReplayPhaseKey other) const ;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
		bool Equals(std::any obj) const;

		int GetHashCode() const;

		public:
        ReplayPhaseKey() = default;
	};

	/// <summary>
	/// Shared replay/epoch vocabulary for phase-aware scheduling and validation.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ReplayPhaseContext
	class ReplayPhaseContext
	{
	private:
		bool IsActive;
		unsigned long long EpochId;
		unsigned long long CachedPc;
		unsigned long long EpochLength;
		unsigned long long CompletedReplays;
		int ValidSlotCount;
		unsigned char StableDonorMask;
		ReplayPhaseInvalidationReason LastInvalidationReason;

	public:
		ReplayPhaseContext(bool isActive, unsigned long long epochId, unsigned long long cachedPc, unsigned long long epochLength, unsigned long long completedReplays, int validSlotCount, unsigned char stableDonorMask, ReplayPhaseInvalidationReason lastInvalidationReason);

		bool isActive() const;

		unsigned long long getEpochId() const;

		unsigned long long getCachedPc() const;

		unsigned long long getEpochLength() const;

		unsigned long long getCompletedReplays() const;

		int getValidSlotCount() const;

		unsigned char getStableDonorMask() const;

		ReplayPhaseInvalidationReason getLastInvalidationReason() const;

		ReplayPhaseKey getKey() const;

		int getStableDonorSlotCount() const;

		bool getHasStableDonorStructure() const;

		bool getCanReusePhaseCertificate() const;

		bool IsDonorStable(int slotIndex) const;

		bool Matches(ReplayPhaseKey key) const;

	private:
		static int CountBits(unsigned char value);

		ReplayPhaseContext() : IsActive(false), EpochId(0), CachedPc(0), EpochLength(0), CompletedReplays(0), ValidSlotCount(0), StableDonorMask(0), LastInvalidationReason(ReplayPhaseInvalidationReason::None) {}
	};

	/// <summary>
	/// Replay-epoch telemetry baseline for Phase 1 observability.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ReplayPhaseMetrics
	class ReplayPhaseMetrics
	{
	private:
		unsigned long long ReplayEpochCount = 0;
		unsigned long long TotalEpochLength = 0;
		unsigned long long StableDonorSlotSamples = 0;
		unsigned long long TotalReplaySlotSamples = 0;
		unsigned long long DeterministicTransitionCount = 0;

	public:
		unsigned long long getReplayEpochCount() const;
		void setReplayEpochCount(unsigned long long value);

		unsigned long long getTotalEpochLength() const;
		void setTotalEpochLength(unsigned long long value);

		unsigned long long getStableDonorSlotSamples() const;
		void setStableDonorSlotSamples(unsigned long long value);

		unsigned long long getTotalReplaySlotSamples() const;
		void setTotalReplaySlotSamples(unsigned long long value);

		unsigned long long getDeterministicTransitionCount() const;
		void setDeterministicTransitionCount(unsigned long long value);

		double getAverageEpochLength() const;

		double getStableDonorSlotRatio() const;
	};

	/// <summary>
	/// Explicit telemetry delta emitted by legality certificate cache seams.
	/// Scheduler counters consume this transport directly instead of inferring
	/// replay reuse or invalidation semantics from checker decisions.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct LegalityCertificateCacheTelemetry
	class LegalityCertificateCacheTelemetry
	{
	private:
		const long long ReadyHitsDelta;
		const long long ReadyMissesDelta;
		const long long EstimatedChecksSavedDelta;
		const long long InvalidationEventsDelta;
		const long long MutationInvalidationsDelta;
		const long long PhaseMismatchInvalidationsDelta;
		const ReplayPhaseInvalidationReason LastInvalidationReason;

	public:
		LegalityCertificateCacheTelemetry(long long readyHitsDelta, long long readyMissesDelta, long long estimatedChecksSavedDelta, long long invalidationEventsDelta, long long mutationInvalidationsDelta, long long phaseMismatchInvalidationsDelta, ReplayPhaseInvalidationReason lastInvalidationReason);

		long long getReadyHitsDelta() const;

		long long getReadyMissesDelta() const;

		long long getEstimatedChecksSavedDelta() const;

		long long getInvalidationEventsDelta() const;

		long long getMutationInvalidationsDelta() const;

		long long getPhaseMismatchInvalidationsDelta() const;

		ReplayPhaseInvalidationReason getLastInvalidationReason() const;

		static LegalityCertificateCacheTelemetry FromReuseDecision(bool attemptedReplayReuse, bool reusedReplayCertificate, long long estimatedChecksSaved);

		static LegalityCertificateCacheTelemetry FromInvalidationEvent(ReplayPhaseInvalidationReason reason, bool anyCacheInvalidated);

		LegalityCertificateCacheTelemetry();
	};

	/// <summary>
	/// Generic legality result plus explicit cache telemetry delta.
	/// </summary>
	template<typename TResult>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct LegalityCertificateCacheEvaluation<TResult>
	class LegalityCertificateCacheEvaluation
	{
	private:
		const TResult LegalityResult;
		const LegalityCertificateCacheTelemetry Telemetry;

	public:
		LegalityCertificateCacheEvaluation(TResult legalityResult, LegalityCertificateCacheTelemetry telemetry) : LegalityResult(legalityResult), Telemetry(telemetry) {}

		TResult getLegalityResult() const
		{
			return LegalityResult;
		}

		LegalityCertificateCacheTelemetry getTelemetry() const
		{
			return Telemetry;
		}

		LegalityCertificateCacheEvaluation() = default;
	};

	/// <summary>
	/// Explicit replay/template key for inter-core bundle certificates.
	/// Replay epoch and structural certificate identity are both part of cache validity.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct PhaseCertificateTemplateKey : IEquatable<PhaseCertificateTemplateKey>
	class PhaseCertificateTemplateKey
	{
	private:
		ReplayPhaseKey PhaseKey{};
		BundleResourceCertificateIdentity CertificateIdentity{};

	public:
		PhaseCertificateTemplateKey(ReplayPhaseKey phaseKey, BundleResourceCertificateIdentity certificateIdentity);

		ReplayPhaseKey getPhaseKey() const;

		BundleResourceCertificateIdentity getCertificateIdentity() const;

		bool isValid() const;

		bool Equals(PhaseCertificateTemplateKey other) const ;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
		bool Equals(std::any obj) const;

		int GetHashCode() const;

		public:
        PhaseCertificateTemplateKey() = default;
	};

	/// <summary>
	/// Reusable certificate template for replay-stable inter-thread bundles.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct PhaseCertificateTemplate
	class PhaseCertificateTemplate
	{
	private:
		PhaseCertificateTemplateKey TemplateKey{};
		std::shared_ptr<BundleResourceCertificate> Certificate{};

	public:
		PhaseCertificateTemplate(PhaseCertificateTemplateKey templateKey, std::shared_ptr<BundleResourceCertificate> certificate);

		PhaseCertificateTemplateKey getTemplateKey() const;

		ReplayPhaseKey getPhaseKey() const;

		BundleResourceCertificateIdentity getCertificateIdentity() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public BundleResourceCertificate? Certificate {get;}
		std::shared_ptr<BundleResourceCertificate> getCertificate() const;

		bool isValid() const;

		bool Matches(ReplayPhaseContext phase) const;

		bool Matches(PhaseCertificateTemplateKey templateKey) const;

		public:
        PhaseCertificateTemplate() = default;
	};
}
