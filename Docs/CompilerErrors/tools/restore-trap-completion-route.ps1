$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion'
$p=Join-Path $base 'Routing\TrapCompletionRoutePolicy.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include <memory>',"#include <memory>`r`n#include <string>`r`n#include <utility>`r`n#include `"../../Events/Traps/NeutralTrapResult.h`"`r`n#include `"../../Services/RuntimeBoundaryAdmissionService.h`"`r`n#include `"YAKSys_Hybrid_CPU.Core.TrapCompletionRouteDescriptor.h`"`r`n#include `"../Records/TrapCompletionPublicationFence.h`"")
$i=$s.IndexOf("// C# TO C++ CONVERTER WARNING: Nullable reference types"); $end=$s.IndexOf('class TrapCompletionRouteService final',$i); if($i -lt 0 -or $end -lt 0){throw 'anchor missing'}
$def=@'
struct TrapCompletionRouteRequest {
    std::shared_ptr<NeutralTrapResult> TrapResult; std::shared_ptr<RuntimeBoundaryAdmissionResult> RuntimeAdmission; std::shared_ptr<TrapCompletionRouteDescriptor> RouteDescriptor;
    bool DomainValidated{}, BackendExecutionAuthorized{}; unsigned long long Qualification{}, FaultAddress{}, FaultAux{};
    EvidenceVisibilityClass CompletionEvidenceClass = EvidenceVisibilityClass::HostOwnedRuntimeEvidence;
    TrapCompletionMigrationClass CompletionMigrationClass = TrapCompletionMigrationClass::Unclassified;
    TrapCompletionRouteRequest(std::shared_ptr<NeutralTrapResult> trap, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<TrapCompletionRouteDescriptor> descriptor, bool domainValidated, bool backendAuthorized, unsigned long long qualification = 0, unsigned long long faultAddress = 0, unsigned long long faultAux = 0, EvidenceVisibilityClass evidence = EvidenceVisibilityClass::HostOwnedRuntimeEvidence, TrapCompletionMigrationClass migration = TrapCompletionMigrationClass::Unclassified)
       : TrapResult(std::move(trap)), RuntimeAdmission(std::move(admission)), RouteDescriptor(std::move(descriptor)), DomainValidated(domainValidated), BackendExecutionAuthorized(backendAuthorized), Qualification(qualification), FaultAddress(faultAddress), FaultAux(faultAux), CompletionEvidenceClass(evidence), CompletionMigrationClass(migration) {}
    static std::shared_ptr<TrapCompletionRouteRequest> ProjectionOnlyDenied(std::shared_ptr<NeutralTrapResult> trap, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, bool backendAuthorized = false) { return std::make_shared<TrapCompletionRouteRequest>(std::move(trap), std::move(admission), TrapCompletionRouteDescriptor::getProjectionOnlyDenied(), true, backendAuthorized); }
};
struct TrapCompletionRouteResult {
    const TrapCompletionRouteDecision Decision; const bool CompletionPublicationAuthorized, RetirePublicationAuthorized; const unsigned int NeutralReasonCode; const std::wstring Reason;
    const bool CompletionPublicationAuthorizedOnly, IsFullyRetirable, IsAllowed, DeniesBackendExecution;
    TrapCompletionRouteResult(TrapCompletionRouteDecision decision, bool completion, bool retire, unsigned int reasonCode, std::wstring reason)
      : Decision(decision), CompletionPublicationAuthorized(completion), RetirePublicationAuthorized(retire), NeutralReasonCode(reasonCode), Reason(std::move(reason)), CompletionPublicationAuthorizedOnly(completion && !retire), IsFullyRetirable(decision == TrapCompletionRouteDecision::Allowed && completion && retire), IsAllowed(IsFullyRetirable), DeniesBackendExecution(decision == TrapCompletionRouteDecision::DeniedBackendExecution) {}
    static std::shared_ptr<TrapCompletionRouteResult> Denied(TrapCompletionRouteDecision decision, std::shared_ptr<NeutralTrapResult> trap, std::wstring reason) { return std::make_shared<TrapCompletionRouteResult>(decision, false, false, static_cast<unsigned int>(trap->Kind), std::move(reason)); }
    static std::shared_ptr<TrapCompletionRouteResult> CompletionAuthorizedButRetireDenied(std::shared_ptr<NeutralTrapResult> trap, std::wstring reason) { return std::make_shared<TrapCompletionRouteResult>(TrapCompletionRouteDecision::DeniedRetirePublication, true, false, static_cast<unsigned int>(trap->Kind), std::move(reason)); }
};
'@
$s=$s.Substring(0,$i)+$def+"`r`n`t"+$s.Substring($end); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'Routing\TrapCompletionRoutePolicy.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('request->RuntimeAdmission.IsAllowed','request->RuntimeAdmission->IsAllowed').Replace('request->TrapResult.ShouldTrap','request->TrapResult->ShouldTrap').Replace('request->TrapResult.Kind','request->TrapResult->Kind')
$s=$s.Replace('CompletionPublicationAuthorized: true, RetirePublicationAuthorized: true, NeutralReasonCode: ', 'true, true, ').Replace('Reason: L""','L""'); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'Records\TrapCompletionPublicationFence.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('request->RuntimeAdmission.IsAllowed','request->RuntimeAdmission->IsAllowed').Replace('request->TrapResult.ShouldTrap','request->TrapResult->ShouldTrap')
$s=$s.Replace('return new(false,', 'return std::make_shared<TrapCompletionOwnerPolicyResult>(false,').Replace('return new(true,','return std::make_shared<TrapCompletionOwnerPolicyResult>(true,')
$s=$s.Replace('return new(retirePublicationAuthorized ?', 'return std::make_shared<TrapCompletionPublicationFenceResult>(retirePublicationAuthorized ?').Replace('return new(decision, result, CompletionRecord::getNone(), RetirePublicationAuthorized: false, reason);','return std::make_shared<TrapCompletionPublicationFenceResult>(decision, result, CompletionRecord::getNone(), false, reason);')
[IO.File]::WriteAllText($p,$s)