#pragma once
#include "ExecutionPlacement.h"
#include "ExecutionBindingIdentity.h"
#include "../Pipeline/Safety/ResourceBitset.h"
#include <memory>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
struct LegacyAdmissionProjection final {
    const int VirtualThreadId, OwnerContextId;
    const unsigned long long DomainTag;
    const bool IsStealable;
    const std::vector<int> ReadRegisters, WriteRegisters;
    const std::shared_ptr<const ExecutionPlacement> Placement;
    const ResourceBitset ResourceMask;
    LegacyAdmissionProjection(int vt, int owner, unsigned long long domain, bool stealable,
        std::vector<int> reads, std::vector<int> writes, std::shared_ptr<const ExecutionPlacement> placement, ResourceBitset mask)
        : VirtualThreadId(vt), OwnerContextId(owner), DomainTag(domain), IsStealable(stealable),
          ReadRegisters(std::move(reads)), WriteRegisters(std::move(writes)), Placement(std::move(placement)), ResourceMask(mask) {
        if (!Placement) throw std::invalid_argument("placement must not be null");
    }
};
struct AdmissionProjectionFingerprint final {
    const std::wstring Value;
    explicit AdmissionProjectionFingerprint(std::wstring value) : Value(RequireBindingIdentity(std::move(value))) {}
    bool operator==(const AdmissionProjectionFingerprint &) const = default;
};
class LegacyProjectionGuard final {
public:
    static std::shared_ptr<AdmissionProjectionFingerprint> Capture(std::shared_ptr<LegacyAdmissionProjection> projection);
    static void EnsureCurrent(std::shared_ptr<AdmissionProjectionFingerprint> expected, std::shared_ptr<LegacyAdmissionProjection> current);
};
}
