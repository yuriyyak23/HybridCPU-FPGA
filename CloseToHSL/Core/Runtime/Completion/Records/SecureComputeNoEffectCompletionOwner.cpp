#include "SecureComputeNoEffectCompletionOwner.h"
#include "../../../Architecture/Registers/Architectural/ArchContextState.h"
#include "../../../Architecture/Registers/Physical/PhysicalRegisterFile.h"
#include "../Observation/DomainCompletionObservationOwner.h"
#include "../../Events/Hypercalls/VirtualizationOperandSnapshot.h"
#include "../Commit/NeutralArchitecturalCompletionFacts.h"
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <span>
#include <utility>
using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
namespace YAKSys_Hybrid_CPU::Core
{
    namespace
    {
        std::vector<std::shared_ptr<ArchContextState>> CreateNoEffectContexts()
        {
            std::vector<std::shared_ptr<ArchContextState>> contexts;
            contexts.reserve(4);
            for (int vt = 0; vt < 4; ++vt) contexts.push_back(std::make_shared<ArchContextState>(32, vt));
            return contexts;
        }
        std::shared_ptr<ArchitecturalCompletionReceiptBinding> RequireBinding(
            const std::shared_ptr<ArchitecturalCompletionReceiptBinding>& binding)
        {
            if (!binding) throw std::invalid_argument("binding");
            return binding;
        }
        template<class T> class DisposeGuard final
        {
            std::shared_ptr<T> value_;
        public:
            explicit DisposeGuard(std::shared_ptr<T> value) : value_(std::move(value)) {}
            ~DisposeGuard() noexcept { if (value_) { try { value_->dispose(); } catch (...) {} } }
        };
    }

    SecureComputeNoEffectRetirement::SecureComputeNoEffectRetirement(
        std::shared_ptr<ExternalSecureNoEffectSubmission> submission, unsigned long long acceptanceSequence,
        std::shared_ptr<ArchitecturalCompletionReceiptBinding> binding, unsigned long long register3Value)
        : Submission(submission ? std::move(submission) : throw std::invalid_argument("submission")),
          AcceptanceSequence(acceptanceSequence), CanonicalOrderSequence(RequireBinding(binding)->CanonicalOrderSequence),
          CommitSequence(RequireBinding(binding)->CommitSequence), RestoreGeneration(RequireBinding(binding)->RestoreGeneration),
          Register3Value(register3Value) {}

    std::shared_ptr<ExternalSecureNoEffectSubmission> SecureComputeNoEffectRetirement::getSubmission() const { return Submission; }
    unsigned long long SecureComputeNoEffectRetirement::getAcceptanceSequence() const { return AcceptanceSequence; }
    unsigned long long SecureComputeNoEffectRetirement::getCanonicalOrderSequence() const { return CanonicalOrderSequence; }
    unsigned long long SecureComputeNoEffectRetirement::getCommitSequence() const { return CommitSequence; }
    unsigned long long SecureComputeNoEffectRetirement::getRestoreGeneration() const { return RestoreGeneration; }
    unsigned long long SecureComputeNoEffectRetirement::getRegister3Value() const { return Register3Value; }
    int SecureComputeNoEffectRetirement::getEffectCount() const { return 0; }
    std::shared_ptr<SecureComputePreparedMachineAdmission> SecureComputeNoEffectCompletionOwner::Pending::getPrepared() const { return Prepared; }
    std::shared_ptr<ExternalSecureNoEffectSubmission> SecureComputeNoEffectCompletionOwner::Pending::getSubmission() const { return Submission; }
    unsigned long long SecureComputeNoEffectCompletionOwner::Pending::getDomainScope() const { return DomainScope; }
    std::shared_ptr<IExternalSecureNoEffectExecutionProvider> SecureComputeNoEffectCompletionOwner::Pending::getProvider() const { return Provider; }
    std::shared_ptr<SecureComputeNoEffectCompletionOwner::Pending> SecureComputeNoEffectCompletionOwner::Acceptance::getPending() const { return Pending; }
    std::shared_ptr<ExternalSecureNoEffectExecutionReceipt> SecureComputeNoEffectCompletionOwner::Acceptance::getReceipt() const { return Receipt; }
    unsigned long long SecureComputeNoEffectCompletionOwner::Acceptance::getSequence() const { return Sequence; }
    std::vector<std::shared_ptr<ArchContextState>> SecureComputeNoEffectCompletionOwner::getContexts() const { return Contexts; }
    std::shared_ptr<ArchitecturalCompletionCommitOwner> SecureComputeNoEffectCompletionOwner::getCommitOwner() const { return CommitOwner; }

    SecureComputeNoEffectCompletionOwner::SecureComputeNoEffectCompletionOwner(
        std::shared_ptr<SafetyVerifier> issuer, std::recursive_mutex&)
        : Contexts(CreateNoEffectContexts()),
          CommitOwner(std::make_shared<ArchitecturalCompletionCommitOwner>(
              std::make_shared<VirtualizationRestoreGenerationOwner>(),
              std::make_shared<DomainCompletionObservationOwner>(std::make_shared<CompletionGenerationAuthority>()))),
          issuer(issuer ? std::move(issuer) : throw std::invalid_argument("issuer")),
          producer(CommitOwner->RegisterProducer(std::make_shared<ArchitecturalCompletionProducerPolicy>(
              ArchitecturalCompletionProducerPolicy{L"SecureComputeStandaloneNoEffect",
                  NeutralArchitecturalCompletionClass::SecureNoEffect, false, false,
                  NeutralFaultAddressSemantic::None, NeutralFaultAuxiliarySemantic::None}))),
          retire(std::make_shared<RetireCoordinator>(std::make_shared<PhysicalRegisterFile>(),
              std::make_shared<RenameMap>(4), std::make_shared<CommitMap>(4),
              std::make_shared<ArchContextCollection>(Contexts))) {}

    std::shared_ptr<SecureComputeNoEffectCompletionOwner::Pending> SecureComputeNoEffectCompletionOwner::Begin(
        std::shared_ptr<SecureComputePreparedMachineAdmission> prepared,
        std::shared_ptr<ExternalSecureExecutionAttempt> attempt,
        std::shared_ptr<ExternalSecureExecutionConsumptionReceipt> consumption,
        std::shared_ptr<IExternalSecureNoEffectExecutionProvider> provider)
    {
        if (!prepared || !attempt || !consumption || !provider) return nullptr;
        std::scoped_lock<std::mutex> lock(gate);
        if (!issuer->IsSecureNoEffectOwner(shared_from_this()) || !issuer->HasConsumedSecureMachinePermit(prepared) ||
            !issuer->IsSecureBackendAttempt(prepared, attempt) || live.size() >= 8 ||
            nextGeneration == std::numeric_limits<unsigned long long>::max() ||
            nextDomainScope == std::numeric_limits<unsigned long long>::max() ||
            issued.find(prepared->getCertificate()) != issued.end() || consumption->Attempt != attempt ||
            !Matches(prepared, attempt)) return nullptr;
        if (!issuer->TryRegisterSecureSubmission(prepared)) return nullptr;
        int slot = -1;
        for (int candidate = 0; candidate < 8; ++candidate)
        {
            const bool occupied = std::any_of(live.begin(), live.end(), [candidate](const auto& item)
            { return item && item->getSubmission()->CompletionSlot == candidate; });
            if (!occupied) { slot = candidate; break; }
        }
        if (slot < 0) return nullptr;
        const auto domain = attempt->Request->Binding.SecureDomain;
        auto domainScope = std::find_if(domainScopes.begin(), domainScopes.end(), [&domain](const auto& entry)
        { return entry.first.Equals(domain); });
        unsigned long long scope = 0;
        if (domainScope == domainScopes.end()) { scope = nextDomainScope++; domainScopes.emplace_back(domain, scope); }
        else scope = domainScope->second;
        auto submission = std::make_shared<ExternalSecureNoEffectSubmission>(consumption, slot, nextGeneration++);
        auto pending = std::make_shared<Pending>(prepared, std::move(submission), scope, std::move(provider));
        issued.insert(prepared->getCertificate()); live.insert(pending);
        return pending;
    }

    std::optional<std::shared_ptr<ExternalSecureNoEffectExecutionResult>>
    SecureComputeNoEffectCompletionOwner::ObserveOriginal(std::shared_ptr<Pending> pending)
    {
        if (!pending) return std::nullopt;
        std::shared_ptr<ExternalSecureNoEffectExecutionResult> original;
        try { original = pending->getProvider()->QueryNoEffect(pending->getSubmission()); }
        catch (...) { InvalidateSource(pending); return std::nullopt; }
        if (!original || original->Submission != pending->getSubmission()) { InvalidateSource(pending); return std::nullopt; }
        std::scoped_lock<std::mutex> lock(gate);
        if (!original->Receipt)
        {
            observed.erase(pending);
            if (live.find(pending) != live.end()) invalidSources.insert(pending);
        }
        else if (live.find(pending) != live.end() && invalidSources.find(pending) == invalidSources.end() &&
            issuer->HasConsumedSecureMachinePermit(pending->getPrepared()))
        {
            auto previous = observed.find(pending);
            if (previous != observed.end() && previous->second != original->Receipt)
            { observed.erase(previous); invalidSources.insert(pending); return std::nullopt; }
            observed[pending] = original->Receipt;
        }
        return original;
    }

    void SecureComputeNoEffectCompletionOwner::InvalidateSource(std::shared_ptr<Pending> pending)
    {
        if (!pending) return;
        std::scoped_lock<std::mutex> lock(gate);
        observed.erase(pending);
        if (live.find(pending) != live.end()) invalidSources.insert(pending);
    }

    bool SecureComputeNoEffectCompletionOwner::Matches(
        std::shared_ptr<SecureComputePreparedMachineAdmission> prepared,
        std::shared_ptr<ExternalSecureExecutionAttempt> attempt)
    {
        if (!prepared || !attempt || !prepared->getCertificate() || !prepared->getCertificate()->getPlaced() ||
            !prepared->getCertificate()->getPlaced()->getScheduledOperation() || !attempt->Request) return false;
        auto scheduled = prepared->getCertificate()->getPlaced()->getScheduledOperation();
        return prepared->getCertificate()->getReservation() == attempt->Reservation &&
            prepared->getCertificate()->getPlaced()->getRequest() == attempt->Request &&
            attempt->MachineScope == issuer->getSecureRegisterLocalScope() &&
            attempt->MachineGeneration == prepared->getCertificate()->getGeneration() &&
            attempt->CpuAttempt == scheduled->getOperationId()->OperationAttempt &&
            attempt->WorkingBundle == scheduled->getOperationId()->WorkingBundleSequence &&
            attempt->VirtualThread == scheduled->getOperationId()->VirtualThreadId &&
            attempt->OwnerContext == scheduled->getAdmission()->OwnerContextId &&
            attempt->SourceSlot == scheduled->getAdmission()->SourceProvenance->SourceSlotIndex &&
            attempt->WorkingSlot == scheduled->getOperationId()->WorkingSlotIndex &&
            attempt->PhysicalLane == scheduled->getPhysicalLane();
    }

    std::shared_ptr<SecureComputeNoEffectCompletionOwner::Acceptance> SecureComputeNoEffectCompletionOwner::Accept(
        std::shared_ptr<Pending> pending, std::shared_ptr<ExternalSecureNoEffectExecutionReceipt> receipt)
    {
        if (!pending || !receipt) return nullptr;
        std::scoped_lock<std::mutex> lock(gate);
        auto submission = pending->getSubmission(); auto original = observed.find(pending);
        if (live.find(pending) == live.end() || pending->Accepted || nextAcceptance == std::numeric_limits<unsigned long long>::max() ||
            invalidSources.find(pending) != invalidSources.end() || original == observed.end() || original->second != receipt ||
            !issuer->HasConsumedSecureMachinePermit(pending->getPrepared()) || receipt->Submission != submission ||
            receipt->Destination != submission->ExpectedDestination || receipt->EffectCount != 0 ||
            receipt->CompletionSlot != submission->CompletionSlot || receipt->CompletionGeneration != submission->CompletionGeneration)
            return nullptr;
        pending->Accepted = std::make_shared<Acceptance>(pending, receipt, nextAcceptance++);
        return pending->Accepted;
    }

    std::shared_ptr<SecureComputeNoEffectRetirement> SecureComputeNoEffectCompletionOwner::Retire(
        std::shared_ptr<Pending> pending, std::shared_ptr<Acceptance> acceptance)
    {
        if (!pending || !acceptance) return nullptr;
        std::scoped_lock<std::mutex> lock(gate);
        auto original = observed.find(pending);
        const bool hasOlderLiveGeneration = std::any_of(live.begin(), live.end(), [&pending](const auto& item)
        { return item && item->getSubmission()->CompletionGeneration < pending->getSubmission()->CompletionGeneration; });
        if (live.find(pending) == live.end() || pending->Accepted != acceptance || invalidSources.find(pending) != invalidSources.end() ||
            original == observed.end() || original->second != acceptance->getReceipt() || acceptance->getPending() != pending ||
            !issuer->HasConsumedSecureMachinePermit(pending->getPrepared()) || hasOlderLiveGeneration) return nullptr;
        auto attempt = pending->getSubmission()->Attempt;
        const NeutralArchitecturalCompletionFacts facts{NeutralArchitecturalCompletionClass::SecureNoEffect, {}, {}, {}, {}};
        auto candidate = std::make_shared<ArchitecturalCompletionCandidate>(ArchitecturalCompletionCandidate{
            pending->getDomainScope(), attempt->OwnerContext, attempt->VirtualThread, attempt->CpuAttempt,
            pending->getSubmission()->CompletionGeneration, facts, {}});
        retire->Prevalidate(std::span<const RetireRecord>{});
        std::vector<std::shared_ptr<ArchitecturalCompletionCandidate>> candidates{candidate};
        auto commit = CommitOwner->PrepareCanonicalRetireWindow(producer, candidates);
        if (!commit->isPrepared() || !commit->Lease) return nullptr;
        auto lease = commit->Lease;
        DisposeGuard<ArchitecturalCompletionCommitOwner::CanonicalRetireWindowCommitLease> leaseGuard(lease);
        retire->Retire(std::span<const RetireRecord>{});
        auto committed = lease->Complete().at(0);
        if (!committed->isCommitted() || !committed->Receipt) throw std::logic_error("Prepared no-effect canonical commit failed.");
        auto binding = committed->Receipt->getBinding();
        if (!CommitOwner->TryConsumeReceipt(committed->Receipt, binding)) throw std::logic_error("Canonical receipt was not original/live.");
        live.erase(pending); observed.erase(pending); invalidSources.erase(pending);
        const auto registers = Contexts.at(static_cast<std::size_t>(attempt->VirtualThread))->getCommittedRegs();
        return std::make_shared<SecureComputeNoEffectRetirement>(pending->getSubmission(), acceptance->getSequence(), binding, (*registers)[3]);
    }

    void SecureComputeNoEffectCompletionOwner::Cancel(std::shared_ptr<Pending> pending)
    {
        if (!pending) return;
        std::scoped_lock<std::mutex> lock(gate); live.erase(pending); observed.erase(pending); invalidSources.erase(pending);
    }
    void SecureComputeNoEffectCompletionOwner::PruneInvalidated()
    {
        std::scoped_lock<std::mutex> lock(gate);
        for (auto it = live.begin(); it != live.end();)
            if (!issuer->IsCurrentSecureComputeCertificate((*it)->getPrepared()->getCertificate())) it = live.erase(it); else ++it;
        for (auto it = observed.begin(); it != observed.end();)
            if (live.find(it->first) == live.end()) it = observed.erase(it); else ++it;
        for (auto it = invalidSources.begin(); it != invalidSources.end();)
            if (live.find(*it) == live.end()) it = invalidSources.erase(it); else ++it;
        for (auto it = issued.begin(); it != issued.end();)
            if (!issuer->IsCurrentSecureComputeCertificate(*it)) it = issued.erase(it); else ++it;
    }
    void SecureComputeNoEffectCompletionOwner::InvalidateAfterRestore()
    {
        std::scoped_lock<std::mutex> lock(gate);
        live.clear(); observed.clear(); invalidSources.clear(); issued.clear(); CommitOwner->InvalidateAfterRestore();
    }
}


