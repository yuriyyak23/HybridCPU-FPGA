#include "ArchContextState.h"
#include <limits>
#include <stdexcept>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core::Registers {
namespace {
int checkedCount(int n) { if (n < 1) throw std::out_of_range("committedRegisterCount"); return n; }
int checkedVt(int n) { if (n < 0) throw std::out_of_range("vtId"); return n; }
struct AccessGuard { std::shared_ptr<Disposable> access; ~AccessGuard() { if (access) access->dispose(); } };
}
ArchContextState::ArchContextState(int count, int vtId)
    : vtId_(checkedVt(vtId)), registers_(static_cast<std::size_t>(checkedCount(count))) {}
std::shared_ptr<ArchContextState::CommittedRegisterStorage> ArchContextState::getCommittedRegs() const {
    std::lock_guard<std::mutex> lock(gate_);
    auto view = view_.lock();
    if (!view) { view = std::make_shared<CommittedRegisterStorage>(const_cast<ArchContextState*>(this)->shared_from_this()); view_ = view; }
    return view;
}
unsigned long long ArchContextState::Read(const std::function<unsigned long long()>& read) const {
    std::shared_ptr<CoreContextBinding> owner;
    { std::lock_guard<std::mutex> lock(gate_); owner = binding_; if (!owner) return read(); }
    AccessGuard access{owner->Reserve(static_cast<CanonicalStateAccessKind>(0))};
    return read();
}
void ArchContextState::Write(ArchContextWriteKind kind, int index, unsigned long long value, std::function<void()> store) {
    std::shared_ptr<CoreContextBinding> owner;
    { std::lock_guard<std::mutex> lock(gate_); owner = binding_; if (!owner) { store(); return; } }
    owner->Publish(kind, index, value, std::move(store));
}
unsigned long long ArchContextState::getCommittedPc() const { return Read([this]{ return pc_; }); }
void ArchContextState::setCommittedPc(unsigned long long value) { Write(ArchContextWriteKind::Pc,0,value,[this,value]{ pc_=value; }); }
PrivilegeLevel ArchContextState::getCurrentPrivilege() const { return static_cast<PrivilegeLevel>(Read([this]{ return static_cast<unsigned long long>(static_cast<int>(privilege_)); })); }
void ArchContextState::setCurrentPrivilege(PrivilegeLevel value) { Write(ArchContextWriteKind::Privilege,0,static_cast<unsigned long long>(static_cast<int>(value)),[this,value]{ privilege_=value; }); }
bool ArchContextState::isVmxNonRoot() const { return Read([this]{ return nonRoot_ ? 1ULL : 0ULL; }) != 0; }
void ArchContextState::setVmxNonRoot(bool value) { Write(ArchContextWriteKind::Vmx,0,value ? 1ULL : 0ULL,[this,value]{ nonRoot_=value; }); }
bool ArchContextState::getHasCoreOwner() const { std::lock_guard<std::mutex> lock(gate_); return !!binding_; }
bool ArchContextState::BelongsTo(const std::shared_ptr<void>& core) const { std::lock_guard<std::mutex> lock(gate_); return binding_ && binding_->Core == core; }
void ArchContextState::AttachCore(std::shared_ptr<CoreContextBinding> owner) {
    if (!owner || !owner->Core || !owner->Reserve || !owner->Publish) throw std::invalid_argument("owner");
    std::unique_lock<std::mutex> lock(gate_, std::try_to_lock);
    if (!lock || binding_ || standaloneUsers_ != 0) throw std::logic_error("Context already owned or reserved.");
    binding_ = std::move(owner);
}
class ArchContextState::StandaloneReservation final : public Disposable {
    std::shared_ptr<ArchContextState> context_;
    bool disposed_ = false;
public:
    explicit StandaloneReservation(std::shared_ptr<ArchContextState> context) : context_(std::move(context)) {}
    ~StandaloneReservation() { if (!disposed_) { try { dispose(); } catch (...) {} } }
    void dispose() override {
        std::lock_guard<std::mutex> lock(context_->gate_);
        if (disposed_) return;
        if (context_->standaloneUsers_ <= 0) throw std::logic_error("Standalone context reservation lost its owner.");
        --context_->standaloneUsers_; disposed_ = true;
    }
};
std::shared_ptr<Disposable> ArchContextState::ReserveStandaloneRetirement() {
    std::lock_guard<std::mutex> lock(gate_);
    if (binding_) throw std::logic_error("Context already has a core owner.");
    if (standaloneUsers_ == std::numeric_limits<int>::max()) throw std::overflow_error("Reservation depth");
    auto reservation = std::make_shared<StandaloneReservation>(shared_from_this());
    ++standaloneUsers_; return reservation;
}
void ArchContextState::CommittedRegisterStorage::Check(int index) const {
    if (index < 0 || static_cast<std::size_t>(index) >= context_->registers_.size()) throw std::out_of_range("register index");
}
int ArchContextState::CommittedRegisterStorage::getLength() const noexcept { return static_cast<int>(context_->registers_.size()); }
unsigned long long ArchContextState::CommittedRegisterStorage::operator[](int index) const {
    Check(index); return context_->Read([this,index]{ return context_->registers_[index]; });
}
void ArchContextState::CommittedRegisterStorage::setIndexer(int index, unsigned long long value) {
    Check(index); context_->Write(ArchContextWriteKind::Register,index,value,[this,index,value]{ context_->registers_[index]=value; });
}
std::vector<unsigned long long> ArchContextState::CommittedRegisterStorage::Clone() const {
    std::vector<unsigned long long> result; result.reserve(context_->registers_.size());
    for (int i=0; i<getLength(); ++i) result.push_back((*this)[i]);
    return result;
}
}