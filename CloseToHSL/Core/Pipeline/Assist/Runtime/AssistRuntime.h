#pragma once

#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"

#include "../../Metadata/LocalityHint.h"

#include <cmath>
#include <stdexcept>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class AssistMemoryQuotaState; }
namespace YAKSys_Hybrid_CPU::Core { class AssistBackpressureSnapshot; }
namespace YAKSys_Hybrid_CPU::Core { class AssistBackpressureState; }

namespace YAKSys_Hybrid_CPU::Core
{
	class MicroOp;
	class AssistMicroOp;
	/// <summary>
	/// Narrow post-phase-08 assist taxonomy.
	/// All kinds remain architecturally invisible and non-retiring.
	/// </summary>
	enum class AssistKind : unsigned char
	{
		DonorPrefetch = 0,
		Ldsa = 1,
		Vdsa = 2
	};

	/// <summary>
	/// Concrete runtime action selected for an assist micro-op.
	/// Carrier selection remains explicit and orthogonal to the execution mode itself.
	/// </summary>
	enum class AssistExecutionMode : unsigned char
	{
		CachePrefetch = 0,
		StreamRegisterPrefetch = 1
	};

	/// <summary>
	/// Explicit physical carrier selected for one assist micro-op.
	/// Placement selects the carrier; live owner, replay and donor-source
	/// checks independently govern execution. This enum records the chosen carrier.
	/// </summary>
	enum class AssistCarrierKind : unsigned char
	{
		LsuHosted = 0,
		Lane6Dma = 1
	};

	/// <summary>
	/// Explicit donor-source identity carried by an assist micro-op.
	/// Keeps local same-VT reuse distinct from true donor sourcing and
	/// from explicit pod-local inter-core seed transport.
	/// </summary>
	enum class AssistDonorSourceKind : unsigned char
	{
		SameThreadSeed = 0,
		IntraCoreVtDonorVector = 1,
		InterCoreVtDonorVector = 2,
		InterCoreSameVtSeed = 3,
		InterCoreVtDonorSeed = 4,
		InterCoreSameVtVector = 5,
		InterCoreSameVtVectorWriteback = 6,
		InterCoreVtDonorVectorWriteback = 7,
		InterCoreSameVtHotLoadSeed = 8,
		InterCoreVtDonorHotLoadSeed = 9,
		InterCoreSameVtHotStoreSeed = 10,
		InterCoreVtDonorHotStoreSeed = 11,
		InterCoreSameVtColdStoreSeed = 12,
		InterCoreVtDonorColdStoreSeed = 13,
		InterCoreSameVtDefaultStoreSeed = 14,
		InterCoreVtDonorDefaultStoreSeed = 15
	};

	class AssistCarrierKindExtensions final
	{
	public:
		static SlotClass ResolveSlotClass(AssistCarrierKind carrierKind);
	};

	/// <summary>
	/// Freezes the landed assist kind/execution/carrier contour.
	/// This closes the first three tuple axes:
	/// <see cref="AssistKind"/>, <see cref="AssistExecutionMode"/>, and
	/// <see cref="AssistCarrierKind"/>.
	/// This does not replace <see cref="MicroOp.AdmissionMetadata"/> checks or
	/// <see cref="MicroOp.Placement"/> carrier constraints; it only prevents unsupported
	/// runtime tuples from materializing implicitly through direct assist construction.
	/// </summary>
	class AssistTupleSupport final
	{
	public:
		static bool IsSupportedCarrierTuple(AssistKind assistKind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind);
	};

	/// <summary>
	/// Explicit donor-source descriptor for one assist micro-op.
	/// Together with <see cref="AssistTupleSupport"/>, this closes the fourth
	/// donor-source axis of the reviewer-facing assist tuple matrix.
	/// This remains assist-owned runtime metadata and does not replace slot admission authority.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AssistDonorSourceDescriptor
	class AssistDonorSourceDescriptor
	{
	private:
		AssistDonorSourceKind Kind{};
		int DonorVirtualThreadId{};
		int TargetVirtualThreadId{};
		int OwnerContextId{};
		unsigned long long DomainTag{};
		int SourceCoreId{};
		unsigned short SourcePodId{};
		unsigned long long SourceAssistEpochId{};

	public:
		AssistDonorSourceDescriptor(AssistDonorSourceKind kind, int donorVirtualThreadId, int targetVirtualThreadId, int ownerContextId, unsigned long long domainTag, int sourceCoreId = -1, unsigned short sourcePodId = 0, unsigned long long sourceAssistEpochId = 0);

		AssistDonorSourceKind getKind() const;

		int getDonorVirtualThreadId() const;

		int getTargetVirtualThreadId() const;

		int getOwnerContextId() const;

		unsigned long long getDomainTag() const;

		int getSourceCoreId() const;

		unsigned short getSourcePodId() const;

		unsigned long long getSourceAssistEpochId() const;

		bool getHasExplicitCoreSource() const;

		bool isCrossVirtualThread() const;

		/// <summary>
		/// Validate the complete assist four-tuple after the carrier tuple has already
		/// been frozen by <see cref="AssistTupleSupport"/>.
		/// </summary>
		static bool TryCreate(AssistKind assistKind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind, int donorVirtualThreadId, int targetVirtualThreadId, int ownerContextId, unsigned long long domainTag, int sourceCoreId, unsigned short sourcePodId, unsigned long long sourceAssistEpochId, AssistDonorSourceDescriptor &donorSource);

		static bool TryCreate(AssistKind assistKind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind, int donorVirtualThreadId, int targetVirtualThreadId, int ownerContextId, unsigned long long domainTag, int sourceCoreId, unsigned short sourcePodId, unsigned long long sourceAssistEpochId, bool isWriteBackVectorSeed, bool isHotLoadScalarSeed, bool isHotStoreScalarSeed, bool isColdStoreScalarSeed, bool isDefaultStoreScalarSeed, AssistDonorSourceDescriptor &donorSource);

		static bool TryCreate(AssistKind assistKind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind, int donorVirtualThreadId, int targetVirtualThreadId, int ownerContextId, unsigned long long domainTag, AssistDonorSourceDescriptor &donorSource);

		static bool TryCreate(AssistKind assistKind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind, int donorVirtualThreadId, int targetVirtualThreadId, int ownerContextId, unsigned long long domainTag, bool isWriteBackVectorSeed, bool isHotLoadScalarSeed, bool isHotStoreScalarSeed, bool isColdStoreScalarSeed, bool isDefaultStoreScalarSeed, AssistDonorSourceDescriptor &donorSource);

		bool IsLegalFor(AssistKind assistKind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind) const;

		bool MatchesScope(int ownerContextId, unsigned long long domainTag) const;

		bool MatchesCoreSource(int sourceCoreId, unsigned short sourcePodId) const;

		AssistDonorSourceDescriptor() = default;
	};

	/// <summary>
	/// Deterministic assist invalidation reasons.
	/// </summary>
	enum class AssistInvalidationReason : unsigned char
	{
		None = 0,
		Replay = 1,
		Trap = 2,
		Fence = 3,
		VmTransition = 4,
		SerializingBoundary = 5,
		OwnerInvalidation = 6,
		Manual = 7,
		PipelineFlush = 8,
		InterCoreOwnerDrift = 9,
		InterCoreBoundaryDrift = 10
	};

	/// <summary>
	/// Stable replay/trace signature for landed assist ownership tuples.
	/// Keeps widened assist-owner identity explicit without making assist architecturally visible.
	/// </summary>
	class AssistOwnershipFingerprint final
	{
	private:
		static constexpr unsigned long long FnvOffsetBasis = 14695981039346656037ULL;
		static constexpr unsigned long long FnvPrime = 1099511628211ULL;

	public:
		static unsigned long long Compute(std::shared_ptr<AssistMicroOp> assistMicroOp);

	private:
		static unsigned long long Mix(unsigned long long hash, unsigned char value);

		static unsigned long long Mix(unsigned long long hash, unsigned short value);

		static unsigned long long Mix(unsigned long long hash, int value);

		static unsigned long long Mix(unsigned long long hash, unsigned long long value);
	};

	/// <summary>
	/// Explicit assist-only quota reject taxonomy.
	/// </summary>
	enum class AssistQuotaRejectKind : unsigned char
	{
		None = 0,
		IssueCredits = 1,
		LineCredits = 2
	};

	/// <summary>
	/// Explicit cache-side assist partition reject taxonomy.
	/// Keeps cache pressure distinct from scheduler-side issue/line credits.
	/// </summary>
	enum class AssistCacheRejectKind : unsigned char
	{
		None = 0,
		TotalLineBudget = 1,
		CarrierLineBudget = 2,
		NoAssistVictim = 3
	};

	/// <summary>
	/// Explicit assist widened-owner backpressure taxonomy.
	/// Keeps assist pressure distinct from foreground scoreboard/hardware-budget rejects.
	/// </summary>
	enum class AssistBackpressureRejectKind : unsigned char
	{
		None = 0,
		SharedOuterCap = 1,
		OutstandingMemory = 2,
		DmaStreamRegisterFile = 3
	};

	/// <summary>
	/// Explicit assist-only SRF resident/loading partition policy for lane6/DMA carriers.
	/// Assist may reuse existing valid/loading SRF entries, but may not evict foreground-owned registers.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AssistStreamRegisterPartitionPolicy
	class AssistStreamRegisterPartitionPolicy
	{
	private:
		const unsigned char ResidentRegisterBudget;
		const unsigned char LoadingRegisterBudget;
		static const AssistStreamRegisterPartitionPolicy Default;

	public:
		AssistStreamRegisterPartitionPolicy(unsigned char residentRegisterBudget, unsigned char loadingRegisterBudget);

		unsigned char getResidentRegisterBudget() const;

		unsigned char getLoadingRegisterBudget() const;

		static AssistStreamRegisterPartitionPolicy getDefault();

		AssistStreamRegisterPartitionPolicy() : ResidentRegisterBudget(0), LoadingRegisterBudget(0) {}
	};

	/// <summary>
	/// Explicit assist-only SRF reject taxonomy.
	/// </summary>
	enum class AssistStreamRegisterRejectKind : unsigned char
	{
		None = 0,
		ResidentBudget = 1,
		LoadingBudget = 2,
		NoAssistVictim = 3
	};

	/// <summary>
	/// Explicit VT / domain / epoch ownership for one assist micro-op.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AssistOwnerBinding
	class AssistOwnerBinding
	{
	private:
		const int CarrierVirtualThreadId;
		const int DonorVirtualThreadId;
		const int TargetVirtualThreadId;
		const int OwnerContextId;
		const unsigned long long DomainTag;
		const unsigned long long ReplayEpochId;
		const unsigned long long AssistEpochId;
		const YAKSys_Hybrid_CPU::Core::LocalityHint LocalityHint;
		const AssistDonorSourceDescriptor DonorSource;
		const int CarrierCoreId;
		const int TargetCoreId;
		const unsigned short PodId;

	public:
		AssistOwnerBinding(int carrierVirtualThreadId, int donorVirtualThreadId, int targetVirtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned long long replayEpochId, unsigned long long assistEpochId, YAKSys_Hybrid_CPU::Core::LocalityHint localityHint, AssistDonorSourceDescriptor donorSource, int carrierCoreId = -1, int targetCoreId = -1, unsigned short podId = 0);

		AssistOwnerBinding(int carrierVirtualThreadId, int donorVirtualThreadId, int targetVirtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned long long replayEpochId, unsigned long long assistEpochId, YAKSys_Hybrid_CPU::Core::LocalityHint localityHint);

		int getCarrierVirtualThreadId() const;

		int getDonorVirtualThreadId() const;

		int getTargetVirtualThreadId() const;

		int getOwnerContextId() const;

		unsigned long long getDomainTag() const;

		unsigned long long getReplayEpochId() const;

		unsigned long long getAssistEpochId() const;

		YAKSys_Hybrid_CPU::Core::LocalityHint getLocalityHint() const;

		AssistDonorSourceDescriptor getDonorSource() const;

		int getCarrierCoreId() const;

		int getTargetCoreId() const;

		unsigned short getPodId() const;

		bool getHasExplicitCoreOwnership() const;

		AssistOwnerBinding() = default;
	};

	/// <summary>
	/// Explicit pod-local inter-core assist transport.
	/// Carries a real production seed plus donor-core identity without tunneling
	/// through the normal inter-core slot-steal nomination ports.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AssistInterCoreTransport
	class AssistInterCoreTransport
	{
	private:
		std::shared_ptr<MicroOp> Seed{};
		int DonorCoreId{};
		unsigned short DonorPodId{};
		unsigned long long DonorAssistEpochId{};
		int DonorVirtualThreadId{};
		int DonorOwnerContextId{};
		unsigned long long DonorDomainTag{};

	public:
		AssistInterCoreTransport(std::shared_ptr<MicroOp> seed, int donorCoreId, unsigned short donorPodId, unsigned long long donorAssistEpochId);

		std::shared_ptr<MicroOp> getSeed() const;

		int getDonorCoreId() const;

		unsigned short getDonorPodId() const;

		unsigned long long getDonorAssistEpochId() const;

		int getDonorVirtualThreadId() const;

		int getDonorOwnerContextId() const;

		unsigned long long getDonorDomainTag() const;

		bool isSupportedCurrentClassSet() const;

		bool isValid() const;

		AssistInterCoreTransport() = default;
	};

	/// <summary>
	/// Dedicated assist-only memory quota authority.
	/// Foreground hardware issue budgets remain separate.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AssistMemoryQuota
	class AssistMemoryQuota
	{
	private:
		const unsigned char IssueCredits;
		const unsigned short LineCredits;
		const unsigned char HotLineCap;
		const unsigned char ColdLineCap;
		static const AssistMemoryQuota Default;

	public:
		AssistMemoryQuota(unsigned char issueCredits, unsigned short lineCredits, unsigned char hotLineCap, unsigned char coldLineCap);

		unsigned char getIssueCredits() const;

		unsigned short getLineCredits() const;

		unsigned char getHotLineCap() const;

		unsigned char getColdLineCap() const;

		unsigned char ResolveLineCap(LocalityHint localityHint) const;

		AssistMemoryQuotaState CreateState() const;

		static AssistMemoryQuota getDefault();

		AssistMemoryQuota() : IssueCredits(0), LineCredits(0), HotLineCap(0), ColdLineCap(0) {}
	};

	/// <summary>
	/// Dedicated widened-owner assist backpressure authority.
	/// Scheduler samples shared read-side pressure but reports assist pressure through an explicit owner.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AssistBackpressurePolicy
	class AssistBackpressurePolicy
	{
	private:
		const unsigned char SharedOuterCapCredits;
		const unsigned char LsuCarrierCredits;
		const unsigned char DmaCarrierCredits;
		const AssistStreamRegisterPartitionPolicy DmaSrfPartitionPolicy;
		static const AssistBackpressurePolicy Default;

	public:
		AssistBackpressurePolicy(unsigned char sharedOuterCapCredits, unsigned char lsuCarrierCredits, unsigned char dmaCarrierCredits, AssistStreamRegisterPartitionPolicy dmaSrfPartitionPolicy);

		unsigned char getSharedOuterCapCredits() const;

		unsigned char getLsuCarrierCredits() const;

		unsigned char getDmaCarrierCredits() const;

		AssistStreamRegisterPartitionPolicy getDmaSrfPartitionPolicy() const;

		unsigned char ResolveCarrierCredits(AssistCarrierKind carrierKind) const;

		AssistBackpressureState CreateState(AssistBackpressureSnapshot snapshot) const;

		static AssistBackpressurePolicy getDefault();

		AssistBackpressurePolicy() : SharedOuterCapCredits(0), LsuCarrierCredits(0), DmaCarrierCredits(0), DmaSrfPartitionPolicy() {}
	};

	/// <summary>
	/// Deterministic pack-pass snapshot used by assist backpressure reservation.
	/// Captures only live shared read-side pressure and projected outstanding-memory truth.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AssistBackpressureSnapshot
	class AssistBackpressureSnapshot
	{
	private:
		const unsigned char SharedOuterCapCredits;
		const unsigned int ConsumedSharedReadBudgetByBank;
		const unsigned short SharedReadBudgetAtLeastOneMask;
		const unsigned short SharedReadBudgetAtLeastTwoMask;
		const unsigned char ProjectedOutstandingCountVt0;
		const unsigned char ProjectedOutstandingCountVt1;
		const unsigned char ProjectedOutstandingCountVt2;
		const unsigned char ProjectedOutstandingCountVt3;
		const unsigned char ProjectedOutstandingCapacityVt0;
		const unsigned char ProjectedOutstandingCapacityVt1;
		const unsigned char ProjectedOutstandingCapacityVt2;
		const unsigned char ProjectedOutstandingCapacityVt3;

	public:
		AssistBackpressureSnapshot(unsigned char sharedOuterCapCredits, unsigned int consumedSharedReadBudgetByBank, unsigned short sharedReadBudgetAtLeastOneMask, unsigned short sharedReadBudgetAtLeastTwoMask, unsigned char projectedOutstandingCountVt0, unsigned char projectedOutstandingCountVt1, unsigned char projectedOutstandingCountVt2, unsigned char projectedOutstandingCountVt3, unsigned char projectedOutstandingCapacityVt0, unsigned char projectedOutstandingCapacityVt1, unsigned char projectedOutstandingCapacityVt2, unsigned char projectedOutstandingCapacityVt3);

		unsigned char getSharedOuterCapCredits() const;

		unsigned int getConsumedSharedReadBudgetByBank() const;

		unsigned short getSharedReadBudgetAtLeastOneMask() const;

		unsigned short getSharedReadBudgetAtLeastTwoMask() const;

		unsigned char getProjectedOutstandingCountVt0() const;

		unsigned char getProjectedOutstandingCountVt1() const;

		unsigned char getProjectedOutstandingCountVt2() const;

		unsigned char getProjectedOutstandingCountVt3() const;

		unsigned char getProjectedOutstandingCapacityVt0() const;

		unsigned char getProjectedOutstandingCapacityVt1() const;

		unsigned char getProjectedOutstandingCapacityVt2() const;

		unsigned char getProjectedOutstandingCapacityVt3() const;

		AssistBackpressureSnapshot() = default;
	};

	/// <summary>
	/// Dedicated assist-only L1 data cache partition policy.
	/// Assist may use only a small bounded resident window and may evict only other assist-owned lines.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AssistCachePartitionPolicy
	class AssistCachePartitionPolicy
	{
	private:
		const unsigned char TotalLineBudget;
		const unsigned char LsuHostedLineBudget;
		const unsigned char DmaHostedLineBudget;
		static const AssistCachePartitionPolicy Default;

	public:
		AssistCachePartitionPolicy(unsigned char totalLineBudget, unsigned char lsuHostedLineBudget, unsigned char dmaHostedLineBudget);

		unsigned char getTotalLineBudget() const;

		unsigned char getLsuHostedLineBudget() const;

		unsigned char getDmaHostedLineBudget() const;

		unsigned char ResolveCarrierLineBudget(AssistCarrierKind carrierKind) const;

		static AssistCachePartitionPolicy getDefault();

		AssistCachePartitionPolicy() : TotalLineBudget(0), LsuHostedLineBudget(0), DmaHostedLineBudget(0) {}
	};

	/// <summary>
	/// Mutable per-bundle assist quota state used by scheduler-side assist injection.
	/// </summary>
	class AssistMemoryQuotaState
	{
	private:
		const AssistMemoryQuota _quota;
		unsigned char _remainingIssueCredits = 0;
		unsigned short _remainingLineCredits = 0;

	public:
		AssistMemoryQuotaState(AssistMemoryQuota quota);

		unsigned char getRemainingIssueCredits() const;

		unsigned short getRemainingLineCredits() const;

		bool TryReserve(std::shared_ptr<AssistMicroOp> assistMicroOp, unsigned int &reservedLineDemand, AssistQuotaRejectKind &rejectKind);

		AssistMemoryQuotaState() : _quota(YAKSys_Hybrid_CPU::Core::AssistMemoryQuota())
		{
		}
	};

	/// <summary>
	/// Mutable per-bundle assist widened-owner backpressure state.
	/// This state mirrors shared read-side pressure without borrowing foreground reject taxonomy.
	/// </summary>
	class AssistBackpressureState
	{
	private:
		const AssistBackpressurePolicy _policy;
		const unsigned short _sharedReadBudgetAtLeastOneMask;
		const unsigned short _sharedReadBudgetAtLeastTwoMask;
		const unsigned char _projectedOutstandingCapacityVt0;
		const unsigned char _projectedOutstandingCapacityVt1;
		const unsigned char _projectedOutstandingCapacityVt2;
		const unsigned char _projectedOutstandingCapacityVt3;
		unsigned char _remainingSharedOuterCapCredits = 0;
		unsigned char _remainingLsuCarrierCredits = 0;
		unsigned char _remainingDmaCarrierCredits = 0;
		unsigned char _projectedOutstandingCountVt0 = 0;
		unsigned char _projectedOutstandingCountVt1 = 0;
		unsigned char _projectedOutstandingCountVt2 = 0;
		unsigned char _projectedOutstandingCountVt3 = 0;
		unsigned int _consumedSharedReadBudgetByBank = 0;

	public:
		AssistBackpressureState(AssistBackpressurePolicy policy, AssistBackpressureSnapshot snapshot);

		AssistStreamRegisterPartitionPolicy getDmaSrfPartitionPolicy() const;

		bool TryReserve(std::shared_ptr<AssistMicroOp> assistMicroOp, bool dmaSrfAvailable, AssistBackpressureRejectKind &rejectKind);

	private:
		bool HasProjectedOutstandingCapacity(int virtualThreadId) const;

		bool HasSharedOuterCap(int memoryBankId) const;

		bool HasCarrierCredits(AssistCarrierKind carrierKind) const;

		void ConsumeSharedOuterCap(int memoryBankId);

		void ConsumeCarrierCredits(AssistCarrierKind carrierKind);

		void IncrementProjectedOutstandingCount(int virtualThreadId);

		unsigned char GetSharedReadBudgetForBank(int bankId) const;

		unsigned char GetConsumedSharedReadBudget(int bankId) const;

		static unsigned int IncrementPackedConsumedHardwareBudget(unsigned int consumedPackedBudgetByBank, int bankId);

		AssistBackpressureState() : _policy(YAKSys_Hybrid_CPU::Core::AssistBackpressurePolicy()), _sharedReadBudgetAtLeastOneMask(0), _sharedReadBudgetAtLeastTwoMask(0), _projectedOutstandingCapacityVt0(0), _projectedOutstandingCapacityVt1(0), _projectedOutstandingCapacityVt2(0), _projectedOutstandingCapacityVt3(0)
		{
		}
	};
inline const AssistStreamRegisterPartitionPolicy AssistStreamRegisterPartitionPolicy::Default = AssistStreamRegisterPartitionPolicy(2, 1);
inline const AssistMemoryQuota AssistMemoryQuota::Default = AssistMemoryQuota(1, 4, 2, 4);
inline const AssistBackpressurePolicy AssistBackpressurePolicy::Default = AssistBackpressurePolicy(1, 1, 1, AssistStreamRegisterPartitionPolicy::getDefault());
inline const AssistCachePartitionPolicy AssistCachePartitionPolicy::Default = AssistCachePartitionPolicy(8, 6, 2);
}
