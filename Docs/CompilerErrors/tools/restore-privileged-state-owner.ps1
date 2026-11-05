$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\SecureCompute\Descriptors\ExecutionState\PrivilegedExecutionStateDescriptor.h'
$s=@'
#pragma once
#include <limits>
namespace YAKSys_Hybrid_CPU::Core {
enum class PrivilegedControlRegisterKind : unsigned char { GuestCr0 = 1, GuestCr4 = 2 };
enum class PrivilegedExecutionStateEvidenceClass : unsigned char { Unclassified = 0, GuestVisibleReadOnlyProjection = 1, HostOwnedQuarantined = 2, CompatibilityAlias = 3 };
enum class PrivilegedExecutionStateMigrationClass : unsigned char { Unclassified = 0, RevalidatedAfterRestore = 1, DomainLocal = 2 };
struct PrivilegedControlRegisterValue { PrivilegedControlRegisterKind Kind{}; unsigned long long Value{}; };
struct PrivilegedExecutionStateEpoch {
    unsigned long long Current{};
    bool isMaterialized() const { return Current != 0; }
    bool IsCurrent(PrivilegedExecutionStateEpoch candidate) const { return isMaterialized() && candidate.isMaterialized() && Current == candidate.Current; }
};
struct PrivilegedControlRegisterLegalityPolicy {
    unsigned long long GuestCr0AllowedMask{}, GuestCr0RequiredMask{}, GuestCr4AllowedMask{}, GuestCr4RequiredMask{}; bool Materialized{};
    bool isMaterialized() const { return Materialized && (GuestCr0RequiredMask & ~GuestCr0AllowedMask) == 0 && (GuestCr4RequiredMask & ~GuestCr4AllowedMask) == 0; }
    unsigned long long AllowedMask(PrivilegedControlRegisterKind kind) const { return kind == PrivilegedControlRegisterKind::GuestCr0 ? GuestCr0AllowedMask : kind == PrivilegedControlRegisterKind::GuestCr4 ? GuestCr4AllowedMask : 0; }
    unsigned long long RequiredMask(PrivilegedControlRegisterKind kind) const { return kind == PrivilegedControlRegisterKind::GuestCr0 ? GuestCr0RequiredMask : kind == PrivilegedControlRegisterKind::GuestCr4 ? GuestCr4RequiredMask : std::numeric_limits<unsigned long long>::max(); }
    bool HasReservedBits(PrivilegedControlRegisterValue value) const { return (value.Value & ~AllowedMask(value.Kind)) != 0; }
    bool HasAllRequiredBits(PrivilegedControlRegisterValue value) const { auto mask=RequiredMask(value.Kind); return (value.Value & mask) == mask; }
};
struct PrivilegedExecutionStateDescriptor {
    unsigned long long DomainTag{}, AddressSpaceTag{}; PrivilegedExecutionStateEpoch PolicyEpoch{}; bool Materialized{};
    PrivilegedControlRegisterValue GuestCr0{}, GuestCr4{}; PrivilegedControlRegisterLegalityPolicy LegalityPolicy{};
    PrivilegedExecutionStateEvidenceClass EvidenceClass{}; PrivilegedExecutionStateMigrationClass MigrationClass{};
    bool isMaterialized() const { return Materialized && DomainTag != 0 && AddressSpaceTag != 0 && PolicyEpoch.isMaterialized() && LegalityPolicy.isMaterialized(); }
    bool hasCanonicalRegisterKinds() const { return GuestCr0.Kind == PrivilegedControlRegisterKind::GuestCr0 && GuestCr4.Kind == PrivilegedControlRegisterKind::GuestCr4; }
};
}
'@
[IO.File]::WriteAllText($p,$s)
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\SecureCompute\Policies\ExecutionState\PrivilegedExecutionStateOwnerPolicy.h'
$s=[IO.File]::ReadAllText($p); $s=$s.Replace('#include <string>',"#include `"../../Descriptors/ExecutionState/PrivilegedExecutionStateDescriptor.h`"`r`n#include <string>`r`n#include <utility>")
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); $end=$s.IndexOf('class PrivilegedExecutionStateOwnerPolicy final',$i); if($i -lt 0 -or $end -lt 0){throw 'anchor missing'}
$def=@'
struct PrivilegedExecutionStateOwnerRequest {
    std::shared_ptr<PrivilegedExecutionStateDescriptor> Descriptor; unsigned long long RuntimeDomainTag{}, RuntimeAddressSpaceTag{}; PrivilegedExecutionStateEpoch CurrentEpoch{};
    PrivilegedExecutionStateOwnerRequest(std::shared_ptr<PrivilegedExecutionStateDescriptor> descriptor, unsigned long long domain, unsigned long long space, PrivilegedExecutionStateEpoch epoch) : Descriptor(std::move(descriptor)), RuntimeDomainTag(domain), RuntimeAddressSpaceTag(space), CurrentEpoch(epoch) {}
};
struct PrivilegedExecutionStateOwnerResult {
    const PrivilegedExecutionStateOwnerDecision Decision; const bool OwnerAccepted, ReadOnlyProjectionAuthorized, MutationAuthorized, BackendExecutionAuthorized, CompletionPublicationAuthorized, RetirePublicationAuthorized; const std::wstring Reason; const bool IsAllowed;
    PrivilegedExecutionStateOwnerResult(PrivilegedExecutionStateOwnerDecision d, bool owner, bool projection, bool mutation, bool backend, bool completion, bool retire, std::wstring reason) : Decision(d), OwnerAccepted(owner), ReadOnlyProjectionAuthorized(projection), MutationAuthorized(mutation), BackendExecutionAuthorized(backend), CompletionPublicationAuthorized(completion), RetirePublicationAuthorized(retire), Reason(std::move(reason)), IsAllowed(d == PrivilegedExecutionStateOwnerDecision::AllowedOwnerMaterializedProjectionClosed && owner) {}
    static std::shared_ptr<PrivilegedExecutionStateOwnerResult> Denied(PrivilegedExecutionStateOwnerDecision d, std::wstring reason) { return std::make_shared<PrivilegedExecutionStateOwnerResult>(d, false, false, false, false, false, false, std::move(reason)); }
    static const std::shared_ptr<PrivilegedExecutionStateOwnerResult> AllowedOwnerOnly;
};
inline const std::shared_ptr<PrivilegedExecutionStateOwnerResult> PrivilegedExecutionStateOwnerResult::AllowedOwnerOnly = std::make_shared<PrivilegedExecutionStateOwnerResult>(PrivilegedExecutionStateOwnerDecision::AllowedOwnerMaterializedProjectionClosed, true, false, false, false, false, false, L"Neutral privileged execution-state ownership is accepted; projection, mutation, execution and publication remain closed.");
'@
$s=$s.Substring(0,$i)+$def+"`r`n`t"+$s.Substring($end); [IO.File]::WriteAllText($p,$s)
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\SecureCompute\Policies\ExecutionState\PrivilegedExecutionStateOwnerPolicy.cpp'; $s=[IO.File]::ReadAllText($p).Replace('request->Descriptor->Value','request->Descriptor').Replace('descriptor->IsMaterialized','descriptor->isMaterialized()').Replace('descriptor->HasCanonicalRegisterKinds','descriptor->hasCanonicalRegisterKinds()'); [IO.File]::WriteAllText($p,$s)