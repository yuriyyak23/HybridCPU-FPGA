#include "YAKSys_Hybrid_CPU.Core.NestedDomainCapabilityProjectionContract.h"
#include "../../../Virtualization/Compatibility/Frontend/Decode/VmxInstructionPayload.h"
#include "../../../Runtime/Capabilities/Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"

namespace YAKSys_Hybrid_CPU::Core::Nested
{
	const NestedDomainCapabilityProjection NestedDomainCapabilityProjection::VmxCompatibility =
		NestedDomainCapabilityProjection(NestedDomainCapability::NestedCompatibilityProjection, VmxV2InstructionCaps::NestedVmx);
}

namespace YAKSys_Hybrid_CPU::Core
{
	NestedDomainCapabilityProjectionViolation NestedDomainCapabilityProjectionContract::Evaluate(
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Nested::NestedCapabilityPublication> publication)
	{
		if (!publication)
			return NestedDomainCapabilityProjectionViolation::MissingTypedAuthority;

		if (publication->Requirement.Capability == YAKSys_Hybrid_CPU::Core::Nested::NestedDomainCapability::None)
			return NestedDomainCapabilityProjectionViolation::MissingGenericCapability;

		if (publication->Requirement.CapabilityMask == 0)
			return NestedDomainCapabilityProjectionViolation::MissingFrontendProjectionMask;

		if (publication->Requirement.RequiredGrant.getOwnerDomainId() == CapabilityGrant::NoOwnerDomainId)
			return NestedDomainCapabilityProjectionViolation::MissingOwnerDomain;

		return publication->Requirement.IsTypedAuthority
			? NestedDomainCapabilityProjectionViolation::None
			: NestedDomainCapabilityProjectionViolation::MissingTypedAuthority;
	}

	bool NestedDomainCapabilityProjectionContract::IsSatisfied(
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Nested::NestedCapabilityPublication> publication)
	{
		return Evaluate(std::move(publication)) == NestedDomainCapabilityProjectionViolation::None;
	}
}


