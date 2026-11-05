#include "YAKSys_Hybrid_CPU.Core.Nested.NestedCapabilityPublicationContract.h"

namespace YAKSys_Hybrid_CPU::Core::Nested
{
	NestedCapabilityPublicationContractViolation NestedCapabilityPublicationContract::Evaluate(
		std::shared_ptr<NestedEnablementRequest> request)
	{
		if (!request)
			return NestedCapabilityPublicationContractViolation::RequestBypassesPublicationAuthority;

		const auto &publication = request->CapabilityPublication;
		if (!publication || !publication->Projection.IsProjectionOnly)
			return NestedCapabilityPublicationContractViolation::MissingCompatibilityProjection;

		if (!publication->Requirement.IsTypedAuthority)
			return NestedCapabilityPublicationContractViolation::MissingTypedAuthority;

		if (!publication->CanPublishRequiredCapability)
			return NestedCapabilityPublicationContractViolation::MissingProjectedNestedCapability;

		if (!request->HasExplicitVmxCapability)
			return NestedCapabilityPublicationContractViolation::RequestBypassesPublicationAuthority;

		return NestedCapabilityPublicationContractViolation::None;
	}

	bool NestedCapabilityPublicationContract::IsSatisfied(std::shared_ptr<NestedEnablementRequest> request)
	{
		return Evaluate(std::move(request)) == NestedCapabilityPublicationContractViolation::None;
	}
}
