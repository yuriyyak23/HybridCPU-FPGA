#pragma once
#include <limits>
#include <memory>
namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute {
class DmaStreamComputeProgressDiagnostics final {
    static unsigned long long SaturatingAdd(unsigned long long a, unsigned long long b) noexcept {
        const auto max = std::numeric_limits<unsigned long long>::max();
        return b > max - a ? max : a + b;
    }
public:
    const unsigned long long BytesRead, BytesStaged, ElementOperations, ModeledLatencyCycles, BackendStepCount;
    DmaStreamComputeProgressDiagnostics(unsigned long long read, unsigned long long staged, unsigned long long operations, unsigned long long latency, unsigned long long steps)
        : BytesRead(read), BytesStaged(staged), ElementOperations(operations), ModeledLatencyCycles(latency), BackendStepCount(steps) {}
    bool isAuthoritative() const noexcept { return false; }
    bool getCanIssueToken() const noexcept { return false; }
    bool getCanSetSucceeded() const noexcept { return false; }
    bool getCanSetCommitted() const noexcept { return false; }
    bool getCanPublishMemory() const noexcept { return false; }
    bool isRetirePublication() const noexcept { return false; }
    std::shared_ptr<DmaStreamComputeProgressDiagnostics> Add(unsigned long long read, unsigned long long staged, unsigned long long operations, unsigned long long latency, unsigned long long steps) const {
        return std::make_shared<DmaStreamComputeProgressDiagnostics>(SaturatingAdd(BytesRead, read), SaturatingAdd(BytesStaged, staged), SaturatingAdd(ElementOperations, operations), SaturatingAdd(ModeledLatencyCycles, latency), SaturatingAdd(BackendStepCount, steps));
    }
    static const std::shared_ptr<DmaStreamComputeProgressDiagnostics> Empty;
};
inline const std::shared_ptr<DmaStreamComputeProgressDiagnostics> DmaStreamComputeProgressDiagnostics::Empty = std::make_shared<DmaStreamComputeProgressDiagnostics>(0, 0, 0, 0, 0);
}
