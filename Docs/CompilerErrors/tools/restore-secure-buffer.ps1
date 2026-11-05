$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\SecureCompute'
$p=Join-Path $r 'Authority\SecureGrantHandle.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('struct SecureGrantHandle final')) {$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"SecureRevocationEpoch.h`""); $i=$s.LastIndexOf('}'); $s=$s.Insert($i,@"
struct SecureGrantHandle final {
    SecureGrantHandleKind Kind = SecureGrantHandleKind::None;
    unsigned long long LocalId = 0, ProvenanceHash = 0, Epoch = 0;
    bool hasScalarShape() const noexcept { return Kind != SecureGrantHandleKind::None && LocalId != 0 && Epoch != 0; }
    bool hasProvenance() const noexcept { return ProvenanceHash != 0; }
    bool isMaterialized() const noexcept { return hasScalarShape() && hasProvenance(); }
    bool MatchesEpoch(const SecureRevocationEpoch& epoch) const noexcept { return isMaterialized() && epoch.IsCurrent(Epoch); }
    static bool TryMaterializeFromGuestScalar(unsigned long long, SecureGrantHandle& handle) noexcept { handle = {}; return false; }
    bool operator==(const SecureGrantHandle&) const = default;
};
"@+"`r`n")}; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'Descriptors\Io\SecureIoDomainDescriptor.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('struct SecureSharedBufferDescriptor final')) { $s=$s.Replace('#pragma once',"#pragma once`r`n#include `"../../Authority/SecureGrantHandle.h`"`r`n#include `"../../Policies/Evidence/SecureEvidencePolicy.h`""); $anchor='// C# TO C++ CONVERTER TASK:'; $def=@"
struct SecureSharedBufferDescriptor final {
    unsigned long long BufferId = 0, Start = 0, Length = 0;
    SecureSharedBufferDirection Direction = SecureSharedBufferDirection::None;
    SecureGrantHandle Grant;
    SecureEvidenceVisibilityClass EvidenceClass = SecureEvidenceVisibilityClass::Denied;
    unsigned long long OwnerDomainTag = 0, LifetimeEpoch = 0;
    bool isMaterialized() const noexcept { return BufferId != 0 && Length != 0 && Direction != SecureSharedBufferDirection::None && Grant.isMaterialized() && EvidenceClass != SecureEvidenceVisibilityClass::Denied && OwnerDomainTag != 0 && LifetimeEpoch != 0; }
    bool Contains(unsigned long long address, unsigned long long length) const noexcept { if (!Length || !length || address < Start) return false; auto offset = address - Start; return offset < Length && length <= Length - offset; }
    bool AllowsDirection(SecureSharedBufferDirection required) const noexcept { return required != SecureSharedBufferDirection::None && (Direction == SecureSharedBufferDirection::Bidirectional || Direction == required); }
    bool IsOwnedBy(unsigned long long domain) const noexcept { return OwnerDomainTag != 0 && OwnerDomainTag == domain; }
    bool HasCurrentLifetime(const SecureRevocationEpoch& epoch) const noexcept { return epoch.IsCurrent(LifetimeEpoch); }
};
"@; $s=$s.Replace($anchor,$def+"`r`n"+$anchor) }; [IO.File]::WriteAllText($p,$s)
