#include "EvidenceBoundaryRequirement.h"
#include "../Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
namespace YAKSys_Hybrid_CPU::Core {
bool EvidenceBoundaryRequirement::IsSatisfiedBy(std::shared_ptr<EvidencePolicyDescriptor> policy) const {
    if (!RequiresGuestVisibility && !RequiresMigrationSerialization) return true;
    if (!policy) return false;
    if (RequiresGuestVisibility && !policy->CanExposeToGuest(EvidenceClass)) return false;
    if (RequiresMigrationSerialization && !policy->CanSerializeAcrossMigration(EvidenceClass, RestorePolicy)) return false;
    return !policy->MustRecomputeAfterRestore(EvidenceClass);
}
}