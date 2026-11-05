#include "VmReadScalarDeliveryAcceptedPolicyResolver.h"
#include "Phase41VmReadScalarDeliveryE0Contract.h"
#include "VirtualizationDecisionContractsV2.h"
#include "VmReadScalarDeliveryDecisionValidatorV2.h"
#include "Phase41VmReadScalarDeliveryDecisionAcceptanceV2.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<VmReadScalarDeliveryPolicyResolution> VmReadScalarDeliveryAcceptedPolicyResolver::Resolve(std::shared_ptr<VmReadScalarDeliveryPolicyLookup> lookup)
	{
		if (!lookup || lookup->Revoked || lookup->DecisionId != VmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId || lookup->OperationNamespace != VmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationNamespace || lookup->OperationId != VmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationId || lookup->SourceOwner != L"PrivilegedExecutionStateOwnerPolicy" || lookup->ResultAbi != VirtualizationDecisionResultAbiV2::ScalarU64ToDestinationRegister || lookup->EffectClass != VirtualizationDecisionEffectClassV2::ArchitecturalRegisterResultOnly || lookup->CapabilityRequirement != VirtualizationCapabilityRequirementV2::None || lookup->MigrationPolicy != VirtualizationOperationMigrationPolicyV2::DrainOnly || lookup->FieldIds.empty() || lookup->FieldIds != Phase41VmReadScalarDeliveryE0Contract::getExactFieldIds())
		{
			return std::make_shared<VmReadScalarDeliveryPolicyResolution>(VmReadScalarDeliveryPolicyResolution{nullptr, L"Exact scalar-delivery D2 lookup mismatched or was revoked."});
		}

		std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> validation = Phase41VmReadScalarDeliveryDecisionAcceptanceV2::ValidateRepositoryArtifact(AcceptanceContainingCommitSha);
		if (!validation || !validation->IsAcceptedPolicyObject)
		{
			return std::make_shared<VmReadScalarDeliveryPolicyResolution>(VmReadScalarDeliveryPolicyResolution{nullptr, validation ? validation->Reason : L"Scalar-delivery decision validation produced no result."});
		}
		return std::make_shared<VmReadScalarDeliveryPolicyResolution>(VmReadScalarDeliveryPolicyResolution{validation->AcceptedDecision, L"Exact accepted scalar-delivery governance policy resolved without runtime authority."});
	}

	std::shared_ptr<VmReadScalarDeliveryPolicyLookup> VmReadScalarDeliveryAcceptedPolicyResolver::ExactLookup(bool revoked)
	{
		return std::make_shared<VmReadScalarDeliveryPolicyLookup>(VmReadScalarDeliveryPolicyLookup{
			VmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId,
			VmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationNamespace,
			VmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationId,
			Phase41VmReadScalarDeliveryE0Contract::getExactFieldIds(),
			L"PrivilegedExecutionStateOwnerPolicy",
			VirtualizationDecisionResultAbiV2::ScalarU64ToDestinationRegister,
			VirtualizationDecisionEffectClassV2::ArchitecturalRegisterResultOnly,
			VirtualizationCapabilityRequirementV2::None,
			VirtualizationOperationMigrationPolicyV2::DrainOnly,
			revoked});
	}
}
