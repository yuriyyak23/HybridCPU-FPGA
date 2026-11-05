
#include "DomainHypercallCompletionOwner.h"
#include "DomainHypercallRetireOwner.h"
#include "../../../Decoder/Sha256Digest.h"
#include <Windows.h>
#include <stdexcept>
#include <limits>
#include "../../Services/YAKSys_Hybrid_CPU.Core.RuntimeBoundaryAdmissionService.h"
#include "../../Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeOperation.h"
#include "../../Capabilities/Governance/YAKSys_Hybrid_CPU.Core.RuntimeCapabilityIds.h"
#include "../../Capabilities/Grants/CapabilityGrant.h"
#include "../../Domains/Services/DomainRuntimeOperation.h"
#include "../../Events/Traps/NeutralTrapResult.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../Routing/YAKSys_Hybrid_CPU.Core.TrapCompletionRouteDescriptor.h"
#include "../Routing/TrapCompletionRoutePolicy.h"
#include "CompletionRecord.h"
#include <algorithm>

namespace YAKSys_Hybrid_CPU::Core
{

	DomainHypercallCompletionOwner::CompletionPublicationToken::CompletionPublicationToken(DomainHypercallIdentitySealToken issuerSeal, unsigned long long attemptId, int virtualThreadId, unsigned long long domainTag, const std::wstring &decisionId, unsigned long long ownerId, unsigned long long executionSequence, unsigned long long completionSequence, const std::wstring &e3Digest, const std::wstring &effectDigest, const std::wstring &completionDigest, unsigned long long restoreGeneration, const std::wstring &tokenDigest)
		: AttemptId(attemptId), VirtualThreadId(virtualThreadId), DomainTag(domainTag), DecisionId(decisionId), OwnerId(ownerId), ExecutionSequence(executionSequence), CompletionSequence(completionSequence), E3Digest(e3Digest), EffectDigest(effectDigest), CompletionDigest(completionDigest), RestoreGeneration(restoreGeneration), TokenDigest(tokenDigest), _issuerSeal(std::move(issuerSeal))
	{
		ArgumentNullException::ThrowIfNull(_issuerSeal);
	}

	unsigned int DomainHypercallCompletionOwner::CompletionPublicationToken::getSchemaVersion() const
	{
		return 1;
	}

	unsigned long long DomainHypercallCompletionOwner::CompletionPublicationToken::getAttemptId() const
	{
		return AttemptId;
	}

	int DomainHypercallCompletionOwner::CompletionPublicationToken::getVirtualThreadId() const
	{
		return VirtualThreadId;
	}

	unsigned long long DomainHypercallCompletionOwner::CompletionPublicationToken::getDomainTag() const
	{
		return DomainTag;
	}

	std::wstring DomainHypercallCompletionOwner::CompletionPublicationToken::getDecisionId() const
	{
		return DecisionId;
	}

	unsigned long long DomainHypercallCompletionOwner::CompletionPublicationToken::getOwnerId() const
	{
		return OwnerId;
	}

	unsigned long long DomainHypercallCompletionOwner::CompletionPublicationToken::getExecutionSequence() const
	{
		return ExecutionSequence;
	}

	unsigned long long DomainHypercallCompletionOwner::CompletionPublicationToken::getCompletionSequence() const
	{
		return CompletionSequence;
	}

	std::wstring DomainHypercallCompletionOwner::CompletionPublicationToken::getE3Digest() const
	{
		return E3Digest;
	}

	std::wstring DomainHypercallCompletionOwner::CompletionPublicationToken::getEffectDigest() const
	{
		return EffectDigest;
	}

	std::wstring DomainHypercallCompletionOwner::CompletionPublicationToken::getCompletionDigest() const
	{
		return CompletionDigest;
	}

	EvidenceVisibilityClass DomainHypercallCompletionOwner::CompletionPublicationToken::getEvidenceClass() const
	{
		return EvidenceVisibilityClass::HostOwnedRuntimeEvidence;
	}

	TrapCompletionMigrationClass DomainHypercallCompletionOwner::CompletionPublicationToken::getMigrationClass() const
	{
		return TrapCompletionMigrationClass::HostOwnedNonMigratable;
	}

	unsigned long long DomainHypercallCompletionOwner::CompletionPublicationToken::getRestoreGeneration() const
	{
		return RestoreGeneration;
	}

	std::wstring DomainHypercallCompletionOwner::CompletionPublicationToken::getTokenDigest() const
	{
		return TokenDigest;
	}

	bool DomainHypercallCompletionOwner::CompletionPublicationToken::getRetirePublicationAuthorized() const
	{
		return false;
	}

	bool DomainHypercallCompletionOwner::CompletionPublicationToken::WasIssuedBy(const DomainHypercallIdentitySealToken &issuerSeal) const
	{
		return _issuerSeal != nullptr && _issuerSeal == issuerSeal;
	}

	std::shared_ptr<DomainHypercallCompletionOwner::CompletionPublicationToken> DomainHypercallCompletionOwner::CompletionPublicationToken::Create(DomainHypercallIdentitySealToken issuerSeal, unsigned long long sequence, std::shared_ptr<DomainHypercallRuntimeExecutor::ExecutionReceipt> receipt, std::shared_ptr<CompletionRecord> completion)
	{
		std::wstring completionDigest = DomainHypercallCompletionDigest::ComputeRecord(completion);
		std::wstring tokenDigest = DomainHypercallCompletionDigest::ComputeToken(receipt->getAttemptId(), receipt->getVirtualThreadId(), receipt->getDomainTag(), receipt->getDecisionId(), receipt->getOwnerId(), receipt->getExecutionSequence(), sequence, receipt->getReceiptDigest(), receipt->getEffectDigest(), completionDigest, receipt->getRestoreGeneration());
		return std::shared_ptr<CompletionPublicationToken>(new CompletionPublicationToken(std::move(issuerSeal), receipt->getAttemptId(), receipt->getVirtualThreadId(), receipt->getDomainTag(), receipt->getDecisionId(), receipt->getOwnerId(), receipt->getExecutionSequence(), sequence, receipt->getReceiptDigest(), receipt->getEffectDigest(), completionDigest, receipt->getRestoreGeneration(), tokenDigest));
	}

	void DomainHypercallCompletionOwner::BindRetireOwner(std::shared_ptr<DomainHypercallRetireOwner> owner)
	{
		ArgumentNullException::ThrowIfNull(owner);
		{
			std::scoped_lock<std::mutex> lock(_sync);
			const std::any sealValue = owner->getConsumerSeal();
			const auto *seal = std::any_cast<DomainHypercallIdentitySealToken>(&sealValue);
			if (seal == nullptr || *seal == nullptr)
			{
				throw InvalidOperationException(L"Canonical retire owner supplied an invalid consumer identity seal.");
			}
			if (_retireConsumerSeal != nullptr && _retireConsumerSeal != *seal)
			{
				throw InvalidOperationException(L"Completion owner is already bound to another canonical retire owner.");
			}
			_retireConsumerSeal = *seal;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallCompletionPublicationResult PublishExactProbe(DomainHypercallRuntimeExecutor executor, DomainHypercallRuntimeExecutor.ExecutionReceipt? receipt, VirtualizationRestoreGenerationOwner restoreOwner, DomainRuntimeContext context, RootAuthorityDescriptor root, DomainHypercallLifecycleGate lifecycleGate)
	std::shared_ptr<DomainHypercallCompletionPublicationResult> DomainHypercallCompletionOwner::PublishExactProbe(std::shared_ptr<DomainHypercallRuntimeExecutor> executor, std::shared_ptr<DomainHypercallRuntimeExecutor::ExecutionReceipt> receipt, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<DomainHypercallLifecycleGate> lifecycleGate)
	{
		ArgumentNullException::ThrowIfNull(executor);
		ArgumentNullException::ThrowIfNull(restoreOwner);
		ArgumentNullException::ThrowIfNull(context);
		ArgumentNullException::ThrowIfNull(root);
		if (receipt == nullptr)
		{
			return Deny(DomainHypercallCompletionDecision::MissingExecution, L"E5 requires a live E3 receipt.");
		}
		std::shared_ptr<DomainHypercallLifecycleGate::TransitionLease> transition;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!lifecycleGate.TryBeginTransition(receipt.DomainTag, DomainHypercallTransitionKind.E3ToE5, out DomainHypercallLifecycleGate.TransitionLease? transition) || transition == null)
		if (!lifecycleGate->TryBeginTransition(receipt->getDomainTag(), DomainHypercallTransitionKind::E3ToE5, transition) || transition == nullptr)
		{
			return Deny(DomainHypercallCompletionDecision::LifecycleGateDenied, L"E3-to-E5 handoff denied by the disabled or draining lifecycle gate.");
		}

// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using (transition)
		{
			{
				std::scoped_lock<std::mutex> lock(_sync);
				std::shared_ptr<RuntimeBoundaryAdmissionService> admissionService = std::make_shared<RuntimeBoundaryAdmissionService>();
				std::shared_ptr<DomainRuntimeOperation> operation = std::make_shared<DomainRuntimeOperation>(DomainRuntimeOperationKind::InvokeHypercall, DomainRuntimeOperationSource::RuntimeService, true, DomainRuntimeOperationAuthorityClass::NoStateExecution);
				std::shared_ptr<RuntimeBoundaryAdmissionRequest> admissionRequest = std::make_shared<RuntimeBoundaryAdmissionRequest>(context, root, nullptr, operation, DomainBoundaryDescriptor::ExecutionOnly, CapabilityBoundaryRequirement::TypedGrant(RuntimeCapabilityIds::VmCallProbeNoStateV1Mask, CapabilityGrantScope::DomainGranted), EvidenceBoundaryRequirement::None);
				std::shared_ptr<RuntimeBoundaryAdmissionResult> admission = admissionService->Validate(admissionRequest);
				if (!admission->IsAllowed)
				{
					return Deny(DomainHypercallCompletionDecision::RuntimeAdmissionDenied, admission->Message);
				}
				// These bounds are the transport widths of TrapRequest's byte VT and
				// ushort domain tag. Spell them as literals to avoid Windows min/max
				// macros corrupting the standard-library calls.
				if (receipt->getVirtualThreadId() < 0 || receipt->getVirtualThreadId() > 255 || receipt->getDomainTag() == 0 || receipt->getDomainTag() > 65535ULL)
				{
					return Deny(DomainHypercallCompletionDecision::InvalidExecution, L"E3 VT/domain identity is outside the neutral trap transport width.");
				}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: NeutralTrapResult neutral = NeutralTrapResult.Trap(TrapRequest.ForInstruction(YAKSys_Hybrid_CPU.Processor.CPU_Core.IsaOpcodeValues.VMCALL, checked((byte)receipt.VirtualThreadId), checked((ushort)receipt.DomainTag)), NeutralTrapResultKind.InstructionIntercept);
			std::shared_ptr<NeutralTrapResult> neutral = NeutralTrapResult::Trap(TrapRequest::ForInstruction(YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::VMCALL, static_cast<unsigned char>(receipt->getVirtualThreadId()), static_cast<unsigned short>(receipt->getDomainTag())), NeutralTrapResultKind::InstructionIntercept);
				std::shared_ptr<TrapCompletionRouteRequest> request = std::make_shared<TrapCompletionRouteRequest>(neutral, admission, TrapCompletionRouteDescriptor::getRuntimeOwnedCompletionPublication(), context->IsBoundToDomain(receipt->getDomainTag()), true, 0, 0, 0, EvidenceVisibilityClass::HostOwnedRuntimeEvidence, TrapCompletionMigrationClass::HostOwnedNonMigratable);
			std::shared_ptr<TrapCompletionRouteResult> route = TrapCompletionRouteService::getDefault()->Authorize(request);
			if (!route->CompletionPublicationAuthorizedOnly)
			{
				return Deny(DomainHypercallCompletionDecision::RouteDenied, route->Reason);
			}

			std::shared_ptr<TrapCompletionOwnerPolicyResult> policy = TrapCompletionPublicationFence::getDefault()->EvaluateOwnerPolicy(request, route);
			if (!policy->IsAllowed)
			{
				return Deny(DomainHypercallCompletionDecision::FenceDenied, policy->Reason);
			}

				std::shared_ptr<DomainHypercallReceiptValidationResult> consumed = executor->ConsumeReceiptForCompletion(receipt, restoreOwner, lifecycleGate, std::any(_consumerSeal));
				if (!consumed || !consumed->isValid())
				{
					return Deny(DomainHypercallCompletionDecision::InvalidExecution, consumed->Reason);
				}

	#if defined(TESTING)
				lifecycleGate->NotifyTransitionGapForTesting(DomainHypercallTransitionKind::E3ToE5);
	#endif

				std::shared_ptr<CompletionRecord> completion = std::make_shared<CompletionRecord>(CompletionRecordClass::Event, route->NeutralReasonCode, 0, 0, 0);
			unsigned long long sequence = AllocateSequence();
			std::shared_ptr<CompletionPublicationToken> token = CompletionPublicationToken::Create(_issuerSeal, sequence, receipt, completion);
				_live->Add(token, std::make_shared<LivePublication>(completion, lifecycleGate));
				_liveTokens.insert(token);
				return std::make_shared<DomainHypercallCompletionPublicationResult>(DomainHypercallCompletionDecision::Published, completion, token, L"Neutral completion owner atomically published one record and opaque E5; retire remains denied.", shared_from_this(), restoreOwner);
			}
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool ValidateLive(CompletionPublicationToken? token, System.Nullable<CompletionRecord> completion, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner)
	bool DomainHypercallCompletionOwner::ValidateLive(std::shared_ptr<CompletionPublicationToken> token, std::shared_ptr<CompletionRecord> completion, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner)
	{
		std::shared_ptr<LivePublication> published;
		if (token == nullptr || completion == nullptr || restoreOwner == nullptr || !token->WasIssuedBy(_issuerSeal) || !_live->TryGetValue(token, published) || published->Completion != completion || token->getRestoreGeneration() != restoreOwner->getCurrentGeneration())
		{
			return false;
		}
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		return token->getCompletionDigest() == DomainHypercallCompletionDigest::ComputeRecord(completion) && token->getTokenDigest() == DomainHypercallCompletionDigest::ComputeToken(token->getAttemptId(), token->getVirtualThreadId(), token->getDomainTag(), token->getDecisionId(), token->getOwnerId(), token->getExecutionSequence(), token->getCompletionSequence(), token->getE3Digest(), token->getEffectDigest(), token->getCompletionDigest(), token->getRestoreGeneration());
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool ConsumeForRetire(CompletionPublicationToken? token, System.Nullable<CompletionRecord> completion, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner, Object retireConsumerSeal)
	bool DomainHypercallCompletionOwner::ConsumeForRetire(std::shared_ptr<CompletionPublicationToken> token, std::shared_ptr<CompletionRecord> completion, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, const DomainHypercallIdentitySealToken &retireConsumerSeal)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			if (_retireConsumerSeal == nullptr || _retireConsumerSeal != retireConsumerSeal || !ValidateLive(token, completion, restoreOwner) || token == nullptr)
			{
				return false;
			}

			_liveTokens.erase(token);
			return _live->Remove(token);
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool TryBeginE5ToE6Transition(CompletionPublicationToken? token, out System.Nullable<DomainHypercallLifecycleGate.TransitionLease> transition)
	bool DomainHypercallCompletionOwner::TryBeginE5ToE6Transition(std::shared_ptr<CompletionPublicationToken> token, std::shared_ptr<DomainHypercallLifecycleGate::TransitionLease> &transition)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			std::shared_ptr<LivePublication> publication;
			if (token == nullptr || !token->WasIssuedBy(_issuerSeal) || !_live->TryGetValue(token, publication))
			{
				transition = nullptr;
				return false;
			}

			return publication->LifecycleGate->TryBeginTransition(token->getDomainTag(), DomainHypercallTransitionKind::E5ToE6, transition);
		}
	}

	int DomainHypercallCompletionOwner::CountLiveTokens(unsigned long long domainTag)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return static_cast<int>(std::count_if(_liveTokens.begin(), _liveTokens.end(), [domainTag](const auto &token)
			{
				return token && token->getDomainTag() == domainTag;
			}));
		}
	}

	int DomainHypercallCompletionOwner::CancelLiveTokensForDrain(unsigned long long domainTag)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			std::vector<std::shared_ptr<CompletionPublicationToken>> cancelled;
			for (const auto &token : _liveTokens)
			{
				if (token && token->getDomainTag() == domainTag)
					cancelled.push_back(token);
			}
			for (auto token : cancelled)
			{
				_liveTokens.erase(token);
				_live->Remove(token);
			}
			return cancelled.size();
		}
	}

	std::any DomainHypercallCompletionOwner::getConsumerSeal() const
	{
		return std::any(_consumerSeal);
	}

	unsigned long long DomainHypercallCompletionOwner::AllocateSequence()
	{
		unsigned long long value = _nextCompletionSequence++;
		return value == 0 ? _nextCompletionSequence++: value;
	}

	std::shared_ptr<DomainHypercallCompletionPublicationResult> DomainHypercallCompletionOwner::Deny(DomainHypercallCompletionDecision decision, const std::wstring &reason)
	{
		return std::make_shared<DomainHypercallCompletionPublicationResult>(decision, CompletionRecord::getNone(), nullptr, reason);
	}

	std::wstring DomainHypercallCompletionDigest::ComputeRecord(std::shared_ptr<CompletionRecord> record)
	{
        std::vector<unsigned char> stream = RecordEnvelope;
        stream.push_back(static_cast<unsigned char>(record->getRecordClass()));
        WriteU32(stream, record->getReasonCode()); WriteU64(stream, record->getQualification());
        WriteU64(stream, record->getFaultAddress()); WriteU64(stream, record->getFaultAux());
        return Hash(stream);
    }
    std::wstring DomainHypercallCompletionDigest::ComputeToken(unsigned long long attemptId, int virtualThreadId, unsigned long long domainTag, const std::wstring &decisionId, unsigned long long ownerId, unsigned long long executionSequence, unsigned long long completionSequence, const std::wstring &e3Digest, const std::wstring &effectDigest, const std::wstring &completionDigest, unsigned long long restoreGeneration)
    {
        if (virtualThreadId < 0) throw std::overflow_error("virtualThreadId");
        std::vector<unsigned char> stream = TokenEnvelope;
        WriteU64(stream, attemptId); WriteU32(stream, static_cast<unsigned int>(virtualThreadId));
        WriteU64(stream, domainTag); WriteText(stream, decisionId); WriteU64(stream, ownerId);
        WriteU64(stream, executionSequence); WriteU64(stream, completionSequence);
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
        appendHex(e3Digest); appendHex(effectDigest); appendHex(completionDigest);
        stream.push_back(static_cast<unsigned char>(EvidenceVisibilityClass::HostOwnedRuntimeEvidence));
        stream.push_back(static_cast<unsigned char>(TrapCompletionMigrationClass::HostOwnedNonMigratable));
        WriteU64(stream, restoreGeneration); return Hash(stream);
    }
    std::wstring DomainHypercallCompletionDigest::Hash(const std::vector<unsigned char>& stream)
    { return YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(stream); }
    void DomainHypercallCompletionDigest::WriteText(std::vector<unsigned char>& stream, const std::wstring& value)
    {
        if (value.size() > static_cast<size_t>((std::numeric_limits<int>::max)())) throw std::overflow_error("text length");
        int count = value.empty() ? 0 : WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
        if (!value.empty() && !count) throw std::invalid_argument("UTF-8 conversion failed");
        WriteU32(stream, static_cast<unsigned int>(count)); auto start = stream.size(); stream.resize(start + count);
        if (count && !WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), reinterpret_cast<char*>(stream.data() + start), count, nullptr, nullptr)) throw std::invalid_argument("UTF-8 conversion failed");
    }
    void DomainHypercallCompletionDigest::WriteU32(std::vector<unsigned char>& stream, unsigned int value)
    { for (int shift = 24; shift >= 0; shift -= 8) stream.push_back(static_cast<unsigned char>(value >> shift)); }
    void DomainHypercallCompletionDigest::WriteU64(std::vector<unsigned char>& stream, unsigned long long value)
    { for (int shift = 56; shift >= 0; shift -= 8) stream.push_back(static_cast<unsigned char>(value >> shift)); }
}
