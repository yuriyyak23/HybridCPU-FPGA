#pragma once
#include "NeutralArchitecturalCompletionFacts.h"
#include "SystemSealKey.h"
#include <span>

#include "../../Events/Hypercalls/VirtualizationOperandSnapshot.h"
#include "../Observation/DomainCompletionObservationOwner.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <optional>
#include <mutex>
#include <thread>
#include "tangible_exception_helper.h"
#include <memory>
#include <utility>
#include "../../../../../tangible_disposable_interface.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class ArchitecturalCompletionCommitDecision : unsigned char
	{
		Committed = 0,
		DeniedUnregisteredProducer = 1,
		DeniedProducerPolicy = 2,
		DeniedMissingIdentity = 3,
		DeniedDuplicateOrReplay = 4,
		DeniedStaleCanonicalOrder = 5
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct ArchitecturalCompletionCommitResult(ArchitecturalCompletionCommitDecision Decision, ArchitecturalCompletionCommitOwner.ArchitecturalCompletionCommitReceipt? Receipt, string Reason)
//	internal readonly record struct ArchitecturalCompletionCommitResult(ArchitecturalCompletionCommitDecision Decision, ArchitecturalCompletionCommitOwner.ArchitecturalCompletionCommitReceipt Receipt, string Reason)
//	{
//		internal bool IsCommitted
//		{
//			get
//			{
//				return Decision == ArchitecturalCompletionCommitDecision.Committed && Receipt != nullptr;
//			}
//		}
//	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct ArchitecturalCompletionBatchPreparationResult(ArchitecturalCompletionCommitDecision Decision, ArchitecturalCompletionCommitOwner.CanonicalRetireWindowCommitLease? Lease, string Reason)
//	internal readonly record struct ArchitecturalCompletionBatchPreparationResult(ArchitecturalCompletionCommitDecision Decision, ArchitecturalCompletionCommitOwner.CanonicalRetireWindowCommitLease Lease, string Reason)
//	{
//		internal bool IsPrepared
//		{
//			get
//			{
//				return Decision == ArchitecturalCompletionCommitDecision.Committed && Lease != nullptr;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct ArchitecturalCompletionProducerPolicy(string OwnerName, NeutralArchitecturalCompletionClass CompletionClass, bool RequiresReason, bool AllowsQualification, NeutralFaultAddressSemantic AllowedAddressSemantic, NeutralFaultAuxiliarySemantic AllowedAuxiliarySemantic, bool RequiresTranslationProvenance = false, bool RequiresSecondStageProvenance = false);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct ArchitecturalCompletionReceiptBinding(ulong CompletionIdentity, ulong ProducerOwnerIdentity, ulong ProducerOwnerEpoch, ulong DomainId, int ContextId, int VirtualThreadId, ulong AttemptId, ulong EventId, NeutralTranslationProvenance TranslationProvenance, NeutralArchitecturalCompletionClass CompletionClass, string CompletionDigest, ulong CanonicalOrderSequence, ulong CommitSequence, ulong RestoreGeneration);

	/// <summary>
	/// Neutral issuer for evidence that a typed completion crossed the canonical
	/// architectural write-back retire boundary. This owner does not store generic
	/// retire effects and has no compatibility-frontend or projection authority.
	/// </summary>
	struct ArchitecturalCompletionCommitResult;
struct ArchitecturalCompletionBatchPreparationResult;
struct ArchitecturalCompletionProducerPolicy final {
    std::wstring OwnerName; NeutralArchitecturalCompletionClass CompletionClass = NeutralArchitecturalCompletionClass::None;
    bool RequiresReason = false, AllowsQualification = false;
    NeutralFaultAddressSemantic AllowedAddressSemantic = NeutralFaultAddressSemantic::None;
    NeutralFaultAuxiliarySemantic AllowedAuxiliarySemantic = NeutralFaultAuxiliarySemantic::None;
    bool RequiresTranslationProvenance = false, RequiresSecondStageProvenance = false;
};
struct ArchitecturalCompletionReceiptBinding final {
    unsigned long long CompletionIdentity = 0, ProducerOwnerIdentity = 0, ProducerOwnerEpoch = 0, DomainId = 0;
    int ContextId = 0, VirtualThreadId = 0;
    unsigned long long AttemptId = 0, EventId = 0;
    NeutralTranslationProvenance TranslationProvenance;
    NeutralArchitecturalCompletionClass CompletionClass = NeutralArchitecturalCompletionClass::None;
    std::wstring CompletionDigest;
    unsigned long long CanonicalOrderSequence = 0, CommitSequence = 0, RestoreGeneration = 0;
};
class ArchitecturalCompletionCommitOwner final : public std::enable_shared_from_this<ArchitecturalCompletionCommitOwner>
	{
	public:
		class ProducerRegistration;

		class CanonicalRetireWindowCommitLease final : public Disposable, public std::enable_shared_from_this<CanonicalRetireWindowCommitLease>
		{
		private:
			const std::shared_ptr<ProducerRegistration> Producer;
			const std::vector<std::shared_ptr<ArchitecturalCompletionCandidate>> Candidates;

			const std::shared_ptr<ArchitecturalCompletionCommitOwner> _issuer;
			const std::thread::id _owningThread;
			bool _released = false;

		public:
			CanonicalRetireWindowCommitLease(std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer, std::shared_ptr<ProducerRegistration> producer, std::vector<std::shared_ptr<ArchitecturalCompletionCandidate>> &candidates);

			std::shared_ptr<ProducerRegistration> getProducer() const;
			std::vector<std::shared_ptr<ArchitecturalCompletionCandidate>> getCandidates() const;
			bool IsOwnedByCurrentThread() const noexcept;

			std::vector<std::shared_ptr<ArchitecturalCompletionCommitResult>> Complete();

			void dispose() override;
		};

	public:
		class ProducerRegistration final : public std::enable_shared_from_this<ProducerRegistration>
		{
		private:
			const unsigned long long OwnerIdentity;
			const unsigned long long OwnerEpoch;
			const std::shared_ptr<ArchitecturalCompletionProducerPolicy> Policy;

			const std::shared_ptr<ArchitecturalCompletionCommitOwner> _issuer;

		public:
			ProducerRegistration(std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer, unsigned long long ownerIdentity, unsigned long long ownerEpoch, std::shared_ptr<ArchitecturalCompletionProducerPolicy> policy);

			unsigned long long getOwnerIdentity() const;
			unsigned long long getOwnerEpoch() const;
			std::shared_ptr<ArchitecturalCompletionProducerPolicy> getPolicy() const;
			bool IsIssuedBy(std::shared_ptr<ArchitecturalCompletionCommitOwner> owner);
		};

	public:
		class ArchitecturalCompletionCommitReceipt final : public std::enable_shared_from_this<ArchitecturalCompletionCommitReceipt>
		{
		private:
			const std::shared_ptr<ArchitecturalCompletionCommitOwner> Issuer;
			const std::wstring IssuerSeal;
			const std::shared_ptr<ArchitecturalCompletionReceiptBinding> Binding;

		public:
			ArchitecturalCompletionCommitReceipt(std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer, std::shared_ptr<ArchitecturalCompletionReceiptBinding> binding, const std::wstring &issuerSeal);

		private:
			std::shared_ptr<ArchitecturalCompletionCommitOwner> getIssuer() const;
			std::wstring getIssuerSeal() const;
		public:
			std::shared_ptr<ArchitecturalCompletionReceiptBinding> getBinding() const;
			bool IsIssuedBy(std::shared_ptr<ArchitecturalCompletionCommitOwner> owner);

			bool SealEquals(const std::wstring &seal);

		private:
			static bool IsWellFormedSeal(const std::wstring &seal);
		};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
        struct LiveReceipt final {
            const std::shared_ptr<ArchitecturalCompletionCommitReceipt> Receipt;
            const std::wstring Seal;
            LiveReceipt(std::shared_ptr<ArchitecturalCompletionCommitReceipt> receipt, std::wstring seal) : Receipt(std::move(receipt)), Seal(std::move(seal)) {}
        };

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
        struct CompletionIdentityKey final {
            unsigned long long ProducerOwnerIdentity = 0, ProducerOwnerEpoch = 0, DomainId = 0;
            int ContextId = 0, VirtualThreadId = 0;
            unsigned long long AttemptId = 0, EventId = 0, MemoryOperationId = 0;
            bool operator==(const CompletionIdentityKey&) const = default;
        };

	private:
		mutable std::recursive_mutex _gate;
		const std::shared_ptr<VirtualizationRestoreGenerationOwner> _restoreOwner;
		std::shared_ptr<DomainCompletionObservationOwner> _observationOwner;
		std::shared_ptr<DomainCompletionObservationOwner::CommitInstaller> _observationInstaller;
		const std::vector<unsigned char> _sealKey = GenerateSystemSealKey();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, ProducerRegistration> _registrations = new();
		std::unordered_map<unsigned long long, std::shared_ptr<ProducerRegistration>> _registrations = std::unordered_map<unsigned long long, std::shared_ptr<ProducerRegistration>>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, LiveReceipt> _liveReceipts = new();
		std::unordered_map<unsigned long long, std::shared_ptr<LiveReceipt>> _liveReceipts = std::unordered_map<unsigned long long, std::shared_ptr<LiveReceipt>>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<CompletionIdentityKey> _committedCompletionKeys = new();
		std::unordered_set<std::shared_ptr<CompletionIdentityKey>> _committedCompletionKeys = std::unordered_set<std::shared_ptr<CompletionIdentityKey>>();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private CanonicalRetireWindowCommitLease? _activeRetireWindowLease;
		std::shared_ptr<CanonicalRetireWindowCommitLease> _activeRetireWindowLease;
		unsigned long long _nextProducerIdentity = 1;
		unsigned long long _nextCompletionIdentity = 1;
		unsigned long long _nextCommitSequence = 1;
		unsigned long long _nextCanonicalOrderSequence = 1;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ArchitecturalCompletionCommitReceipt? _latestReceipt;
		std::shared_ptr<ArchitecturalCompletionCommitReceipt> _latestReceipt;

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal ArchitecturalCompletionCommitOwner(VirtualizationRestoreGenerationOwner restoreOwner, DomainCompletionObservationOwner observationOwner)
		ArchitecturalCompletionCommitOwner(std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainCompletionObservationOwner> observationOwner);

		std::shared_ptr<DomainCompletionObservationOwner> getObservationOwner() const;

		unsigned long long getCurrentRestoreGeneration() const;

		int getLiveReceiptCount() const;

		std::shared_ptr<ProducerRegistration> RegisterProducer(std::shared_ptr<ArchitecturalCompletionProducerPolicy> policy);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ArchitecturalCompletionCommitResult CommitAtCanonicalRetireBoundary(ProducerRegistration? producer, in ArchitecturalCompletionCandidate candidate)
		std::shared_ptr<ArchitecturalCompletionCommitResult> CommitAtCanonicalRetireBoundary(std::shared_ptr<ProducerRegistration> producer, std::shared_ptr<ArchitecturalCompletionCandidate> candidate);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ArchitecturalCompletionCommitResult CommitAtCanonicalPreciseFaultBoundary(ProducerRegistration? producer, in ArchitecturalCompletionCandidate candidate)
		std::shared_ptr<ArchitecturalCompletionCommitResult> CommitAtCanonicalPreciseFaultBoundary(std::shared_ptr<ProducerRegistration> producer, std::shared_ptr<ArchitecturalCompletionCandidate> candidate);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ArchitecturalCompletionBatchPreparationResult PrepareCanonicalRetireWindow(ProducerRegistration? producer, ReadOnlySpan<ArchitecturalCompletionCandidate> candidates)
		std::shared_ptr<ArchitecturalCompletionBatchPreparationResult> PrepareCanonicalRetireWindow(std::shared_ptr<ProducerRegistration> producer, std::span<const std::shared_ptr<ArchitecturalCompletionCandidate>> candidates);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool ValidateLiveReceipt(ArchitecturalCompletionCommitReceipt? receipt, in ArchitecturalCompletionReceiptBinding expected)
		bool ValidateLiveReceipt(std::shared_ptr<ArchitecturalCompletionCommitReceipt> receipt, std::shared_ptr<ArchitecturalCompletionReceiptBinding> expected);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool TryConsumeReceipt(ArchitecturalCompletionCommitReceipt? receipt, in ArchitecturalCompletionReceiptBinding expected)
		bool TryConsumeReceipt(std::shared_ptr<ArchitecturalCompletionCommitReceipt> receipt, std::shared_ptr<ArchitecturalCompletionReceiptBinding> expected);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool TryGetLatestLiveReceipt(out ArchitecturalCompletionCommitReceipt? receipt)
		bool TryGetLatestLiveReceipt(std::shared_ptr<ArchitecturalCompletionCommitReceipt> &receipt);

		void InvalidateAfterRestore();

		void ClearObservation(std::shared_ptr<CompletionObservationScope> scope);

		void RebindObservation(std::shared_ptr<CompletionObservationScope> previousScope);

		void InvalidateAfterMemoryDomainRebind(std::shared_ptr<CompletionObservationScope> previousScope);

		std::shared_ptr<DomainCompletionObservationOwner> ReplaceObservationOwner();

		std::shared_ptr<DomainCompletionObservationOwner> ReplaceObservationOwnerAfterArchitecturalStateReplacement();

	private:
		std::vector<std::shared_ptr<ArchitecturalCompletionCommitResult>> CompletePreparedRetireWindow(std::shared_ptr<CanonicalRetireWindowCommitLease> lease);

		void AbortPreparedRetireWindow(std::shared_ptr<CanonicalRetireWindowCommitLease> lease);

		void EnsureActiveLeaseOnOwningThread(std::shared_ptr<CanonicalRetireWindowCommitLease> lease);

		void EnsureNoPreparedRetireWindow();

		void EnsureObservationIssuerUnderLock();

		void InvalidateLiveScopeUnderLock(std::shared_ptr<CompletionObservationScope> scope);

		static bool BindingMatchesScope(std::shared_ptr<ArchitecturalCompletionReceiptBinding> binding, std::shared_ptr<CompletionObservationScope> scope);

		static bool KeyMatchesScope(std::shared_ptr<CompletionIdentityKey> key, std::shared_ptr<CompletionObservationScope> scope);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private bool ValidateLiveReceiptUnderLock(ArchitecturalCompletionCommitReceipt? receipt, in ArchitecturalCompletionReceiptBinding expected)
		bool ValidateLiveReceiptUnderLock(std::shared_ptr<ArchitecturalCompletionCommitReceipt> receipt, std::shared_ptr<ArchitecturalCompletionReceiptBinding> expected);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ArchitecturalCompletionCommitResult ValidateCandidateUnderLock(ProducerRegistration? producer, in ArchitecturalCompletionCandidate candidate, System.Nullable<HashSet<CompletionIdentityKey>> pendingKeys)
		std::shared_ptr<ArchitecturalCompletionCommitResult> ValidateCandidateUnderLock(std::shared_ptr<ProducerRegistration> producer, std::shared_ptr<ArchitecturalCompletionCandidate> candidate, std::optional<std::unordered_set<std::shared_ptr<CompletionIdentityKey>>> &pendingKeys);

		std::shared_ptr<ArchitecturalCompletionCommitResult> CommitValidatedCandidateUnderLock(std::shared_ptr<ProducerRegistration> producer, std::shared_ptr<ArchitecturalCompletionCandidate> candidate);

		static std::shared_ptr<CompletionIdentityKey> CreateIdentityKey(std::shared_ptr<ProducerRegistration> producer, std::shared_ptr<ArchitecturalCompletionCandidate> candidate);

		static bool HasCompleteIdentity(std::shared_ptr<ArchitecturalCompletionCandidate> candidate, std::shared_ptr<ArchitecturalCompletionProducerPolicy> policy);

		static bool PolicyAllows(const ArchitecturalCompletionProducerPolicy &policy, const NeutralArchitecturalCompletionFacts &facts);

		static std::wstring ComputeCompletionDigest(const NeutralArchitecturalCompletionFacts &facts);

		std::wstring ComputeSeal(std::shared_ptr<ArchitecturalCompletionReceiptBinding> binding);

		static unsigned long long AllocateNonZero(unsigned long long &next);

		static void AdvanceNonZero(unsigned long long &value);

		static std::shared_ptr<ArchitecturalCompletionCommitResult> Denied(ArchitecturalCompletionCommitDecision decision, const std::wstring &reason);
	};

}

namespace YAKSys_Hybrid_CPU::Core {
struct ArchitecturalCompletionCommitResult final {
    ArchitecturalCompletionCommitDecision Decision;
    std::shared_ptr<ArchitecturalCompletionCommitOwner::ArchitecturalCompletionCommitReceipt> Receipt;
    std::wstring Reason;
    bool isCommitted() const noexcept { return Decision == ArchitecturalCompletionCommitDecision::Committed && Receipt != nullptr; }
};
struct ArchitecturalCompletionBatchPreparationResult final {
    ArchitecturalCompletionCommitDecision Decision;
    std::shared_ptr<ArchitecturalCompletionCommitOwner::CanonicalRetireWindowCommitLease> Lease;
    std::wstring Reason;
    bool isPrepared() const noexcept { return Decision == ArchitecturalCompletionCommitDecision::Committed && Lease != nullptr; }
};
}
