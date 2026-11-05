#include "DomainHypercallDrainLifecycleOwner.h"
#include "../../Events/Hypercalls/Governance/Phase38VirtualizationDecisionSpecV2.h"
#include "../../Governance/Virtualization/VirtualizationDecisionValidatorV2.h"
#include "../../../Pipeline/Certificates/ReplayPhaseSubstrate.h"
#include "../../../Architecture/Common/BinaryIOCompat.h"
#include "../../../Decoder/Sha256Digest.h"
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	DomainHypercallArchitecturalTrace DomainHypercallArchitecturalTrace::ExactProbe(int virtualThreadId, unsigned long long domainTag, bool retired, bool faulted)
	{
		const std::string operation = "PROBE_NO_STATE_V1";
		const std::string canonical = operation + "|" + std::to_string(virtualThreadId) + "|" + std::to_string(domainTag) + "|" + (retired ? "1" : "0") + "|" + (faulted ? "1" : "0") + "|0|0|0|0";
		const auto* canonicalBytes = reinterpret_cast<const unsigned char*>(canonical.data());
		const std::wstring digest = Decoder::Sha256Hex(std::span<const unsigned char>(canonicalBytes, canonical.size()));
		return DomainHypercallArchitecturalTrace(L"PROBE_NO_STATE_V1", virtualThreadId, domainTag, retired, faulted, 0, 0, 0, 0, digest);
	}

	DomainHypercallDrainLifecycleOwner::DomainHypercallDrainLifecycleOwner(unsigned long long domainTag, std::shared_ptr<DomainHypercallCanonicalComposition> composition, std::shared_ptr<SafetyVerifier> verifier, std::shared_ptr<DomainHypercallRuntimeExecutor> executor, std::shared_ptr<DomainHypercallCompletionOwner> completionOwner, std::shared_ptr<DomainHypercallRetireOwner> retireOwner, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner)
		: _domainTag(domainTag), _composition(std::move(composition)), _verifier(std::move(verifier)), _executor(std::move(executor)), _completionOwner(std::move(completionOwner)), _retireOwner(std::move(retireOwner)), _restoreOwner(std::move(restoreOwner))
	{
		if (domainTag == 0)
		{
			throw ArgumentOutOfRangeException(L"domainTag");
		}
		if (!_composition)
		{
			throw ArgumentNullException(L"composition");
		}
		if (!_verifier)
		{
			throw ArgumentNullException(L"verifier");
		}
		if (!_executor)
		{
			throw ArgumentNullException(L"executor");
		}
		if (!_completionOwner)
		{
			throw ArgumentNullException(L"completionOwner");
		}
		if (!_retireOwner)
		{
			throw ArgumentNullException(L"retireOwner");
		}
		if (!_restoreOwner)
		{
			throw ArgumentNullException(L"restoreOwner");
		}

		_lifecycleGate = _composition->getLifecycleGate();
		if (!_lifecycleGate || _lifecycleGate->getDomainTag() != domainTag)
		{
			throw InvalidOperationException(L"Drain lifecycle owner requires the composition's matching per-domain lifecycle gate.");
		}
	}

	std::shared_ptr<DomainHypercallLiveAuthorityCounts> DomainHypercallDrainLifecycleOwner::ObserveLiveAuthorities()
	{
		std::shared_ptr<DomainHypercallLifecycleSnapshot> lifecycle = _lifecycleGate->Observe();
		return std::make_shared<DomainHypercallLiveAuthorityCounts>(
			_verifier->CountLiveVirtualizationE2(_domainTag),
			_executor->CountLiveReceipts(_domainTag),
			_completionOwner->CountLiveTokens(_domainTag),
			_retireOwner->CountLiveGrants(_domainTag),
			lifecycle->TransitionsInFlight);
	}

	std::shared_ptr<DomainHypercallDrainResult> DomainHypercallDrainLifecycleOwner::TryCheckpoint()
	{
		std::scoped_lock<std::recursive_mutex> lock(_sync);
		_composition->BeginDrain();
		std::shared_ptr<DomainHypercallLiveAuthorityCounts> counts = ObserveLiveAuthorities();
		if (!counts->getIsDrained())
		{
			return std::make_shared<DomainHypercallDrainResult>(DomainHypercallDrainDecision::InFlightAuthority, *counts, nullptr, 0, L"DrainOnly checkpoint denied while E2/E3/E5/E6 authority is live.");
		}

		const unsigned long long epoch = AllocateCheckpointEpoch();
		const std::wstring specDigest = Phase38VirtualizationDecisionSpecV2::getInstance()->SpecDigest;
		const std::wstring digest = ComputeCheckpointDigest(_domainTag, epoch, _restoreOwner->getCurrentGeneration(), VirtualizationDecisionValidatorV2::ExpectedDecisionId, specDigest);
		std::shared_ptr<DomainHypercallDrainCheckpoint> checkpoint = std::make_shared<DomainHypercallDrainCheckpoint>(_domainTag, epoch, _restoreOwner->getCurrentGeneration(), VirtualizationDecisionValidatorV2::ExpectedDecisionId, specDigest, digest);
		return std::make_shared<DomainHypercallDrainResult>(DomainHypercallDrainDecision::CheckpointReady, *counts, checkpoint, 0, L"DrainOnly checkpoint contains policy identity and zero live runtime authority.");
	}

	std::shared_ptr<DomainHypercallDrainResult> DomainHypercallDrainLifecycleOwner::CancelAndCheckpoint(std::optional<std::chrono::milliseconds> transitionTimeout)
	{
		std::scoped_lock<std::recursive_mutex> lock(_sync);
		_composition->BeginDrain();
		if (!_lifecycleGate->WaitForTransitionQuiescence(transitionTimeout.value_or(std::chrono::seconds(30))))
		{
			std::shared_ptr<DomainHypercallLiveAuthorityCounts> inFlight = ObserveLiveAuthorities();
			return std::make_shared<DomainHypercallDrainResult>(DomainHypercallDrainDecision::InFlightAuthority, *inFlight, nullptr, 0, L"Drain cancellation timed out waiting for transition-in-flight quiescence.");
		}

		int cancelled = 0;
		cancelled += _verifier->CancelLiveVirtualizationE2ForDrain(_domainTag);
		cancelled += _executor->CancelLiveReceiptsForDrain(_domainTag);
		cancelled += _completionOwner->CancelLiveTokensForDrain(_domainTag);
		cancelled += _retireOwner->CancelLiveGrantsForDrain(_domainTag);

		// The C# Monitor lock is reentrant; recursive_mutex preserves the atomic
		// cancel-and-checkpoint operation when TryCheckpoint reacquires _sync.
		const std::shared_ptr<DomainHypercallDrainResult> result = TryCheckpoint();
		return std::make_shared<DomainHypercallDrainResult>(result->Decision, result->Counts, result->Checkpoint, cancelled, result->Reason);
	}

	std::shared_ptr<DomainHypercallDrainResult> DomainHypercallDrainLifecycleOwner::Restore(std::optional<std::shared_ptr<DomainHypercallDrainCheckpoint>>& checkpoint)
	{
		std::scoped_lock<std::recursive_mutex> lock(_sync);
		std::shared_ptr<DomainHypercallLiveAuthorityCounts> counts = ObserveLiveAuthorities();
		const std::shared_ptr<DomainHypercallDrainCheckpoint> checkpointRecord = checkpoint.value_or(nullptr);
		if (checkpointRecord && _restoredCheckpointDigests.find(checkpointRecord->CheckpointDigest) != _restoredCheckpointDigests.end())
		{
			return std::make_shared<DomainHypercallDrainResult>(DomainHypercallDrainDecision::AlreadyRestored, *counts, nullptr, 0, L"A DrainOnly checkpoint may restore only once.");
		}

		if (!checkpointRecord || checkpointRecord->getSchemaVersion() != DomainHypercallDrainCheckpoint::CurrentSchemaVersion || checkpointRecord->DomainTag != _domainTag || checkpointRecord->CheckpointEpoch == 0 || !counts->getIsDrained() || checkpointRecord->LiveAuthorityCounts.getTotal() != 0 || checkpointRecord->ContainsRuntimeAuthority || checkpointRecord->RestoreGeneration != _restoreOwner->getCurrentGeneration() || checkpointRecord->DecisionId != VirtualizationDecisionValidatorV2::ExpectedDecisionId || checkpointRecord->SpecDigest != Phase38VirtualizationDecisionSpecV2::getInstance()->SpecDigest || checkpointRecord->CheckpointDigest != ComputeCheckpointDigest(checkpointRecord->DomainTag, checkpointRecord->CheckpointEpoch, checkpointRecord->RestoreGeneration, checkpointRecord->DecisionId, checkpointRecord->SpecDigest))
		{
			return std::make_shared<DomainHypercallDrainResult>(DomainHypercallDrainDecision::InvalidCheckpoint, *counts, nullptr, 0, L"Restore denied a stale, foreign, non-drained, or noncanonical checkpoint.");
		}

		_restoredCheckpointDigests.insert(checkpointRecord->CheckpointDigest);
		_verifier->InvalidateVirtualizationAdmissions(ReplayPhaseInvalidationReason::DomainBoundary);
		_restoreOwner->AdvanceAfterRestore();
		if (!_composition->ResumeAfterValidatedRestore(checkpointRecord->SpecDigest))
		{
			return std::make_shared<DomainHypercallDrainResult>(DomainHypercallDrainDecision::InvalidCheckpoint, *counts, nullptr, 0, L"Local accepted D2/O1 reload failed after restore.");
		}
		return std::make_shared<DomainHypercallDrainResult>(DomainHypercallDrainDecision::CheckpointReady, *ObserveLiveAuthorities(), checkpointRecord, 0, L"Restore advanced generation, invalidated pre-restore authority, and reloaded exact local policy.");
	}

	unsigned long long DomainHypercallDrainLifecycleOwner::AllocateCheckpointEpoch()
	{
		const unsigned long long value = _nextCheckpointEpoch++;
		return value == 0 ? _nextCheckpointEpoch++ : value;
	}

	std::wstring DomainHypercallDrainLifecycleOwner::ComputeCheckpointDigest(unsigned long long domainTag, unsigned long long epoch, unsigned long long restoreGeneration, const std::wstring& decisionId, const std::wstring& specDigest)
	{
		std::vector<unsigned char> bytes = Envelope;
		WriteU64(bytes, domainTag);
		WriteU64(bytes, epoch);
		WriteU64(bytes, restoreGeneration);
		WriteString(bytes, decisionId);
		WriteString(bytes, specDigest);
		return Decoder::Sha256Hex(std::span<const unsigned char>(bytes.data(), bytes.size()));
	}

	void DomainHypercallDrainLifecycleOwner::WriteString(std::vector<unsigned char>& bytes, const std::wstring& value)
	{
		const std::string utf8 = System::IO::ToUtf8(value);
		if (utf8.size() > (std::numeric_limits<unsigned int>::max)())
		{
			throw std::length_error("Checkpoint string exceeds the canonical uint32 length field.");
		}
		WriteU32(bytes, static_cast<unsigned int>(utf8.size()));
		bytes.reserve(bytes.size() + utf8.size());
		for (unsigned char byte : utf8)
		{
			bytes.push_back(byte);
		}
	}

	void DomainHypercallDrainLifecycleOwner::WriteU32(std::vector<unsigned char>& bytes, unsigned int value)
	{
		for (unsigned int shift = 0; shift < 32; shift += 8)
		{
			bytes.push_back(static_cast<unsigned char>((value >> shift) & 0xFFU));
		}
	}

	void DomainHypercallDrainLifecycleOwner::WriteU64(std::vector<unsigned char>& bytes, unsigned long long value)
	{
		for (unsigned int shift = 0; shift < 64; shift += 8)
		{
			bytes.push_back(static_cast<unsigned char>((value >> shift) & 0xFFULL));
		}
	}
}