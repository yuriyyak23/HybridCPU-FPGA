#pragma once
#include "SecureRevocationEpoch.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureGrantHandleKind : unsigned char
	{
		None = 0,
		DomainIdentity = 1,
		MemoryPolicy = 2,
		EvidencePolicy = 3,
		MigrationPolicy = 4,
		IoPolicy = 5,
		HypercallPolicy = 6,
		DebugPolicy = 7,
		CompatibilityProjectionPolicy = 8
	};


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
}
