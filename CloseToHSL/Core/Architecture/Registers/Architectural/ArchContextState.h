#pragma once
#include "ArchContextCollection.h"
#include "../CSR/CsrAccessPolicy.h"
#include "../../../../../tangible_disposable_interface.h"
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core::Registers {
enum class CanonicalStateAccessKind;
enum class ArchContextWriteKind { Register, Pc, Privilege, Vmx };

// Core is retained as an identity; callbacks preserve the original core's authority.
struct CoreContextBinding {
    std::shared_ptr<void> Core;
    std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> Reserve;
    std::function<void(ArchContextWriteKind, int, unsigned long long, std::function<void()>)> Publish;
    CoreContextBinding(std::shared_ptr<void> core,
        std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> reserve,
        std::function<void(ArchContextWriteKind, int, unsigned long long, std::function<void()>)> publish)
        : Core(std::move(core)), Reserve(std::move(reserve)), Publish(std::move(publish)) {}
};

class ArchContextState final : public std::enable_shared_from_this<ArchContextState> {
public:
    class CommittedRegisterStorage;
private:
    const int vtId_;
    mutable std::mutex gate_;
    std::shared_ptr<CoreContextBinding> binding_;
    int standaloneUsers_ = 0;
    std::vector<unsigned long long> registers_;
    unsigned long long pc_ = 0;
    PrivilegeLevel privilege_ = PrivilegeLevel::Machine;
    bool nonRoot_ = false;
    mutable std::weak_ptr<CommittedRegisterStorage> view_;
    unsigned long long Read(const std::function<unsigned long long()>& read) const;
    void Write(ArchContextWriteKind kind, int index, unsigned long long value, std::function<void()> store);
    class StandaloneReservation;
public:
    ArchContextState(int committedRegisterCount, int vtId);
    int getVtId() const noexcept { return vtId_; }
    std::shared_ptr<CommittedRegisterStorage> getCommittedRegs() const;
    unsigned long long getCommittedPc() const;
    void setCommittedPc(unsigned long long value);
    PrivilegeLevel getCurrentPrivilege() const;
    void setCurrentPrivilege(PrivilegeLevel value);
    bool isVmxNonRoot() const;
    void setVmxNonRoot(bool value);
    bool getHasCoreOwner() const;
    bool BelongsTo(const std::shared_ptr<void>& core) const;
    void AttachCore(std::shared_ptr<CoreContextBinding> owner);
    std::shared_ptr<Disposable> ReserveStandaloneRetirement();

    class CommittedRegisterStorage final {
        std::shared_ptr<ArchContextState> context_;
        void Check(int index) const;
    public:
        explicit CommittedRegisterStorage(std::shared_ptr<ArchContextState> context) : context_(std::move(context)) {}
        int getLength() const noexcept;
        int getCount() const noexcept { return getLength(); }
        std::size_t size() const noexcept { return static_cast<std::size_t>(getLength()); }
        unsigned long long operator[](int index) const;
        void setIndexer(int index, unsigned long long value);
        std::vector<unsigned long long> Clone() const;
    };
};
}