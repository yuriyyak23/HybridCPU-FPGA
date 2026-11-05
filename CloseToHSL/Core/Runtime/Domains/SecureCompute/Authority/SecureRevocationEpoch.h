#pragma once
#include <memory>
namespace YAKSys_Hybrid_CPU::Core {
struct SecureRevocationEpoch final {
    const unsigned long long Current;
    explicit SecureRevocationEpoch(unsigned long long current = 0) : Current(current) {}
    bool isMaterialized() const noexcept { return Current != 0; }
    bool IsCurrent(unsigned long long candidate) const noexcept { return isMaterialized() && candidate == Current; }
    bool IsStale(unsigned long long candidate) const noexcept { return !IsCurrent(candidate); }
    bool Equals(const std::shared_ptr<SecureRevocationEpoch>& other) const noexcept { return other && Current == other->Current; }
    static const std::shared_ptr<SecureRevocationEpoch> Unmaterialized;
};
inline const std::shared_ptr<SecureRevocationEpoch> SecureRevocationEpoch::Unmaterialized = std::make_shared<SecureRevocationEpoch>();
}
