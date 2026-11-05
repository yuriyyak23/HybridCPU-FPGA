#include "YAKSys_Hybrid_CPU.Core.DomainRuntimeAuthority.h"
#include "../DomainBoundaryDescriptor.h"
#include "../Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.ExecutionDomainDescriptor.h"
#include "../../Capabilities/CapabilityBoundaryRequirement.h"
#include "../../Capabilities/Grants/CapabilityGrant.h"
#include "../Services/DomainRuntimeOperation.h"

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DomainRuntimeAuthorityResult Validate(RootAuthorityDescriptor? root, DomainRuntimeContext context, DomainRuntimeOperation operation, ulong requiredCapabilityMask = 0)
	std::shared_ptr<DomainRuntimeAuthorityResult> DomainRuntimeAuthority::Validate(std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainRuntimeOperation> operation, unsigned long long requiredCapabilityMask)
	{
		std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement = requiredCapabilityMask == 0 ? CapabilityBoundaryRequirement::None : CapabilityBoundaryRequirement::TypedGrant(requiredCapabilityMask, CapabilityGrantScope::CompatibilityProjection);

		return Validate(root, context, operation, capabilityRequirement);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DomainRuntimeAuthorityResult Validate(RootAuthorityDescriptor? root, DomainRuntimeContext context, DomainRuntimeOperation operation, CapabilityBoundaryRequirement capabilityRequirement)
	std::shared_ptr<DomainRuntimeAuthorityResult> DomainRuntimeAuthority::Validate(std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainRuntimeOperation> operation, std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement)
	{
		return Validate(root, context, operation, DomainBoundaryDescriptor::FullDomainRuntime, capabilityRequirement);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DomainRuntimeAuthorityResult Validate(RootAuthorityDescriptor? root, DomainRuntimeContext context, DomainRuntimeOperation operation, DomainBoundaryDescriptor operationDomainRequirement, CapabilityBoundaryRequirement capabilityRequirement)
	std::shared_ptr<DomainRuntimeAuthorityResult> DomainRuntimeAuthority::Validate(std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainRuntimeOperation> operation, std::shared_ptr<DomainBoundaryDescriptor> operationDomainRequirement, std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement)
	{
		if (!operationDomainRequirement->IsSatisfiedBy(context))
		{
			return Deny(DomainRuntimeAuthorityDecision::MissingRuntimeContext, L"Domain runtime authority requires the exact operation domain boundary.");
		}

		if (root == nullptr)
		{
			return Deny(DomainRuntimeAuthorityDecision::MissingRootAuthority, L"Domain runtime authority requires a root authority descriptor.");
		}

		if (operation->getKind() == DomainRuntimeOperationKind::ActivateCompatibilityFrontend && !root->getCanActivateCompatibilityFrontend())
		{
			return Deny(DomainRuntimeAuthorityDecision::FrontendActivationDenied, L"Root authority denies compatibility frontend activation.");
		}

		if (operation->getRequiresCapabilityGrant())
		{
			if (capabilityRequirement->CapabilityMask == 0 || !capabilityRequirement->IsSatisfiedBy(context->getCapabilities()) || !root->HasCapability(capabilityRequirement->CapabilityMask))
			{
				return Deny(DomainRuntimeAuthorityDecision::CapabilityDenied, L"Required capability is not backed by typed domain authority.");
			}
		}

		if (operation->isProjectionOnly())
		{
			return (context->getExecution() != nullptr && context->getExecution()->getCompatibilityProjectionEnabled()) ? DomainRuntimeAuthorityResult::Allowed : Deny(DomainRuntimeAuthorityDecision::ProjectionDenied, L"Compatibility projection is disabled by the execution descriptor.");
		}


		if (operation->isNoStateExecution())
		{
			return operation->getSource() == DomainRuntimeOperationSource::RuntimeService && root->isRuntimeRoot() ? DomainRuntimeAuthorityResult::Allowed : Deny(DomainRuntimeAuthorityDecision::MutationDenied, L"No-state execution requires neutral runtime-root authority.");
		}

		if (!root->CanMutateAuthoritativeState(operation))
		{
			return Deny(DomainRuntimeAuthorityDecision::MutationDenied, L"Root authority denies authoritative state mutation.");
		}

		return DomainRuntimeAuthorityResult::Allowed;
	}

	std::shared_ptr<DomainRuntimeAuthorityResult> DomainRuntimeAuthority::Deny(DomainRuntimeAuthorityDecision decision, const std::wstring &message)
	{
		return DomainRuntimeAuthorityResult::Denied(decision, message);
	}
}
