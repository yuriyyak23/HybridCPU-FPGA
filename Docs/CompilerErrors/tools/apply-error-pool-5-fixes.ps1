$ErrorActionPreference = 'Stop'
$root = 'C:\Users\Yuriy\Desktop\HybridCPU_FPGA'

$memoryContract = Join-Path $root 'ExternalRuntime\\Contracts\ExternalSecureMemoryRead.h'
$memoryProvider = Join-Path $root 'ExternalRuntime\\Contracts\ExternalSecureMemoryReadProvider.h'
$memoryHeader = Join-Path $root 'CloseToHSL\\Core\Runtime\Completion\Records\SecureComputeMemoryReadCompletionOwner.h'
$memoryCpp = Join-Path $root 'CloseToHSL\\Core\Runtime\Completion\Records\SecureComputeMemoryReadCompletionOwner.cpp'
$noEffectCpp = Join-Path $root 'CloseToHSL\\Core\Runtime\Completion\Records\SecureComputeNoEffectCompletionOwner.cpp'
$safetyCpp = Join-Path $root 'CloseToHSL\\Core\Pipeline\Safety\YAKSys_Hybrid_CPU.Core.SafetyVerifier.cpp'

# Ensure the contract arithmetic uses an explicit limit without signed wraparound.
$contractText = Get-Content -LiteralPath $memoryContract -Raw
if (-not $contractText.Contains('#include <limits>')) { $contractText = $contractText.Replace('#include <memory>', '#include <memory>' + "`r`n#include <limits>") }
Set-Content -LiteralPath $memoryContract -Value $contractText -Encoding utf8
Set-Content -LiteralPath $memoryProvider -Value @'
#pragma once

// Compatibility include retained for existing project files.
#include "ExternalSecureMemoryRead.h"
'@ -Encoding utf8

$h = Get-Content -LiteralPath $memoryHeader -Raw
if (-not $h.Contains('#include "../../../../../ExternalRuntime/Contracts/ExternalSecureMemoryRead.h"')) {
    $anchor = '#include "../../../Pipeline/Safety/SecureComputeMemoryReadAdmission.h"'
    $h = $h.Replace($anchor, $anchor + "`r`n#include ""../../../../../ExternalRuntime/Contracts/ExternalSecureMemoryRead.h""")
}
$h = $h.Replace('std::unordered_map<std::shared_ptr<ExternalSecureDomainLease>, unsigned long long> domains = std::unordered_map<std::shared_ptr<ExternalSecureDomainLease>, unsigned long long>();', 'std::vector<std::pair<ExternalSecureDomainLease, unsigned long long>> domains;')
$h = $h.Replace('SecureComputeMemoryReadCompletionOwner(std::shared_ptr<SafetyVerifier> issuer, std::any gate);', 'SecureComputeMemoryReadCompletionOwner(std::shared_ptr<SafetyVerifier> issuer, std::mutex& issuerGate);')
Set-Content -LiteralPath $memoryHeader -Value $h -Encoding utf8

Set-Content -LiteralPath $memoryCpp -Value @'
#include "SecureComputeMemoryReadCompletionOwner.h"
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
#include "../../../Architecture/Registers/Physical/PhysicalRegisterFile.h"
#include "../Commit/NeutralArchitecturalCompletionFacts.h"
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
        template<class T> class DisposeGuard final
        {
            std::shared_ptr<T> value_;
        public:
            explicit DisposeGuard(std::shared_ptr<T> value) : value_(std::move(value)) {}
            ~DisposeGuard() noexcept { if (value_) { try { value_->dispose(); } catch (...) {} } }
        };
    }

    SecureComputeMemoryReadCompletionOwner::SecureComputeMemoryReadCompletionOwner(
        std::shared_ptr<SafetyVerifier> issuer, std::mutex&)
        : issuer(issuer ? std::move(issuer) : throw std::invalid_argument("issuer")),
          producer(commit->RegisterProducer(std::make_shared<ArchitecturalCompletionProducerPolicy>(
              ArchitecturalCompletionProducerPolicy{L"SecureComputeStandaloneMemoryRead",
                  NeutralArchitecturalCompletionClass::SecureRegisterLocal, false, false,
                  NeutralFaultAddressSemantic::None, NeutralFaultAuxiliarySemantic::None})))
    {
    }

    std::shared_ptr<SecureComputeMemoryReadCompletionOwner::Pending>
    SecureComputeMemoryReadCompletionOwner::Begin(
        std::shared_ptr<SecureComputeMemoryReadCertificate> certificate,
        std::shared_ptr<SecureComputeMemoryReadIssuePermit> permit,
        std::shared_ptr<ExternalSecureMemoryReadSubmission> submission)
    {
        if (!certificate || !permit || !submission || !certificate->getRequest()) return nullptr;
        std::scoped_lock<std::mutex> lock(gate);
        const auto attempt = certificate->Attempt;
        if (!attempt || !*attempt || !issuer->HasConsumedMemoryReadPermit(permit) ||
            permit->getCertificate() != certificate || submission->Attempt != *attempt ||
            live.size() >= 8 || nextGeneration == std::numeric_limits<unsigned long long>::max() ||
            nextDomain == std::numeric_limits<unsigned long long>::max() || !issued.insert(certificate).second)
            return nullptr;

        const auto domain = certificate->getRequest()->Binding.SecureDomain;
        auto domainEntry = std::find_if(domains.begin(), domains.end(), [&domain](const auto& entry)
        { return entry.first.Equals(domain); });
        unsigned long long scope = 0;
        if (domainEntry == domains.end()) { scope = nextDomain++; domains.emplace_back(domain, scope); }
        else scope = domainEntry->second;

        int slot = -1;
        for (int candidate = 0; candidate < 8; ++candidate)
        {
            const bool occupied = std::any_of(live.begin(), live.end(), [candidate](const auto& pending)
            { return pending && pending->Slot == candidate; });
            if (!occupied) { slot = candidate; break; }
        }
        if (slot < 0) return nullptr;

        auto pending = std::make_shared<Pending>(certificate, permit, submission, slot, nextGeneration++, scope);
        pending->Contexts.at(1)->getCommittedRegs()->setIndexer(1, certificate->getRequest()->Address);
        live.insert(pending);
        return pending;
    }

    std::shared_ptr<SecureComputeMemoryReadRetirement>
    SecureComputeMemoryReadCompletionOwner::AcceptAndRetire(
        std::shared_ptr<Pending> pending, std::shared_ptr<ExternalSecureMemoryReadReceipt> receipt)
    {
        if (!pending || !receipt) return nullptr;
        std::scoped_lock<std::mutex> lock(gate);
        const auto certificate = pending->Certificate;
        const auto origin = certificate ? certificate->getOrigin() : nullptr;
        const auto registers = pending->Contexts.at(1)->getCommittedRegs();
        if (live.find(pending) == live.end() || pending->Accepted ||
            nextAcceptance == std::numeric_limits<unsigned long long>::max() ||
            !issuer->HasConsumedMemoryReadPermit(pending->Permit) || receipt->Submission != pending->Submission ||
            !origin || !origin->IsOriginalRead(receipt) || (*registers)[1] != certificate->getRequest()->Address)
            return nullptr;

        // Acceptance owns no effect. Canonical retire separately consumes the exact token below.
        auto acceptance = std::make_shared<Acceptance>(pending, receipt, nextAcceptance++);
        pending->Accepted = acceptance;
        return Retire(pending, acceptance);
    }

    std::shared_ptr<SecureComputeMemoryReadRetirement>
    SecureComputeMemoryReadCompletionOwner::Retire(
        std::shared_ptr<Pending> pending, std::shared_ptr<Acceptance> acceptance)
    {
        if (!pending || !acceptance) return nullptr;
        const auto certificate = pending->Certificate;
        const auto origin = certificate ? certificate->getOrigin() : nullptr;
        const bool hasOlderLiveGeneration = std::any_of(live.begin(), live.end(), [&pending](const auto& item)
        { return item && item->Generation < pending->Generation; });
        if (live.find(pending) == live.end() || pending->Accepted != acceptance ||
            acceptance->Pending != pending || !issuer->HasConsumedMemoryReadPermit(pending->Permit) ||
            !origin || !origin->IsOriginalRead(acceptance->Receipt) || hasOlderLiveGeneration)
            return nullptr;

        const auto issuedAttempt = certificate->getIssued();
        const auto attemptValue = certificate->Attempt;
        if (!issuedAttempt || !attemptValue || !*attemptValue) return nullptr;
        const auto attempt = *attemptValue;

        std::vector<RetireRecord> records{RetireRecord::RegisterWrite(1, 3, acceptance->Receipt->Value)};
        if (!pending->Retire)
        {
            pending->Retire = std::make_shared<RetireCoordinator>(std::make_shared<PhysicalRegisterFile>(),
                std::make_shared<RenameMap>(4), std::make_shared<CommitMap>(4),
                std::make_shared<ArchContextCollection>(pending->Contexts));
        }
        const auto retire = pending->Retire;
        retire->Prevalidate(std::span<const RetireRecord>(records));

        const NeutralArchitecturalCompletionFacts facts{
            NeutralArchitecturalCompletionClass::SecureRegisterLocal, {}, {}, {}, {}};
        const auto candidate = std::make_shared<ArchitecturalCompletionCandidate>(
            ArchitecturalCompletionCandidate{pending->DomainScope, 1, 1, attempt->CpuAttempt,
                pending->Generation, facts, {}});
        std::vector<std::shared_ptr<ArchitecturalCompletionCandidate>> candidates{candidate};
        auto prepared = commit->PrepareCanonicalRetireWindow(producer,
            std::span<const std::shared_ptr<ArchitecturalCompletionCandidate>>(candidates));
        if (!prepared->isPrepared() || !prepared->Lease) return nullptr;

        const auto lease = prepared->Lease;
        DisposeGuard<ArchitecturalCompletionCommitOwner::CanonicalRetireWindowCommitLease> leaseGuard(lease);
        issuedAttempt->CompleteScalarRegisterWrite(records[0]);
        retire->Retire(std::span<const RetireRecord>(records));
        const auto completed = lease->Complete().at(0);
        if (!completed->isCommitted() || !completed->Receipt)
            throw std::logic_error("Prepared memory-load canonical commit failed.");
        const auto binding = completed->Receipt->getBinding();
        if (!commit->TryConsumeReceipt(completed->Receipt, binding))
            throw std::logic_error("Canonical memory-read receipt was not original/live.");

        live.erase(pending);
        const auto committedRegisters = pending->Contexts.at(1)->getCommittedRegs();
        return std::make_shared<SecureComputeMemoryReadRetirement>(acceptance->Receipt,
            acceptance->Sequence, binding->CommitSequence, (*committedRegisters)[3],
            issuedAttempt->getScalarRegisterWriteEffect());
    }

    void SecureComputeMemoryReadCompletionOwner::Cancel(std::shared_ptr<Pending> pending)
    {
        if (!pending) return;
        std::scoped_lock<std::mutex> lock(gate);
        live.erase(pending);
        issued.erase(pending->Certificate);
    }
}
'@ -Encoding utf8

$noEffectText = Get-Content -LiteralPath $noEffectCpp -Raw
$noEffectText = $noEffectText.Replace('scheduled->getAdmission()->SourceProvenance.SourceSlotIndex', 'scheduled->getAdmission()->SourceProvenance->SourceSlotIndex')
$candidateOld = @'
        auto facts = std::make_shared<NeutralArchitecturalCompletionFacts>(NeutralArchitecturalCompletionClass::SecureNoEffect,
            NeutralScalarFact::Absent, NeutralScalarFact::Absent, NeutralAddressFact::Absent, NeutralAuxiliaryFact::Absent);
        auto candidate = std::make_shared<ArchitecturalCompletionCandidate>(pending->getDomainScope(), attempt->OwnerContext,
            attempt->VirtualThread, attempt->CpuAttempt, pending->getSubmission()->CompletionGeneration, facts);
'@
$candidateNew = @'
        const NeutralArchitecturalCompletionFacts facts{NeutralArchitecturalCompletionClass::SecureNoEffect, {}, {}, {}, {}};
        auto candidate = std::make_shared<ArchitecturalCompletionCandidate>(ArchitecturalCompletionCandidate{
            pending->getDomainScope(), attempt->OwnerContext, attempt->VirtualThread, attempt->CpuAttempt,
            pending->getSubmission()->CompletionGeneration, facts, {}});
'@
if ($noEffectText.Contains($candidateOld)) { $noEffectText = $noEffectText.Replace($candidateOld, $candidateNew) }
$noEffectText = $noEffectText.Replace('commit->IsPrepared', 'commit->isPrepared()')
$noEffectText = $noEffectText.Replace('committed->IsCommitted', 'committed->isCommitted()')
$noEffectText = $noEffectText.Replace('auto binding = committed->Receipt->Binding;', 'auto binding = committed->Receipt->getBinding();')
Set-Content -LiteralPath $noEffectCpp -Value $noEffectText -Encoding utf8

# Fix the value access in the related verifier path to match the reference-held region contract.
$verify = Get-Content -LiteralPath $safetyCpp -Raw
$verify = $verify.Replace('request->Region.Address', 'request->Region->Address')
Set-Content -LiteralPath $safetyCpp -Value $verify -Encoding utf8

Write-Output 'Applied pool 5 contract and owner-port fixes. No build or compilation was run.'
