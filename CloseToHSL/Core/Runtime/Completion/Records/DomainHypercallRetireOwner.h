#pragma once

#include "DomainHypercallCompletionOwner.h"
#include "../../Events/Hypercalls/VirtualizationOperandSnapshot.h"
#include <string>
#include <unordered_set>
#include <vector>
#include <any>
#include <mutex>
#include <map>
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class DomainHypercallRetireDecision : unsigned char
	{
		Issued = 0,
		MissingCompletion = 1,
		NotCanonicalHead = 2,
		InvalidCarrier = 3,
		IdentityMismatch = 4,
		StaleRestoreGeneration = 5,
		DuplicateOrForeignE5 = 6,
		InvalidWindow = 7,
		LifecycleGateDenied = 8
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct DomainHypercallRetireEligibility(VmxMicroOp Carrier, int VirtualThreadId, ulong DomainTag, int SourceSlotId, int WorkingSlotId, ulong WorkingBundleSequence, ulong OperationAttempt, int PhysicalLaneId, byte RetireOrderIndex, ulong RetireWindowIdentity, ulong OrderEpoch, bool IsCanonicalHead, bool IsSquashed, bool HasWinningException);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct DomainHypercallRetireResult(DomainHypercallRetireDecision Decision, DomainHypercallRetireOwner.VirtualizationRetireGrant? E6, string Reason)
//	internal readonly record struct DomainHypercallRetireResult(DomainHypercallRetireDecision Decision, DomainHypercallRetireOwner.VirtualizationRetireGrant E6, string Reason)
//	{
//		internal bool IsIssued
//		{
//			get
//			{
//				return Decision == DomainHypercallRetireDecision.Issued && E6 != nullptr;
//			}
//		}
//	}

	/// <summary>
	/// Per-CPU neutral retire owner for the exact no-state Phase-38 operation. It can
	/// issue E6 only from the already selected canonical WB retire head and consumes
	/// both E5 and E6 exactly once. It is not a compatibility effect or registry.
	/// </summary>
	class VmxMicroOp;
struct DomainHypercallRetireResult;
struct DomainHypercallRetireEligibility final
{
	const std::shared_ptr<VmxMicroOp> Carrier;
	const int VirtualThreadId;
	const unsigned long long DomainTag;
	const int SourceSlotId;
	const int WorkingSlotId;
	const unsigned long long WorkingBundleSequence;
	const unsigned long long OperationAttempt;
	const int PhysicalLaneId;
	const unsigned char RetireOrderIndex;
	const unsigned long long RetireWindowIdentity;
	const unsigned long long OrderEpoch;
	const bool IsCanonicalHead;
	const bool IsSquashed;
	const bool HasWinningException;

	DomainHypercallRetireEligibility(std::shared_ptr<VmxMicroOp> carrier, int virtualThreadId,
		unsigned long long domainTag, int sourceSlotId, int workingSlotId,
		unsigned long long workingBundleSequence, unsigned long long operationAttempt,
		int physicalLaneId, unsigned char retireOrderIndex, unsigned long long retireWindowIdentity,
		unsigned long long orderEpoch, bool isCanonicalHead, bool isSquashed, bool hasWinningException)
		: Carrier(std::move(carrier)), VirtualThreadId(virtualThreadId), DomainTag(domainTag),
		  SourceSlotId(sourceSlotId), WorkingSlotId(workingSlotId),
		  WorkingBundleSequence(workingBundleSequence), OperationAttempt(operationAttempt),
		  PhysicalLaneId(physicalLaneId), RetireOrderIndex(retireOrderIndex),
		  RetireWindowIdentity(retireWindowIdentity), OrderEpoch(orderEpoch),
		  IsCanonicalHead(isCanonicalHead), IsSquashed(isSquashed),
		  HasWinningException(hasWinningException) {}
	bool operator==(const DomainHypercallRetireEligibility &) const = default;
};
class DomainHypercallRetireOwner final : public std::enable_shared_from_this<DomainHypercallRetireOwner>
	{
	public:
		class VirtualizationRetireGrant final : public std::enable_shared_from_this<VirtualizationRetireGrant>
		{
		private:
			const unsigned long long Sequence;
			const unsigned long long AttemptId;
			const int VirtualThreadId;
			const unsigned long long DomainTag;
			const int SourceSlotId;
			const int WorkingSlotId;
			const unsigned long long WorkingBundleSequence;
			const unsigned long long OperationAttempt;
			const int PhysicalLaneId;
			const unsigned long long RetireWindowIdentity;
			const unsigned long long OrderEpoch;
			const unsigned long long RestoreGeneration;
			const std::wstring E5Digest;
			const std::wstring GrantDigest;

			const DomainHypercallIdentitySealToken _issuerSeal;

			VirtualizationRetireGrant(DomainHypercallIdentitySealToken issuerSeal, unsigned long long sequence, std::shared_ptr<DomainHypercallCompletionOwner::CompletionPublicationToken> e5, std::shared_ptr<DomainHypercallRetireEligibility> eligibility, const std::wstring &digest);

		public:
			unsigned int getSchemaVersion() const;
			unsigned long long getSequence() const;
			unsigned long long getAttemptId() const;
			int getVirtualThreadId() const;
			unsigned long long getDomainTag() const;
			int getSourceSlotId() const;
			int getWorkingSlotId() const;
			unsigned long long getWorkingBundleSequence() const;
			unsigned long long getOperationAttempt() const;
			int getPhysicalLaneId() const;
			unsigned long long getRetireWindowIdentity() const;
			unsigned long long getOrderEpoch() const;
			unsigned long long getRestoreGeneration() const;
			std::wstring getE5Digest() const;
			std::wstring getGrantDigest() const;
			bool WasIssuedBy(const DomainHypercallIdentitySealToken &seal) const;

			static std::shared_ptr<VirtualizationRetireGrant> Create(DomainHypercallIdentitySealToken issuerSeal, unsigned long long sequence, std::shared_ptr<DomainHypercallCompletionOwner::CompletionPublicationToken> e5, std::shared_ptr<DomainHypercallRetireEligibility> eligibility, const std::wstring &digest);
		};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:

		struct LiveGrant final
		{
			const std::wstring Digest;
			explicit LiveGrant(std::wstring digest) : Digest(std::move(digest)) {}
		};
	private:
		static inline const std::vector<unsigned char> Envelope = {'H', 'C', 'P', 'U', 'E', '6', 'V', '1', 0};
		const DomainHypercallIdentitySealToken _consumerSeal = std::make_shared<DomainHypercallIdentitySeal>();
		const DomainHypercallIdentitySealToken _issuerSeal = std::make_shared<DomainHypercallIdentitySeal>();
		std::mutex _sync;
		std::map<std::weak_ptr<VirtualizationRetireGrant>, std::shared_ptr<LiveGrant>, std::owner_less<std::weak_ptr<VirtualizationRetireGrant>>> _live;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<VirtualizationRetireGrant> _liveGrants = new();
		std::unordered_set<std::shared_ptr<VirtualizationRetireGrant>> _liveGrants = std::unordered_set<std::shared_ptr<VirtualizationRetireGrant>>();
		unsigned long long _nextSequence = 1;

	public:
		std::any getConsumerSeal() const;

		std::shared_ptr<DomainHypercallRetireResult> Issue(std::shared_ptr<DomainHypercallCompletionOwner> completionOwner, std::shared_ptr<DomainHypercallCompletionPublicationResult> publication, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainHypercallRetireEligibility> eligibility);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool ConsumeAtPreciseRetire(VirtualizationRetireGrant? grant, VirtualizationRestoreGenerationOwner restoreOwner, ulong retireWindowIdentity, ulong orderEpoch)
		bool ConsumeAtPreciseRetire(std::shared_ptr<VirtualizationRetireGrant> grant, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, unsigned long long retireWindowIdentity, unsigned long long orderEpoch);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool ValidateLive(VirtualizationRetireGrant? grant, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner, ulong retireWindowIdentity, ulong orderEpoch)
		bool ValidateLive(std::shared_ptr<VirtualizationRetireGrant> grant, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, unsigned long long retireWindowIdentity, unsigned long long orderEpoch);

		int CountLiveGrants(unsigned long long domainTag);

		int CancelLiveGrantsForDrain(unsigned long long domainTag);

	private:
		unsigned long long AllocateSequence();

		static std::wstring ComputeDigest(unsigned long long sequence, std::shared_ptr<DomainHypercallCompletionOwner::CompletionPublicationToken> e5, std::shared_ptr<DomainHypercallRetireEligibility> eligibility);

		static void WriteU32(std::vector<unsigned char> &stream, unsigned int value);

		static void WriteU64(std::vector<unsigned char> &stream, unsigned long long value);

		static std::shared_ptr<DomainHypercallRetireResult> Deny(DomainHypercallRetireDecision decision, const std::wstring &reason);
	};

}

namespace YAKSys_Hybrid_CPU::Core {
struct DomainHypercallRetireResult final {
    const DomainHypercallRetireDecision Decision;
    const std::shared_ptr<DomainHypercallRetireOwner::VirtualizationRetireGrant> E6;
    const std::wstring Reason;
    DomainHypercallRetireResult(DomainHypercallRetireDecision decision, std::shared_ptr<DomainHypercallRetireOwner::VirtualizationRetireGrant> grant, std::wstring reason) : Decision(decision), E6(std::move(grant)), Reason(std::move(reason)) {}
    bool isIssued() const noexcept { return Decision == DomainHypercallRetireDecision::Issued && E6 != nullptr; }
    bool operator==(const DomainHypercallRetireResult &) const = default;
};
}
