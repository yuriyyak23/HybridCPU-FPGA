#include "YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CapabilityGrant::CapabilityGrant() : CapabilityGrant(0, CapabilityGrantScope::None, false, NoOwnerDomainId, CapabilityDelegationPolicy::None, CapabilityRevocationPolicy::None, CapabilityMigrationClass::None, CapabilityEvidenceVisibility::None, CapabilityFrontendProjectionPolicy::NeverProject)
	{
	}

	CapabilityGrant::CapabilityGrant(unsigned long long capabilityMask, CapabilityGrantScope scope, bool isGranted) : CapabilityGrant(capabilityMask, scope, isGranted, isGranted && scope == CapabilityGrantScope::DomainGranted ? DefaultRuntimeOwnerDomainId: NoOwnerDomainId, CapabilityDelegationPolicy::NonDelegable, CapabilityRevocationPolicy::RuntimeRevocable, CapabilityMigrationClass::DomainLocal, CapabilityEvidenceVisibility::HostOnly, CapabilityFrontendProjectionPolicy::ProjectIfCompatible)
	{
	}

	CapabilityGrant::CapabilityGrant(unsigned long long capabilityMask, CapabilityGrantScope scope, bool isGranted, unsigned long long ownerDomainId, CapabilityDelegationPolicy delegationPolicy, CapabilityRevocationPolicy revocationPolicy, CapabilityMigrationClass migrationClass, CapabilityEvidenceVisibility evidenceVisibility, CapabilityFrontendProjectionPolicy frontendProjectionPolicy) : CapabilityMask(capabilityMask), Scope(scope), IsGranted(isGranted), OwnerDomainId(ownerDomainId), DelegationPolicy(delegationPolicy), RevocationPolicy(revocationPolicy), MigrationClass(migrationClass), EvidenceVisibility(evidenceVisibility), FrontendProjectionPolicy(frontendProjectionPolicy) {}

	std::shared_ptr<CapabilityGrant> CapabilityGrant::getDenied()
	{
		return Denied;
	}

	unsigned long long CapabilityGrant::getCapabilityMask() const
	{
		return CapabilityMask;
	}

	CapabilityGrantScope CapabilityGrant::getScope() const
	{
		return Scope;
	}

	bool CapabilityGrant::isGranted() const
	{
		return IsGranted;
	}

	unsigned long long CapabilityGrant::getOwnerDomainId() const
	{
		return OwnerDomainId;
	}

	CapabilityDelegationPolicy CapabilityGrant::getDelegationPolicy() const
	{
		return DelegationPolicy;
	}

	CapabilityRevocationPolicy CapabilityGrant::getRevocationPolicy() const
	{
		return RevocationPolicy;
	}

	CapabilityMigrationClass CapabilityGrant::getMigrationClass() const
	{
		return MigrationClass;
	}

	CapabilityEvidenceVisibility CapabilityGrant::getEvidenceVisibility() const
	{
		return EvidenceVisibility;
	}

	CapabilityFrontendProjectionPolicy CapabilityGrant::getFrontendProjectionPolicy() const
	{
		return FrontendProjectionPolicy;
	}

	bool CapabilityGrant::getHasTypedAuthority() const
	{
		return isGranted() && getCapabilityMask() != 0 && getScope() != CapabilityGrantScope::None && getDelegationPolicy() != CapabilityDelegationPolicy::None && getRevocationPolicy() != CapabilityRevocationPolicy::None && getMigrationClass() != CapabilityMigrationClass::None && getEvidenceVisibility() != CapabilityEvidenceVisibility::None && getFrontendProjectionPolicy() != CapabilityFrontendProjectionPolicy::None;
	}

	bool CapabilityGrant::Grants(unsigned long long capabilityMask)
	{
		return isGranted() && capabilityMask != 0 && (getCapabilityMask() & capabilityMask) == capabilityMask;
	}

	bool CapabilityGrant::isPublishableCompatibilityGrant() const
	{
		return isGranted() && getScope() == CapabilityGrantScope::CompatibilityProjection && getFrontendProjectionPolicy() == CapabilityFrontendProjectionPolicy::ProjectIfCompatible;
	}
}
