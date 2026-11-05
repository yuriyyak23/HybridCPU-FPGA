#include "TrapCompletionRoutePolicy.h"
#include "YAKSys_Hybrid_CPU.Core.TrapCompletionRouteDescriptor.h"
#include "../Records/TrapCompletionPublicationFence.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<TrapCompletionRouteService> TrapCompletionRouteService::getDefault()
	{
		return Default;
	}

	std::shared_ptr<TrapCompletionRouteResult> TrapCompletionRouteService::Authorize(std::shared_ptr<TrapCompletionRouteRequest> request)
	{
		if (!request->RuntimeAdmission->IsAllowed)
		{
			return TrapCompletionRouteResult::Denied(TrapCompletionRouteDecision::DeniedRuntimeAdmission, request->TrapResult, L"Trap completion route requires runtime boundary admission.");
		}

		if (!request->TrapResult->ShouldTrap)
		{
			return TrapCompletionRouteResult::Denied(TrapCompletionRouteDecision::DeniedNoNeutralTrap, request->TrapResult, L"Trap completion route requires a neutral trap result.");
		}

		if (request->RouteDescriptor == nullptr)
		{
			return TrapCompletionRouteResult::Denied(TrapCompletionRouteDecision::MissingRouteDescriptor, request->TrapResult, L"Trap completion route requires a runtime-owned route descriptor.");
		}

		std::shared_ptr<TrapCompletionRouteDescriptor> route = request->RouteDescriptor;
		if (!route->isRuntimeAuthoritative())
		{
			return TrapCompletionRouteResult::Denied(TrapCompletionRouteDecision::DeniedRouteAuthority, request->TrapResult, L"Compatibility projection cannot own trap completion routing.");
		}

		if (route->getRequiresValidatedDomain() && !request->DomainValidated)
		{
			return TrapCompletionRouteResult::Denied(TrapCompletionRouteDecision::DeniedDomainValidation, request->TrapResult, L"Trap completion route requires a validated runtime domain.");
		}

		if (!request->BackendExecutionAuthorized)
		{
			return TrapCompletionRouteResult::Denied(TrapCompletionRouteDecision::DeniedBackendExecution, request->TrapResult, L"Trap completion route denies publication until backend execution is authorized by runtime policy.");
		}

		if (!route->getAllowsCompletionPublication())
		{
			return TrapCompletionRouteResult::Denied(TrapCompletionRouteDecision::DeniedCompletionPublication, request->TrapResult, L"Trap completion route descriptor denies completion publication.");
		}

		if (!route->getAllowsRetirePublication())
		{
			return TrapCompletionRouteResult::CompletionAuthorizedButRetireDenied(request->TrapResult, L"Trap completion route descriptor authorizes completion publication but denies retire publication.");
		}

		return std::make_shared<TrapCompletionRouteResult>(TrapCompletionRouteDecision::Allowed, true, true, static_cast<unsigned int>(request->TrapResult->Kind), L"");
	}

	std::shared_ptr<TrapCompletionPublicationFenceResult> TrapCompletionRouteService::EvaluateFence(std::shared_ptr<TrapCompletionRouteRequest> request, std::shared_ptr<TrapCompletionRouteResult> route)
	{
		return TrapCompletionPublicationFence::getDefault()->Evaluate(request->TrapResult, request->RuntimeAdmission->IsAllowed, route->CompletionPublicationAuthorized, route->RetirePublicationAuthorized, route->NeutralReasonCode, request->Qualification, request->FaultAddress, request->FaultAux, request->CompletionEvidenceClass, request->CompletionMigrationClass);
	}
}
