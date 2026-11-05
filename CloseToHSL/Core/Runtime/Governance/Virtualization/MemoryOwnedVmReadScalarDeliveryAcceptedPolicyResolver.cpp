#include "MemoryOwnedVmReadScalarDeliveryAcceptedPolicyResolver.h"
#include "MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2.h"
#include "Phase45MemoryOwnedVmReadScalarDeliveryE0Contract.h"
#include "VirtualizationDecisionContractsV2.h"
#include "Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<MemoryOwnedVmReadScalarDeliveryPolicyResolution> MemoryOwnedVmReadScalarDeliveryAcceptedPolicyResolver::Resolve(std::shared_ptr<MemoryOwnedVmReadScalarDeliveryPolicyLookup> lookup)
	{
		if (!lookup || lookup->Revoked || lookup->DecisionId != MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId || lookup->OperationNamespace != MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationNamespace || lookup->OperationId != MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationId || lookup->SourceOwner != L"MemoryDomainDescriptor" || lookup->ValueSource != L"MemoryDomainReadOnlyTranslationView" || lookup->ResultAbi != VirtualizationDecisionResultAbiV2::ScalarU64ToDestinationRegister || lookup->EffectClass != VirtualizationDecisionEffectClassV2::ArchitecturalRegisterResultOnly || lookup->CapabilityRequirement != VirtualizationCapabilityRequirementV2::None || lookup->MigrationPolicy != VirtualizationOperationMigrationPolicyV2::DrainOnly || lookup->FieldIds.empty() || lookup->FieldIds != Phase45MemoryOwnedVmReadScalarDeliveryE0Contract::getExactFieldIds())
		{
			return std::make_shared<MemoryOwnedVmReadScalarDeliveryPolicyResolution>(MemoryOwnedVmReadScalarDeliveryPolicyResolution{nullptr, L"Exact GuestCr3/EptPointer/Vpid/Cr3TargetCount scalar-delivery D2 lookup mismatched or was revoked."});
		}

		std::shared_ptr<VmReadScalarDeliveryDecisionValidationResultV2> validation = Phase45MemoryOwnedVmReadScalarDeliveryDecisionAcceptanceV2::ValidateRepositoryArtifact(AcceptanceContainingCommitSha);
		if (validation && validation->IsAcceptedPolicyObject)
		{
			return std::make_shared<MemoryOwnedVmReadScalarDeliveryPolicyResolution>(MemoryOwnedVmReadScalarDeliveryPolicyResolution{validation->AcceptedDecision, L"Exact accepted memory-owned scalar-delivery governance policy resolved without runtime authority."});
		}
		return std::make_shared<MemoryOwnedVmReadScalarDeliveryPolicyResolution>(MemoryOwnedVmReadScalarDeliveryPolicyResolution{nullptr, validation ? validation->Reason : L"Repository artifact validation returned no result."});
	}

	std::shared_ptr<MemoryOwnedVmReadScalarDeliveryPolicyLookup> MemoryOwnedVmReadScalarDeliveryAcceptedPolicyResolver::ExactLookup(bool revoked)
	{
		return std::make_shared<MemoryOwnedVmReadScalarDeliveryPolicyLookup>(MemoryOwnedVmReadScalarDeliveryPolicyLookup{
			MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ExpectedDecisionId,
			MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationNamespace,
			MemoryOwnedVmReadScalarDeliveryDecisionValidatorV2::ExpectedOperationId,
			Phase45MemoryOwnedVmReadScalarDeliveryE0Contract::getExactFieldIds(),
			L"MemoryDomainDescriptor", L"MemoryDomainReadOnlyTranslationView",
			VirtualizationDecisionResultAbiV2::ScalarU64ToDestinationRegister,
			VirtualizationDecisionEffectClassV2::ArchitecturalRegisterResultOnly,
			VirtualizationCapabilityRequirementV2::None,
			VirtualizationOperationMigrationPolicyV2::DrainOnly,
			revoked});
	}
}
