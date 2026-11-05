#pragma once
#include <limits>
namespace YAKSys_Hybrid_CPU::Core {
enum class PrivilegedControlRegisterKind : unsigned char { GuestCr0 = 1, GuestCr4 = 2 };
enum class PrivilegedExecutionStateEvidenceClass : unsigned char { Unclassified = 0, GuestVisibleReadOnlyProjection = 1, HostOwnedQuarantined = 2, CompatibilityAlias = 3 };
enum class PrivilegedExecutionStateMigrationClass : unsigned char { Unclassified = 0, RevalidatedAfterRestore = 1, DomainLocal = 2 };
struct PrivilegedControlRegisterValue { const PrivilegedControlRegisterKind Kind{}; const unsigned long long Value{}; };
struct PrivilegedExecutionStateEpoch {
    const unsigned long long Current{};
    bool isMaterialized() const { return Current != 0; }
    bool IsCurrent(PrivilegedExecutionStateEpoch candidate) const { return isMaterialized() && candidate.isMaterialized() && Current == candidate.Current; }
};
struct PrivilegedControlRegisterLegalityPolicy {
    const unsigned long long GuestCr0AllowedMask{}, GuestCr0RequiredMask{}, GuestCr4AllowedMask{}, GuestCr4RequiredMask{}; const bool Materialized{};
    bool isMaterialized() const { return Materialized && (GuestCr0RequiredMask & ~GuestCr0AllowedMask) == 0 && (GuestCr4RequiredMask & ~GuestCr4AllowedMask) == 0; }
    unsigned long long AllowedMask(PrivilegedControlRegisterKind kind) const { return kind == PrivilegedControlRegisterKind::GuestCr0 ? GuestCr0AllowedMask : kind == PrivilegedControlRegisterKind::GuestCr4 ? GuestCr4AllowedMask : 0; }
    unsigned long long RequiredMask(PrivilegedControlRegisterKind kind) const { return kind == PrivilegedControlRegisterKind::GuestCr0 ? GuestCr0RequiredMask : kind == PrivilegedControlRegisterKind::GuestCr4 ? GuestCr4RequiredMask : (std::numeric_limits<unsigned long long>::max)(); }
    bool HasReservedBits(PrivilegedControlRegisterValue value) const { return (value.Value & ~AllowedMask(value.Kind)) != 0; }
    bool HasAllRequiredBits(PrivilegedControlRegisterValue value) const { auto mask=RequiredMask(value.Kind); return (value.Value & mask) == mask; }
};
struct PrivilegedExecutionStateDescriptor {
    const unsigned long long DomainTag{}, AddressSpaceTag{}; const PrivilegedExecutionStateEpoch PolicyEpoch{}; const bool Materialized{};
    const PrivilegedControlRegisterValue GuestCr0{}, GuestCr4{}; const PrivilegedControlRegisterLegalityPolicy LegalityPolicy{};
    const PrivilegedExecutionStateEvidenceClass EvidenceClass{}; const PrivilegedExecutionStateMigrationClass MigrationClass{};
    bool isMaterialized() const { return Materialized && DomainTag != 0 && AddressSpaceTag != 0 && PolicyEpoch.isMaterialized() && LegalityPolicy.isMaterialized(); }
    bool hasCanonicalRegisterKinds() const { return GuestCr0.Kind == PrivilegedControlRegisterKind::GuestCr0 && GuestCr4.Kind == PrivilegedControlRegisterKind::GuestCr4; }
};
}