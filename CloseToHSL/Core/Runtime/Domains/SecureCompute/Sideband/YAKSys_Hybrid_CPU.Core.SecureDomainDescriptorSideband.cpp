#include "YAKSys_Hybrid_CPU.Core.SecureDomainDescriptorSideband.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureDomainDescriptorSideband::SecureDomainDescriptorSideband() : SecureDomainDescriptorSideband(0, SecureSidebandVisibility::InternalOnly, 0)
	{
	}

	SecureDomainDescriptorSideband::SecureDomainDescriptorSideband(unsigned long long domainTag, SecureSidebandVisibility visibility, unsigned long long provenanceHash)
		: DomainTag(domainTag), Visibility(visibility), ProvenanceHash(provenanceHash)
	{
	}

	unsigned long long SecureDomainDescriptorSideband::getDomainTag() const
	{
		return DomainTag;
	}

	SecureSidebandVisibility SecureDomainDescriptorSideband::getVisibility() const
	{
		return Visibility;
	}

	unsigned long long SecureDomainDescriptorSideband::getProvenanceHash() const
	{
		return ProvenanceHash;
	}

	bool SecureDomainDescriptorSideband::isAuthorityCarrier() const
	{
		return false;
	}
}
