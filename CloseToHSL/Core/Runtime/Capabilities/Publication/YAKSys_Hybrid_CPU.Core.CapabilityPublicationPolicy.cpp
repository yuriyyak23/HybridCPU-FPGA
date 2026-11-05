#include "YAKSys_Hybrid_CPU.Core.CapabilityPublicationPolicy.h"
#include "../Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"
#include "../Grants/CapabilityGrant.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CapabilityPublicationPolicy::CapabilityPublicationPolicy() : CapabilityPublicationPolicy(false, CapabilityWriteDisposition::Reject)
	{
	}

	CapabilityPublicationPolicy::CapabilityPublicationPolicy(bool allowCompatibilityAliasPublication, CapabilityWriteDisposition writeDisposition)
		: AllowCompatibilityAliasPublication(allowCompatibilityAliasPublication), WriteDisposition(writeDisposition)
	{
	}

	std::shared_ptr<CapabilityPublicationPolicy> CapabilityPublicationPolicy::getFailClosed()
	{
		return FailClosed;
	}

	std::shared_ptr<CapabilityPublicationPolicy> CapabilityPublicationPolicy::getReadOnlyCompatibilityAlias()
	{
		return ReadOnlyCompatibilityAlias;
	}

	bool CapabilityPublicationPolicy::getAllowCompatibilityAliasPublication() const
	{
		return AllowCompatibilityAliasPublication;
	}

	CapabilityWriteDisposition CapabilityPublicationPolicy::getWriteDisposition() const
	{
		return WriteDisposition;
	}

	unsigned long long CapabilityPublicationPolicy::ProjectCompatibilityAlias(std::shared_ptr<CapabilityDescriptorSet> descriptorSet)
	{
		if (!getAllowCompatibilityAliasPublication() || descriptorSet == nullptr)
		{
			return 0;
		}

		return descriptorSet->getCompatibilityCapsProjection();
	}

	bool CapabilityPublicationPolicy::CanPublishCapability(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask)
	{
		std::shared_ptr<CapabilityGrant> grant;
		return getAllowCompatibilityAliasPublication() && descriptorSet != nullptr && descriptorSet->getTypedGrants()->TryGetGrant(capabilityMask, CapabilityGrantScope::CompatibilityProjection, grant) && grant->getHasTypedAuthority() && grant->isPublishableCompatibilityGrant();
	}

	bool CapabilityPublicationPolicy::ShouldRejectCompatibilityWrite()
	{
		return getWriteDisposition() == CapabilityWriteDisposition::Reject;
	}

	bool CapabilityPublicationPolicy::IsCompatibilityWriteArchitecturallyNoEffect()
	{
		return getWriteDisposition() == CapabilityWriteDisposition::CompatibilityNoEffect;
	}
}
