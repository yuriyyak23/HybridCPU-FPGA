#include "YAKSys_Hybrid_CPU.Core.SecureComputeDescriptorSidebandEnvelope.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeDescriptorSidebandEnvelope::SecureComputeDescriptorSidebandEnvelope() : SecureComputeDescriptorSidebandEnvelope(SecureComputeDescriptorSidebandKind::DomainDescriptor, 0, 0)
	{
	}

	SecureComputeDescriptorSidebandEnvelope::SecureComputeDescriptorSidebandEnvelope(SecureComputeDescriptorSidebandKind kind, unsigned long long domainTag, unsigned long long provenanceHash)
		: Kind(kind),
		  DomainTag(domainTag),
		  ProvenanceHash(provenanceHash)
	{
	}

	SecureComputeDescriptorSidebandKind SecureComputeDescriptorSidebandEnvelope::getKind() const
	{
		return Kind;
	}

	unsigned long long SecureComputeDescriptorSidebandEnvelope::getDomainTag() const
	{
		return DomainTag;
	}

	unsigned long long SecureComputeDescriptorSidebandEnvelope::getProvenanceHash() const
	{
		return ProvenanceHash;
	}

	bool SecureComputeDescriptorSidebandEnvelope::isAuthorityCarrier() const
	{
		return false;
	}
}
