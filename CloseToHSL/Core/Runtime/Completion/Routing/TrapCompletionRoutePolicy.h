#pragma once

#include <memory>
#include <string>
#include <utility>
#include "../../Events/Traps/NeutralTrapResult.h"
#include "../../Services/RuntimeBoundaryAdmissionService.h"
#include "YAKSys_Hybrid_CPU.Core.TrapCompletionRouteDescriptor.h"
#include "../Records/TrapCompletionPublicationFence.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class TrapCompletionRouteDecision : unsigned char
	{
		Allowed = 0,
		DeniedRuntimeAdmission = 1,
		DeniedNoNeutralTrap = 2,
		MissingRouteDescriptor = 3,
		DeniedRouteAuthority = 4,
		DeniedDomainValidation = 5,
		DeniedBackendExecution = 6,
		DeniedCompletionPublication = 7,
		DeniedRetirePublication = 8
	};


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
	class TrapCompletionRouteService final : public std::enable_shared_from_this<TrapCompletionRouteService>
	{
	private:
		static inline const std::shared_ptr<TrapCompletionRouteService> Default = std::make_shared<TrapCompletionRouteService>();

	public:
		static std::shared_ptr<TrapCompletionRouteService> getDefault();

		std::shared_ptr<TrapCompletionRouteResult> Authorize(std::shared_ptr<TrapCompletionRouteRequest> request);

		std::shared_ptr<TrapCompletionPublicationFenceResult> EvaluateFence(std::shared_ptr<TrapCompletionRouteRequest> request, std::shared_ptr<TrapCompletionRouteResult> route);
	};

}
