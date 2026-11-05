#include "YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CapabilityDescriptorSet::CapabilityDescriptorSet() : CapabilityDescriptorSet(CapabilityGrantCollection::getEmpty())
	{
	}

	CapabilityDescriptorSet::CapabilityDescriptorSet(std::shared_ptr<CapabilityGrantCollection> typedGrants) : TypedGrants(typedGrants ? typedGrants : CapabilityGrantCollection::getEmpty()) {}

	std::shared_ptr<CapabilityDescriptorSet> CapabilityDescriptorSet::getEmpty()
	{
		static const auto empty = std::make_shared<CapabilityDescriptorSet>();
		return empty;
	}

	unsigned long long CapabilityDescriptorSet::getGlobalHardwareCaps() const
	{
		return getTypedGrants()->ProjectMask(CapabilityGrantScope::HardwareAvailable);
	}

	unsigned long long CapabilityDescriptorSet::getRuntimeEnabledCaps() const
	{
		return getTypedGrants()->ProjectMask(CapabilityGrantScope::RuntimeEnabled);
	}

	unsigned long long CapabilityDescriptorSet::getDomainGrantedCaps() const
	{
		return getTypedGrants()->ProjectMask(CapabilityGrantScope::DomainGranted);
	}

	std::shared_ptr<CapabilityGrantCollection> CapabilityDescriptorSet::getTypedGrants() const
	{
		return TypedGrants;
	}

	unsigned long long CapabilityDescriptorSet::getCompatibilityCapsProjection() const
	{
		return getTypedGrants()->getEffectiveCompatibilityMask();
	}

	unsigned long long CapabilityDescriptorSet::getEffectiveCaps() const
	{
		return getCompatibilityCapsProjection();
	}

	bool CapabilityDescriptorSet::HasHardwareCapability(unsigned long long capabilityMask)
	{
		return HasTypedGrant(capabilityMask, CapabilityGrantScope::HardwareAvailable);
	}

	bool CapabilityDescriptorSet::IsRuntimeEnabled(unsigned long long capabilityMask)
	{
		return HasTypedGrant(capabilityMask, CapabilityGrantScope::RuntimeEnabled);
	}

	bool CapabilityDescriptorSet::IsDomainGranted(unsigned long long capabilityMask)
	{
		return HasTypedGrant(capabilityMask, CapabilityGrantScope::DomainGranted);
	}

	bool CapabilityDescriptorSet::HasEffectiveCapability(unsigned long long capabilityMask)
	{
		return HasTypedGrant(capabilityMask, CapabilityGrantScope::CompatibilityProjection);
	}

	std::shared_ptr<CapabilityGrant> CapabilityDescriptorSet::CreateGrant(unsigned long long capabilityMask, CapabilityGrantScope scope)
	{
		std::shared_ptr<CapabilityGrant> grant;
		return getTypedGrants()->TryGetGrant(capabilityMask, scope, grant) ? grant : CapabilityGrant::getDenied();
	}

	std::shared_ptr<CapabilityGrant> CapabilityDescriptorSet::CreateGrant(unsigned long long capabilityMask, CapabilityGrantScope scope, unsigned long long ownerDomainId, CapabilityDelegationPolicy delegationPolicy, CapabilityRevocationPolicy revocationPolicy, CapabilityMigrationClass migrationClass, CapabilityEvidenceVisibility evidenceVisibility, CapabilityFrontendProjectionPolicy frontendProjectionPolicy)
	{
		std::shared_ptr<CapabilityGrant> grant;
		return getTypedGrants()->TryGetGrant(capabilityMask, scope, grant) ? std::make_shared<CapabilityGrant>(grant->getCapabilityMask(), grant->getScope(), grant->isGranted(), ownerDomainId, delegationPolicy, revocationPolicy, migrationClass, evidenceVisibility, frontendProjectionPolicy) : std::make_shared<CapabilityGrant>(capabilityMask, scope, false, ownerDomainId, delegationPolicy, revocationPolicy, migrationClass, evidenceVisibility, frontendProjectionPolicy);
	}

	bool CapabilityDescriptorSet::HasTypedGrant(unsigned long long capabilityMask, CapabilityGrantScope scope)
	{
		std::shared_ptr<CapabilityGrant> grant;
		return getTypedGrants()->TryGetGrant(capabilityMask, scope, grant) && grant->getHasTypedAuthority();
	}

	CapabilityDescriptorSet::CapabilityDescriptorSet(unsigned long long globalHardwareCaps, unsigned long long runtimeEnabledCaps, unsigned long long domainGrantedCaps) : CapabilityDescriptorSet(CapabilityGrantCollection::FromCompatibilityMasks(globalHardwareCaps, runtimeEnabledCaps, domainGrantedCaps, CapabilityDescriptorSetSchema::getVmxCompatibility()))
	{
	}

	std::shared_ptr<CapabilityDescriptorSet> CapabilityDescriptorSet::WithRuntimeEnabledCaps(unsigned long long runtimeEnabledCaps)
	{
		return std::make_shared<CapabilityDescriptorSet>(getGlobalHardwareCaps(), runtimeEnabledCaps, getDomainGrantedCaps());
	}

	std::shared_ptr<CapabilityDescriptorSet> CapabilityDescriptorSet::WithDomainGrantedCaps(unsigned long long domainGrantedCaps)
	{
		return std::make_shared<CapabilityDescriptorSet>(getGlobalHardwareCaps(), getRuntimeEnabledCaps(), domainGrantedCaps);
	}
}
