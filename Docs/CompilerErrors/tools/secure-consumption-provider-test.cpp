#include "../../ExternalRuntime/Contracts/ExternalSecureExecutionConsumptionProvider.h"
#include <cassert>
using namespace HybridCPU::ExternalRuntime::Contracts;
namespace HybridCPU::ExternalRuntime::Contracts { class ExternalSecureExecutionAttempt {}; class ExternalSecureExecutionConsumptionResult {}; }
struct Provider final : IExternalSecureExecutionConsumptionProvider {
 std::shared_ptr<ExternalSecureExecutionAttempt> seen;
 std::shared_ptr<ExternalSecureExecutionConsumptionResult> Consume(std::shared_ptr<ExternalSecureExecutionAttempt> a) override { seen=a; return {}; }
 std::shared_ptr<ExternalSecureExecutionConsumptionResult> QueryConsumption(std::shared_ptr<ExternalSecureExecutionAttempt> a) override { assert(a==seen); return {}; }
};
int main(){ Provider p; IExternalSecureExecutionConsumptionProvider& base=p; auto attempt=std::make_shared<ExternalSecureExecutionAttempt>(); base.Consume(attempt); base.QueryConsumption(attempt); }
