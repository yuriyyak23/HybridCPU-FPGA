#pragma once

#include "SecureComputeEvidenceSidebandEnvelope.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeEvidenceSidebandEnvelope final : public std::enable_shared_from_this<SecureComputeEvidenceSidebandEnvelope>
	{
	private:
		const SecureComputeEvidenceSidebandClass EvidenceClass;
		const unsigned long long DomainTag;
		const unsigned long long EvidenceHash;

	public:
		SecureComputeEvidenceSidebandEnvelope();

		SecureComputeEvidenceSidebandEnvelope(SecureComputeEvidenceSidebandClass evidenceClass, unsigned long long domainTag, unsigned long long evidenceHash);

		SecureComputeEvidenceSidebandClass getEvidenceClass() const;

		unsigned long long getDomainTag() const;

		unsigned long long getEvidenceHash() const;

		bool getCanExposeToGuest() const;

		bool getMustRecomputeAfterRestore() const;
	};
}
