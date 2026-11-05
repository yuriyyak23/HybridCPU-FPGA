#pragma once
#include "../../Runtime/Completion/Commit/ArchitecturalCompletionCommitOwner.h"
#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core::Registers { class ArchContextState; }
namespace YAKSys_Hybrid_CPU::Core::Registers::Retire { class RetireCoordinator; }
namespace YAKSys_Hybrid_CPU::Core {
class SecureComputeRegisterLocalRetirement;

// Holds the exact core-side owners and checks freshness under the established gate order.
class SecureComputeLiveCoreTarget final {
    std::mutex& gate_;
    std::function<bool()> current_;
    int virtualThread_;
    std::vector<std::shared_ptr<Registers::ArchContextState>> contexts_;
    std::shared_ptr<Registers::Retire::RetireCoordinator> retire_;
    std::shared_ptr<ArchitecturalCompletionCommitOwner> commit_;
    std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer_;
    std::mutex* sourceParentGate_;
    std::mutex* sourceChildGate_;
    std::mutex* memoryGate_;
public:
    SecureComputeLiveCoreTarget(std::mutex& gate, std::function<bool()> current, int virtualThread,
        std::vector<std::shared_ptr<Registers::ArchContextState>> contexts,
        std::shared_ptr<Registers::Retire::RetireCoordinator> retire,
        std::shared_ptr<ArchitecturalCompletionCommitOwner> commit,
        std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer,
        std::mutex* sourceParentGate = nullptr, std::mutex* sourceChildGate = nullptr,
        std::mutex* memoryGate = nullptr)
        : gate_(gate), current_(std::move(current)), virtualThread_(virtualThread),
          contexts_(std::move(contexts)), retire_(std::move(retire)), commit_(std::move(commit)),
          producer_(std::move(producer)), sourceParentGate_(sourceParentGate),
          sourceChildGate_(sourceChildGate), memoryGate_(memoryGate) {}

    bool isCurrent() const { return current_(); }
    int getVirtualThread() const { return virtualThread_; }
    const std::vector<std::shared_ptr<Registers::ArchContextState>>& getContexts() const { return contexts_; }
    std::shared_ptr<Registers::Retire::RetireCoordinator> getRetire() const { return retire_; }
    std::shared_ptr<ArchitecturalCompletionCommitOwner> getCommit() const { return commit_; }
    std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> getProducer() const { return producer_; }

    std::shared_ptr<SecureComputeRegisterLocalRetirement> CommitIfCurrent(
        const std::function<std::shared_ptr<SecureComputeRegisterLocalRetirement>()>& action) {
        std::lock_guard<std::mutex> gateLock(gate_);
        auto commitWithMemory = [&]() {
            if (!memoryGate_) return isCurrent() ? action() : nullptr;
            std::lock_guard<std::mutex> lock(*memoryGate_);
            return isCurrent() ? action() : nullptr;
        };
        auto commitWithChild = [&]() {
            if (!sourceChildGate_) return commitWithMemory();
            std::lock_guard<std::mutex> lock(*sourceChildGate_);
            return commitWithMemory();
        };
        if (!sourceParentGate_) return commitWithChild();
        std::lock_guard<std::mutex> lock(*sourceParentGate_);
        return commitWithChild();
    }
};
}