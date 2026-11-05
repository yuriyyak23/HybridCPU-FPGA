#include "YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSetGrantFirstAuthorityContract.h"
#include "../../../Runtime/Capabilities/Grants/CapabilityGrant.h"
#include "../../../Runtime/Capabilities/Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"
#include "../../Compatibility/Generated/CapabilityProjection/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSetSchema.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CapabilityDescriptorSetGrantFirstAuthorityViolation CapabilityDescriptorSetGrantFirstAuthorityContract::Evaluate(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask)
	{
		if (descriptorSet == nullptr)
		{
			return CapabilityDescriptorSetGrantFirstAuthorityViolation::MissingDescriptorSet;
		}

		if (descriptorSet->getGlobalHardwareCaps() != descriptorSet->getTypedGrants()->ProjectMask(CapabilityGrantScope::HardwareAvailable))
		{
			return CapabilityDescriptorSetGrantFirstAuthorityViolation::HardwareProjectionBypassesTypedGrants;
		}

		if (descriptorSet->getRuntimeEnabledCaps() != descriptorSet->getTypedGrants()->ProjectMask(CapabilityGrantScope::RuntimeEnabled))
		{
			return CapabilityDescriptorSetGrantFirstAuthorityViolation::RuntimeProjectionBypassesTypedGrants;
		}

		if (descriptorSet->getDomainGrantedCaps() != descriptorSet->getTypedGrants()->ProjectMask(CapabilityGrantScope::DomainGranted))
		{
			return CapabilityDescriptorSetGrantFirstAuthorityViolation::DomainProjectionBypassesTypedGrants;
		}

		if (descriptorSet->getCompatibilityCapsProjection() != descriptorSet->getTypedGrants()->getEffectiveCompatibilityMask())
		{
			return CapabilityDescriptorSetGrantFirstAuthorityViolation::CompatibilityProjectionBypassesTypedGrants;
		}

		if (capabilityMask != 0)
		{
			std::shared_ptr<CapabilityGrant> grant;
			bool grantExists = descriptorSet->getTypedGrants()->TryGetGrant(capabilityMask, CapabilityGrantScope::CompatibilityProjection, grant) && grant->getHasTypedAuthority();

			if (descriptorSet->HasEffectiveCapability(capabilityMask) != grantExists)
			{
				return CapabilityDescriptorSetGrantFirstAuthorityViolation::EffectiveCapabilityBypassesTypedGrant;
			}
		}

		for (auto entry : CapabilityDescriptorSetSchema::getVmxCompatibilityBits())
		{
			if (entry->TypedGrantSource.starts_with(L"CapabilityDescriptorSet."))
			{
				return CapabilityDescriptorSetGrantFirstAuthorityViolation::CompatibilitySchemaClaimsDescriptorSetAuthority;
			}
		}

		return CapabilityDescriptorSetGrantFirstAuthorityViolation::None;
	}

	bool CapabilityDescriptorSetGrantFirstAuthorityContract::IsSatisfied(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask)
	{
		return Evaluate(descriptorSet, capabilityMask) == CapabilityDescriptorSetGrantFirstAuthorityViolation::None;
	}
}
