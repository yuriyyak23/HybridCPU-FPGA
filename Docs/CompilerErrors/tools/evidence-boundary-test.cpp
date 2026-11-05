#include "CloseToHSL/Core/Runtime/Evidence/EvidenceBoundaryRequirement.h"
#include "CloseToHSL/Core/Runtime/Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core;
int main(){ auto denied=EvidencePolicyDescriptor::getFailClosed(); auto allow=std::make_shared<EvidencePolicyDescriptor>(true,true,true); assert(EvidenceBoundaryRequirement::None.IsSatisfiedBy(nullptr)); assert(!EvidenceBoundaryRequirement::MigrationSerializableGuestState.IsSatisfiedBy(nullptr)); assert(!EvidenceBoundaryRequirement::MigrationSerializableGuestState.IsSatisfiedBy(denied)); assert(EvidenceBoundaryRequirement::MigrationSerializableGuestState.IsSatisfiedBy(allow)); assert(EvidenceBoundaryRequirement::GuestVisible(EvidenceVisibilityClass::CompatibilityAlias).IsSatisfiedBy(allow)); assert(!EvidenceBoundaryRequirement::GuestVisible(EvidenceVisibilityClass::HostOwnedRuntimeEvidence).IsSatisfiedBy(allow)); }
