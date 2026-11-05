#pragma once
#include <memory>
namespace HybridCPU::ExternalRuntime::Contracts {
class ExternalSecureExecutionAttempt;
class ExternalSecureExecutionConsumptionResult;
// Provider methods retain the original attempt; neither call authorizes execution.
class IExternalSecureExecutionConsumptionProvider {
public:
    virtual ~IExternalSecureExecutionConsumptionProvider() = default;
    virtual std::shared_ptr<ExternalSecureExecutionConsumptionResult> Consume(std::shared_ptr<ExternalSecureExecutionAttempt> attempt) = 0;
    virtual std::shared_ptr<ExternalSecureExecutionConsumptionResult> QueryConsumption(std::shared_ptr<ExternalSecureExecutionAttempt> attempt) = 0;
};
}