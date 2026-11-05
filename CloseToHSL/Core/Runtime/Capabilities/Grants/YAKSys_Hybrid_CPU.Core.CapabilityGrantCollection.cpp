#include "YAKSys_Hybrid_CPU.Core.CapabilityGrantCollection.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CapabilityGrantCollection::CapabilityGrantCollection() : CapabilityGrantCollection(std::vector<std::shared_ptr<CapabilityGrant>>{})
	{
	}

	CapabilityGrantCollection::CapabilityGrantCollection(const std::vector<std::shared_ptr<CapabilityGrant>> &grants) : _grants(grants)
	{
	}

	std::shared_ptr<CapabilityGrantCollection> CapabilityGrantCollection::getEmpty()
	{
		static const auto empty = std::make_shared<CapabilityGrantCollection>();
		return empty;
	}

	std::span<const std::shared_ptr<CapabilityGrant>> CapabilityGrantCollection::getGrants() const
	{
		return _grants;
	}

	int CapabilityGrantCollection::getCount() const
	{
		return _grants.size();
	}

	unsigned long long CapabilityGrantCollection::ProjectMask(CapabilityGrantScope scope) const
	{
		unsigned long long mask = 0;

		for (auto grant : _grants)
		{
			if (grant->isGranted() && grant->getScope() == scope)
			{
				mask |= grant->getCapabilityMask();
			}
		}

		return mask;
	}

	unsigned long long CapabilityGrantCollection::getEffectiveCompatibilityMask() const
	{
		return ProjectMask(CapabilityGrantScope::CompatibilityProjection);
	}

	bool CapabilityGrantCollection::getHasAuthoritativeTypedGrants() const
	{
		for (auto grant : _grants)
		{
			if (!grant->getHasTypedAuthority())
			{
				return false;
			}
		}

		return _grants.size() > 0;
	}

	bool CapabilityGrantCollection::TryGetGrant(unsigned long long capabilityMask, CapabilityGrantScope scope, std::shared_ptr<CapabilityGrant> &grant)
	{
		for (auto candidate : _grants)
		{
			if (candidate->getScope() == scope && candidate->Grants(capabilityMask))
			{
				grant = candidate;
				return true;
			}
		}

		grant = CapabilityGrant::getDenied();
		return false;
	}

	std::shared_ptr<CapabilityGrant> CapabilityGrantCollection::CreateInternalGrant(unsigned long long capabilityMask, CapabilityGrantScope scope, unsigned long long ownerDomainId)
	{
		return std::make_shared<CapabilityGrant>(capabilityMask, scope, true, ownerDomainId, CapabilityDelegationPolicy::NonDelegable, CapabilityRevocationPolicy::RuntimeRevocable, CapabilityMigrationClass::DomainLocal, CapabilityEvidenceVisibility::HostOnly, CapabilityFrontendProjectionPolicy::NeverProject);
	}

	std::shared_ptr<CapabilityGrantCollection> CapabilityGrantCollection::FromCompatibilityMasks(unsigned long long globalHardwareCaps, unsigned long long runtimeEnabledCaps, unsigned long long domainGrantedCaps, std::shared_ptr<CapabilityDescriptorSetSchema> schema, unsigned long long ownerDomainId)
	{
		schema = (schema != nullptr) ? schema : CapabilityDescriptorSetSchema::getVmxCompatibility();
		unsigned long long effectiveCaps = globalHardwareCaps & runtimeEnabledCaps & domainGrantedCaps & schema->getCompatibilityPublicationMask();

		std::vector<std::shared_ptr<CapabilityGrant>> grants(CapabilityDescriptorSetSchema::getVmxCompatibilityBits().size() * 4);
		int count = 0;

		for (auto entry : CapabilityDescriptorSetSchema::getVmxCompatibilityBits())
		{
			if ((globalHardwareCaps & entry->CapabilityMask) == entry->CapabilityMask)
			{
				grants[count++] = CreateInternalGrant(entry->CapabilityMask, CapabilityGrantScope::HardwareAvailable, ownerDomainId);
			}

			if ((runtimeEnabledCaps & entry->CapabilityMask) == entry->CapabilityMask)
			{
				grants[count++] = CreateInternalGrant(entry->CapabilityMask, CapabilityGrantScope::RuntimeEnabled, ownerDomainId);
			}

			if ((domainGrantedCaps & entry->CapabilityMask) == entry->CapabilityMask)
			{
				grants[count++] = CreateInternalGrant(entry->CapabilityMask, CapabilityGrantScope::DomainGranted, ownerDomainId);
			}

			if ((effectiveCaps & entry->CapabilityMask) == entry->CapabilityMask)
			{
				grants[count++] = std::make_shared<CapabilityGrant>(entry->CapabilityMask, entry->ProjectionScope, true, ownerDomainId, CapabilityDelegationPolicy::NonDelegable, CapabilityRevocationPolicy::RuntimeRevocable, CapabilityMigrationClass::DomainLocal, entry->EvidenceVisibility, entry->FrontendProjectionPolicy);
			}
		}

		if (count == grants.size())
		{
			return std::make_shared<CapabilityGrantCollection>(grants);
		}

		grants.resize(count);
		return std::make_shared<CapabilityGrantCollection>(grants);
	}
}
