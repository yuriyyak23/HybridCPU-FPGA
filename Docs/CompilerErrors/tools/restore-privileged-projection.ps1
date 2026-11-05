$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\SecureCompute\Compatibility\Projection'
$p=Join-Path $base 'PrivilegedExecutionStateProjectionService.h'; $s=[IO.File]::ReadAllText($p)
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); $end=$s.IndexOf('class PrivilegedExecutionStateProjectionService final',$i); if($i -lt 0 -or $end -lt 0){throw 'anchor missing'}
$def=@'
struct PrivilegedExecutionStateProjectionRequest {
    const PrivilegedControlRegisterKind Register; const std::shared_ptr<PrivilegedExecutionStateDescriptor> Descriptor; const unsigned long long RuntimeDomainTag, RuntimeAddressSpaceTag; const PrivilegedExecutionStateEpoch CurrentEpoch; const bool SecureVisibilityAllowed, MigrationClassified, ConformanceProven;
    PrivilegedExecutionStateProjectionRequest(PrivilegedControlRegisterKind reg, std::shared_ptr<PrivilegedExecutionStateDescriptor> descriptor, unsigned long long domain, unsigned long long space, PrivilegedExecutionStateEpoch epoch, bool visibility, bool migration, bool conformance) : Register(reg), Descriptor(std::move(descriptor)), RuntimeDomainTag(domain), RuntimeAddressSpaceTag(space), CurrentEpoch(epoch), SecureVisibilityAllowed(visibility), MigrationClassified(migration), ConformanceProven(conformance) {}
};
struct PrivilegedExecutionStateProjectionResult {
    const PrivilegedExecutionStateProjectionDecision Decision; const std::shared_ptr<PrivilegedExecutionStateOwnerResult> OwnerAdmission; const bool ValueAvailable; const unsigned long long Value; const bool BackendSuccessAuthorized, MutationAuthorized, CompletionPublicationAuthorized, RetirePublicationAuthorized; const std::wstring Reason; const bool IsAllowed;
    PrivilegedExecutionStateProjectionResult(PrivilegedExecutionStateProjectionDecision decision, std::shared_ptr<PrivilegedExecutionStateOwnerResult> owner, bool available, unsigned long long value, bool backend, bool mutation, bool completion, bool retire, std::wstring reason) : Decision(decision), OwnerAdmission(std::move(owner)), ValueAvailable(available), Value(value), BackendSuccessAuthorized(backend), MutationAuthorized(mutation), CompletionPublicationAuthorized(completion), RetirePublicationAuthorized(retire), Reason(std::move(reason)), IsAllowed(decision == PrivilegedExecutionStateProjectionDecision::AllowedReadOnlyProjection && OwnerAdmission && OwnerAdmission->IsAllowed && available && !backend && !mutation && !completion && !retire) {}
    static std::shared_ptr<PrivilegedExecutionStateProjectionResult> Denied(PrivilegedExecutionStateProjectionDecision decision, std::shared_ptr<PrivilegedExecutionStateOwnerResult> owner, std::wstring reason) { return std::make_shared<PrivilegedExecutionStateProjectionResult>(decision, std::move(owner), false, 0, false, false, false, false, std::move(reason)); }
};
'@
$s=$s.Substring(0,$i)+$def+"`r`n`t"+$s.Substring($end); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'PrivilegedExecutionStateProjectionService.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('request->Register is not (PrivilegedControlRegisterKind::GuestCr0 or PrivilegedControlRegisterKind::GuestCr4)','request->Register != PrivilegedControlRegisterKind::GuestCr0 && request->Register != PrivilegedControlRegisterKind::GuestCr4').Replace('request->Descriptor->Value','request->Descriptor').Replace('descriptor->GuestCr0->Value','descriptor->GuestCr0.Value').Replace('descriptor->GuestCr4->Value','descriptor->GuestCr4.Value')
$s=$s.Replace('ValueAvailable: true, value, BackendSuccessAuthorized: false, MutationAuthorized: false, CompletionPublicationAuthorized: false, RetirePublicationAuthorized: false, Reason:','true, value, false, false, false, false,')
$s=$s.Replace('std::format("Guest control-register projection denied by {0:s}.", visibility)','std::wstring(L"Guest control-register projection denied by ") + VisibilityName(visibility) + L"."')
$anchor='PrivilegedExecutionStateProjectionService::PrivilegedExecutionStateProjectionService()'
$fn=@'
namespace {
const wchar_t* VisibilityName(SecureComputeVmReadVisibilityDecision value) {
    switch (value) {
    case SecureComputeVmReadVisibilityDecision::AllowedReadOnlyProjection: return L"AllowedReadOnlyProjection";
    case SecureComputeVmReadVisibilityDecision::DeniedNoNeutralOwner: return L"DeniedNoNeutralOwner";
    case SecureComputeVmReadVisibilityDecision::DeniedNoReadOnlySource: return L"DeniedNoReadOnlySource";
    case SecureComputeVmReadVisibilityDecision::DeniedNoSecureVisibility: return L"DeniedNoSecureVisibility";
    case SecureComputeVmReadVisibilityDecision::DeniedNoMigrationClass: return L"DeniedNoMigrationClass";
    case SecureComputeVmReadVisibilityDecision::DeniedNoConformanceProof: return L"DeniedNoConformanceProof";
    default: return L"Unknown";
    }
}
}
'@
$s=$s.Replace($anchor,$fn+"`r`n`t"+$anchor)
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: The following 'switch expression'"); $end=$s.IndexOf("`n`t}",$i); if($i -lt 0 -or $end -lt 0){throw 'switch missing'}
$impl=@'
switch (decision) {
        case SecureComputeVmReadVisibilityDecision::DeniedNoSecureVisibility: return PrivilegedExecutionStateProjectionDecision::DeniedNoSecureVisibility;
        case SecureComputeVmReadVisibilityDecision::DeniedNoMigrationClass: return PrivilegedExecutionStateProjectionDecision::DeniedNoMigrationClass;
        case SecureComputeVmReadVisibilityDecision::DeniedNoConformanceProof: return PrivilegedExecutionStateProjectionDecision::DeniedNoConformanceProof;
        case SecureComputeVmReadVisibilityDecision::DeniedNoReadOnlySource: return PrivilegedExecutionStateProjectionDecision::DeniedNoReadOnlySource;
        default: return PrivilegedExecutionStateProjectionDecision::DeniedOwnerAdmission;
        }
'@
$s=$s.Substring(0,$i)+$impl+$s.Substring($end); [IO.File]::WriteAllText($p,$s)