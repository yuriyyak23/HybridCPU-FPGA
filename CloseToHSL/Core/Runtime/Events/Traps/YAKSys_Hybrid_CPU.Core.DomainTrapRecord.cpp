#include "YAKSys_Hybrid_CPU.Core.DomainTrapRecord.h"

namespace YAKSys_Hybrid_CPU::Core
{

	DomainTrapRecord::DomainTrapRecord() : DomainTrapRecord(DomainTrapRecordAuthority::Runtime, 0, 0, false, 0, 0, true)
	{
	}

	DomainTrapRecord::DomainTrapRecord(DomainTrapRecordAuthority authority, unsigned long long sequence, unsigned long long domainId, bool shouldExit, unsigned long long descriptorEpoch, unsigned long long evidenceHash, bool allowsCompatibilityProjection)
		: Authority(authority),
		  Sequence(sequence),
		  DomainId(domainId),
		  ShouldExit(shouldExit),
		  DescriptorEpoch(descriptorEpoch),
		  EvidenceHash(evidenceHash),
		  AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	DomainTrapRecordAuthority DomainTrapRecord::getAuthority() const
	{
		return Authority;
	}

	unsigned long long DomainTrapRecord::getSequence() const
	{
		return Sequence;
	}

	unsigned long long DomainTrapRecord::getDomainId() const
	{
		return DomainId;
	}

	bool DomainTrapRecord::getShouldExit() const
	{
		return ShouldExit;
	}

	unsigned long long DomainTrapRecord::getDescriptorEpoch() const
	{
		return DescriptorEpoch;
	}

	unsigned long long DomainTrapRecord::getEvidenceHash() const
	{
		return EvidenceHash;
	}

	bool DomainTrapRecord::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool DomainTrapRecord::isRuntimeAuthoritative() const
	{
		return getAuthority() == DomainTrapRecordAuthority::Runtime;
	}

	bool DomainTrapRecord::getHasDomainBinding() const
	{
		return getDomainId() != 0;
	}

	bool DomainTrapRecord::getHasEvidenceHash() const
	{
		return getEvidenceHash() != 0;
	}

	bool DomainTrapRecord::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && isRuntimeAuthoritative();
	}

	std::shared_ptr<DomainTrapRecord> DomainTrapRecord::WithSequence(unsigned long long sequence)
	{
		return std::make_shared<DomainTrapRecord>(getAuthority(), sequence, getDomainId(), getShouldExit(), getDescriptorEpoch(), getEvidenceHash(), getAllowsCompatibilityProjection());
	}
}
