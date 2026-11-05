#pragma once
#include "ExternalSecureRegisterLocalContracts.h"
#include <memory>
#include <stdexcept>
#include <utility>
namespace HybridCPU::ExternalRuntime::Contracts
{
    // Bounded transport submission; it carries no execution or architectural authority.
    class ExternalSecureNoEffectSubmission final
    {
    public:
        const std::shared_ptr<ExternalSecureExecutionConsumptionReceipt> Consumption;
        const std::shared_ptr<ExternalSecureExecutionAttempt> Attempt;
        const int CompletionSlot;
        const unsigned long long CompletionGeneration;
        const int ExpectedDestination;
        static constexpr unsigned int SchemaVersion = 1;
        ExternalSecureNoEffectSubmission(std::shared_ptr<ExternalSecureExecutionConsumptionReceipt> consumption,
            int completionSlot, unsigned long long completionGeneration, int expectedDestination = 3,
            unsigned int schemaVersion = 1)
            : Consumption(std::move(consumption)), Attempt(Consumption ? Consumption->Attempt : nullptr),
              CompletionSlot(completionSlot), CompletionGeneration(completionGeneration), ExpectedDestination(expectedDestination)
        {
            if (!Consumption) throw std::invalid_argument("consumption");
            if (schemaVersion != SchemaVersion) throw std::out_of_range("schemaVersion");
            if (CompletionSlot < 0 || CompletionSlot >= 8 || CompletionGeneration == 0 || ExpectedDestination != 3)
                throw std::invalid_argument("Exact bounded no-effect completion slot/generation/destination required.");
        }
        std::shared_ptr<ExternalSecureExecutionConsumptionReceipt> getConsumption() const { return Consumption; }
        std::shared_ptr<ExternalSecureExecutionAttempt> getAttempt() const { return Attempt; }
        int getCompletionSlot() const noexcept { return CompletionSlot; }
        unsigned long long getCompletionGeneration() const noexcept { return CompletionGeneration; }
        int getExpectedDestination() const noexcept { return ExpectedDestination; }
        unsigned int getSchemaVersion() const noexcept { return SchemaVersion; }
    };

    class ExternalSecureNoEffectExecutionReceipt final
    {
    public:
        const std::shared_ptr<ExternalSecureNoEffectSubmission> Submission;
        const int Destination;
        const int EffectCount;
        const int CompletionSlot;
        const unsigned long long CompletionGeneration;
        ExternalSecureNoEffectExecutionReceipt(std::shared_ptr<ExternalSecureNoEffectSubmission> submission,
            int destination, int effectCount, int completionSlot, unsigned long long completionGeneration)
            : Submission(std::move(submission)), Destination(destination), EffectCount(effectCount),
              CompletionSlot(completionSlot), CompletionGeneration(completionGeneration)
        {
            if (!Submission) throw std::invalid_argument("submission");
            if (Destination < 0 || Destination >= 32 || EffectCount < 0 || EffectCount > 1 ||
                CompletionSlot < 0 || CompletionSlot >= 8 || CompletionGeneration == 0)
                throw std::invalid_argument("Bounded execution receipt facts required.");
        }
        std::shared_ptr<ExternalSecureNoEffectSubmission> getSubmission() const { return Submission; }
        int getDestination() const noexcept { return Destination; }
        int getEffectCount() const noexcept { return EffectCount; }
        int getCompletionSlot() const noexcept { return CompletionSlot; }
        unsigned long long getCompletionGeneration() const noexcept { return CompletionGeneration; }
    };

    class ExternalSecureNoEffectExecutionResult final
    {
    public:
        const std::shared_ptr<ExternalSecureNoEffectSubmission> Submission;
        const ExternalSecureDomainProviderStatus Status;
        const std::shared_ptr<ExternalSecureNoEffectExecutionReceipt> Receipt;
        ExternalSecureNoEffectExecutionResult(std::shared_ptr<ExternalSecureNoEffectSubmission> submission,
            ExternalSecureDomainProviderStatus status, std::shared_ptr<ExternalSecureNoEffectExecutionReceipt> receipt = nullptr)
            : Submission(std::move(submission)), Status(status), Receipt(std::move(receipt))
        {
            if (!Submission) throw std::invalid_argument("submission");
            if (!IsDefined(Status) || (Status == ExternalSecureDomainProviderStatus::Receipt) != static_cast<bool>(Receipt) ||
                (Receipt && Receipt->Submission != Submission))
                throw std::invalid_argument("Exact original submission and receipt status must agree.");
        }
        std::shared_ptr<ExternalSecureNoEffectSubmission> getSubmission() const { return Submission; }
        ExternalSecureDomainProviderStatus getStatus() const noexcept { return Status; }
        std::shared_ptr<ExternalSecureNoEffectExecutionReceipt> getReceipt() const { return Receipt; }
    };

    class IExternalSecureNoEffectExecutionProvider
    {
    public:
        virtual ~IExternalSecureNoEffectExecutionProvider() = default;
        virtual std::shared_ptr<ExternalSecureNoEffectExecutionResult> SubmitNoEffect(
            std::shared_ptr<ExternalSecureNoEffectSubmission> submission) = 0;
        virtual std::shared_ptr<ExternalSecureNoEffectExecutionResult> QueryNoEffect(
            std::shared_ptr<ExternalSecureNoEffectSubmission> submission) = 0;
    };
}
