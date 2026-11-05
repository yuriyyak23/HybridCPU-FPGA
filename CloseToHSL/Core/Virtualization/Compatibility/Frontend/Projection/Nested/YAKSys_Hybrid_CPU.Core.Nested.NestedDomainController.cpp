#include "YAKSys_Hybrid_CPU.Core.Nested.NestedDomainController.h"
#include "../../../../../Runtime/Nested/Validation/NestedValidationResult.h"
#include "../../../Generated/VmcsProjection/ShadowVmcsNestedProjectionService.h"
#include "../../../../../Runtime/Nested/Descriptors/NestedDomainDescriptor.h"
#include "NestedDomainController.h"
#include "../../../../../System/Vmcs/V2/VmcsV2BlockDirectory.h"
#include "../../../../../System/Vmcs/V2/VmcsV2FieldModel.h"

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core::Nested
{

	bool NestedDomainController::TryEnable(std::shared_ptr<NestedDomainDescriptor> domain, std::shared_ptr<INestedProjectionService> projectionService, std::shared_ptr<NestedEnablementRequest> request, std::shared_ptr<NestedValidationResult> &validation)
	{
		ArgumentNullException::ThrowIfNull(domain);
		ArgumentNullException::ThrowIfNull(projectionService);

		if (!domain->getCanProjectToCompatibilityFrontend())
		{
			validation = NestedValidationResult::Fail(NestedValidationCode::ProjectionDenied, L"Nested domain must be runtime-authoritative and composition-ready before compatibility projection can be enabled.");
			return false;
		}

		return projectionService->TryEnable(domain, request, validation);
	}

	void NestedDomainController::Disable(std::shared_ptr<NestedDomainDescriptor> domain, std::shared_ptr<INestedProjectionService> projectionService)
	{
		ArgumentNullException::ThrowIfNull(domain);
		ArgumentNullException::ThrowIfNull(projectionService);
		projectionService->Disable(domain);
	}

	bool NestedDomainController::TryEnable(std::shared_ptr<VmcsV2Descriptor> descriptor, std::shared_ptr<NestedEnablementRequest> request, std::shared_ptr<VmcsV2ValidationResult> &validation)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		std::shared_ptr<NestedValidationResult> nestedValidation;
		bool enabled = TryEnable(CreateCompatibilityProjectionDomain(request), std::make_shared<ShadowVmcsNestedProjectionService>(descriptor), request, nestedValidation);

		validation = ToVmcsValidation(nestedValidation);
		return enabled;
	}

	void NestedDomainController::Disable(std::shared_ptr<VmcsV2Descriptor> descriptor)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		Disable(CreateCompatibilityProjectionDomain(), std::make_shared<ShadowVmcsNestedProjectionService>(descriptor));
	}

	std::shared_ptr<NestedDomainDescriptor> NestedDomainController::CreateCompatibilityProjectionDomain()
	{
		return std::make_shared<NestedDomainDescriptor>(NestedDomainAuthority::Runtime, 1, 2,
			NestedCapabilityGrantMask::RequiredForPhase7, true, true, true, true);
	}

	std::shared_ptr<NestedDomainDescriptor> NestedDomainController::CreateCompatibilityProjectionDomain(std::shared_ptr<NestedEnablementRequest> request)
	{
		return std::make_shared<NestedDomainDescriptor>(NestedDomainAuthority::Runtime, 1, 2,
			request->NestedCapabilities, true, true,
			(request->ProvenGates & NestedEnablementGate::HostEvidenceExcluded) != NestedEnablementGate::None,
			(request->ProvenGates & NestedEnablementGate::Lane6Lane7PassthroughBlocked) != NestedEnablementGate::None);
	}

	std::shared_ptr<VmcsV2ValidationResult> NestedDomainController::ToVmcsValidation(std::shared_ptr<NestedValidationResult> validation)
	{
		return validation->Succeeded ? VmcsV2ValidationResult::Success(VmcsV2BlockDirectory::ShadowVmcsBlockFieldId) : VmcsV2ValidationResult::Fail(validation->Code == NestedValidationCode::ProjectionDenied ? VmcsV2ValidationCode::NestedPolicyGateFailed : VmcsV2ValidationCode::InvalidVmcs12, VmcsV2BlockDirectory::ShadowVmcsBlockFieldId, validation->Message);
	}
}


