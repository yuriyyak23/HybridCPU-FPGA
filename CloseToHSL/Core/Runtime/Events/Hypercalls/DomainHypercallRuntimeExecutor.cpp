
#include "DomainHypercallRuntimeExecutor.h"
#include "../../Completion/Records/DomainHypercallCompletionOwner.h"
#include "../../../Decoder/Sha256Digest.h"
#include <Windows.h>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include "../../../Pipeline/Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include "../../Governance/Virtualization/VirtualizationDecisionValidatorV2.h"

namespace YAKSys_Hybrid_CPU::Core
{
    const std::wstring DomainHypercallExecutionReceiptDigest::NoEffectDigest = HashConstant(L"HybridCPU.PROBE_NO_STATE_V1.NO_EFFECT.v1");
    const std::wstring DomainHypercallExecutionReceiptDigest::NoResultDigest = HashConstant(L"HybridCPU.PROBE_NO_STATE_V1.NO_RESULT.v1");

	DomainHypercallRuntimeExecutor::ExecutionReceipt::ExecutionReceipt(std::any issuerSeal, unsigned long long executionSequence, const std::wstring &e2Digest, unsigned long long attemptId, int virtualThreadId, unsigned long long domainTag, const std::wstring &decisionId, unsigned long long ownerId, unsigned int ownerPolicyVersion, unsigned int ownerEpoch, const std::wstring &operationNamespace, const std::wstring &operationId, unsigned short numericLeaf, unsigned long long restoreGeneration, VirtualizationDecisionEffectClassV2 effectClass, VirtualizationDecisionResultAbiV2 resultAbi, const std::wstring &effectDigest, const std::wstring &resultDigest, const std::wstring &receiptDigest)
		: ExecutionSequence(executionSequence), E2Digest(e2Digest), AttemptId(attemptId), VirtualThreadId(virtualThreadId), DomainTag(domainTag),
		  DecisionId(decisionId), OwnerId(ownerId), OwnerPolicyVersion(ownerPolicyVersion), OwnerEpoch(ownerEpoch),
		  OperationNamespace(operationNamespace), OperationId(operationId), NumericLeaf(numericLeaf), RestoreGeneration(restoreGeneration),
		  EffectClass(effectClass), ResultAbi(resultAbi), EffectDigest(effectDigest), ResultDigest(resultDigest), ReceiptDigest(receiptDigest),
		  _issuerSeal(std::move(issuerSeal))
	{
		if (!_issuerSeal.has_value())
			throw std::invalid_argument("issuerSeal");
	}

	unsigned int DomainHypercallRuntimeExecutor::ExecutionReceipt::getSchemaVersion() const
	{
		return 2;
	}

	unsigned long long DomainHypercallRuntimeExecutor::ExecutionReceipt::getExecutionSequence() const
	{
		return ExecutionSequence;
	}

	std::wstring DomainHypercallRuntimeExecutor::ExecutionReceipt::getE2Digest() const
	{
		return E2Digest;
	}

	unsigned long long DomainHypercallRuntimeExecutor::ExecutionReceipt::getAttemptId() const
	{
		return AttemptId;
	}

	int DomainHypercallRuntimeExecutor::ExecutionReceipt::getVirtualThreadId() const
	{
		return VirtualThreadId;
	}

	unsigned long long DomainHypercallRuntimeExecutor::ExecutionReceipt::getDomainTag() const
	{
		return DomainTag;
	}

	std::wstring DomainHypercallRuntimeExecutor::ExecutionReceipt::getDecisionId() const
	{
		return DecisionId;
	}

	unsigned long long DomainHypercallRuntimeExecutor::ExecutionReceipt::getOwnerId() const
	{
		return OwnerId;
	}

	unsigned int DomainHypercallRuntimeExecutor::ExecutionReceipt::getOwnerPolicyVersion() const
	{
		return OwnerPolicyVersion;
	}

	unsigned int DomainHypercallRuntimeExecutor::ExecutionReceipt::getOwnerEpoch() const
	{
		return OwnerEpoch;
	}

	std::wstring DomainHypercallRuntimeExecutor::ExecutionReceipt::getOperationNamespace() const
	{
		return OperationNamespace;
	}

	std::wstring DomainHypercallRuntimeExecutor::ExecutionReceipt::getOperationId() const
	{
		return OperationId;
	}

	unsigned short DomainHypercallRuntimeExecutor::ExecutionReceipt::getNumericLeaf() const
	{
		return NumericLeaf;
	}

	unsigned long long DomainHypercallRuntimeExecutor::ExecutionReceipt::getRestoreGeneration() const
	{
		return RestoreGeneration;
	}

	VirtualizationDecisionEffectClassV2 DomainHypercallRuntimeExecutor::ExecutionReceipt::getEffectClass() const
	{
		return EffectClass;
	}

	VirtualizationDecisionResultAbiV2 DomainHypercallRuntimeExecutor::ExecutionReceipt::getResultAbi() const
	{
		return ResultAbi;
	}

	std::wstring DomainHypercallRuntimeExecutor::ExecutionReceipt::getEffectDigest() const
	{
		return EffectDigest;
	}

	std::wstring DomainHypercallRuntimeExecutor::ExecutionReceipt::getResultDigest() const
	{
		return ResultDigest;
	}

	std::wstring DomainHypercallRuntimeExecutor::ExecutionReceipt::getReceiptDigest() const
	{
		return ReceiptDigest;
	}

	bool DomainHypercallRuntimeExecutor::ExecutionReceipt::getHasPayload() const
	{
		return false;
	}

	bool DomainHypercallRuntimeExecutor::ExecutionReceipt::getHasStateEffect() const
	{
		return false;
	}

	bool DomainHypercallRuntimeExecutor::ExecutionReceipt::getCompletionPublicationAuthorized() const
	{
		return false;
	}

	bool DomainHypercallRuntimeExecutor::ExecutionReceipt::getRetirePublicationAuthorized() const
	{
		return false;
	}

	bool DomainHypercallRuntimeExecutor::ExecutionReceipt::WasIssuedBy(std::any issuerSeal)
	{
		const auto *stored = std::any_cast<DomainHypercallExecutorSealToken>(&_issuerSeal);
		const auto *candidate = std::any_cast<DomainHypercallExecutorSealToken>(&issuerSeal);
		return stored && candidate && *stored && *stored == *candidate;
	}

	std::shared_ptr<DomainHypercallRuntimeExecutor::ExecutionReceipt> DomainHypercallRuntimeExecutor::ExecutionReceipt::Create(std::any issuerSeal, unsigned long long executionSequence, std::shared_ptr<VirtualizationOperationAdmissionCertificate> e2)
	{
		std::wstring digest = DomainHypercallExecutionReceiptDigest::Compute(executionSequence, e2->getCertificateDigest(), e2->getAttemptId(), e2->getVirtualThreadId(), e2->getDomainTag(), e2->getDecisionId(), e2->getOwnerId(), e2->getOwnerPolicyVersion(), e2->getOwnerEpoch(), e2->getOperationNamespace(), e2->getOperationId(), e2->getNumericLeaf(), e2->getRestoreGeneration(), VirtualizationDecisionEffectClassV2::NoStateNoPayload, VirtualizationDecisionResultAbiV2::NoPayload, DomainHypercallExecutionReceiptDigest::getNoEffectDigest(), DomainHypercallExecutionReceiptDigest::getNoResultDigest());
		return std::shared_ptr<DomainHypercallRuntimeExecutor::ExecutionReceipt>(new DomainHypercallRuntimeExecutor::ExecutionReceipt(issuerSeal, executionSequence, e2->getCertificateDigest(), e2->getAttemptId(), e2->getVirtualThreadId(), e2->getDomainTag(), e2->getDecisionId(), e2->getOwnerId(), e2->getOwnerPolicyVersion(), e2->getOwnerEpoch(), e2->getOperationNamespace(), e2->getOperationId(), e2->getNumericLeaf(), e2->getRestoreGeneration(), VirtualizationDecisionEffectClassV2::NoStateNoPayload, VirtualizationDecisionResultAbiV2::NoPayload, DomainHypercallExecutionReceiptDigest::getNoEffectDigest(), DomainHypercallExecutionReceiptDigest::getNoResultDigest(), digest));
	}

	DomainHypercallRuntimeExecutor::LiveReceipt::LiveReceipt(std::shared_ptr<VirtualizationOperationAdmissionCertificate> e2, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainHypercallLifecycleGate> lifecycleGate)
		: E2(std::move(e2)), RestoreOwner(std::move(restoreOwner)), LifecycleGate(std::move(lifecycleGate))
	{
	}

	std::shared_ptr<VirtualizationOperationAdmissionCertificate> DomainHypercallRuntimeExecutor::LiveReceipt::getE2() const
	{
		return E2;
	}

	std::shared_ptr<VirtualizationRestoreGenerationOwner> DomainHypercallRuntimeExecutor::LiveReceipt::getRestoreOwner() const
	{
		return RestoreOwner;
	}

	std::shared_ptr<DomainHypercallLifecycleGate> DomainHypercallRuntimeExecutor::LiveReceipt::getLifecycleGate() const
	{
		return LifecycleGate;
	}

	bool DomainHypercallRuntimeExecutor::LiveReceipt::getConsumedByCompletionOwner() const
	{
		return ConsumedByCompletionOwner;
	}

	void DomainHypercallRuntimeExecutor::LiveReceipt::setConsumedByCompletionOwner(bool value)
	{
		ConsumedByCompletionOwner = value;
	}

	DomainHypercallRuntimeExecutor::DomainHypercallRuntimeExecutor(ExactProbeExecutionMode mode) : _mode(mode)
	{
	}

	ExactProbeExecutionMode DomainHypercallRuntimeExecutor::getMode() const
	{
		return _mode;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallExecutionResult ExecuteExactProbe(SafetyVerifier verifier, SafetyVerifier.VirtualizationOperationAdmissionCertificate? e2, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner, System.Nullable<DomainHypercallLifecycleGate> lifecycleGate)
	std::shared_ptr<DomainHypercallExecutionResult> DomainHypercallRuntimeExecutor::ExecuteExactProbe(std::shared_ptr<SafetyVerifier> verifier, std::shared_ptr<SafetyVerifier::VirtualizationOperationAdmissionCertificate> e2, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainHypercallLifecycleGate> lifecycleGate)
	{
		ArgumentNullException::ThrowIfNull(verifier);
		if (_mode != ExactProbeExecutionMode::ExactProbeOnly)
		{
			return Deny(DomainHypercallExecutionDecision::Disabled, L"Exact probe executor kill switch is disabled.");
		}
		if (e2 == nullptr || restoreOwner == nullptr || lifecycleGate == nullptr)
		{
			return Deny(DomainHypercallExecutionDecision::MissingAdmission, L"Execution requires live E2, restore owner, and lifecycle gate.");
		}
		if (!IsExactBinding(e2))
		{
			return Deny(DomainHypercallExecutionDecision::OperationBindingMismatch, L"E2 does not bind the exact accepted probe operation.");
		}
		std::shared_ptr<DomainHypercallLifecycleGate::TransitionLease> transition;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!lifecycleGate.TryBeginTransition(e2.DomainTag, DomainHypercallTransitionKind.E2ToE3, out DomainHypercallLifecycleGate.TransitionLease? transition) || transition == null)
		if (!lifecycleGate->TryBeginTransition(e2->getDomainTag(), DomainHypercallTransitionKind::E2ToE3, transition) || transition == nullptr)
		{
			return Deny(DomainHypercallExecutionDecision::LifecycleGateDenied, L"E2-to-E3 handoff denied by the disabled or draining lifecycle gate.");
		}

// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using (transition)
		{
			{
				std::scoped_lock<std::mutex> lock(_executionSync);
				std::shared_ptr<VirtualizationE2Result> validation = verifier->ValidateVirtualizationE2(e2, restoreOwner);
				if (!validation || !validation->IsLive())
				{
					return Deny(DomainHypercallExecutionDecision::InvalidAdmission,
						validation ? validation->Reason : L"Virtualization E2 validation returned no result.");
				}

				unsigned long long sequence = AllocateExecutionSequence();
				std::shared_ptr<ExecutionReceipt> receipt = ExecutionReceipt::Create(_issuerSeal, sequence, e2);
				std::shared_ptr<VirtualizationE2ConsumptionResult> consumption = verifier->ConsumeVirtualizationE2FromExactExecutor(e2, restoreOwner, lifecycleGate, ExactConsumerSeal);
				if (!consumption || !consumption->IsConsumed())
				{
					return Deny(DomainHypercallExecutionDecision::InvalidAdmission,
						consumption ? consumption->Reason : L"Virtualization E2 consumption returned no result.");
				}

	#if defined(TESTING)
				lifecycleGate->NotifyTransitionGapForTesting(DomainHypercallTransitionKind::E2ToE3);
	#endif
				for (auto it = _liveReceipts.begin(); it != _liveReceipts.end();) { if (it->first.expired()) it = _liveReceipts.erase(it); else ++it; }
                _liveReceipts.emplace(receipt, std::make_shared<LiveReceipt>(e2, restoreOwner, lifecycleGate));
				_unconsumedReceipts.insert(receipt);
				return std::make_shared<DomainHypercallExecutionResult>(DomainHypercallExecutionDecision::Executed, receipt, L"Exact no-state/no-payload probe executed once; receipt grants no publication authority.");
			}
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallReceiptValidationResult ValidateReceipt(ExecutionReceipt? receipt, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner)
	std::shared_ptr<DomainHypercallReceiptValidationResult> DomainHypercallRuntimeExecutor::ValidateReceipt(std::shared_ptr<ExecutionReceipt> receipt, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner)
	{
		if (receipt == nullptr || restoreOwner == nullptr)
		{
			return Invalid(DomainHypercallReceiptValidationDecision::Missing, L"E3 receipt or restore owner is missing.");
		}
		if (!receipt->WasIssuedBy(_issuerSeal))
		{
			return Invalid(DomainHypercallReceiptValidationDecision::ForeignIssuer, L"E3 issuer mismatch.");
		}
		std::shared_ptr<LiveReceipt> live;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
// ORIGINAL LINE: if (!_liveReceipts.TryGetValue(receipt, out LiveReceipt? live) || !ReferenceEquals(live.RestoreOwner, restoreOwner) || !string.Equals(live.E2.CertificateDigest, receipt.E2Digest, StringComparison.Ordinal) || live.ConsumedByCompletionOwner)
		if (((_liveReceipts.find(receipt) == _liveReceipts.end()) || !(live = _liveReceipts.find(receipt)->second)) || live->getRestoreOwner() != restoreOwner || live->getE2()->getCertificateDigest() != receipt->getE2Digest() || live->getConsumedByCompletionOwner())
		{
			return Invalid(DomainHypercallReceiptValidationDecision::NotLive, L"E3 is not live in this executor registry.");
		}
		if (restoreOwner->getCurrentGeneration() == 0 || receipt->getRestoreGeneration() != restoreOwner->getCurrentGeneration())
		{
			return Invalid(DomainHypercallReceiptValidationDecision::RestoreGenerationMismatch, L"E3 was invalidated by restore generation change.");
		}

		std::wstring digest = DomainHypercallExecutionReceiptDigest::Compute(receipt->getExecutionSequence(), receipt->getE2Digest(), receipt->getAttemptId(), receipt->getVirtualThreadId(), receipt->getDomainTag(), receipt->getDecisionId(), receipt->getOwnerId(), receipt->getOwnerPolicyVersion(), receipt->getOwnerEpoch(), receipt->getOperationNamespace(), receipt->getOperationId(), receipt->getNumericLeaf(), receipt->getRestoreGeneration(), receipt->getEffectClass(), receipt->getResultAbi(), receipt->getEffectDigest(), receipt->getResultDigest());
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		if (receipt->getReceiptDigest() != digest || receipt->getEffectDigest() != DomainHypercallExecutionReceiptDigest::getNoEffectDigest() || receipt->getResultDigest() != DomainHypercallExecutionReceiptDigest::getNoResultDigest())
		{
			return Invalid(DomainHypercallReceiptValidationDecision::DigestMismatch, L"E3 canonical digest mismatch.");
		}

		return std::make_shared<DomainHypercallReceiptValidationResult>(DomainHypercallReceiptValidationDecision::Valid, L"E3 is a live exact execution receipt only.");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallReceiptValidationResult ConsumeReceiptForCompletion(ExecutionReceipt? receipt, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner, DomainHypercallLifecycleGate lifecycleGate, Object consumerSeal)
	std::shared_ptr<DomainHypercallReceiptValidationResult> DomainHypercallRuntimeExecutor::ConsumeReceiptForCompletion(std::shared_ptr<ExecutionReceipt> receipt, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainHypercallLifecycleGate> lifecycleGate, std::any consumerSeal)
	{
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
		const auto *providedSeal = std::any_cast<DomainHypercallIdentitySealToken>(&consumerSeal);
		const auto *boundSeal = std::any_cast<DomainHypercallIdentitySealToken>(&_completionConsumerSeal);
		if (!providedSeal || !boundSeal || !*providedSeal || *providedSeal != *boundSeal)
		{
			return Invalid(DomainHypercallReceiptValidationDecision::ForeignIssuer, L"Only the neutral completion owner may consume E3.");
		}

		{
			std::scoped_lock<std::mutex> lock(_executionSync);
			std::shared_ptr<DomainHypercallReceiptValidationResult> validation = ValidateReceipt(receipt, restoreOwner);
			if (!validation->isValid() || receipt == nullptr)
			{
				return validation;
			}
			std::shared_ptr<LiveReceipt> live = _liveReceipts.at(receipt);
			if (live->getConsumedByCompletionOwner() || live->getLifecycleGate() != lifecycleGate)
			{
				return Invalid(DomainHypercallReceiptValidationDecision::NotLive, L"E3 was already consumed by the completion owner.");
			}
			live->setConsumedByCompletionOwner(true);
			_unconsumedReceipts.erase(receipt);
			return std::make_shared<DomainHypercallReceiptValidationResult>(DomainHypercallReceiptValidationDecision::Valid, L"Neutral completion owner consumed E3 exactly once.");
		}
	}

	void DomainHypercallRuntimeExecutor::BindCompletionOwner(std::shared_ptr<DomainHypercallCompletionOwner> owner)
	{
		ArgumentNullException::ThrowIfNull(owner);
		{
			std::scoped_lock<std::mutex> lock(_executionSync);
			if (_completionConsumerSeal.has_value())
			{
				throw InvalidOperationException(L"Exact executor already has a neutral completion owner binding.");
			}
			_completionConsumerSeal = owner->getConsumerSeal();
		}
	}

	int DomainHypercallRuntimeExecutor::CountLiveReceipts(unsigned long long domainTag)
	{
		{
			std::scoped_lock<std::mutex> lock(_executionSync);
			return static_cast<int>(std::count_if(_unconsumedReceipts.begin(), _unconsumedReceipts.end(), [domainTag](const auto &receipt)
			{
				return receipt && receipt->getDomainTag() == domainTag;
			}));
		}
	}

	int DomainHypercallRuntimeExecutor::CancelLiveReceiptsForDrain(unsigned long long domainTag)
	{
		{
			std::scoped_lock<std::mutex> lock(_executionSync);
			std::vector<std::shared_ptr<ExecutionReceipt>> cancelled;
			for (const auto &receipt : _unconsumedReceipts)
			{
				if (receipt && receipt->getDomainTag() == domainTag)
					cancelled.push_back(receipt);
			}
			for (auto receipt : cancelled)
			{
				std::shared_ptr<LiveReceipt> live;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (_liveReceipts.TryGetValue(receipt, out LiveReceipt? live))
				if (((_liveReceipts.find(receipt) != _liveReceipts.end()) && (live = _liveReceipts.find(receipt)->second)))
				{
					live->setConsumedByCompletionOwner(true);
				}
				_unconsumedReceipts.erase(receipt);
			}
			return static_cast<int>(cancelled.size());
		}
	}

	bool DomainHypercallRuntimeExecutor::IsExactConsumerSeal(std::any consumerSeal)
	{
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
		const auto *candidate = std::any_cast<DomainHypercallExecutorSealToken>(&consumerSeal);
		return candidate && *candidate && *candidate == ExactConsumerSeal;
	}

	bool DomainHypercallRuntimeExecutor::IsExactBinding(std::shared_ptr<SafetyVerifier::VirtualizationOperationAdmissionCertificate> e2)
	{
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		return e2 && e2->getAttemptId() != 0 && e2->getRestoreGeneration() != 0 && e2->getOwnerId() == VirtualizationDecisionValidatorV2::ExpectedOwnerId && e2->getOwnerPolicyVersion() == 1 && e2->getOwnerEpoch() == 1 && e2->getDecisionId() == VirtualizationDecisionValidatorV2::ExpectedDecisionId && e2->getOperationNamespace() == VirtualizationDecisionValidatorV2::ExpectedOperationNamespace && e2->getOperationId() == VirtualizationDecisionValidatorV2::ExpectedOperationId && e2->getNumericLeaf() == 1;
	}

	unsigned long long DomainHypercallRuntimeExecutor::AllocateExecutionSequence()
	{
		unsigned long long sequence = _nextExecutionSequence++;
		if (sequence == 0)
		{
			sequence = _nextExecutionSequence++;
		}
		return sequence;
	}

	std::shared_ptr<DomainHypercallExecutionResult> DomainHypercallRuntimeExecutor::Deny(DomainHypercallExecutionDecision decision, const std::wstring &reason)
	{
		return std::make_shared<DomainHypercallExecutionResult>(decision, nullptr, reason);
	}

	std::shared_ptr<DomainHypercallReceiptValidationResult> DomainHypercallRuntimeExecutor::Invalid(DomainHypercallReceiptValidationDecision decision, const std::wstring &reason)
	{
		return std::make_shared<DomainHypercallReceiptValidationResult>(decision, reason);
	}

	std::wstring DomainHypercallExecutionReceiptDigest::getNoEffectDigest()
	{
		return NoEffectDigest;
	}

	std::wstring DomainHypercallExecutionReceiptDigest::getNoResultDigest()
	{
		return NoResultDigest;
	}

	std::wstring DomainHypercallExecutionReceiptDigest::Compute(unsigned long long sequence, const std::wstring &e2Digest, unsigned long long attemptId, int virtualThreadId, unsigned long long domainTag, const std::wstring &decisionId, unsigned long long ownerId, unsigned int ownerPolicyVersion, unsigned int ownerEpoch, const std::wstring &operationNamespace, const std::wstring &operationId, unsigned short numericLeaf, unsigned long long restoreGeneration, VirtualizationDecisionEffectClassV2 effectClass, VirtualizationDecisionResultAbiV2 resultAbi, const std::wstring &effectDigest, const std::wstring &resultDigest)
	{
        if (virtualThreadId < 0) throw std::overflow_error("virtualThreadId");
        std::vector<unsigned char> stream = Envelope;
        auto appendHex = [&](const std::wstring& hex) {
            if (hex.size() % 2) throw std::invalid_argument("Invalid hexadecimal digest");
            auto digit = [](wchar_t c) -> unsigned char {
                if (c >= L'0' && c <= L'9') return static_cast<unsigned char>(c - L'0');
                if (c >= L'a' && c <= L'f') return static_cast<unsigned char>(c - L'a' + 10);
                if (c >= L'A' && c <= L'F') return static_cast<unsigned char>(c - L'A' + 10);
                throw std::invalid_argument("Invalid hexadecimal digest");
            };
            for (size_t i = 0; i < hex.size(); i += 2) stream.push_back(static_cast<unsigned char>((digit(hex[i]) << 4) | digit(hex[i + 1])));
        };
        WriteU64(stream, sequence); appendHex(e2Digest); WriteU64(stream, attemptId);
        WriteU32(stream, static_cast<unsigned int>(virtualThreadId)); WriteU64(stream, domainTag);
        WriteText(stream, decisionId); WriteU64(stream, ownerId); WriteU32(stream, ownerPolicyVersion);
        WriteU32(stream, ownerEpoch); WriteText(stream, operationNamespace); WriteText(stream, operationId);
        WriteU16(stream, numericLeaf); WriteU64(stream, restoreGeneration);
        stream.push_back(static_cast<unsigned char>(effectClass)); stream.push_back(static_cast<unsigned char>(resultAbi));
        appendHex(effectDigest); appendHex(resultDigest);
        return YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(stream);
    }
    std::wstring DomainHypercallExecutionReceiptDigest::HashConstant(const std::wstring &value)
    {
        std::vector<unsigned char> bytes;
        for (wchar_t c : value) bytes.push_back(c <= 127 ? static_cast<unsigned char>(c) : static_cast<unsigned char>('?'));
        return YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(bytes);
    }
    void DomainHypercallExecutionReceiptDigest::WriteText(std::vector<unsigned char> &stream, const std::wstring &value)
    {
        if (value.size() > static_cast<size_t>((std::numeric_limits<int>::max)())) throw std::overflow_error("text length");
        int size = value.empty() ? 0 : WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
        if (!value.empty() && size == 0) throw std::invalid_argument("UTF-8 conversion failed");
        WriteU32(stream, static_cast<unsigned int>(size));
        auto start = stream.size(); stream.resize(start + size);
        if (size && !WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), reinterpret_cast<char*>(stream.data() + start), size, nullptr, nullptr)) throw std::invalid_argument("UTF-8 conversion failed");
    }
    void DomainHypercallExecutionReceiptDigest::WriteU16(std::vector<unsigned char> &stream, unsigned short value)
    { for (int shift = 8; shift >= 0; shift -= 8) stream.push_back(static_cast<unsigned char>(value >> shift)); }
    void DomainHypercallExecutionReceiptDigest::WriteU32(std::vector<unsigned char> &stream, unsigned int value)
    { for (int shift = 24; shift >= 0; shift -= 8) stream.push_back(static_cast<unsigned char>(value >> shift)); }
    void DomainHypercallExecutionReceiptDigest::WriteU64(std::vector<unsigned char> &stream, unsigned long long value)
    { for (int shift = 56; shift >= 0; shift -= 8) stream.push_back(static_cast<unsigned char>(value >> shift)); }
}
