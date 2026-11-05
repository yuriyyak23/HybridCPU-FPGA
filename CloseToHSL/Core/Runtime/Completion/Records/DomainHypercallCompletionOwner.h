#pragma once

#include "../../Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "TrapCompletionPublicationFence.h"
#include "YAKSys_Hybrid_CPU.Core.CompletionRecord.h"
#include "../../Events/Hypercalls/DomainHypercallRuntimeExecutor.h"

#include "../../Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "../../Domains/Authority/YAKSys_Hybrid_CPU.Core.RootAuthorityDescriptor.h"
#include "../../Events/Hypercalls/VirtualizationOperandSnapshot.h"
#include "../../Migration/Checkpoint/DomainHypercallLifecycleGate.h"
#include <string>
#include <unordered_set>
#include <vector>
#include <limits>
#include <any>
#include <mutex>
#include <map>
#include <stdexcept>
#include "tangible_exception_helper.h"
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	// Reference-identity seal carried across the opaque E5/E6 ownership boundary.
	struct DomainHypercallIdentitySeal final {};
	using DomainHypercallIdentitySealToken = std::shared_ptr<const DomainHypercallIdentitySeal>;

	enum class DomainHypercallCompletionDecision : unsigned char
	{
		Published = 0,
		MissingExecution = 1,
		RuntimeAdmissionDenied = 2,
		RouteDenied = 3,
		FenceDenied = 4,
		InvalidExecution = 5,
		DuplicatePublication = 6,
		LifecycleGateDenied = 7
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct DomainHypercallCompletionPublicationResult(DomainHypercallCompletionDecision Decision, CompletionRecord Completion, DomainHypercallCompletionOwner.CompletionPublicationToken? E5, string Reason, System.Nullable<DomainHypercallCompletionOwner> Owner = null, System.Nullable<VirtualizationRestoreGenerationOwner> RestoreOwner = null)
//	internal readonly record struct DomainHypercallCompletionPublicationResult(DomainHypercallCompletionDecision Decision, CompletionRecord Completion, DomainHypercallCompletionOwner.CompletionPublicationToken E5, string Reason, DomainHypercallCompletionOwner Owner = nullptr, VirtualizationRestoreGenerationOwner RestoreOwner = nullptr)
//	{
//		internal bool IsPublished
//		{
//			get
//			{
//				return Decision == DomainHypercallCompletionDecision.Published && !Completion.IsEmpty && E5 != nullptr;
//			}
//		}
//		internal bool RetirePublicationAuthorized
//		{
//			get
//			{
//				return false;
//			}
//		}
//	}

	struct DomainHypercallCompletionPublicationResult;
    class DomainHypercallRetireOwner;
    class DomainHypercallCompletionOwner final : public std::enable_shared_from_this<DomainHypercallCompletionOwner>
	{
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
        struct LivePublication final {
            const std::shared_ptr<CompletionRecord> Completion;
            const std::shared_ptr<DomainHypercallLifecycleGate> LifecycleGate;
            LivePublication(std::shared_ptr<CompletionRecord> completion, std::shared_ptr<DomainHypercallLifecycleGate> gate) : Completion(std::move(completion)), LifecycleGate(std::move(gate)) {}
        };

	public:
		class CompletionPublicationToken final : public std::enable_shared_from_this<CompletionPublicationToken>
		{
		private:
			const unsigned long long AttemptId;
			const int VirtualThreadId;
			const unsigned long long DomainTag;
			const std::wstring DecisionId;
			const unsigned long long OwnerId;
			const unsigned long long ExecutionSequence;
			const unsigned long long CompletionSequence;
			const std::wstring E3Digest;
			const std::wstring EffectDigest;
			const std::wstring CompletionDigest;
			const unsigned long long RestoreGeneration;
			const std::wstring TokenDigest;

			const DomainHypercallIdentitySealToken _issuerSeal;

			CompletionPublicationToken(DomainHypercallIdentitySealToken issuerSeal, unsigned long long attemptId, int virtualThreadId, unsigned long long domainTag, const std::wstring &decisionId, unsigned long long ownerId, unsigned long long executionSequence, unsigned long long completionSequence, const std::wstring &e3Digest, const std::wstring &effectDigest, const std::wstring &completionDigest, unsigned long long restoreGeneration, const std::wstring &tokenDigest);

		public:
			unsigned int getSchemaVersion() const;
			unsigned long long getAttemptId() const;
			int getVirtualThreadId() const;
			unsigned long long getDomainTag() const;
			std::wstring getDecisionId() const;
			unsigned long long getOwnerId() const;
			unsigned long long getExecutionSequence() const;
			unsigned long long getCompletionSequence() const;
			std::wstring getE3Digest() const;
			std::wstring getEffectDigest() const;
			std::wstring getCompletionDigest() const;
			EvidenceVisibilityClass getEvidenceClass() const;
			TrapCompletionMigrationClass getMigrationClass() const;
			unsigned long long getRestoreGeneration() const;
			std::wstring getTokenDigest() const;
			bool getRetirePublicationAuthorized() const;
			bool WasIssuedBy(const DomainHypercallIdentitySealToken &issuerSeal) const;

			static std::shared_ptr<CompletionPublicationToken> Create(DomainHypercallIdentitySealToken issuerSeal, unsigned long long sequence, std::shared_ptr<DomainHypercallRuntimeExecutor::ExecutionReceipt> receipt, std::shared_ptr<CompletionRecord> completion);
		};

	private:
		const DomainHypercallIdentitySealToken _consumerSeal = std::make_shared<DomainHypercallIdentitySeal>();
		const DomainHypercallIdentitySealToken _issuerSeal = std::make_shared<DomainHypercallIdentitySeal>();
		std::mutex _sync;
		class LivePublicationTable final {
            std::mutex mutex_;
            std::map<std::weak_ptr<CompletionPublicationToken>, std::shared_ptr<LivePublication>, std::owner_less<std::weak_ptr<CompletionPublicationToken>>> entries_;
        public:
            void Add(const std::shared_ptr<CompletionPublicationToken>& key, std::shared_ptr<LivePublication> value) {
                std::lock_guard<std::mutex> lock(mutex_);
                for (auto it = entries_.begin(); it != entries_.end();) { if (it->first.expired()) it = entries_.erase(it); else ++it; }
                if (!entries_.emplace(key, std::move(value)).second) throw std::invalid_argument("duplicate publication");
            }
            bool TryGetValue(const std::shared_ptr<CompletionPublicationToken>& key, std::shared_ptr<LivePublication>& value) {
                std::lock_guard<std::mutex> lock(mutex_); auto it = entries_.find(key);
                if (it == entries_.end()) { value.reset(); return false; } value = it->second; return true;
            }
            bool Remove(const std::shared_ptr<CompletionPublicationToken>& key) { std::lock_guard<std::mutex> lock(mutex_); return entries_.erase(key) != 0; }
        };
        const std::shared_ptr<LivePublicationTable> _live = std::make_shared<LivePublicationTable>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<CompletionPublicationToken> _liveTokens = new();
		std::unordered_set<std::shared_ptr<CompletionPublicationToken>> _liveTokens = std::unordered_set<std::shared_ptr<CompletionPublicationToken>>();
		unsigned long long _nextCompletionSequence = 1;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private Object? _retireConsumerSeal;
		DomainHypercallIdentitySealToken _retireConsumerSeal;

	public:
		void BindRetireOwner(std::shared_ptr<DomainHypercallRetireOwner> owner);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallCompletionPublicationResult PublishExactProbe(DomainHypercallRuntimeExecutor executor, DomainHypercallRuntimeExecutor.ExecutionReceipt? receipt, VirtualizationRestoreGenerationOwner restoreOwner, DomainRuntimeContext context, RootAuthorityDescriptor root, DomainHypercallLifecycleGate lifecycleGate)
		std::shared_ptr<DomainHypercallCompletionPublicationResult> PublishExactProbe(std::shared_ptr<DomainHypercallRuntimeExecutor> executor, std::shared_ptr<DomainHypercallRuntimeExecutor::ExecutionReceipt> receipt, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<DomainHypercallLifecycleGate> lifecycleGate);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool ValidateLive(CompletionPublicationToken? token, System.Nullable<CompletionRecord> completion, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner)
		bool ValidateLive(std::shared_ptr<CompletionPublicationToken> token, std::shared_ptr<CompletionRecord> completion, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool ConsumeForRetire(CompletionPublicationToken? token, System.Nullable<CompletionRecord> completion, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner, Object retireConsumerSeal)
		bool ConsumeForRetire(std::shared_ptr<CompletionPublicationToken> token, std::shared_ptr<CompletionRecord> completion, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, const DomainHypercallIdentitySealToken &retireConsumerSeal);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool TryBeginE5ToE6Transition(CompletionPublicationToken? token, out System.Nullable<DomainHypercallLifecycleGate.TransitionLease> transition)
		bool TryBeginE5ToE6Transition(std::shared_ptr<CompletionPublicationToken> token, std::shared_ptr<DomainHypercallLifecycleGate::TransitionLease> &transition);

		int CountLiveTokens(unsigned long long domainTag);

		int CancelLiveTokensForDrain(unsigned long long domainTag);

		std::any getConsumerSeal() const;

	private:
		unsigned long long AllocateSequence();

		static std::shared_ptr<DomainHypercallCompletionPublicationResult> Deny(DomainHypercallCompletionDecision decision, const std::wstring &reason);
	};

	struct DomainHypercallCompletionPublicationResult
	{
		DomainHypercallCompletionDecision Decision;
		std::shared_ptr<CompletionRecord> Completion;
		std::shared_ptr<DomainHypercallCompletionOwner::CompletionPublicationToken> E5;
		std::wstring Reason;
		std::shared_ptr<DomainHypercallCompletionOwner> Owner;
		std::shared_ptr<VirtualizationRestoreGenerationOwner> RestoreOwner;
		bool IsPublished = false;
		DomainHypercallCompletionPublicationResult(DomainHypercallCompletionDecision decision, std::shared_ptr<CompletionRecord> completion, std::shared_ptr<DomainHypercallCompletionOwner::CompletionPublicationToken> e5, std::wstring reason, std::shared_ptr<DomainHypercallCompletionOwner> owner = nullptr, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner = nullptr)
			: Decision(decision), Completion(std::move(completion)), E5(std::move(e5)), Reason(std::move(reason)), Owner(std::move(owner)), RestoreOwner(std::move(restoreOwner)), IsPublished(E5 != nullptr && Completion != nullptr) {}
	};

	class DomainHypercallCompletionDigest final
	{
	private:
		static inline const std::vector<unsigned char> TokenEnvelope = {'H', 'C', 'P', 'U', 'E', '5', 'V', '1', 0};
		static inline const std::vector<unsigned char> RecordEnvelope = {'H', 'C', 'P', 'U', 'C', 'V', '1', 0};

	public:
		static std::wstring ComputeRecord(std::shared_ptr<CompletionRecord> record);

		static std::wstring ComputeToken(unsigned long long attemptId, int virtualThreadId, unsigned long long domainTag, const std::wstring &decisionId, unsigned long long ownerId, unsigned long long executionSequence, unsigned long long completionSequence, const std::wstring &e3Digest, const std::wstring &effectDigest, const std::wstring &completionDigest, unsigned long long restoreGeneration);

	private:
		static std::wstring Hash(const std::vector<unsigned char> &stream);

		static void WriteText(std::vector<unsigned char> &stream, const std::wstring &value);

		static void WriteU32(std::vector<unsigned char> &stream, unsigned int value);

		static void WriteU64(std::vector<unsigned char> &stream, unsigned long long value);
	};

}
