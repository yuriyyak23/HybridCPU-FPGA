#include "SecureComputeRegisterLocalCompletionOwner.h"
#include "../../../Architecture/Registers/Physical/PhysicalRegisterFile.h"
#include "../Observation/DomainCompletionObservationOwner.h"
#include "../../Events/Hypercalls/VirtualizationOperandSnapshot.h"

using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeRegisterLocalRetirement::SecureComputeRegisterLocalRetirement(std::shared_ptr<ExternalSecureRegisterLocalSubmission> submission, unsigned long long acceptanceSequence, std::shared_ptr<ArchitecturalCompletionReceiptBinding> binding, unsigned long long register3Value, std::shared_ptr<ScalarRegisterWriteRetireEffect> effect)
		: Submission(std::move(submission)), AcceptanceSequence(acceptanceSequence),
		  CanonicalOrderSequence(binding->CanonicalOrderSequence), CommitSequence(binding->CommitSequence),
		  RestoreGeneration(binding->RestoreGeneration), Register3Value(register3Value), Effect(std::move(effect))
	{
	}

	std::shared_ptr<ExternalSecureRegisterLocalSubmission> SecureComputeRegisterLocalRetirement::getSubmission() const
	{
		return Submission;
	}

	unsigned long long SecureComputeRegisterLocalRetirement::getAcceptanceSequence() const
	{
		return AcceptanceSequence;
	}

	unsigned long long SecureComputeRegisterLocalRetirement::getCanonicalOrderSequence() const
	{
		return CanonicalOrderSequence;
	}

	unsigned long long SecureComputeRegisterLocalRetirement::getCommitSequence() const
	{
		return CommitSequence;
	}

	unsigned long long SecureComputeRegisterLocalRetirement::getRestoreGeneration() const
	{
		return RestoreGeneration;
	}

	unsigned long long SecureComputeRegisterLocalRetirement::getRegister3Value() const
	{
		return Register3Value;
	}

	int SecureComputeRegisterLocalRetirement::getEffectCount() const
	{
		return 1;
	}

	std::shared_ptr<ScalarRegisterWriteRetireEffect> SecureComputeRegisterLocalRetirement::getEffect() const
	{
		return Effect;
	}

    SecureComputeRegisterLocalCompletionOwner::Pending::Pending(
        std::shared_ptr<SecureComputePreparedMachineAdmission> prepared,
        std::shared_ptr<ExternalSecureRegisterLocalSubmission> submission, unsigned long long domainScope,
        std::shared_ptr<IExternalSecureRegisterLocalExecutionProvider> provider,
        std::shared_ptr<PostStageBIssuedAttempt> issuedAttempt)
        : Prepared(std::move(prepared)), Submission(std::move(submission)), DomainScope(domainScope),
          Provider(std::move(provider)), IssuedAttempt(std::move(issuedAttempt)), Contexts([this] {
              auto target = Prepared->getCertificate()->getLiveCoreTarget();
              if (target) return target->getContexts();
              std::vector<std::shared_ptr<ArchContextState>> contexts;
              for (int vt = 0; vt < 4; ++vt) contexts.push_back(std::make_shared<ArchContextState>(32, vt));
              return contexts;
          }()) {}
	std::shared_ptr<SecureComputePreparedMachineAdmission> SecureComputeRegisterLocalCompletionOwner::Pending::getPrepared() const
	{
		return Prepared;
	}

	std::shared_ptr<ExternalSecureRegisterLocalSubmission> SecureComputeRegisterLocalCompletionOwner::Pending::getSubmission() const
	{
		return Submission;
	}

	unsigned long long SecureComputeRegisterLocalCompletionOwner::Pending::getDomainScope() const
	{
		return DomainScope;
	}

	std::shared_ptr<IExternalSecureRegisterLocalExecutionProvider> SecureComputeRegisterLocalCompletionOwner::Pending::getProvider() const
	{
		return Provider;
	}

	std::shared_ptr<PostStageBIssuedAttempt> SecureComputeRegisterLocalCompletionOwner::Pending::getIssuedAttempt() const
	{
		return IssuedAttempt;
	}

	const std::vector<std::shared_ptr<ArchContextState>> SecureComputeRegisterLocalCompletionOwner::Pending::getContexts() const
	{
		return Contexts;
	}

    std::shared_ptr<RetireCoordinator> SecureComputeRegisterLocalCompletionOwner::Pending::getRetire() const {
        auto target = Prepared->getCertificate()->getLiveCoreTarget();
        if (target) return target->getRetire();
        if (!retire) retire = std::make_shared<RetireCoordinator>(std::make_shared<PhysicalRegisterFile>(),
            std::make_shared<RenameMap>(4), std::make_shared<CommitMap>(4), std::make_shared<ArchContextCollection>(Contexts));
        return retire;
    }
	std::shared_ptr<YAKSys_Hybrid_CPU::Core::SecureComputeRegisterLocalCompletionOwner::Pending> SecureComputeRegisterLocalCompletionOwner::Acceptance::getPending() const
	{
		return Pending;
	}

	std::shared_ptr<ExternalSecureRegisterLocalExecutionReceipt> SecureComputeRegisterLocalCompletionOwner::Acceptance::getReceipt() const
	{
		return Receipt;
	}

	unsigned long long SecureComputeRegisterLocalCompletionOwner::Acceptance::getSequence() const
	{
		return Sequence;
	}

	std::shared_ptr<ArchitecturalCompletionCommitOwner> SecureComputeRegisterLocalCompletionOwner::getCommitOwner() const
	{
		return CommitOwner;
	}

	SecureComputeRegisterLocalCompletionOwner::SecureComputeRegisterLocalCompletionOwner(std::shared_ptr<SafetyVerifier> issuer, std::recursive_mutex &gate)
		: issuer(std::move(issuer)), gate(gate),
		  CommitOwner(std::make_shared<ArchitecturalCompletionCommitOwner>(std::make_shared<VirtualizationRestoreGenerationOwner>(), std::make_shared<DomainCompletionObservationOwner>(std::make_shared<CompletionGenerationAuthority>()))),
		  producer(CommitOwner->RegisterProducer(std::make_shared<ArchitecturalCompletionProducerPolicy>(ArchitecturalCompletionProducerPolicy{L"SecureComputeStandaloneRegisterLocal", NeutralArchitecturalCompletionClass::SecureRegisterLocal, false, false, NeutralFaultAddressSemantic::None, NeutralFaultAuxiliarySemantic::None})))
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Pending? Begin(SecureComputePreparedMachineAdmission prepared, ExternalSecureExecutionAttempt attempt, ExternalSecureExecutionConsumptionReceipt consumption, IExternalSecureRegisterLocalExecutionProvider provider)
	std::shared_ptr<SecureComputeRegisterLocalCompletionOwner::Pending> SecureComputeRegisterLocalCompletionOwner::Begin(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureExecutionAttempt> attempt, std::shared_ptr<ExternalSecureExecutionConsumptionReceipt> consumption, std::shared_ptr<IExternalSecureRegisterLocalExecutionProvider> provider)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(gate);
			if (!issuer->IsSecureRegisterLocalOwner(shared_from_this()) || !issuer->HasConsumedSecureMachinePermit(prepared) || !issuer->IsSecureBackendAttempt(prepared, attempt) || live.size() >= 8 || nextGeneration == std::numeric_limits<unsigned long long>::max() || nextDomainScope == std::numeric_limits<unsigned long long>::max() || std::find(issued.begin(), issued.end(), prepared->getCertificate()) != issued.end() || consumption->Attempt != attempt || !Matches(prepared, attempt))
			{
				return nullptr;
			}
			if (!issuer->TryRegisterSecureSubmission(prepared))
			{
				return nullptr;
			}
			int slot = 0;
			for (; slot < 8; ++slot)
			{
				const bool occupied = std::any_of(live.begin(), live.end(), [slot](const auto &item)
				{
					return item && item->getSubmission()->CompletionSlot == slot;
				});
				if (!occupied) break;
			}
			if (slot >= 8) return nullptr;
			const auto &domain = attempt->Request->Binding.SecureDomain;
			auto domainScopes_iterator = std::find_if(domainScopes.begin(), domainScopes.end(), [&domain](const auto &entry)
			{
				return entry.first && entry.first->Equals(domain);
			});
			unsigned long long scope = 0;
			if (domainScopes_iterator == domainScopes.end())
			{
				scope = nextDomainScope++;
				domainScopes.emplace_back(std::make_shared<ExternalSecureDomainLease>(domain), scope);
			}
			else
			{
				scope = domainScopes_iterator->second;
			}
			auto submission = std::make_shared<ExternalSecureRegisterLocalSubmission>(consumption, slot, nextGeneration++);
			auto pending = std::make_shared<SecureComputeRegisterLocalCompletionOwner::Pending>(prepared, submission, scope, provider, PostStageBIssuedAttempt::RequireOriginalSecureStageBAttempt(issuer, prepared));
			issued.insert(prepared->getCertificate());
			live.insert(pending);
			return pending;
		}
	}

	std::optional<std::shared_ptr<ExternalSecureRegisterLocalExecutionResult>> SecureComputeRegisterLocalCompletionOwner::ObserveOriginal(std::shared_ptr<Pending> pending)
	{
		std::optional<std::shared_ptr<ExternalSecureRegisterLocalExecutionResult>> original;
		try
		{
			original = pending->getProvider()->QueryRegisterLocal(pending->getSubmission());
		}
		catch (...)
		{
			InvalidateSource(pending);
			return std::nullopt;
		}
		if (!original || !original.value() || original.value()->Submission != pending->getSubmission())
		{
			InvalidateSource(pending);
			return std::nullopt;
		}
		{
			std::scoped_lock<std::recursive_mutex> lock(gate);
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			const auto result = original.value();
			if (!result->Receipt)
			{
				InvalidateSource(pending);
			}
			else if (live.contains(pending) && !invalidSources.contains(pending) && issuer->HasConsumedSecureMachinePermit(pending->getPrepared()))
			{
				auto observed_iterator = observed.find(pending);
				if (observed_iterator != observed.end() && observed_iterator->second != result->Receipt)
				{
					InvalidateSource(pending);
					return std::nullopt;
				}
				observed[pending] = result->Receipt;
			}
		}
		return original; // historical cleanup fact even if the machine scope expired during query
	}

	void SecureComputeRegisterLocalCompletionOwner::InvalidateSource(std::shared_ptr<Pending> pending)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(gate);
			observed.erase(pending);
			if (std::find(live.begin(), live.end(), pending) != live.end())
			{
				invalidSources.insert(pending);
			}
		}
	}

	bool SecureComputeRegisterLocalCompletionOwner::Matches(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureExecutionAttempt> attempt)
	{
		auto scheduled = prepared->getCertificate()->getPlaced()->getScheduledOperation();
		return prepared->getCertificate()->getReservation() == attempt->Reservation && prepared->getCertificate()->getPlaced()->getRequest() == attempt->Request && attempt->MachineScope == issuer->getSecureRegisterLocalScope() && attempt->MachineGeneration == prepared->getCertificate()->getGeneration() && attempt->CpuAttempt == scheduled->getOperationId()->OperationAttempt && attempt->WorkingBundle == scheduled->getOperationId()->WorkingBundleSequence && attempt->VirtualThread == scheduled->getOperationId()->VirtualThreadId && attempt->OwnerContext == scheduled->getAdmission()->OwnerContextId && attempt->SourceSlot == scheduled->getAdmission()->SourceProvenance->SourceSlotIndex && attempt->WorkingSlot == scheduled->getOperationId()->WorkingSlotIndex && attempt->PhysicalLane == scheduled->getPhysicalLane();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Acceptance? Accept(Pending pending, ExternalSecureRegisterLocalExecutionReceipt receipt)
		std::shared_ptr<SecureComputeRegisterLocalCompletionOwner::Acceptance> SecureComputeRegisterLocalCompletionOwner::Accept(std::shared_ptr<Pending> pending, std::shared_ptr<ExternalSecureRegisterLocalExecutionReceipt> receipt)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(gate);
			auto submission = pending->getSubmission();
			auto observed_iterator = observed.find(pending);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (!live.Contains(pending) || pending.Accepted != null || nextAcceptance == ulong.MaxValue || invalidSources.Contains(pending) || !observed.TryGetValue(pending, out var original) || !ReferenceEquals(original, receipt) || !issuer.HasConsumedSecureMachinePermit(pending.Prepared) || !ReferenceEquals(receipt.Submission, submission) || receipt.Destination != submission.ExpectedDestination || receipt.EffectCount != 1 || receipt.Value != unchecked(submission.Attempt.Request.Left + submission.Attempt.Request.Right) || receipt.CompletionSlot != submission.CompletionSlot || receipt.CompletionGeneration != submission.CompletionGeneration)
			if (!live.contains(pending) || pending->Accepted != nullptr || nextAcceptance == std::numeric_limits<unsigned long long>::max() || invalidSources.contains(pending) || observed_iterator == observed.end() || observed_iterator->second != receipt || !issuer->HasConsumedSecureMachinePermit(pending->getPrepared()) || receipt->Submission != submission || receipt->Destination != ExternalSecureRegisterLocalSubmission::ExpectedDestination || receipt->EffectCount != 1 || receipt->Value != submission->Attempt->Request->Left + submission->Attempt->Request->Right || receipt->CompletionSlot != submission->CompletionSlot || receipt->CompletionGeneration != submission->CompletionGeneration)
			{
				return nullptr;
			}
			pending->Accepted = std::make_shared<SecureComputeRegisterLocalCompletionOwner::Acceptance>(pending, receipt, nextAcceptance++);
			return pending->Accepted;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputeRegisterLocalRetirement? Retire(Pending pending, Acceptance acceptance)
	std::shared_ptr<SecureComputeRegisterLocalRetirement> SecureComputeRegisterLocalCompletionOwner::Retire(std::shared_ptr<Pending> pending, std::shared_ptr<Acceptance> acceptance)
{
    std::scoped_lock<std::recursive_mutex> lock(gate);
    auto target = pending->getPrepared()->getCertificate()->getLiveCoreTarget();
    auto retireCurrent = [&]() -> std::shared_ptr<SecureComputeRegisterLocalRetirement>
    {
        const auto observedReceipt = observed.find(pending);
        if (!live.contains(pending) || pending->Accepted != acceptance || invalidSources.contains(pending) ||
            observedReceipt == observed.end() || observedReceipt->second != acceptance->getReceipt() ||
            acceptance->getPending() != pending || !issuer->HasConsumedSecureMachinePermit(pending->getPrepared()) ||
            std::any_of(live.begin(), live.end(), [&pending](const auto &other)
                { return other && other->getSubmission()->CompletionGeneration < pending->getSubmission()->CompletionGeneration; }))
            return nullptr;

        const auto attempt = pending->getSubmission()->Attempt;
		auto facts = std::make_shared<NeutralArchitecturalCompletionFacts>(NeutralArchitecturalCompletionFacts{
			NeutralArchitecturalCompletionClass::SecureRegisterLocal, {}, {}, {}, {}});
        const int completionContext = target ? attempt->OwnerContext + 1 : attempt->OwnerContext;
        auto candidate = std::make_shared<ArchitecturalCompletionCandidate>(ArchitecturalCompletionCandidate{
            pending->getDomainScope(), completionContext, attempt->VirtualThread, attempt->CpuAttempt,
            pending->getSubmission()->CompletionGeneration, *facts, {}});
        std::vector<RetireRecord> records{RetireRecord::RegisterWrite(attempt->VirtualThread, 3, acceptance->getReceipt()->Value)};
        auto retire = pending->getRetire();
        retire->Prevalidate(records);
		auto commitOwner = target ? target->getCommit() : CommitOwner;
        auto producerRegistration = target ? target->getProducer() : producer;
        std::vector<std::shared_ptr<ArchitecturalCompletionCandidate>> candidates{candidate};
        auto preparedWindow = commitOwner->PrepareCanonicalRetireWindow(producerRegistration, candidates);
		if (!preparedWindow || !preparedWindow->isPrepared() || !preparedWindow->Lease) return nullptr;

        auto lease = preparedWindow->Lease;
        pending->getIssuedAttempt()->CompleteScalarRegisterWrite(records[0]);
        retire->Retire(records);
        const auto commits = lease->Complete();
		if (commits.empty() || !commits[0]->isCommitted() || !commits[0]->Receipt)
            throw std::runtime_error("Prepared arithmetic canonical commit failed.");
		const auto binding = commits[0]->Receipt->getBinding();
        if (!commitOwner->TryConsumeReceipt(commits[0]->Receipt, binding))
            throw std::runtime_error("Canonical receipt was not original/live.");
        live.erase(pending);
        observed.erase(pending);
        invalidSources.erase(pending);
        const auto contexts = pending->getContexts();
        const auto register3 = (*contexts[attempt->VirtualThread]->getCommittedRegs())[3];
        return std::make_shared<SecureComputeRegisterLocalRetirement>(pending->getSubmission(),
            acceptance->getSequence(), binding, register3, pending->getIssuedAttempt()->getScalarRegisterWriteEffect());
    };
    return target ? target->CommitIfCurrent(retireCurrent) : retireCurrent();
}
void SecureComputeRegisterLocalCompletionOwner::Cancel(std::shared_ptr<Pending> pending)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(gate);
			live.erase(pending);
			observed.erase(pending);
			invalidSources.erase(pending);
		}
	}

	void SecureComputeRegisterLocalCompletionOwner::PruneInvalidated()
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(gate);
			for (auto it = live.begin(); it != live.end();)
			{
				if (!*it || !issuer->IsCurrentSecureComputeCertificate((*it)->getPrepared()->getCertificate())) it = live.erase(it);
				else ++it;
			}
			for (auto it = observed.begin(); it != observed.end();)
			{
				if (!live.contains(it->first)) it = observed.erase(it);
				else ++it;
			}
			for (auto it = invalidSources.begin(); it != invalidSources.end();)
			{
				if (!live.contains(*it)) it = invalidSources.erase(it);
				else ++it;
			}
			for (auto it = issued.begin(); it != issued.end();)
			{
				if (!*it || !issuer->IsCurrentSecureComputeCertificate(*it)) it = issued.erase(it);
				else ++it;
			}
		}
	}

	void SecureComputeRegisterLocalCompletionOwner::InvalidateAfterRestore()
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(gate);
			live.clear();
			observed.clear();
			invalidSources.clear();
			issued.clear();
			getCommitOwner()->InvalidateAfterRestore();
		}
	}
}
