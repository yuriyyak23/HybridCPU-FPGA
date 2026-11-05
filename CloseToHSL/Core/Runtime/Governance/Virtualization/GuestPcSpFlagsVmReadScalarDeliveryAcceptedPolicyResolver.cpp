#include "GuestPcSpFlagsVmReadScalarDeliveryAcceptedPolicyResolver.h"
#include "GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2.h"
#include "Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract.h"
#include "VirtualizationDecisionContractsV2.h"
#include "Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<GuestPcSpFlagsVmReadScalarDeliveryPolicyResolution> GuestPcSpFlagsVmReadScalarDeliveryAcceptedPolicyResolver::Resolve(std::shared_ptr<GuestPcSpFlagsVmReadScalarDeliveryPolicyLookup> lookup)
	{
		if (!lookup || lookup->Revoked || lookup->DecisionId != GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId || lookup->OperationNamespace != GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationNamespace || lookup->OperationId != GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationId || lookup->SourceOwner != L"ExecutionDomainDescriptor" || lookup->ValueSource != L"ExecutionDomainReadOnlyStateView" || lookup->ResultAbi != VirtualizationDecisionResultAbiV2::ScalarU64ToDestinationRegister || lookup->EffectClass != VirtualizationDecisionEffectClassV2::ArchitecturalRegisterResultOnly || lookup->CapabilityRequirement != VirtualizationCapabilityRequirementV2::None || lookup->MigrationPolicy != VirtualizationOperationMigrationPolicyV2::DrainOnly || lookup->FieldIds.empty() || lookup->FieldIds != Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getExactFieldIds())
		{
			return std::make_shared<GuestPcSpFlagsVmReadScalarDeliveryPolicyResolution>(GuestPcSpFlagsVmReadScalarDeliveryPolicyResolution{nullptr, L"Exact GuestPc/GuestSp/GuestFlags scalar-delivery D2 lookup mismatched or was revoked."});
		}

		std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> validation = Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionAcceptanceV2::ValidateRepositoryArtifact(AcceptanceContainingCommitSha);
		if (validation && validation->IsAcceptedPolicyObject)
		{
			return std::make_shared<GuestPcSpFlagsVmReadScalarDeliveryPolicyResolution>(GuestPcSpFlagsVmReadScalarDeliveryPolicyResolution{validation->AcceptedDecision, L"Exact accepted GuestPc/GuestSp/GuestFlags scalar-delivery governance policy resolved without runtime authority."});
		}
		return std::make_shared<GuestPcSpFlagsVmReadScalarDeliveryPolicyResolution>(GuestPcSpFlagsVmReadScalarDeliveryPolicyResolution{nullptr, validation ? validation->Reason : L"Repository artifact validation returned no result."});
	}

	std::shared_ptr<GuestPcSpFlagsVmReadScalarDeliveryPolicyLookup> GuestPcSpFlagsVmReadScalarDeliveryAcceptedPolicyResolver::ExactLookup(bool revoked)
	{
		return std::make_shared<GuestPcSpFlagsVmReadScalarDeliveryPolicyLookup>(GuestPcSpFlagsVmReadScalarDeliveryPolicyLookup{
			GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId,
			GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationNamespace,
			GuestPcSpFlagsVmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationId,
			Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getExactFieldIds(),
			L"ExecutionDomainDescriptor",
			L"ExecutionDomainReadOnlyStateView",
			VirtualizationDecisionResultAbiV2::ScalarU64ToDestinationRegister,
			VirtualizationDecisionEffectClassV2::ArchitecturalRegisterResultOnly,
			VirtualizationCapabilityRequirementV2::None,
			VirtualizationOperationMigrationPolicyV2::DrainOnly,
			revoked});
	}
}
