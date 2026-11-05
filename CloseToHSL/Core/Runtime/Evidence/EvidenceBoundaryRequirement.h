#pragma once
#include "../Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include <memory>

// Description: Neutral evidence-boundary requirement for guest visibility and migration-safe serialization policy.
namespace YAKSys_Hybrid_CPU::Core
{

class EvidencePolicyDescriptor;
struct EvidenceBoundaryRequirement {
    EvidenceVisibilityClass EvidenceClass = EvidenceVisibilityClass::GuestArchitecturalState;
    EvidenceRestorePolicy RestorePolicy = EvidenceRestorePolicy::RecomputeAfterRestore;
    bool RequiresGuestVisibility = false, RequiresMigrationSerialization = false;
    static const EvidenceBoundaryRequirement None;
    static const EvidenceBoundaryRequirement MigrationSerializableGuestState;
    static EvidenceBoundaryRequirement GuestVisible(EvidenceVisibilityClass evidenceClass) { return {evidenceClass, EvidenceRestorePolicy::RecomputeAfterRestore, true, false}; }
    bool IsSatisfiedBy(std::shared_ptr<EvidencePolicyDescriptor> policy) const;
};
inline const EvidenceBoundaryRequirement EvidenceBoundaryRequirement::None{};
inline const EvidenceBoundaryRequirement EvidenceBoundaryRequirement::MigrationSerializableGuestState{EvidenceVisibilityClass::GuestArchitecturalState, EvidenceRestorePolicy::PreserveGuestArchitecturalState, true, true};
}