#pragma once
#include "SecureRevocationEpoch.h"
#include <memory>
namespace YAKSys_Hybrid_CPU::Core {
struct SecureMeasurementHandle final {
    const unsigned long long MeasurementId, ProvenanceHash, Epoch;
    const bool IsMaterialized;
    SecureMeasurementHandle(unsigned long long id = 0, unsigned long long provenance = 0, unsigned long long epoch = 0)
        : MeasurementId(id), ProvenanceHash(provenance), Epoch(epoch), IsMaterialized(id != 0 && provenance != 0 && epoch != 0) {}
    bool MatchesEpoch(std::shared_ptr<SecureRevocationEpoch> epoch) const { return IsMaterialized && epoch && epoch->IsCurrent(Epoch); }
    static const std::shared_ptr<SecureMeasurementHandle> None;
};
inline const std::shared_ptr<SecureMeasurementHandle> SecureMeasurementHandle::None = std::make_shared<SecureMeasurementHandle>();
}