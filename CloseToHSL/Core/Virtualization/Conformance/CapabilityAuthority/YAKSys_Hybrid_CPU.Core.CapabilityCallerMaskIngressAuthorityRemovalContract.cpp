#include "YAKSys_Hybrid_CPU.Core.CapabilityCallerMaskIngressAuthorityRemovalContract.h"
#include "../../../Runtime/Capabilities/Publication/YAKSys_Hybrid_CPU.Core.CapabilityPublicationPolicy.h"
#include "../../../Runtime/Capabilities/Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"

using namespace YAKSys_Hybrid_CPU::Core::Nested;

namespace YAKSys_Hybrid_CPU::Core
{

	CapabilityCallerMaskIngressAuthorityViolation CapabilityCallerMaskIngressAuthorityRemovalContract::Evaluate(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask, unsigned long long ownerDomainId)
	{
		std::shared_ptr<CapabilityBoundaryRequirement> nonTypedRequirement = std::make_shared<CapabilityBoundaryRequirement>(capabilityMask, CapabilityGrantScope::CompatibilityProjection, false);

		if (nonTypedRequirement->IsSatisfiedBy(descriptorSet))
		{
			return CapabilityCallerMaskIngressAuthorityViolation::NonTypedRequirementAccepted;
		}

		std::shared_ptr<CapabilityBoundaryRequirement> typedRequirement = CapabilityBoundaryRequirement::TypedGrant(capabilityMask, CapabilityGrantScope::CompatibilityProjection);

		if (!typedRequirement->IsSatisfiedBy(descriptorSet))
		{
			return CapabilityCallerMaskIngressAuthorityViolation::MissingTypedProjectionGrant;
		}

		std::shared_ptr<CapabilityPublicationPolicy> publicationPolicy = CapabilityPublicationPolicy::getReadOnlyCompatibilityAlias();
		if (!publicationPolicy->CanPublishCapability(descriptorSet, capabilityMask))
		{
			return CapabilityCallerMaskIngressAuthorityViolation::CompatibilityAliasPublishedAsAuthority;
		}

		std::shared_ptr<CapabilityGrant> projectionGrant;
		if (!descriptorSet->getTypedGrants()->TryGetGrant(capabilityMask, CapabilityGrantScope::CompatibilityProjection, projectionGrant) || projectionGrant->getEvidenceVisibility() != CapabilityEvidenceVisibility::GuestVisibleProjection)
		{
			return CapabilityCallerMaskIngressAuthorityViolation::HostOwnedEvidenceExposedToCompatibilityProjection;
		}

		std::shared_ptr<NestedCapabilityPublication> compatibilityAliasPublication = NestedCapabilityPublication::FromCompatibilityAlias(capabilityMask, ownerDomainId);

		if (compatibilityAliasPublication->CanPublishRequiredCapability)
		{
			return CapabilityCallerMaskIngressAuthorityViolation::NestedCompatibilityAliasPublishedAsAuthority;
		}

		std::shared_ptr<NestedCapabilityPublication> typedNestedPublication = NestedCapabilityPublication::FromTypedGrant(ownerDomainId);
		if (!typedNestedPublication->CanPublishRequiredCapability)
		{
			return CapabilityCallerMaskIngressAuthorityViolation::MissingTypedProjectionGrant;
		}

		if (typedNestedPublication->Requirement.RequiredGrant.getEvidenceVisibility() != CapabilityEvidenceVisibility::GuestVisibleProjection)
		{
			return CapabilityCallerMaskIngressAuthorityViolation::HostOwnedEvidenceExposedToCompatibilityProjection;
		}

		std::shared_ptr<NestedEnablementProof> failClosedMaskProof = NestedEnablementProof::FromTypedDescriptorProofs(NestedCapabilityGrantMask::RequiredForPhase7, NestedEnablementGate::RequiredForPhase7, NestedProofAuthoritySource::FailClosed);

		return failClosedMaskProof->IsAuthoritative ? CapabilityCallerMaskIngressAuthorityViolation::NestedCompatibilityMaskProofAccepted : CapabilityCallerMaskIngressAuthorityViolation::None;
	}

	bool CapabilityCallerMaskIngressAuthorityRemovalContract::IsSatisfied(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask, unsigned long long ownerDomainId)
	{
		return Evaluate(descriptorSet, capabilityMask, ownerDomainId) == CapabilityCallerMaskIngressAuthorityViolation::None;
	}
}
