#include "YAKSys_Hybrid_CPU.Core.SecureComputeEvidenceSidebandEnvelope.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeEvidenceSidebandEnvelope::SecureComputeEvidenceSidebandEnvelope() : SecureComputeEvidenceSidebandEnvelope(SecureComputeEvidenceSidebandClass::HostOwnedQuarantined, 0, 0)
	{
	}

	SecureComputeEvidenceSidebandEnvelope::SecureComputeEvidenceSidebandEnvelope(SecureComputeEvidenceSidebandClass evidenceClass, unsigned long long domainTag, unsigned long long evidenceHash)
		: EvidenceClass(evidenceClass),
		  DomainTag(domainTag),
		  EvidenceHash(evidenceHash)
	{
	}

	SecureComputeEvidenceSidebandClass SecureComputeEvidenceSidebandEnvelope::getEvidenceClass() const
	{
		return EvidenceClass;
	}

	unsigned long long SecureComputeEvidenceSidebandEnvelope::getDomainTag() const
	{
		return DomainTag;
	}

	unsigned long long SecureComputeEvidenceSidebandEnvelope::getEvidenceHash() const
	{
		return EvidenceHash;
	}

	bool SecureComputeEvidenceSidebandEnvelope::getCanExposeToGuest() const
	{
		return ([&] { const auto codexPatternValue = getEvidenceClass(); return codexPatternValue == SecureComputeEvidenceSidebandClass::GuestVisible || codexPatternValue == SecureComputeEvidenceSidebandClass::CompatibilityAlias; }());
	}

	bool SecureComputeEvidenceSidebandEnvelope::getMustRecomputeAfterRestore() const
	{
		return getEvidenceClass() == SecureComputeEvidenceSidebandClass::RecomputeAfterRestore;
	}
}
