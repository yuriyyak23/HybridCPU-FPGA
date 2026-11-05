#include "TrapCompletionPublicationFence.h"
#include "../Routing/TrapCompletionRoutePolicy.h"
#include "YAKSys_Hybrid_CPU.Core.CompletionRecord.h"
#include "CompletionRecord.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<TrapCompletionPublicationFence> TrapCompletionPublicationFence::getDefault()
	{
		return Default;
	}

	std::shared_ptr<TrapCompletionPublicationFenceResult> TrapCompletionPublicationFence::DenyProjectionOnly(std::shared_ptr<NeutralTrapResult> result, bool runtimeAdmissionAllowed)
	{
		return Evaluate(result, runtimeAdmissionAllowed, false, false, static_cast<unsigned int>(result->Kind), 0, 0, 0, EvidenceVisibilityClass::HostOwnedRuntimeEvidence, TrapCompletionMigrationClass::Unclassified);
	}

	std::shared_ptr<TrapCompletionPublicationFenceResult> TrapCompletionPublicationFence::Evaluate(std::shared_ptr<NeutralTrapResult> result, bool runtimeAdmissionAllowed, bool completionPublicationAuthorized, bool retirePublicationAuthorized, unsigned int neutralReasonCode, unsigned long long qualification, unsigned long long faultAddress, unsigned long long faultAux, EvidenceVisibilityClass evidenceClass, TrapCompletionMigrationClass migrationClass)
	{
		if (!runtimeAdmissionAllowed)
		{
			return Denied(TrapCompletionPublicationDecision::DeniedRuntimeAdmission, result, L"Trap publication requires runtime boundary admission.");
		}

		if (!result->ShouldTrap)
		{
			return Denied(TrapCompletionPublicationDecision::DeniedNoNeutralTrap, result, L"Trap publication requires a neutral trap result.");
		}

		if (!completionPublicationAuthorized)
		{
			return Denied(TrapCompletionPublicationDecision::DeniedBackendExecution, result, L"Trap projection is admitted, but backend completion publication is denied.");
		}

		if (!retirePublicationAuthorized)
		{
			return PublishedCompletion(result, neutralReasonCode, qualification, faultAddress, faultAux, false, L"Trap completion was authorized, but retire publication is denied.");
		}

		if (!CanPublishRetireEvidence(evidenceClass))
		{
			return PublishedCompletion(result, neutralReasonCode, qualification, faultAddress, faultAux, false, L"Trap completion was published, but retire publication cannot expose host-owned evidence.");
		}

		if (!CanPublishRetireMigrationClass(migrationClass))
		{
			return PublishedCompletion(result, neutralReasonCode, qualification, faultAddress, faultAux, false, L"Trap completion was published, but retire publication requires an explicit migration class.");
		}

		return PublishedCompletion(result, neutralReasonCode, qualification, faultAddress, faultAux, true, L"");
	}

	std::shared_ptr<TrapCompletionOwnerPolicyResult> TrapCompletionPublicationFence::EvaluateOwnerPolicy(std::shared_ptr<TrapCompletionRouteRequest> request, std::shared_ptr<TrapCompletionRouteResult> route)
	{
		if (!request->RuntimeAdmission->IsAllowed || !request->TrapResult->ShouldTrap)
		{
			return std::make_shared<TrapCompletionOwnerPolicyResult>(false, L"Completion-owner policy requires live runtime admission and a neutral event.");
		}
		if (!route->CompletionPublicationAuthorizedOnly || route->Decision != TrapCompletionRouteDecision::DeniedRetirePublication)
		{
			return std::make_shared<TrapCompletionOwnerPolicyResult>(false, L"Completion-owner policy requires the split completion-only route.");
		}
		if (request->CompletionEvidenceClass != EvidenceVisibilityClass::HostOwnedRuntimeEvidence || request->CompletionMigrationClass != TrapCompletionMigrationClass::HostOwnedNonMigratable)
		{
			return std::make_shared<TrapCompletionOwnerPolicyResult>(false, L"Exact probe completion must remain host-owned and non-migratable.");
		}

		return std::make_shared<TrapCompletionOwnerPolicyResult>(true, L"Pure completion policy accepted; only the neutral owner may publish record plus E5.");
	}

	std::shared_ptr<TrapCompletionPublicationFenceResult> TrapCompletionPublicationFence::PublishedCompletion(std::shared_ptr<NeutralTrapResult> result, unsigned int neutralReasonCode, unsigned long long qualification, unsigned long long faultAddress, unsigned long long faultAux, bool retirePublicationAuthorized, const std::wstring &reason)
	{
		return std::make_shared<TrapCompletionPublicationFenceResult>(retirePublicationAuthorized ? TrapCompletionPublicationDecision::Allowed : TrapCompletionPublicationDecision::CompletionPublishedRetireDenied, result, std::make_shared<CompletionRecord>(CompletionRecordClass::Trap, neutralReasonCode, qualification, faultAddress, faultAux), retirePublicationAuthorized, reason);
	}

	bool TrapCompletionPublicationFence::CanPublishRetireEvidence(EvidenceVisibilityClass evidenceClass)
	{
		return (evidenceClass == EvidenceVisibilityClass::GuestArchitecturalState || evidenceClass == EvidenceVisibilityClass::CompatibilityAlias);
	}

	bool TrapCompletionPublicationFence::CanPublishRetireMigrationClass(TrapCompletionMigrationClass migrationClass)
	{
		return (migrationClass == TrapCompletionMigrationClass::RecomputedAfterRestore || migrationClass == TrapCompletionMigrationClass::GuestArchitecturalState);
	}

	std::shared_ptr<TrapCompletionPublicationFenceResult> TrapCompletionPublicationFence::Denied(TrapCompletionPublicationDecision decision, std::shared_ptr<NeutralTrapResult> result, const std::wstring &reason)
	{
		return std::make_shared<TrapCompletionPublicationFenceResult>(decision, result, CompletionRecord::getNone(), false, reason);
	}
}
