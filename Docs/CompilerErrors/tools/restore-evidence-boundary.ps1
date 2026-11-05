$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Evidence'
$p=Join-Path $base 'EvidenceBoundaryRequirement.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('#pragma once',"#pragma once`r`n#include `"../Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h`"`r`n#include <memory>")
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); if($i -lt 0){throw 'anchor missing'}
$s=$s.Substring(0,$i)+@'
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
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'EvidenceBoundaryRequirement.cpp'; $s=@'
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
'@
[IO.File]::WriteAllText($p,$s)