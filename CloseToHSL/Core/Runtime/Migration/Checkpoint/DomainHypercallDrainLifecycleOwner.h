#pragma once

#include "../../Events/Hypercalls/DomainHypercallCanonicalComposition.h"
#include "../../../Pipeline/Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include "../../Events/Hypercalls/DomainHypercallRuntimeExecutor.h"
#include "../../Completion/Records/DomainHypercallCompletionOwner.h"
#include "../../Completion/Records/DomainHypercallRetireOwner.h"
#include "../../Events/Hypercalls/VirtualizationOperandSnapshot.h"
#include "DomainHypercallLifecycleGate.h"
#include <string>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <optional>
#include <mutex>
#include <chrono>
#include "tangible_exception_helper.h"
#include <memory>
#include <cstdint>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class DomainHypercallDrainDecision : unsigned char
	{
		CheckpointReady = 0,
		InFlightAuthority = 1,
		InvalidDomain = 2,
		InvalidCheckpoint = 3,
		AlreadyRestored = 4
	};

	// DomainHypercallLiveAuthorityCounts is defined by DomainHypercallLifecycleGate.h.
	struct DomainHypercallDrainCheckpoint final
	{
		static constexpr unsigned int CurrentSchemaVersion = 1;
		const unsigned long long DomainTag;
		const unsigned long long CheckpointEpoch;
		const unsigned long long RestoreGeneration;
		const std::wstring DecisionId;
		const std::wstring SpecDigest;
		const std::wstring CheckpointDigest;
		const DomainHypercallLiveAuthorityCounts LiveAuthorityCounts{ 0, 0, 0, 0, 0 };
		const bool ContainsE1 = false;
		const bool ContainsOperandSnapshot = false;
		const bool ContainsE2 = false;
		const bool ContainsE3 = false;
		const bool ContainsE5 = false;
		const bool ContainsE6 = false;
		const bool ContainsCapabilityHandle = false;
		const bool ContainsOwnerSeal = false;
		const bool ContainsBackendReceipt = false;
		const bool ContainsCompatibilityProjection = false;
		const bool ContainsHostOwnedEvidence = false;
		const bool ContainsRuntimeAuthority = ContainsE1 || ContainsOperandSnapshot || ContainsE2 || ContainsE3 || ContainsE5 || ContainsE6 || ContainsCapabilityHandle || ContainsOwnerSeal || ContainsBackendReceipt || ContainsCompatibilityProjection || ContainsHostOwnedEvidence;

		DomainHypercallDrainCheckpoint(unsigned long long domainTag, unsigned long long checkpointEpoch, unsigned long long restoreGeneration, std::wstring decisionId, std::wstring specDigest, std::wstring checkpointDigest)
			: DomainTag(domainTag), CheckpointEpoch(checkpointEpoch), RestoreGeneration(restoreGeneration), DecisionId(std::move(decisionId)), SpecDigest(std::move(specDigest)), CheckpointDigest(std::move(checkpointDigest)) {}
		unsigned int getSchemaVersion() const noexcept { return CurrentSchemaVersion; }
	};

	struct DomainHypercallDrainResult final
	{
		const DomainHypercallDrainDecision Decision;
		const DomainHypercallLiveAuthorityCounts Counts;
		const std::shared_ptr<DomainHypercallDrainCheckpoint> Checkpoint;
		const int CancelledAuthorities;
		const std::wstring Reason;
		const bool IsCheckpointReady;

		DomainHypercallDrainResult(DomainHypercallDrainDecision decision, DomainHypercallLiveAuthorityCounts counts, std::shared_ptr<DomainHypercallDrainCheckpoint> checkpoint, int cancelledAuthorities, std::wstring reason)
			: Decision(decision), Counts(counts), Checkpoint(std::move(checkpoint)), CancelledAuthorities(cancelledAuthorities), Reason(std::move(reason)), IsCheckpointReady(Decision == DomainHypercallDrainDecision::CheckpointReady && Checkpoint != nullptr) {}
	};

	struct DomainHypercallArchitecturalTrace final
	{
		const std::wstring OperationId;
		const int VirtualThreadId;
		const unsigned long long DomainTag;
		const bool Retired;
		const bool Faulted;
		const int RegisterWrites;
		const int MemoryWrites;
		const int VmStateWrites;
		const int Redirects;
		const std::wstring Digest;

		DomainHypercallArchitecturalTrace(std::wstring operationId, int virtualThreadId, unsigned long long domainTag, bool retired, bool faulted, int registerWrites, int memoryWrites, int vmStateWrites, int redirects, std::wstring digest)
			: OperationId(std::move(operationId)), VirtualThreadId(virtualThreadId), DomainTag(domainTag), Retired(retired), Faulted(faulted), RegisterWrites(registerWrites), MemoryWrites(memoryWrites), VmStateWrites(vmStateWrites), Redirects(redirects), Digest(std::move(digest)) {}
		static DomainHypercallArchitecturalTrace ExactProbe(int virtualThreadId, unsigned long long domainTag, bool retired, bool faulted);
	};
	/// <summary>
	/// Neutral DrainOnly lifecycle owner for the exact Phase-38 operation. A
	/// checkpoint is possible only after every live authority registry reports zero.
	/// The checkpoint contains policy identity only and never serializes runtime
	/// tokens, receipts, seals, compatibility projections, or host evidence.
	/// </summary>
	class DomainHypercallDrainLifecycleOwner final : public std::enable_shared_from_this<DomainHypercallDrainLifecycleOwner>
	{
	private:
		static inline const std::vector<unsigned char> Envelope = {'H', 'C', 'P', 'U', 'E', '7', 'V', '1', 0};
		std::recursive_mutex _sync;
		const unsigned long long _domainTag;
		const std::shared_ptr<DomainHypercallCanonicalComposition> _composition;
		const std::shared_ptr<SafetyVerifier> _verifier;
		const std::shared_ptr<DomainHypercallRuntimeExecutor> _executor;
		const std::shared_ptr<DomainHypercallCompletionOwner> _completionOwner;
		const std::shared_ptr<DomainHypercallRetireOwner> _retireOwner;
		const std::shared_ptr<VirtualizationRestoreGenerationOwner> _restoreOwner;
		std::shared_ptr<DomainHypercallLifecycleGate> _lifecycleGate;
		unsigned long long _nextCheckpointEpoch = 1;
		std::unordered_set<std::wstring> _restoredCheckpointDigests;

	public:
		DomainHypercallDrainLifecycleOwner(unsigned long long domainTag, std::shared_ptr<DomainHypercallCanonicalComposition> composition, std::shared_ptr<SafetyVerifier> verifier, std::shared_ptr<DomainHypercallRuntimeExecutor> executor, std::shared_ptr<DomainHypercallCompletionOwner> completionOwner, std::shared_ptr<DomainHypercallRetireOwner> retireOwner, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner);

		std::shared_ptr<DomainHypercallLiveAuthorityCounts> ObserveLiveAuthorities();

		std::shared_ptr<DomainHypercallDrainResult> TryCheckpoint();

		std::shared_ptr<DomainHypercallDrainResult> CancelAndCheckpoint(std::optional<std::chrono::milliseconds> transitionTimeout = std::nullopt);

		std::shared_ptr<DomainHypercallDrainResult> Restore(std::optional<std::shared_ptr<DomainHypercallDrainCheckpoint>> &checkpoint);

	private:
		unsigned long long AllocateCheckpointEpoch();

		static std::wstring ComputeCheckpointDigest(unsigned long long domainTag, unsigned long long epoch, unsigned long long restoreGeneration, const std::wstring &decisionId, const std::wstring &specDigest);

		static void WriteString(std::vector<unsigned char> &bytes, const std::wstring &value);

		static void WriteU32(std::vector<unsigned char> &bytes, unsigned int value);

		static void WriteU64(std::vector<unsigned char> &bytes, unsigned long long value);
	};

}
